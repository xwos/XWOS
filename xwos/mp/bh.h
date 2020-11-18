/**
 * @file
 * @brief 玄武OS MP内核：中断底半部
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_bh_h__
#define __xwos_mp_bh_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/lock/spinlock.h>

/**
 * @brief 中断底半部控制块
 */
struct xwmp_bh_cb {
        struct xwlib_bclst_head list; /**< 链表头 */
        struct xwmp_splk lock; /**< 保护链表的自旋锁 */
};

/**
 * @brief 中断底半部任务函数类型
 */
typedef void (* xwmp_bh_f)(void *);

/**
 * @brief 中断底半部链表节点
 */
struct xwmp_bh_node {
        struct xwlib_bclst_node node; /**< 链表节点 */
        xwmp_bh_f func; /**< 中断底半部任务函数 */
        void * arg; /**< 中断底半部任务函数的参数 */
};

void xwmp_bh_cb_init(struct xwmp_bh_cb * bhcb);
void xwmp_bh_node_init(struct xwmp_bh_node * bhn, xwmp_bh_f func, void * arg);
void xwmp_bh_node_eq(struct xwmp_bh_cb * bhcb, struct xwmp_bh_node * bhn);
void xwmp_bh_node_dq(struct xwmp_bh_cb * bhcb, struct xwmp_bh_node * bhn);

#endif /* xwos/mp/bh.h */
