/**
 * @file
 * @brief 玄武OS UP内核锁机制：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_lock_mtx_h__
#define __xwos_up_lock_mtx_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/rtwq.h>

struct xwup_mtxtree;

/**
 * @brief 互斥锁
 */
struct xwup_mtx {
        struct xwup_mtxtree * ownertree; /**< 获得此互斥锁的线程的互斥锁树：
                                              如果为空(NULL)，互斥锁处于未加锁状态。
                                              此成员被锁rtwq.lock保护。*/
        xwsq_t reentrant; /**< 同一线程重复获得此互斥锁的计数器，
                               此成员被锁rtwq.lock保护。*/
        struct xwlib_rbtree_node rbnode; /**< 互斥锁树的链表节点：
                                              此成员被锁ownertree->lock保护。*/
        struct xwlib_bclst_node rbbuddy; /**< 互斥锁树的红黑树节点：
                                              此成员被锁ownertree->lock保护。*/

        struct xwup_rtwq rtwq; /**< 实时等待队列：所有节点按优先级在队列中排队。*/
        xwpr_t sprio; /**< 静态优先级 */
        xwpr_t dprio; /**< 动态优先级：此成员被锁rtwq.lock保护。*/
};

xwer_t xwup_mtx_init(struct xwup_mtx * mtx, xwpr_t sprio);
xwer_t xwup_mtx_fini(struct xwup_mtx * mtx);
xwer_t xwup_mtx_create(struct xwup_mtx ** ptrbuf, xwpr_t sprio);
xwer_t xwup_mtx_delete(struct xwup_mtx * mtx);
xwer_t xwup_mtx_intr(struct xwup_mtx * mtx, struct xwup_thd * thd);
xwer_t xwup_mtx_unlock(struct xwup_mtx * mtx);
xwer_t xwup_mtx_lock(struct xwup_mtx * mtx);
xwer_t xwup_mtx_trylock(struct xwup_mtx * mtx);
xwer_t xwup_mtx_timedlock(struct xwup_mtx * mtx, xwtm_t * xwtm);
xwer_t xwup_mtx_lock_unintr(struct xwup_mtx * mtx);
xwer_t xwup_mtx_getlkst(struct xwup_mtx * mtx, xwsq_t * lkst);

#endif /* xwos/up/lock/mtx.h */
