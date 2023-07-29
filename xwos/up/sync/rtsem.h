/**
 * @file
 * @brief XWOS UP内核同步机制：实时信号量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_sync_rtsem_h__
#define __xwos_up_sync_rtsem_h__

#include <xwos/standard.h>
#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
#  include <xwos/mm/memslice.h>
#endif
#include <xwos/up/rtwq.h>
#include <xwos/up/sync/vsem.h>

/**
 * @brief 实时信号量对象
 */
struct xwup_rtsem {
        struct xwup_vsem vsem; /**< C语言面向对象：继承struct xwup_vsem */
        struct xwup_rtwq rtwq; /**< 实时等待队列 */
};

xwer_t xwup_rtsem_intr(struct xwup_rtsem * sem, struct xwup_wqn * wqn);

#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
xwer_t xwup_rtsem_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
xwer_t xwup_rtsem_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif
xwer_t xwup_rtsem_init(struct xwup_rtsem * sem, xwssq_t val, xwssq_t max);
xwer_t xwup_rtsem_fini(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_create(struct xwup_rtsem ** ptrbuf, xwssq_t val, xwssq_t max);
xwer_t xwup_rtsem_delete(struct xwup_rtsem * sem, xwsq_t tik);

xwer_t xwup_rtsem_acquire(struct xwup_rtsem * sem, xwsq_t tik);
xwer_t xwup_rtsem_release(struct xwup_rtsem * sem, xwsq_t tik);
xwer_t xwup_rtsem_grab(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_put(struct xwup_rtsem * sem);

xwer_t xwup_rtsem_bind(struct xwup_rtsem * sem, struct xwup_evt * sel, xwsq_t pos);
xwer_t xwup_rtsem_unbind(struct xwup_rtsem * sem, struct xwup_evt * sel);
xwer_t xwup_rtsem_freeze(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_thaw(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_post(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_wait(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_wait_to(struct xwup_rtsem * sem, xwtm_t xwtm);
xwer_t xwup_rtsem_wait_unintr(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_trywait(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_get_value(struct xwup_rtsem * sem, xwssq_t * sval);

#endif /* xwos/up/sync/rtsem.h */
