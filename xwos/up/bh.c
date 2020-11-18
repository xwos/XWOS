/**
 * @file
 * @brief 玄武OS UP内核：中断底半部
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#include <xwos/up/bh.h>

/**
 * @brief 初始化中断底半部控制块
 * @param bhcb: (I) 中断底半部控制块
 */
__xwup_code
void xwup_bh_cb_init(struct xwup_bh_cb * bhcb)
{
        XWOS_VALIDATE((NULL != bhcb), "nullptr");

        xwlib_bclst_init_head(&bhcb->list);
}

/**
 * @brief 初始化中断底半部链表节点
 * @param bhn: (I) 链表节点
 * @param func: (I) 中断底半部任务函数
 * @param arg: (I) 中断底半部任务函数的参数
 */
__xwup_code
void xwup_bh_node_init(struct xwup_bh_node * bhn, xwup_bh_f func, void * arg)
{
        XWOS_VALIDATE((NULL != bhn), "nullptr");
        XWOS_VALIDATE((NULL != func), "null-function");

        xwlib_bclst_init_node(&bhn->node);
        bhn->func = func;
        bhn->arg = arg;
}

/**
 * @brief 将中断底半部链表节点加入链表
 * @param bhcb: (I) 中断底半部控制块
 * @param hbn: (I) 链表节点
 */
__xwup_code
void xwup_bh_node_eq(struct xwup_bh_cb * bhcb, struct xwup_bh_node * bhn)
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((NULL != bhcb), "nullptr");
        XWOS_VALIDATE((NULL != bhn), "nullptr");

        xwospl_cpuirq_save_lc(&cpuirq);
        if (xwlib_bclst_tst_empty(&bhn->node)) {
                xwlib_bclst_add_tail(&bhcb->list, &bhn->node);
        }
        xwospl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief 将中断底半部链表节点从链表中删除
 * @param bhcb: (I) 中断底半部控制块
 * @param hbn: (I) 链表节点
 */
__xwup_code
void xwup_bh_node_dq(struct xwup_bh_cb * bhcb, struct xwup_bh_node * bhn)
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((NULL != bhn), "nullptr");
        XWOS_UNUSED(bhcb);

        xwospl_cpuirq_save_lc(&cpuirq);
        xwlib_bclst_del_init(&bhn->node);
        xwospl_cpuirq_restore_lc(cpuirq);
}
