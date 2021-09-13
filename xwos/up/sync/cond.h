/**
 * @file
 * @brief 玄武OS UP内核同步机制：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_sync_cond_h__
#define __xwos_up_sync_cond_h__

#include <xwos/standard.h>
#include <xwos/up/plwq.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
#  include <xwos/up/sync/obj.h>
#endif

struct xwup_thd;

/**
 * @brief 条件量对象
 */
struct xwup_cond {
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        struct xwup_synobj synobj; /**< C语言面向对象：继承struct xwup_synobj */
#endif
        bool neg; /**< 是否为负 */
        struct xwup_plwq wq; /**< 等待队列 */
};

void xwup_cond_activate(struct xwup_cond * cond);
void xwup_cond_deactivate(struct xwup_cond * cond);
xwer_t xwup_cond_intr(struct xwup_cond * cond, struct xwup_wqn * wqn);
xwer_t xwup_cond_do_unicast(struct xwup_cond * cond);
xwer_t xwup_cond_do_timedwait(struct xwup_cond * cond,
                              struct xwup_thd * thd,
                              void * lock, xwsq_t lktype, void * lkdata,
                              xwtm_t * xwtm, xwsq_t * lkst);

xwer_t xwup_cond_init(struct xwup_cond * cond);
xwer_t xwup_cond_fini(struct xwup_cond * cond);
xwer_t xwup_cond_create(struct xwup_cond ** ptrbuf);
xwer_t xwup_cond_delete(struct xwup_cond * cond);

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
xwer_t xwup_cond_bind(struct xwup_cond * cond, struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_cond_unbind(struct xwup_cond * cond, struct xwup_evt * evt);
#else
#  define xwup_cond_bind(cond, evt, pos) (-ENOSYS)
#  define xwup_cond_unbind(cond, evt) (ENOSYS)
#endif

xwer_t xwup_cond_freeze(struct xwup_cond * cond);
xwer_t xwup_cond_thaw(struct xwup_cond * cond);
xwer_t xwup_cond_intr_all(struct xwup_cond * cond);
xwer_t xwup_cond_broadcast(struct xwup_cond * cond);
xwer_t xwup_cond_unicast(struct xwup_cond * cond);
xwer_t xwup_cond_wait(struct xwup_cond * cond,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwsq_t * lkst);
xwer_t xwup_cond_timedwait(struct xwup_cond * cond,
                           void * lock, xwsq_t lktype, void * lkdata,
                           xwtm_t * xwtm, xwsq_t * lkst);

#endif /* xwos/up/sync/cond.h */
