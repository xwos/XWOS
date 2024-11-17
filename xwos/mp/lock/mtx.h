/**
 * @file
 * @brief XWOS MP内核锁机制：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_lock_mtx_h__
#define __xwos_mp_lock_mtx_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
#  include <xwos/mm/sma.h>
#endif
#include <xwos/mp/rtwq.h>

struct xwmp_mtxtree;

/**
 * @brief 互斥锁
 * - 上锁的顺序:
 * -- waitqueue.lock
 * -- mtxtree.lock/thread.stlock
 */
struct xwmp_mtx {
        struct xwos_object xwobj; /**< C语言面向对象：继承 `struct xwos_object` */
        struct xwmp_mtxtree * ownertree; /**< 获得此互斥锁的线程的互斥锁树：
                                              如果为空(NULL)，互斥锁处于未加锁状态。
                                              此成员被锁rtwq.lock保护。*/
        xwsq_t reentrant; /**< 同一线程重复获得此互斥锁的计数器，
                               此成员被锁rtwq.lock保护。*/
        struct xwlib_rbtree_node rbnode; /**< 互斥锁树的链表节点：
                                              此成员被锁ownertree->lock保护。*/
        struct xwlib_bclst_node rbbuddy; /**< 互斥锁树的红黑树节点：
                                              此成员被锁ownertree->lock保护。*/

        struct xwmp_rtwq rtwq; /**< 实时等待队列：所有节点按优先级在队列中排队。*/
        xwpr_t sprio; /**< 静态优先级 */
        xwpr_t dprio; /**< 动态优先级：此成员被锁rtwq.lock保护。*/
};

xwer_t xwmp_mtx_intr(struct xwmp_mtx * mtx, struct xwmp_thd * thd);

#if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
xwer_t xwmp_mtx_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
xwer_t xwmp_mtx_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
void xwmp_mtx_cache_init(struct xwmm_sma * sma);
#endif

xwer_t xwmp_mtx_init(struct xwmp_mtx * mtx, xwpr_t sprio);
xwer_t xwmp_mtx_fini(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_create(struct xwmp_mtx ** mtxbuf, xwpr_t sprio);
xwer_t xwmp_mtx_delete(struct xwmp_mtx * mtx, xwsq_t tik);
xwer_t xwmp_mtx_acquire(struct xwmp_mtx * mtx, xwsq_t tik);
xwer_t xwmp_mtx_release(struct xwmp_mtx * mtx, xwsq_t tik);
xwer_t xwmp_mtx_grab(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_put(struct xwmp_mtx * mtx);
xwsq_t xwmp_mtx_get_tik(struct xwmp_mtx * mtx);

xwer_t xwmp_mtx_unlock(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_lock(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_lock_to(struct xwmp_mtx * mtx, xwtm_t to);
xwer_t xwmp_mtx_lock_unintr(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_trylock(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_get_status(struct xwmp_mtx * mtx, xwsq_t * status);

#endif /* xwos/mp/lock/mtx.h */
