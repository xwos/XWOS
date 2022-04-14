/**
 * @file
 * @brief 玄武OS UP内核：实时等待队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_rtwq_h__
#define __xwos_up_rtwq_h__

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/wqn.h>

struct xwup_wqn;

/**
 * @brief 实时等待队列
 */
struct xwup_rtwq {
        struct xwlib_rbtree tree; /**< 红黑树（关键字：优先级） */
        xwpr_t max_prio; /**< 红黑树中最大关键字 */
        struct xwup_wqn * rightmost; /**< 拥有最大关键字的节点 */
};

/**
 * @brief 以删除为目的，遍历实时等待队列中所有节点
 * @param[in] c: 作为循环光标的等待队列节点结构体(struct xwup_wqn)指针
 * @param[in] xwrtwq: 等待队列的指针
 */
#define xwup_rtwq_itr_thd_rm(c, xwrtwq) \
        for (c = (xwrtwq)->rightmost; c; c = (xwrtwq)->rightmost)

void xwup_rtwq_init(struct xwup_rtwq * xwrtwq);
void xwup_rtwq_add(struct xwup_rtwq * xwrtwq, struct xwup_wqn * wqn, xwpr_t prio);
xwer_t xwup_rtwq_remove(struct xwup_rtwq * xwrtwq, struct xwup_wqn * wqn);
struct xwup_wqn * xwup_rtwq_choose(struct xwup_rtwq * xwrtwq);

#endif /* xwos/up/rtwq.h */
