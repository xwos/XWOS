/**
 * @file
 * @brief XWOS UP内核同步机制：管道信号量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_sync_plsem_h__
#define __xwos_up_sync_plsem_h__

#include <xwos/standard.h>
#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
#  include <xwos/mm/memslice.h>
#endif
#include <xwos/up/plwq.h>
#include <xwos/up/sync/vsem.h>

/**
 * @brief 管道信号量对象
 */
struct xwup_plsem {
        struct xwup_vsem vsem; /**< C语言面向对象：继承struct xwup_vsem */
        struct xwup_plwq plwq; /**< 实时等待队列 */
};

xwer_t xwup_plsem_intr(struct xwup_plsem * sem, struct xwup_wqn * wqn);

#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
xwer_t xwup_plsem_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
xwer_t xwup_plsem_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif
xwer_t xwup_plsem_init(struct xwup_plsem * sem, xwssq_t val, xwssq_t max);
xwer_t xwup_plsem_fini(struct xwup_plsem * sem);
xwer_t xwup_plsem_create(struct xwup_plsem ** ptrbuf, xwssq_t val, xwssq_t max);
xwer_t xwup_plsem_delete(struct xwup_plsem * sem, xwsq_t tik);
xwer_t xwup_plsem_acquire(struct xwup_plsem * sem, xwsq_t tik);
xwer_t xwup_plsem_release(struct xwup_plsem * sem, xwsq_t tik);
xwer_t xwup_plsem_grab(struct xwup_plsem * sem);
xwer_t xwup_plsem_put(struct xwup_plsem * sem);

xwer_t xwup_plsem_bind(struct xwup_plsem * sem, struct xwup_evt * sel, xwsq_t pos);
xwer_t xwup_plsem_unbind(struct xwup_plsem * sem, struct xwup_evt * sel);
xwer_t xwup_plsem_freeze(struct xwup_plsem * sem);
xwer_t xwup_plsem_thaw(struct xwup_plsem * sem);
xwer_t xwup_plsem_post(struct xwup_plsem * sem);
xwer_t xwup_plsem_wait(struct xwup_plsem * sem);
xwer_t xwup_plsem_wait_to(struct xwup_plsem * sem, xwtm_t xwtm);
xwer_t xwup_plsem_wait_unintr(struct xwup_plsem * sem);
xwer_t xwup_plsem_trywait(struct xwup_plsem * sem);
xwer_t xwup_plsem_get_max(struct xwup_plsem * sem, xwssq_t * max);
xwer_t xwup_plsem_get_value(struct xwup_plsem * sem, xwssq_t * val);

#endif /* xwos/up/sync/plsem.h */
