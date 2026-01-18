/**
 * @file
 * @brief XWOS MP内核同步机制：信号量
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_sync_sem_h__
#define __xwos_mp_sync_sem_h__

#include <xwos/standard.h>
#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_SEM_SMA) && (1 == XWOSCFG_SYNC_SEM_SMA)
#  include <xwos/mm/sma.h>
#endif
#include <xwos/mp/rtwq.h>
#include <xwos/mp/plwq.h>
#include <xwos/mp/sync/obj.h>

/**
 * @brief 负信号量的值
 */
#define XWMP_SEM_NEGTIVE (-1)

#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
#  define XWMP_SEM_API(api, ...) xwmp_rtsem_##api(__VA_ARGS__)
#elif defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
#  define XWMP_SEM_API(api, ...) xwmp_plsem_##api(__VA_ARGS__)
#endif

struct xwmp_evt;

/**
 * @brief 信号量类型枚举
 */
enum xwmp_sem_type_em {
        XWMP_SEM_TYPE_UNKNOWN = 0U,

#if defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
        XWMP_SEM_TYPE_PIPELINE = 1U, /**< 管道信号量：先进先出 */
#endif

#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
        XWMP_SEM_TYPE_RT = 2U, /**< 实时信号量：高优先级先出 */
#endif
};

/**
 * @brief 信号量对象
 */
struct xwmp_sem {
        struct xwmp_synobj synobj; /**< C语言面向对象：继承 `struct xwmp_synobj` */
        xwsq_t type; /**< 类型 */
        xwssq_t count; /**< 信号量计数器：<0，信号量处于负状态 */
        xwssq_t max; /**< 信号量计数器的最大值 */
        union {
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
                struct xwmp_rtwq rt; /**< 实时信号量的等待队列 */
#endif
#if defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
                struct xwmp_plwq pl; /**< 管道信号量的等待队列 */
#endif
        } wq; /**< 等待队列 */
};

#if defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
xwer_t xwmp_plsem_intr(struct xwmp_sem * sem, struct xwmp_wqn * wqn);
#endif

#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
xwer_t xwmp_rtsem_intr(struct xwmp_sem * sem, struct xwmp_wqn * wqn);
#endif

#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
xwer_t xwmp_sem_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
xwer_t xwmp_sem_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#elif defined(XWOSCFG_SYNC_SEM_SMA) && (1 == XWOSCFG_SYNC_SEM_SMA)
void xwmp_sem_cache_init(struct xwmm_sma * sma);
#endif

xwer_t xwmp_sem_fini(struct xwmp_sem * sem);
xwer_t xwmp_sem_grab(struct xwmp_sem * sem);
xwer_t xwmp_sem_put(struct xwmp_sem * sem);

xwer_t xwmp_sem_create(struct xwmp_sem ** sembuf, xwid_t type,
                       xwssq_t val, xwssq_t max);
xwer_t xwmp_sem_delete(struct xwmp_sem * sem, xwsq_t tik);
xwer_t xwmp_sem_acquire(struct xwmp_sem * sem, xwsq_t tik);
xwer_t xwmp_sem_release(struct xwmp_sem * sem, xwsq_t tik);

#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
xwer_t xwmp_sem_bind(struct xwmp_sem * sem, struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_sem_unbind(struct xwmp_sem * sem, struct xwmp_evt * evt);
#endif

#if defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
xwer_t xwmp_plsem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max);
xwer_t xwmp_plsem_create(struct xwmp_sem ** sembuf, xwssq_t val, xwssq_t max);
xwer_t xwmp_plsem_thaw(struct xwmp_sem * sem);
xwer_t xwmp_plsem_freeze(struct xwmp_sem * sem);
xwer_t xwmp_plsem_post(struct xwmp_sem * sem);
xwer_t xwmp_plsem_wait(struct xwmp_sem * sem);
xwer_t xwmp_plsem_wait_to(struct xwmp_sem * sem, xwtm_t xwtm);
xwer_t xwmp_plsem_wait_unintr(struct xwmp_sem * sem);
xwer_t xwmp_plsem_trywait(struct xwmp_sem * sem);
#endif

#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
xwer_t xwmp_rtsem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max);
xwer_t xwmp_rtsem_create(struct xwmp_sem ** sembuf, xwssq_t val, xwssq_t max);
xwer_t xwmp_rtsem_thaw(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_freeze(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_post(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_wait(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_wait_to(struct xwmp_sem * sem, xwtm_t xwtm);
xwer_t xwmp_rtsem_wait_unintr(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_trywait(struct xwmp_sem * sem);
#endif

xwer_t xwmp_sem_get_max(struct xwmp_sem * sem, xwssq_t * max);
xwer_t xwmp_sem_get_value(struct xwmp_sem * sem, xwssq_t * val);
xwer_t xwmp_sem_get_type(struct xwmp_sem * sem, xwid_t * type);

#endif /* xwos/mp/sync/sem.h */
