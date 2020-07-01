/**
 * @file
 * @brief XuanWuOS内核：互斥锁树
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/lock/mutex.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/thread.h>
#include <xwos/up/mutextree.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化互斥锁树
 * @param mt: (I) 互斥锁树
 */
__xwos_code
void xwos_mtxtree_init(struct xwos_mtxtree * mt)
{
        xwlib_rbtree_init(&mt->rbtree);
        mt->rightmost = NULL;
        mt->maxprio = XWOS_SD_PRIORITY_INVALID;
}

/**
 * @brief 将互斥锁加入到互斥锁树
 * @param mtx: (I) 互斥锁
 * @param mt: (I) 互斥锁树
 * @note
 * - 此函数只能在临界区中调用。
 */
__xwos_code
void xwos_mtxtree_add(struct xwos_mtxtree * mt, struct xwlk_mtx * mtx)
{
        struct xwlib_rbtree_node ** pos;
        struct xwlib_rbtree_node * rbn;
        struct xwlk_mtx * m, * max;
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
                lpc = (xwptr_t)pos | XWLIB_RBTREE_POS_RIGHT;
                mt->maxprio = prio;
                mt->rightmost = mtx;
        } else if (prio == mt->maxprio) {
                m = max;
                lpc = (xwptr_t)0;
        } else {
                rbn = *pos;
                while (rbn) {
                        m = xwlib_rbtree_entry(rbn, struct xwlk_mtx, rbnode);
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
 * @brief 将互斥锁从互斥锁树中删除
 * @param mtx: (I) 互斥锁
 * @param mt: (I) 互斥锁树
 * @note
 * - 此函数只能在临界区中调用。
 */
__xwos_code
void xwos_mtxtree_remove(struct xwos_mtxtree * mt, struct xwlk_mtx * mtx)
{
        struct xwlk_mtx * n;
        struct xwlib_rbtree_node * p;
        struct xwlib_rbtree * tree;

        tree = &mt->rbtree;
        if (xwlib_rbtree_tst_node_unlinked(&mtx->rbnode)) {
                xwlib_bclst_del_init(&mtx->rbbuddy);
        } else if (!xwlib_bclst_tst_empty(&mtx->rbbuddy)) {
                n = xwlib_bclst_first_entry(&mtx->rbbuddy, struct xwlk_mtx,
                                            rbbuddy);
                xwlib_bclst_del_init(&mtx->rbbuddy);
                xwlib_rbtree_replace(&n->rbnode, &mtx->rbnode);
                xwlib_rbtree_init_node(&mtx->rbnode);
                if (mtx == mt->rightmost) {
                        mt->rightmost = n;
                }/* else {} */
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
                                                                   struct xwlk_mtx,
                                                                   rbnode);
                                mt->maxprio = mt->rightmost->dprio;
                        } else {
                                mt->rightmost = NULL;
                                mt->maxprio = XWOS_SD_PRIORITY_INVALID;
                        }
                }
                xwlib_rbtree_remove(tree, &mtx->rbnode);
                xwlib_rbtree_init_node(&mtx->rbnode);
        }
        mtx->ownertree = NULL;
}
