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

#ifndef __xwos_up_sync_cdt_h__
#define __xwos_up_sync_cdt_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/up/plwq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 事件条件量对象
 */
struct xwsync_cdt {
        bool neg; /**< 是否为负 */
        struct xwos_plwq wq; /**< 等待队列 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwsync_cdt_activate(struct xwsync_cdt * cdt);

__xwos_code
void xwsync_cdt_deactivate(struct xwsync_cdt * cdt);

__xwos_code
xwer_t xwsync_cdt_intr(struct xwsync_cdt * cdt, struct xwos_wqn * wqn);

__xwos_code
xwer_t xwsync_cdt_do_unicast(struct xwsync_cdt * cdt);

__xwos_code
xwer_t xwsync_cdt_do_timedwait(struct xwsync_cdt * cdt,
                               struct xwos_tcb * tcb,
                               void * lock, xwsq_t lktype,
                               void * lkdata, xwsz_t datanum,
                               xwtm_t * xwtm, xwsq_t * lkst);;

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwsync_cdt_init(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_destroy(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_create(struct xwsync_cdt ** ptrbuf);

__xwos_api
xwer_t xwsync_cdt_delete(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_thaw(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_freeze(struct xwsync_cdt * cdt);

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
 */
static __xwos_inline_api
xwer_t xwsync_cdt_wait(struct xwsync_cdt * cdt,
                       void * lock, xwsq_t lktype,
                       void * lkdata, xwsz_t datanum,
                       xwsq_t * lkst)
{
        xwtm_t expected;

        expected = XWTM_MAX;
        return xwsync_cdt_timedwait(cdt, lock, lktype, lkdata, datanum,
                                    &expected, lkst);
}

#endif /* xwos/up/sync/cdt.h */
