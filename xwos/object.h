/**
 * @file
 * @brief 玄武OS的对象
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * XWOS对象是XWOS中所有具体对象（例如线程控制块/信号量等等）的父类，
 * XWOS对象提供了最基本的基于引用计数法的垃圾回收(GC)机制。
 */

#ifndef __xwos_object_h__
#define __xwos_object_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwaop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 将对象强制类型转换
 * @patam type: (I) 类型
 * @param obj: (I) 对象
 * @note
 * 类似于C++中的关键字：static_cast<type>(obj)
 */
#define xwos_static_cast(type, obj)     ((type)(obj))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 垃圾回收函数类型
 */
typedef xwer_t (*xwobj_gc_f)(void *);

/**
 * @brief XWOS对象
 */
struct __aligned(XWMMCFG_ALIGNMENT) xwos_object {
        __atomic xwsq_t refcnt; /**< + 引用计数:
                                       - 0: 对象可以被销毁；
                                       - 1: 对象就绪；
                                       - >1: 对象正在被引用。
                                     + 内存算法memslice使用结构体的第一块内存字
                                       作为单链表节点，因此每个对象在定义时必须
                                       保证其第一个成员的大小等于系统指针大小，
                                       并且它们的初始值都相同。内存算法memslice
                                       可以将这个初始值备份起来，当结构体被分配
                                       出去时使用备份值恢复第一块内存的内容。 */
        xwobj_gc_f gcfunc; /**< 垃圾回收的函数 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS对象的构造函数
 * @param obj: (I) 对象指针
 */
static __xw_inline
void xwos_object_construct(struct xwos_object * obj)
{
        obj->refcnt = 0;
        obj->gcfunc = NULL;
}

/**
 * @brief XWOS对象的析构函数
 * @param obj: (I) 对象指针
 */
static __xw_inline
void xwos_object_destruct(struct xwos_object * obj)
{
        obj->gcfunc = NULL;
}

/**
 * @brief 激活一个XWOS对象（将其引用计数初始化位1）
 * @param obj: (I) 对象指针
 * @param gcfunc: (I) 垃圾回收函数：当对象应用计数为0，调用此函数回收资源。
 * @return 错误码
 * @retval OK: OK
 * @retval -EOBJACTIVE: 对象已激活
 */
static __xw_inline
xwer_t xwos_object_activate(struct xwos_object * obj, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwaop_teq_then_add(xwsq_t, &obj->refcnt, 0, 1, NULL, NULL);
        if (__likely(OK == rc)) {
                obj->gcfunc = gcfunc;
        } else {
                rc = -EOBJACTIVE;
        }
        return rc;
}

/**
 * @brief 增加对象的引用计数
 * @param obj: (I) 对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EOBJDEAD: 对象已销毁
 */
static __xw_inline
xwer_t xwos_object_grab(struct xwos_object * obj)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq_t, &obj->refcnt, 1, 1, NULL, NULL);
        if (__unlikely(rc < 0)) {
                rc = -EOBJDEAD;
        }/* else {} */
        return rc;
}

/**
 * @brief 减少对象的引用计数
 * @param obj: (I) 对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EOBJDEAD: 对象已销毁
 */
static __xw_inline
xwer_t xwos_object_put(struct xwos_object * obj)
{
        xwer_t rc;
        xwsq_t nv;

        rc = xwaop_tgt_then_sub(xwsq_t, &obj->refcnt,
                                0, 1,
                                &nv, NULL);
        if (__likely(OK == rc)) {
                if (__unlikely(0 == nv)) {
                        if (obj->gcfunc) {
                                rc = obj->gcfunc(obj);
                        }/* else {} */
                }/* else {} */
        } else {
                rc = -EOBJDEAD;
        }
        return rc;
}

/**
 * @brief 获取XWOS对象的引用计数
 * @param obj: (I) 对象指针
 * @return 引用计数
 */
static __xw_inline
xwsq_t xwos_object_get_refcnt(struct xwos_object * obj)
{
        return xwaop_load(xwsq_t, &obj->refcnt, xwmb_modr_consume);
}

#endif /* xwos/object.h */
