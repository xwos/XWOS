/**
 * @file
 * @brief XWOS UP内核同步机制：条件量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_sync_cond_h__
#define __xwos_up_sync_cond_h__

#include <xwos/standard.h>
#if defined(XWOSCFG_SYNC_COND_MEMPOOL) && (1 == XWOSCFG_SYNC_COND_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_COND_SMA) && (1 == XWOSCFG_SYNC_COND_SMA)
#  include <xwos/mm/sma.h>
#endif
#include <xwos/up/plwq.h>
#include <xwos/up/sync/obj.h>

struct xwup_thd;

/**
 * @brief 条件量对象
 */
struct xwup_cond {
        struct xwup_synobj synobj; /**< C语言面向对象：继承 `struct xwup_synobj` */
        bool neg; /**< 是否为负 */
        struct xwup_plwq wq; /**< 等待队列 */
};

void xwup_cond_construct(struct xwup_cond * cond);
void xwup_cond_destruct(struct xwup_cond * cond);
xwer_t xwup_cond_activate(struct xwup_cond * cond, xwobj_gc_f gcfunc);
xwer_t xwup_cond_intr(struct xwup_cond * cond, struct xwup_wqn * wqn);
xwer_t xwup_cond_intr_all(struct xwup_cond * cond);

#if defined(XWOSCFG_SYNC_COND_MEMPOOL) && (1 == XWOSCFG_SYNC_COND_MEMPOOL)
xwer_t xwup_cond_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_SYNC_COND_MEMSLICE) && (1 == XWOSCFG_SYNC_COND_MEMSLICE)
xwer_t xwup_cond_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#elif defined(XWOSCFG_SYNC_COND_SMA) && (1 == XWOSCFG_SYNC_COND_SMA)
void xwup_cond_cache_init(struct xwmm_sma * sma);
#endif

xwer_t xwup_cond_init(struct xwup_cond * cond);
xwer_t xwup_cond_fini(struct xwup_cond * cond);
xwer_t xwup_cond_create(struct xwup_cond ** ptrbuf);
xwer_t xwup_cond_delete(struct xwup_cond * cond, xwsq_t tik);
xwer_t xwup_cond_acquire(struct xwup_cond * cond, xwsq_t tik);
xwer_t xwup_cond_release(struct xwup_cond * cond, xwsq_t tik);
xwer_t xwup_cond_grab(struct xwup_cond * cond);
xwer_t xwup_cond_put(struct xwup_cond * cond);

xwer_t xwup_cond_bind(struct xwup_cond * cond, struct xwup_evt * sel, xwsq_t pos);
xwer_t xwup_cond_unbind(struct xwup_cond * cond, struct xwup_evt * sel);
xwer_t xwup_cond_freeze(struct xwup_cond * cond);
xwer_t xwup_cond_thaw(struct xwup_cond * cond);
xwer_t xwup_cond_broadcast(struct xwup_cond * cond);
xwer_t xwup_cond_unicast(struct xwup_cond * cond);
xwer_t xwup_cond_wait(struct xwup_cond * cond,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwsq_t * lkst);
xwer_t xwup_cond_wait_to(struct xwup_cond * cond,
                         void * lock, xwsq_t lktype, void * lkdata,
                         xwtm_t to, xwsq_t * lkst);
xwer_t xwup_cond_wait_unintr(struct xwup_cond * cond,
                             void * lock, xwsq_t lktype, void * lkdata,
                             xwsq_t * lkst);

#endif /* xwos/up/sync/cond.h */
