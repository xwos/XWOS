/**
 * @file
 * @brief 玄武OS MP内核同步机制：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_sync_sem_h__
#define __xwos_mp_sync_sem_h__

#include <xwos/standard.h>
#include <xwos/mp/rtwq.h>
#include <xwos/mp/plwq.h>
#include <xwos/mp/sync/obj.h>

/**
 * @brief 负信号量的值
 */
#define XWMP_SEM_NEGTIVE              ((xwssq_t)(-1))

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
#  define XWMP_SEM_API(api, ...) xwmp_rtsem_##api(__VA_ARGS__)
#elif defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
#  define XWMP_SEM_API(api, ...) xwmp_plsem_##api(__VA_ARGS__)
#endif

struct xwmp_evt;

/**
 * @brief 信号量类型枚举
 */
enum xwmp_sem_type_em {
        XWMP_SEM_TYPE_UNKNOWN = 0,

#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
        XWMP_SEM_TYPE_PIPELINE, /**< 管道信号量：先进先出 */
#endif

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
        XWMP_SEM_TYPE_RT, /**< 实时信号量：高优先级先出 */
#endif
};

/**
 * @brief 信号量对象
 */
struct xwmp_sem {
        struct xwmp_synobj synobj; /**< C语言面向对象：继承struct xwmp_synobj */
        xwid_t type; /**< 类型 */
        xwssq_t count; /**< 信号量计数器：<0，信号量处于负状态 */
        xwssq_t max; /**< 信号量计数器的最大值 */
        union {
#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
                struct xwmp_rtwq rt; /**< 实时信号量的等待队列 */
#endif
#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
                struct xwmp_plwq pl; /**< 管道信号量的等待队列 */
#endif
        } wq; /**< 等待队列 */
};

#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
xwer_t xwmp_plsem_intr(struct xwmp_sem * sem, struct xwmp_wqn * wqn);
#endif

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
xwer_t xwmp_rtsem_intr(struct xwmp_sem * sem, struct xwmp_wqn * wqn);
#endif

#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
xwer_t xwmp_sem_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif

xwer_t xwmp_sem_create(struct xwmp_sem ** ptrbuf, xwid_t type,
                       xwssq_t val, xwssq_t max);
xwer_t xwmp_sem_delete(struct xwmp_sem * sem);
xwer_t xwmp_sem_fini(struct xwmp_sem * sem);

xwer_t xwmp_sem_acquire(struct xwmp_sem * sem, xwsq_t tik);
xwer_t xwmp_sem_release(struct xwmp_sem * sem, xwsq_t tik);
xwer_t xwmp_sem_grab(struct xwmp_sem * sem);
xwer_t xwmp_sem_put(struct xwmp_sem * sem);

#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
xwer_t xwmp_sem_bind(struct xwmp_sem * sem, struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_sem_unbind(struct xwmp_sem * sem, struct xwmp_evt * evt);
#endif

#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
xwer_t xwmp_plsem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max);
xwer_t xwmp_plsem_thaw(struct xwmp_sem * sem);
xwer_t xwmp_plsem_freeze(struct xwmp_sem * sem);
xwer_t xwmp_plsem_post(struct xwmp_sem * sem);
xwer_t xwmp_plsem_wait(struct xwmp_sem * sem);
xwer_t xwmp_plsem_trywait(struct xwmp_sem * sem);
xwer_t xwmp_plsem_timedwait(struct xwmp_sem * sem, xwtm_t * xwtm);
xwer_t xwmp_plsem_wait_unintr(struct xwmp_sem * sem);
#endif

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
xwer_t xwmp_rtsem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max);
xwer_t xwmp_rtsem_thaw(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_freeze(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_post(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_wait(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_trywait(struct xwmp_sem * sem);
xwer_t xwmp_rtsem_timedwait(struct xwmp_sem * sem, xwtm_t * xwtm);
xwer_t xwmp_rtsem_wait_unintr(struct xwmp_sem * sem);
#endif

xwer_t xwmp_sem_getvalue(struct xwmp_sem * sem, xwssq_t * sval);
xwer_t xwmp_sem_gettype(struct xwmp_sem * sem, xwid_t * type);

#endif /* xwos/mp/sync/sem.h */
