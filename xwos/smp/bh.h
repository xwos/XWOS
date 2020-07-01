/**
 * @file
 * @brief XuanWuOS内核：中断底半部
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_bh_h__
#define __xwos_smp_bh_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/smp/lock/spinlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 中断底半部控制块
 */
struct xwos_bh_cb {
        struct xwlib_bclst_head list; /**< 链表头 */
        struct xwlk_splk lock; /**< 保护链表的自旋锁 */
};

/**
 * @brief 中断底半部任务函数类型
 */
typedef void (*xwos_bh_f)(void *);

/**
 * @brief 中断底半部链表节点
 */
struct xwos_bh_node {
        struct xwlib_bclst_node node; /**< 链表节点 */
        xwos_bh_f func; /**< 中断底半部任务函数 */
        void * arg; /**< 中断底半部任务函数的参数 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
void xwos_bh_cb_init(struct xwos_bh_cb * bhcb);

__xwos_api
void xwos_bh_node_init(struct xwos_bh_node * bhn, xwos_bh_f func, void * arg);

__xwos_api
void xwos_bh_node_eq(struct xwos_bh_cb * bhcb, struct xwos_bh_node * bhn);

__xwos_api
void xwos_bh_node_dq(struct xwos_bh_cb * bhcb, struct xwos_bh_node * bhn);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/smp/bh.h */
