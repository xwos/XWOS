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
 */

#ifndef __xwos_lib_object_h__
#define __xwos_lib_object_h__

#include <xwos/standard.h>
#include <xwos/lib/xwaop.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwobj 玄武OS的对象
 * @{
 */

/**
 * @brief 将对象强制类型转换
 * @param[in] type: 类型
 * @param[in] obj: 对象
 * @note
 * - 类似于C++中的关键字：static_cast<type>(obj)
 */
#define xwos_cast(type, obj)    ((type)(obj))

#define XWOS_OBJ_MAGIC  0x58574F53U /**< 所有玄武OS的对象都有相同的幻数 */

/**
 * @brief 垃圾回收函数指针类型
 */
typedef xwer_t (* xwobj_gc_f)(void *);

/**
 * @brief XWOS对象
 * @note
 * - 内存算法memslice使用结构体的第一块内存字作为无锁单链表节点，
 *   因此每个对象在定义时必须保证其第一个成员的大小等于系统指针大小，
 *   并且它们的初始值都为0。memslice可以将这个初始值备份起来，当结构体被分配
 *   出去时使用备份值恢复原内容。
 */
struct __xwcc_aligned(XWMM_ALIGNMENT) xwos_object {
        xwsq_t tik; /**< 对象的标签，用于防止ABA问题与野指针问题 */
        xwsq_t magic; /**< 对象的幻数，用于防止ABA问题与野指针问题 */
        xwsq_a refcnt; /**< + 引用计数:
                              - 0: 对象可以被销毁；
                              - 1: 对象就绪；
                              - >1: 对象正在被引用。*/
        xwobj_gc_f gcfunc; /**< 垃圾回收函数 */
};

/**
 * @brief XWOS对象描述符
 */
typedef struct {
        struct xwos_object * obj;
        xwsq_t tik;
} xwobj_d;

void xwos_objtik_init(void);
void xwos_object_construct(struct xwos_object * obj);
void xwos_object_destruct(struct xwos_object * obj);
xwer_t xwos_object_activate(struct xwos_object * obj, xwobj_gc_f gcfunc);
void xwos_object_setgc(struct xwos_object * obj, xwobj_gc_f gcfunc);
xwer_t xwos_object_acquire(struct xwos_object * obj, xwsq_t tik);
xwer_t xwos_object_release(struct xwos_object * obj, xwsq_t tik);
xwer_t xwos_object_grab(struct xwos_object * obj);
xwer_t xwos_object_put(struct xwos_object * obj);
xwer_t xwos_object_rawput(struct xwos_object * obj);
xwsq_t xwos_object_get_refcnt(struct xwos_object * obj);

/**
 * @} xwobj
 */

#endif /* xwos/lib/object.h */
