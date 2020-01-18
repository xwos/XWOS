/**
 * @file
 * @brief XuanWuOS内核：中断底半部
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/bh.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化中断底半部控制块
 * @param bhcb: (I) 中断底半部控制块
 */
__xwos_api
void xwos_bh_cb_init(struct xwos_bh_cb * bhcb)
{
        xwlib_bclst_init_head(&bhcb->list);
        xwlk_splk_init(&bhcb->lock);
}

/**
 * @brief 初始化中断底半部链表节点
 * @param bhn: (I) 链表节点
 * @param func: (I) 中断底半部任务函数
 * @param arg: (I) 中断底半部任务函数的参数
 */
__xwos_api
void xwos_bh_node_init(struct xwos_bh_node * bhn, xwos_bh_f func, void * arg)
{
        xwlib_bclst_init_node(&bhn->node);
        bhn->func = func;
        bhn->arg = arg;
}

/**
 * @brief 将中断底半部链表节点加入链表
 * @param bhcb: (I) 中断底半部控制块
 * @param hbn: (I) 链表节点
 */
__xwos_api
void xwos_bh_node_eq(struct xwos_bh_cb * bhcb, struct xwos_bh_node * bhn)
{
        xwreg_t cpuirq;

        xwlk_rawly_lock_cpuirqsv(&bhcb->lock, &cpuirq);
        if (xwlib_bclst_tst_empty(&bhn->node)) {
                xwlib_bclst_add_tail(&bhcb->list, &bhn->node);
        }
        xwlk_rawly_unlock_cpuirqrs(&bhcb->lock, cpuirq);
}

/**
 * @brief 将中断底半部链表节点从链表中删除
 * @param bhcb: (I) 中断底半部控制块
 * @param hbn: (I) 链表节点
 */
__xwos_api
void xwos_bh_node_dq(struct xwos_bh_cb * bhcb, struct xwos_bh_node * bhn)
{
        xwreg_t cpuirq;

        xwlk_rawly_lock_cpuirqsv(&bhcb->lock, &cpuirq);
        xwlib_bclst_del_init(&bhn->node);
        xwlk_rawly_unlock_cpuirqrs(&bhcb->lock, cpuirq);
}
