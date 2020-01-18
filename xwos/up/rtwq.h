/**
 * @file
 * @brief XuanWuOS内核：实时等待队列
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_rtwq_h__
#define __xwos_up_rtwq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/wqn.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_wqn;

/**
 * @brief 实时等待队列
 */
struct xwos_rtwq {
        struct xwlib_rbtree tree; /**< 红黑树（关键字：优先级） */
        xwpr_t max_prio; /**< 红黑树中最大关键字 */
        struct xwos_wqn * rightmost; /**< 拥有最大关键字的节点 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 以删除为目的，遍历实时等待队列中所有节点
 * @param c: (I) 作为循环光标的等待队列节点结构体(struct xwos_wqn)指针
 * @param xwrtwq: (I) 等待队列的指针
 */
#define xwos_rtwq_itr_thrd_rm(c, xwrtwq) \
        for (c = (xwrtwq)->rightmost; c; c = (xwrtwq)->rightmost)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwos_rtwq_init(struct xwos_rtwq * xwrtwq);

__xwos_code
void xwos_rtwq_add(struct xwos_rtwq * xwrtwq, struct xwos_wqn * wqn, xwpr_t prio);

__xwos_code
xwer_t xwos_rtwq_remove(struct xwos_rtwq * xwrtwq, struct xwos_wqn * wqn);

__xwos_code
struct xwos_wqn * xwos_rtwq_choose(struct xwos_rtwq * xwrtwq);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/up/rtwq.h */
