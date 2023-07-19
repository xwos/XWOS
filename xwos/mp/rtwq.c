/**
 * @file
 * @brief XWOS MP内核：实时等待队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① xwmp_rtwq.lock
 *     + ② xwmp_wqn.lock
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/rtwq.h>
#include <xwos/mp/wqn.h>

static __xwmp_code
void xwmp_rtwq_rmrbb_locked(struct xwmp_rtwq * xwrtwq, struct xwmp_wqn * wqn);

static __xwmp_code
void xwmp_rtwq_rmrbn_locked(struct xwmp_rtwq * xwrtwq, struct xwmp_wqn * wqn);

/**
 * @brief 初始化等待队列
 * @param[in] xwrtwq: 实时等待队列
 */
__xwmp_code
void xwmp_rtwq_init(struct xwmp_rtwq * xwrtwq)
{
        xwrtwq->max_prio = XWMP_SKD_PRIORITY_INVALID;
        xwlib_rbtree_init(&xwrtwq->tree);
        xwrtwq->rightmost = NULL;
        xwmp_splk_init(&xwrtwq->lock);
}

/**
 * @brief 将等待队列节点加入到等待队列中
 * @param[in] xwrtwq: 实时等待队列
 * @param[in] wqn: 等待队列节点结构体指针
 * @param[in] prio: 优先级
 */
__xwmp_code
void xwmp_rtwq_add_locked(struct xwmp_rtwq * xwrtwq, struct xwmp_wqn * wqn,
                          xwpr_t prio)
{
        struct xwlib_rbtree_node ** new;
        struct xwlib_rbtree_node * rbn;
        xwptr_t lpc;
        struct xwmp_wqn * n;
        struct xwmp_wqn * rightmost;
        struct xwlib_rbtree * tree;
        xwpr_t nodeprio;

        tree = &xwrtwq->tree;
        wqn->prio = prio;
        new = &tree->root;
        lpc = (xwptr_t)new;
        n = NULL;
        rightmost = xwrtwq->rightmost;
        if (NULL == xwrtwq->rightmost) {
                xwrtwq->max_prio = prio;
                xwrtwq->rightmost = wqn;
        } else if (prio > xwrtwq->max_prio) {
                new = &(rightmost->rbn.right);
                lpc = (xwptr_t)new | (xwptr_t)XWLIB_RBTREE_POS_RIGHT;
                xwrtwq->max_prio = prio;
                xwrtwq->rightmost = wqn;
        } else if (prio == xwrtwq->max_prio) {
                lpc = (xwptr_t)0;
                n = rightmost;
        } else {
                rbn = *new;
                while (NULL != rbn) {
                        n = xwlib_rbtree_entry(rbn, struct xwmp_wqn, rbn);
                        nodeprio = n->prio;
                        if (prio < nodeprio) {
                                new = &rbn->left;
                                lpc = (xwptr_t)new;
                                rbn = rbn->left;
                        } else if (prio > nodeprio) {
                                new = &rbn->right;
                                lpc = (xwptr_t)new | (xwptr_t)XWLIB_RBTREE_POS_RIGHT;
                                rbn = rbn->right;
                        } else {
                                lpc = (xwptr_t)0;
                                break;
                        }
                }
        }
        if ((xwptr_t)0 != lpc) {
                xwlib_rbtree_link(&wqn->rbn, lpc);
                xwlib_rbtree_insert_color(tree, &wqn->rbn);
        } else {
                xwlib_bclst_add_tail(&n->cln.rbb, &wqn->cln.rbb);
        }
}

/**
 * @brief 将等待队列节点从实时等待队列中移除（节点有伙伴）
 * @param[in] xwrtwq: 实时等待队列
 * @param[in] wqn: 等待队列节点结构体指针
 * @note
 * - 这个函数只能在获得锁xwrtwq->lock时调用。
 */
static __xwmp_code
void xwmp_rtwq_rmrbb_locked(struct xwmp_rtwq * xwrtwq, struct xwmp_wqn * wqn)
{
        struct xwmp_wqn * n;

        n = xwlib_bclst_first_entry(&wqn->cln.rbb, struct xwmp_wqn, cln.rbb);
        xwlib_bclst_del_init(&wqn->cln.rbb);
        xwlib_rbtree_replace(&n->rbn, &wqn->rbn);
        xwlib_rbtree_init_node(&wqn->rbn);
        if (wqn == xwrtwq->rightmost) {
                xwrtwq->rightmost = n;
        }
}

/**
 * @brief 将等待队列节点从实时等待队列中移除（节点无伙伴）
 * @param[in] xwrtwq: 实时等待队列
 * @param[in] wqn: 等待队列节点结构体指针
 * @note
 * - 这个函数只能在获得锁xwrtwq->lock时调用。
 */
static __xwmp_code
void xwmp_rtwq_rmrbn_locked(struct xwmp_rtwq * xwrtwq, struct xwmp_wqn * wqn)
{
        struct xwlib_rbtree_node * p;

        if (wqn == xwrtwq->rightmost) {
                p = wqn->rbn.left; /* The predecessor of a max node is its
                                      left child due to the property 5 of
                                      red-black tree. Or if there is no
                                      left child, the predecessor is its
                                      parent. */
                if (NULL == p) {
                        p = xwlib_rbtree_get_parent(&wqn->rbn);
                }
                if (p != (struct xwlib_rbtree_node *)&xwrtwq->tree.root) {
                        xwrtwq->rightmost = xwlib_rbtree_entry(p, struct xwmp_wqn,
                                                               rbn);
                        xwrtwq->max_prio = xwrtwq->rightmost->prio;
                } else {
                        xwrtwq->rightmost = NULL;
                        xwrtwq->max_prio = XWMP_SKD_PRIORITY_INVALID;
                }
        }
        xwlib_rbtree_remove(&xwrtwq->tree, &wqn->rbn);
        xwlib_rbtree_init_node(&wqn->rbn);
}

/**
 * @brief 将等待队列节点从实时等待队列中移除
 * @param[in] xwrtwq: 实时等待队列
 * @param[in] wqn: 等待队列节点结构体指针
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 等待队列中不存在该节点
 * @note
 * - 这个函数只能在获得锁xwrtwq->lock时调用。
 */
__xwmp_code
xwer_t xwmp_rtwq_remove_locked(struct xwmp_rtwq * xwrtwq, struct xwmp_wqn * wqn)
{
        xwer_t rc;

        if ((NULL == wqn->cb) || (xwrtwq != wqn->wq)) {
                rc = -ESRCH;
        } else {
                if (xwlib_rbtree_tst_node_unlinked(&wqn->rbn)) {
                        xwlib_bclst_del_init(&wqn->cln.rbb);
                } else if (!xwlib_bclst_tst_empty(&wqn->cln.rbb)) {
                        xwmp_rtwq_rmrbb_locked(xwrtwq, wqn);
                } else {
                        xwmp_rtwq_rmrbn_locked(xwrtwq, wqn);
                }
                wqn->prio = XWMP_SKD_PRIORITY_INVALID;
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 从实时等待队列中选择一个节点
 * @param[in] xwrtwq: 实时等待队列
 * @return 实时等待队列节点指针或空指针
 * @note
 * - 这个函数只能在获得锁xwrtwq->lock时调用。
 */
__xwmp_code
struct xwmp_wqn * xwmp_rtwq_choose_locked(struct xwmp_rtwq * xwrtwq)
{
        struct xwmp_wqn * rightmost;

        rightmost = xwrtwq->rightmost;
        if (NULL != rightmost) {
                xwmp_splk_lock(&rightmost->lock);
                if (!xwlib_bclst_tst_empty(&rightmost->cln.rbb)) {
                        xwmp_rtwq_rmrbb_locked(xwrtwq, rightmost);
                } else {
                        xwmp_rtwq_rmrbn_locked(xwrtwq, rightmost);
                }
        }
        return rightmost;
}
