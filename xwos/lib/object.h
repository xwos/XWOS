/**
 * @file
 * @brief XWOS通用库：玄武OS的对象
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * XWOS对象是XWOS中所有具体对象（例如线程控制块/信号量等等）的父类，
 * XWOS对象提供了最基本的基于引用计数法的垃圾回收(GC)机制。
 */

#ifndef __xwos_lib_object_h__
#define __xwos_lib_object_h__

#include <xwos/standard.h>
#include <xwos/lib/xwaop.h>

/**
 * @brief 将对象强制类型转换
 * @patam type: (I) 类型
 * @param obj: (I) 对象
 * @note
 * - 类似于C++中的关键字：static_cast<type>(obj)
 */
#define xwos_cast(type, obj)    ((type)(obj))

/**
 * @brief 垃圾回收函数指针类型
 */
typedef xwer_t (* xwobj_gc_f)(void *);

/**
 * @brief XWOS对象
 */
struct __xwcc_aligned(XWMMCFG_ALIGNMENT) xwos_object {
        xwsq_a refcnt; /**< + 引用计数:
                              - 0: 对象可以被销毁；
                              - 1: 对象就绪；
                              - >1: 对象正在被引用。
                            + 内存算法memslice使用结构体的第一块内存字
                              作为单链表节点，因此每个对象在定义时必须
                              保证其第一个成员的大小等于系统指针大小，
                              并且它们的初始值都相同。内存算法memslice
                              可以将这个初始值备份起来，当结构体被分配
                              出去时使用备份值恢复第一块内存的内容。 */
        xwobj_gc_f gcfunc; /**< 垃圾回收函数 */
        xwsq_t ticket; /**< 对象的标签 */
};

/**
 * @brief XWOS对象描述符
 */
typedef struct {
        struct xwos_object * obj;
        xwsq_t ticket;
} xwobj_d;

void xwos_objtix_init(void);
void xwos_object_construct(struct xwos_object * obj);
void xwos_object_destruct(struct xwos_object * obj);
xwer_t xwos_object_activate(struct xwos_object * obj, xwobj_gc_f gcfunc);
xwobj_d xwos_object_get_d(struct xwos_object * obj);
xwer_t xwos_object_grab(struct xwos_object * obj);
xwer_t xwos_object_grab_safely(xwobj_d objd);
xwer_t xwos_object_put(struct xwos_object * obj);
xwsq_t xwos_object_get_refcnt(struct xwos_object * obj);

#endif /* xwos/lib/object.h */
