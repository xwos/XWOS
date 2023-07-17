/**
 * @file
 * @brief XWOS UP内核：等待队列节点
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_wqn_h__
#define __xwos_up_wqn_h__

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/bclst.h>
#include <xwos/up/skd.h>

/**
 * @brief 等待队列类型枚举
 */
enum xwup_wqtype_em {
        XWUP_WQTYPE_UNKNOWN = 0,
        XWUP_WQTYPE_NULL,
        XWUP_WQTYPE_PLSEM,
        XWUP_WQTYPE_RTSEM,
        XWUP_WQTYPE_COND,
        XWUP_WQTYPE_MTX,
};

/**
 * @brief 等待队列节点唤醒原因枚举
 */
enum xwup_wqn_reason_em {
        XWUP_WQN_REASON_UNKNOWN = 0,
        XWUP_WQN_REASON_UP, /**< 资源可用 */
        XWUP_WQN_REASON_INTR, /**< 等待被中断 */
};

struct xwup_wqn;

/**
 * @brief 等待队列节点回调函数指针类型
 */
typedef void (* xwup_wqn_f)(struct xwup_wqn *);

/**
 * @brief 等待队列节点
 */
struct xwup_wqn {
        xwu16_t type; /**< 等待队列类型（信号量、互斥锁） */
        volatile xwu16_t reason; /**< 唤醒原因 */
        void * wq; /**< 指向所属的等待队列的指针 */
        xwup_wqn_f cb; /**< 被唤醒时的回调函数 */
        xwpr_t prio; /**< 优先级 */
        struct xwlib_rbtree_node rbn; /**> 红黑树节点 */
        union {
                struct xwlib_bclst_node rbb; /**< 当等待队列为 `rtwq` 时的链表节点 */
                struct xwlib_bclst_node pl; /**< 当等待队列为 `plwq` 时的链表节点 */
        } cln; /**< 双循环链表节点 */
};

void xwup_wqn_init(struct xwup_wqn * wqn);

#endif /* xwos/up/wqn.h */
