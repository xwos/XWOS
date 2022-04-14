/**
 * @file
 * @brief 玄武OS MP内核：互斥锁树
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - 锁的顺序：
 *   + ① xwmp_mtx.rtwq.lock
 *     + ② xwmp_mtxtree.lock
 */

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mp/lock/seqlock.h>
#include <xwos/mp/lock/mtx.h>
#include <xwos/mp/mtxtree.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/thd.h>

/**
 * @brief 初始化互斥锁树
 * @param[in] mt: 互斥锁树
 */
__xwmp_code
void xwmp_mtxtree_init(struct xwmp_mtxtree * mt)
{
        xwlib_rbtree_init(&mt->rbtree);
        mt->rightmost = NULL;
        mt->maxprio = XWMP_SKD_PRIORITY_INVALID;
        xwmp_sqlk_init(&mt->lock);
}

/**
 * @brief 将互斥锁加入到互斥锁树
 * @param[in] mtx: 互斥锁
 * @param[in] mt: 互斥锁树
 * @note
 * - 这个函数只能在获得锁mtx->rtwq.lock 与 mt->lock时调用。
 */
__xwmp_code
void xwmp_mtxtree_add_locked(struct xwmp_mtx * mtx, struct xwmp_mtxtree * mt)
{
        struct xwlib_rbtree_node ** pos;
        struct xwlib_rbtree_node * rbn;
        xwptr_t lpc;
        xwpr_t prio;
        struct xwmp_mtx * m, * max;
        struct xwlib_rbtree * tree = &mt->rbtree;

        prio = mtx->dprio;
        pos = &tree->root;
        lpc = (xwptr_t)pos;
        m = NULL;
        max = mt->rightmost;
        if (NULL == max) { /* rbtree is empty. */
                mt->maxprio = prio;
                mt->rightmost = mtx;
        } else if (prio > mt->maxprio) {
                pos = &(max->rbnode.right);
                lpc = (xwptr_t)pos | XWLIB_RBTREE_POS_RIGHT;
                mt->maxprio = prio;
                mt->rightmost = mtx;
        } else if (prio == mt->maxprio) {
                m = max;
                lpc = (xwptr_t)0;
        } else {
                rbn = *pos;
                while (rbn) {
                        m = xwlib_rbtree_entry(rbn, struct xwmp_mtx, rbnode);
                        if (prio < m->dprio) {
                                pos = &rbn->left;
                                lpc = (xwptr_t)pos;
                                rbn = rbn->left;
                        } else if (prio > m->dprio) {
                                pos = &rbn->right;
                                lpc = (xwptr_t)pos | XWLIB_RBTREE_POS_RIGHT;
                                rbn = rbn->right;
                        } else {
                                lpc = (xwptr_t)0;
                                break;
                        }
                }
        }
        if (lpc) {
                xwlib_rbtree_link(&mtx->rbnode, lpc);
                xwlib_rbtree_insert_color(tree, &mtx->rbnode);
        } else {
                xwlib_bclst_add_tail(&m->rbbuddy, &mtx->rbbuddy);
        }
}

/**
 * @brief 将互斥锁加入到互斥锁树
 * @param[in] mtx: 互斥锁
 * @param[in] mt: 互斥锁树
 * @note
 * - 这个函数只能在获得锁mtx->rtwq.lock时调用。
 */
__xwmp_code
void xwmp_mtxtree_add(struct xwmp_mtx * mtx, struct xwmp_mtxtree * mt)
{
        mtx->ownertree = mt;
        xwmp_sqlk_wr_lock(&mt->lock);
        xwmp_mtxtree_add_locked(mtx, mt);
        xwmp_sqlk_wr_unlock(&mt->lock);
}

/**
 * @brief 将互斥锁从互斥锁树中删除
 * @param[in] mtx: 互斥锁
 * @param[in] mt: 互斥锁树
 * @note
 * - 这个函数只能在获得锁mtx->rtwq.lock 与 mt->lock时调用。
 */
__xwmp_code
void xwmp_mtxtree_remove_locked(struct xwmp_mtx * mtx, struct xwmp_mtxtree * mt)
{
        struct xwmp_mtx * n;
        struct xwlib_rbtree_node * p;
        struct xwlib_rbtree * tree = &mt->rbtree;

        if (xwlib_rbtree_tst_node_unlinked(&mtx->rbnode)) {
                xwlib_bclst_del_init(&mtx->rbbuddy);
        } else if (!xwlib_bclst_tst_empty(&mtx->rbbuddy)) {
                n = xwlib_bclst_first_entry(&mtx->rbbuddy, struct xwmp_mtx,
                                            rbbuddy);
                xwlib_bclst_del_init(&mtx->rbbuddy);
                xwlib_rbtree_replace(&n->rbnode, &mtx->rbnode);
                xwlib_rbtree_init_node(&mtx->rbnode);
                if (mtx == mt->rightmost) {
                        mt->rightmost = n;
                }
        } else {
                if (mtx == mt->rightmost) {
                        p = mtx->rbnode.left; /* The predecessor of a max node is its
                                                 left child due to the property 5 of
                                                 red-black tree. Or if there is no
                                                 left child, the predecessor is its
                                                 parent. */
                        if (!p) {
                                p = xwlib_rbtree_get_parent(&mtx->rbnode);
                        }/* else {} */
                        if (p != (struct xwlib_rbtree_node *)&tree->root) {
                                mt->rightmost = xwlib_rbtree_entry(p,
                                                                   struct xwmp_mtx,
                                                                   rbnode);
                                mt->maxprio = mt->rightmost->dprio;
                        } else {
                                mt->rightmost = NULL;
                                mt->maxprio = XWMP_SKD_PRIORITY_INVALID;
                        }
                }
                xwlib_rbtree_remove(tree, &mtx->rbnode);
                xwlib_rbtree_init_node(&mtx->rbnode);
        }
}

/**
 * @brief 将互斥锁从互斥锁树中删除
 * @param[in] mtx: 互斥锁
 * @param[in] mt: 互斥锁树
 * @note
 * - 这个函数只能在获得锁mtx->rtwq.lock时调用。
 */
__xwmp_code
void xwmp_mtxtree_remove(struct xwmp_mtx * mtx, struct xwmp_mtxtree * mt)
{
        xwmp_sqlk_wr_lock(&mt->lock);
        xwmp_mtxtree_remove_locked(mtx, mt);
        xwmp_sqlk_wr_unlock(&mt->lock);
        mtx->ownertree = NULL;
}
