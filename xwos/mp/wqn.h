/**
 * @file
 * @brief 玄武OS MP内核：等待队列节点
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - 锁的顺序：
 *   + ① xwmp_rtwq.lock
 *     + ② xwmp_wqn.lock
 */

#ifndef __xwos_mp_wqn_h__
#define __xwos_mp_wqn_h__

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/lock/spinlock.h>

/**
 * @brief 等待队列类型枚举
 */
enum xwmp_wqtype_em {
        XWMP_WQTYPE_UNKNOWN = 0,
        XWMP_WQTYPE_NULL,
        XWMP_WQTYPE_PLSEM,
        XWMP_WQTYPE_RTSEM,
        XWMP_WQTYPE_COND,
        XWMP_WQTYPE_EVENT,
        XWMP_WQTYPE_MTX,
};

/**
 * @brief 等待队列节点所属对象枚举
 */
enum xwmp_wqowner_em {
        XWMP_WQOWNER_UNKNOWN = 0,
        XWMP_WQOWNER_THD,
};

/**
 * @brief 等待队列节点唤醒原因枚举
 */
enum xwmp_wqn_reason_em {
        XWMP_WQN_REASON_UNKNOWN = 0,
        XWMP_WQN_REASON_UP, /**< 资源可用 */
        XWMP_WQN_REASON_INTR, /**< 等待被中断 */
};

/**
 * @brief 等待队列节点回调函数指针类型
 */
typedef void (* xwmp_wqn_f)(void *);

/**
 * @brief 等待队列节点
 */
struct xwmp_wqn {
        xwsq_t type; /**< 等待队列类型（信号量、互斥锁） */
        atomic_xwsq_t reason; /**< 唤醒原因 */
        void * wq; /**< 指向所属的等待队列的指针 */
        xwmp_wqn_f cb; /**< 被唤醒时的回调函数 */
        struct xwmp_splk lock; /**< 保护此结构体的锁 */
        void * owner; /**< 拥有结构体的对象的指针 */
        xwpr_t prio; /**< 优先级 */
        struct xwlib_rbtree_node rbn; /**> 红黑树节点 */
        union {
                struct xwlib_bclst_node rbb;
                struct xwlib_bclst_node pl;
        } cln; /**< 双循环链表节点 */
};

void xwmp_wqn_init(struct xwmp_wqn * wqn, void * owner);

#endif /* xwmp/mp/wqn.h */
