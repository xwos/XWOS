/**
 * @file
 * @brief 玄武OS MP内核同步机制：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_sync_cond_h__
#define __xwos_mp_sync_cond_h__

#include <xwos/standard.h>
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/plwq.h>

struct xwmp_thd;
struct xwmp_evt;

/**
 * @brief 条件量对象
 */
struct xwmp_cond {
        struct xwmp_synobj synobj; /**< C语言面向对象：继承struct xwmp_synobj */
        xwssq_t count; /**< 计数器：<0，条件量处于负状态；*/
        union {
                struct xwmp_plwq pl; /**< 管道的等待队列 */
        } wq; /**< 条件量的等待队列 */
};

void xwmp_cond_construct(struct xwmp_cond * cond);
void xwmp_cond_destruct(struct xwmp_cond * cond);
xwer_t xwmp_cond_activate(struct xwmp_cond * cond, xwobj_gc_f gcfunc);
xwer_t xwmp_cond_intr(struct xwmp_cond * cond, struct xwmp_wqn * wqn);
xwer_t xwmp_cond_do_unicast(struct xwmp_cond * cond);
xwer_t xwmp_cond_do_timedwait(struct xwmp_cond * cond, struct xwmp_thd * thd,
                              void * lock, xwsq_t lktype, void * lkdata,
                              xwtm_t * xwtm, xwsq_t * lkst);

#if defined(XWMPCFG_SYNC_COND_MEMSLICE) && (1 == XWMPCFG_SYNC_COND_MEMSLICE)
xwer_t xwmp_cond_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif

xwer_t xwmp_cond_create(struct xwmp_cond ** ptrbuf);
xwer_t xwmp_cond_delete(struct xwmp_cond * cond);
xwer_t xwmp_cond_init(struct xwmp_cond * cond);
xwer_t xwmp_cond_fini(struct xwmp_cond * cond);

xwer_t xwmp_cond_acquire(struct xwmp_cond * cond, xwsq_t tik);
xwer_t xwmp_cond_release(struct xwmp_cond * cond, xwsq_t tik);
xwer_t xwmp_cond_grab(struct xwmp_cond * cond);
xwer_t xwmp_cond_put(struct xwmp_cond * cond);

#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
xwer_t xwmp_cond_bind(struct xwmp_cond * cond,
                      struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_cond_unbind(struct xwmp_cond * cond,
                        struct xwmp_evt * evt);
#endif

xwer_t xwmp_cond_freeze(struct xwmp_cond * cond);
xwer_t xwmp_cond_thaw(struct xwmp_cond * cond);
xwer_t xwmp_cond_intr_all(struct xwmp_cond * cond);
xwer_t xwmp_cond_broadcast(struct xwmp_cond * cond);
xwer_t xwmp_cond_unicast(struct xwmp_cond * cond);
xwer_t xwmp_cond_wait(struct xwmp_cond * cond,
                      void * lock, xwsq_t lktype,
                      void * lkdata, xwsq_t * lkst);
xwer_t xwmp_cond_timedwait(struct xwmp_cond * cond,
                           void * lock, xwsq_t lktype, void * lkdata,
                           xwtm_t * xwtm, xwsq_t * lkst);

#endif /* xwos/mp/sync/cond.h */
