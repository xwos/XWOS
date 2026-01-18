/**
 * @file
 * @brief XWOS UP内核：互斥锁树
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/lock/mtx.h>
#include <xwos/up/skd.h>
#include <xwos/up/thd.h>
#include <xwos/up/mtxtree.h>

/**
 * @brief 初始化互斥锁树
 * @param[in] mt: 互斥锁树
 */
__xwup_code
void xwup_mtxtree_init(struct xwup_mtxtree * mt)
{
        xwlib_rbtree_init(&mt->rbtree);
        mt->rightmost = NULL;
        mt->maxprio = XWUP_SKD_PRIORITY_INVALID;
}

/**
 * @brief 将互斥锁加入到互斥锁树
 * @param[in] mtx: 互斥锁
 * @param[in] mt: 互斥锁树
 * @note
 * - 此函数只能在临界区中调用。
 */
__xwup_code
void xwup_mtxtree_add(struct xwup_mtxtree * mt, struct xwup_mtx * mtx)
{
        struct xwlib_rbtree_node ** pos;
        struct xwlib_rbtree_node * rbn;
        struct xwup_mtx * m;
        struct xwup_mtx * max;
        struct xwlib_rbtree * tree;
        xwptr_t lpc;
        xwpr_t prio;

        mtx->ownertree = mt;
        tree = &mt->rbtree;
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
                lpc = (xwptr_t)pos | (xwptr_t)XWLIB_RBTREE_POS_RIGHT;
                mt->maxprio = prio;
                mt->rightmost = mtx;
        } else if (prio == mt->maxprio) {
                m = max;
                lpc = (xwptr_t)0;
        } else {
                rbn = *pos;
                while (NULL != rbn) {
                        m = xwlib_rbtree_entry(rbn, struct xwup_mtx, rbnode);
                        if (prio < m->dprio) {
                                pos = &rbn->left;
                                lpc = (xwptr_t)pos;
                                rbn = rbn->left;
                        } else if (prio > m->dprio) {
                                pos = &rbn->right;
                                lpc = (xwptr_t)pos | (xwptr_t)XWLIB_RBTREE_POS_RIGHT;
                                rbn = rbn->right;
                        } else {
                                lpc = (xwptr_t)0;
                                break;
                        }
                }
        }
        if ((xwptr_t)0 != lpc) {
                xwlib_rbtree_link(&mtx->rbnode, lpc);
                xwlib_rbtree_insert_color(tree, &mtx->rbnode);
        } else {
                xwlib_bclst_add_tail(&m->rbbuddy, &mtx->rbbuddy);
        }
}

/**
 * @brief 将互斥锁从互斥锁树中删除
 * @param[in] mtx: 互斥锁
 * @param[in] mt: 互斥锁树
 * @note
 * - 此函数只能在临界区中调用。
 */
__xwup_code
void xwup_mtxtree_remove(struct xwup_mtxtree * mt, struct xwup_mtx * mtx)
{
        struct xwup_mtx * n;
        struct xwlib_rbtree_node * p;
        struct xwlib_rbtree * tree;

        tree = &mt->rbtree;
        if (xwlib_rbtree_tst_node_unlinked(&mtx->rbnode)) {
                xwlib_bclst_del_init(&mtx->rbbuddy);
        } else if (!xwlib_bclst_tst_empty(&mtx->rbbuddy)) {
                n = xwlib_bclst_first_entry(&mtx->rbbuddy, struct xwup_mtx,
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
                        if (NULL == p) {
                                p = xwlib_rbtree_get_parent(&mtx->rbnode);
                        }
                        if (p != (struct xwlib_rbtree_node *)&tree->root) {
                                mt->rightmost = xwlib_rbtree_entry(p,
                                                                   struct xwup_mtx,
                                                                   rbnode);
                                mt->maxprio = mt->rightmost->dprio;
                        } else {
                                mt->rightmost = NULL;
                                mt->maxprio = XWUP_SKD_PRIORITY_INVALID;
                        }
                }
                xwlib_rbtree_remove(tree, &mtx->rbnode);
                xwlib_rbtree_init_node(&mtx->rbnode);
        }
        mtx->ownertree = NULL;
}
