/**
 * @file
 * @brief 玄武OS UP内核：实时等待队列
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/skd.h>
#include <xwos/up/wqn.h>
#include <xwos/up/rtwq.h>

static __xwup_code
void xwup_rtwq_rmrbb(struct xwup_rtwq * xwrtwq, struct xwup_wqn * wqn);

static __xwup_code
void xwup_rtwq_rmrbn(struct xwup_rtwq * xwrtwq, struct xwup_wqn * wqn);

/**
 * @brief 初始化等待队列
 * @param xwrtwq: (I) 实时等待队列
 */
__xwup_code
void xwup_rtwq_init(struct xwup_rtwq * xwrtwq)
{
        xwrtwq->max_prio = XWUP_SKD_PRIORITY_INVALID;
        xwlib_rbtree_init(&xwrtwq->tree);
        xwrtwq->rightmost = NULL;
}

/**
 * @brief 将等待队列节点加入到等待队列中
 * @param xwrtwq: (I) 实时等待队列
 * @param wqn: (I) 等待队列节点结构体指针
 * @param prio: (I) 优先级
 */
__xwup_code
void xwup_rtwq_add(struct xwup_rtwq * xwrtwq, struct xwup_wqn * wqn, xwpr_t prio)
{
        struct xwlib_rbtree_node ** new;
        struct xwlib_rbtree_node * rbn;
        xwptr_t lpc;
        struct xwup_wqn * n, * rightmost;
        struct xwlib_rbtree * tree;
        xwpr_t nprio;

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
                lpc = (xwptr_t)new | XWLIB_RBTREE_POS_RIGHT;
                xwrtwq->max_prio = prio;
                xwrtwq->rightmost = wqn;
        } else if (prio == xwrtwq->max_prio) {
                lpc = (xwptr_t)0;
                n = rightmost;
        } else {
                rbn = *new;
                while (rbn) {
                        n = xwlib_rbtree_entry(rbn, struct xwup_wqn, rbn);
                        nprio = n->prio;
                        if (prio < nprio) {
                                new = &rbn->left;
                                lpc = (xwptr_t)new;
                                rbn = rbn->left;
                        } else if (prio > nprio) {
                                new = &rbn->right;
                                lpc = (xwptr_t)new | XWLIB_RBTREE_POS_RIGHT;
                                rbn = rbn->right;
                        } else {
                                lpc = (xwptr_t)0;
                                break;
                        }
                }
        }
        if (lpc) {
                xwlib_rbtree_link(&wqn->rbn, lpc);
                xwlib_rbtree_insert_color(tree, &wqn->rbn);
        } else {
                xwlib_bclst_add_tail(&n->cln.rbb, &wqn->cln.rbb);
        }
}

/**
 * @brief 将等待队列节点从实时等待队列中移除（节点有伙伴）
 * @param xwrtwq: (I) 实时等待队列
 * @param wqn: (I) 等待队列节点结构体指针
 * @note
 * - 这个函数只能在临界区中调用。
 */
static __xwup_code
void xwup_rtwq_rmrbb(struct xwup_rtwq * xwrtwq, struct xwup_wqn * wqn)
{
        struct xwup_wqn * n;

        n = xwlib_bclst_first_entry(&wqn->cln.rbb, struct xwup_wqn, cln.rbb);
        xwlib_bclst_del_init(&wqn->cln.rbb);
        xwlib_rbtree_replace(&n->rbn, &wqn->rbn);
        xwlib_rbtree_init_node(&wqn->rbn);
        if (wqn == xwrtwq->rightmost) {
                xwrtwq->rightmost = n;
        }/* else {} */
}

/**
 * @brief 将等待队列节点从实时等待队列中移除（节点无伙伴）
 * @param xwrtwq: (I) 实时等待队列
 * @param wqn: (I) 等待队列节点结构体指针
 * @note
 * - 这个函数只能在临界区中调用。
 */
static __xwup_code
void xwup_rtwq_rmrbn(struct xwup_rtwq * xwrtwq, struct xwup_wqn * wqn)
{
        struct xwlib_rbtree_node * p;

        if (wqn == xwrtwq->rightmost) {
                p = wqn->rbn.left; /* The predecessor of a max node is its
                                      left child due to the property 5 of
                                      red-black tree. Or if there is no
                                      left child, the predecessor is its
                                      parent. */
                if (!p) {
                        p = xwlib_rbtree_get_parent(&wqn->rbn);
                }/* else {} */
                if (p != (struct xwlib_rbtree_node *)&xwrtwq->tree.root) {
                        xwrtwq->rightmost = xwlib_rbtree_entry(p,
                                                               struct xwup_wqn,
                                                               rbn);
                        xwrtwq->max_prio = xwrtwq->rightmost->prio;
                } else {
                        xwrtwq->rightmost = NULL;
                        xwrtwq->max_prio = XWUP_SKD_PRIORITY_INVALID;
                }
        }
        xwlib_rbtree_remove(&xwrtwq->tree, &wqn->rbn);
        xwlib_rbtree_init_node(&wqn->rbn);
}

/**
 * @brief 将等待队列节点从实时等待队列中移除
 * @param xwrtwq: (I) 实时等待队列
 * @param wqn: (I) 等待队列节点结构体指针
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 等待队列中不存在该节点
 * @note
 * - 这个函数只能在临界区中调用。
 */
__xwup_code
xwer_t xwup_rtwq_remove(struct xwup_rtwq * xwrtwq, struct xwup_wqn * wqn)
{
        xwer_t rc;

        if (__xwcc_unlikely((NULL == wqn->cb) || (xwrtwq != wqn->wq))) {
                rc = -ESRCH;
        } else {
                if (xwlib_rbtree_tst_node_unlinked(&wqn->rbn)) {
                        xwlib_bclst_del_init(&wqn->cln.rbb);
                } else if (!xwlib_bclst_tst_empty(&wqn->cln.rbb)) {
                        xwup_rtwq_rmrbb(xwrtwq, wqn);
                } else {
                        xwup_rtwq_rmrbn(xwrtwq, wqn);
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 从实时等待队列中选择一个节点
 * @param xwrtwq: (I) 实时等待队列
 * @return 实时等待队列节点指针或空指针
 * @note
 * - 这个函数只能在临界区中调用。
 */
__xwup_code
struct xwup_wqn * xwup_rtwq_choose(struct xwup_rtwq * xwrtwq)
{
        struct xwup_wqn * rightmost;

        rightmost = xwrtwq->rightmost;
        if (NULL != rightmost) {
                if (!xwlib_bclst_tst_empty(&rightmost->cln.rbb)) {
                        xwup_rtwq_rmrbb(xwrtwq, rightmost);
                } else {
                        xwup_rtwq_rmrbn(xwrtwq, rightmost);
                }
        }
        return rightmost;
}
