/**
 * @file
 * @brief XuanWuOS的同步机制：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_sync_condition_h__
#define __xwos_smp_sync_condition_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/smp/plwq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_tcb;

/**
 * @brief 条件量对象
 */
struct xwsync_cdt {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        xwssq_t count; /**< 计数器：<0，条件量处于负状态；*/
        union {
                struct xwos_plwq pl; /**< 管道的等待队列 */
        } wq; /**< 条件量的等待队列 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param cdt: (I) 条件量对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwsync_cdt_grab(struct xwsync_cdt * cdt)
{
        return xwos_object_grab(&cdt->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param cdt: (I) 条件量对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwsync_cdt_put(struct xwsync_cdt * cdt)
{
        return xwos_object_put(&cdt->xwobj);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwsync_cdt_construct(struct xwsync_cdt * cdt);

__xwos_code
void xwsync_cdt_destruct(struct xwsync_cdt * cdt);

__xwos_code
xwer_t xwsync_cdt_activate(struct xwsync_cdt * cdt, xwobj_gc_f gcfunc);

__xwos_code
xwer_t xwsync_cdt_intr(struct xwsync_cdt * cdt, struct xwos_wqn * wqn);

__xwos_code
xwer_t xwsync_cdt_do_unicast(struct xwsync_cdt * cdt);

__xwos_code
xwer_t xwsync_cdt_do_timedwait(struct xwsync_cdt * cdt, struct xwos_tcb * tcb,
                               void * lock, xwsq_t lktype,
                               void * lkdata, xwsz_t datanum,
                               xwtm_t * xwtm, xwsq_t * lkst);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SYNC_CDT_MEMSLICE) && (1 == XWSMPCFG_SYNC_CDT_MEMSLICE)
__xwos_init_code
xwer_t xwsync_cdt_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif /* XWSMPCFG_SYNC_CDT_MEMSLICE */

__xwos_api
xwer_t xwsync_cdt_create(struct xwsync_cdt ** ptrbuf);

__xwos_api
xwer_t xwsync_cdt_delete(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_destroy(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_init(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_freeze(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_thaw(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_intr_all(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_broadcast(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_unicast(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_timedwait(struct xwsync_cdt * cdt,
                            void * lock, xwsq_t lktype,
                            void * lkdata, xwsz_t datanum,
                            xwtm_t * xwtm, xwsq_t * lkst);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：等待条件量
 * @param cdt: (I) 条件量对象指针
 * @param lock: (I) (I) 锁的地址
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval OK: OK
 * @retval -EINVAL: 无效的参数
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * @note
 * - 调用的线程会加入到条件量的等待队列中等待。在这个过程中，线程还可以解锁
 *   已经取得的锁。注意整个过程是原子的（不可打断的）。此函数支持的“锁”的类型
 *   有很多种，因此，传递锁的指针的参数指定为类型void * ，具体的类型由参数
 *   lktype决定。有些“锁”可能还需要额外的参数，由lkdata指明首地址，datanum
 *   指明数据的数量或大小。这个函数返回时，会在lkst中返回锁的状态。
 */
static __xwos_inline_api
xwer_t xwsync_cdt_wait(struct xwsync_cdt * cdt,
                       void * lock, xwsq_t lktype,
                       void * lkdata, xwsz_t datanum,
                       xwsq_t * lkst)
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_cdt_timedwait(cdt, lock, lktype, lkdata, datanum,
                                    &expected, lkst);
}

#endif /* xwos/smp/sync/condition.h */
