/**
 * @file
 * @brief 玄武OS MP内核：中断底半部
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/bh.h>

/**
 * @brief 初始化中断底半部控制块
 * @param[in] bhcb: 中断底半部控制块
 */
__xwmp_code
void xwmp_bh_cb_init(struct xwmp_bh_cb * bhcb)
{
        xwlib_bclst_init_head(&bhcb->list);
        xwmp_splk_init(&bhcb->lock);
}

/**
 * @brief 初始化中断底半部链表节点
 * @param[in] bhn: 链表节点
 * @param[in] func: 中断底半部任务函数
 * @param[in] arg: 中断底半部任务函数的参数
 */
__xwmp_code
void xwmp_bh_node_init(struct xwmp_bh_node * bhn, xwmp_bh_f func, void * arg)
{
        xwlib_bclst_init_node(&bhn->node);
        bhn->func = func;
        bhn->arg = arg;
}

/**
 * @brief 将中断底半部链表节点加入链表
 * @param[in] bhcb: 中断底半部控制块
 * @param[in] hbn: 链表节点
 */
__xwmp_code
void xwmp_bh_node_eq(struct xwmp_bh_cb * bhcb, struct xwmp_bh_node * bhn)
{
        xwreg_t cpuirq;

        xwmp_rawly_lock_cpuirqsv(&bhcb->lock, &cpuirq);
        if (xwlib_bclst_tst_empty(&bhn->node)) {
                xwlib_bclst_add_tail(&bhcb->list, &bhn->node);
        }
        xwmp_rawly_unlock_cpuirqrs(&bhcb->lock, cpuirq);
}

/**
 * @brief 将中断底半部链表节点从链表中删除
 * @param[in] bhcb: 中断底半部控制块
 * @param[in] hbn: 链表节点
 */
__xwmp_code
void xwmp_bh_node_dq(struct xwmp_bh_cb * bhcb, struct xwmp_bh_node * bhn)
{
        xwreg_t cpuirq;

        xwmp_rawly_lock_cpuirqsv(&bhcb->lock, &cpuirq);
        xwlib_bclst_del_init(&bhn->node);
        xwmp_rawly_unlock_cpuirqrs(&bhcb->lock, cpuirq);
}
