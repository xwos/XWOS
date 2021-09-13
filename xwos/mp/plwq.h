/**
 * @file
 * @brief 玄武OS MP内核：管道等待队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_plwq_h__
#define __xwos_mp_plwq_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/wqn.h>

struct xwmp_thd;

/**
 * @brief 管道等待队列
 */
struct xwmp_plwq {
        struct xwlib_bclst_head head; /**< 链表头 */
        struct xwmp_splk lock; /**< 保护该结构体的锁 */
};

/**
 * @brief 安全地遍历管道等待队列中所有节点，以防其被删除
 * @param[in] c: 作为循环光标的等待队列节点结构体(struct xwmp_wqn)指针
 * @param[in] n: 作为临时变量的另一个等待队列节点结构体(struct xwmp_wqn)指针
 * @param[in] xwplwq: 管道等待队列的指针
 */
#define xwmp_plwq_itr_wqn_safe(c, n, xwplwq)                            \
        xwlib_bclst_itr_next_entry_safe(c, n, &((xwplwq)->head),        \
                                        struct xwmp_wqn,                \
                                        cln.pl)

/**
 * @brief 以删除为目的，遍历管道等待队列中所有节点
 * @param[in] c: 作为循环光标的等待队列节点结构体(struct xwmp_wqn)指针
 * @param[in] xwplwq: 管道等待队列的指针
 */
#define xwmp_plwq_itr_wqn_rm(c, xwplwq)                                 \
        xwlib_bclst_itr_next_entry_del(c, &((xwplwq)->head),            \
                                       struct xwmp_wqn,                 \
                                       cln.pl)

void xwmp_plwq_init(struct xwmp_plwq * xwplwq);
void xwmp_plwq_add_head_locked(struct xwmp_plwq * xwplwq, struct xwmp_wqn * wqn);
void xwmp_plwq_add_tail_locked(struct xwmp_plwq * xwplwq, struct xwmp_wqn * wqn);
xwer_t xwmp_plwq_remove_locked(struct xwmp_plwq * xwplwq, struct xwmp_wqn * wqn);
struct xwmp_wqn * xwmp_plwq_choose_locked(struct xwmp_plwq * xwplwq);

/**
 * @brief 锁住管道等待队列
 * @param[in] xwplwq: 管道等待队列结构体指针
 */
static __xwmp_inline
void xwmp_plwq_lock(struct xwmp_plwq * xwplwq)
{
        xwmp_splk_lock(&xwplwq->lock);
}

/**
 * @brief 解锁管道等待队列
 * @param[in] xwplwq: 管道等待队列结构体指针
 */
static __xwmp_inline
void xwmp_plwq_unlock(struct xwmp_plwq * xwplwq)
{
        xwmp_splk_unlock(&xwplwq->lock);
}

/**
 * @brief 锁住管道等待队列，并关闭本地CPU的中断
 * @param[in] xwplwq: 管道等待队列结构体指针
 */
static __xwmp_inline
void xwmp_plwq_lock_cpuirq(struct xwmp_plwq * xwplwq)
{
        xwmp_splk_lock_cpuirq(&xwplwq->lock);
}

/**
 * @brief 解锁管道等待队列，并开启本地CPU的中断
 * @param[in] xwplwq: 管道等待队列结构体指针
 */
static __xwmp_inline
void xwmp_plwq_unlock_cpuirq(struct xwmp_plwq * xwplwq)
{
        xwmp_splk_unlock_cpuirq(&xwplwq->lock);
}

/**
 * @brief 锁住管道等待队列，保存本地CPU的中断标志，并关闭
 * @param[in] xwplwq: 管道等待队列结构体指针
 * @param[out] cpuirq: 保存本地CPU的中断标志的缓冲区的指针
 */
static __xwmp_inline
void xwmp_plwq_lock_cpuirqsv(struct xwmp_plwq * xwplwq, xwreg_t * cpuirq)
{
        xwmp_splk_lock_cpuirqsv(&xwplwq->lock, cpuirq);
}

/**
 * @brief 解锁管道等待队列，并恢复本地CPU的中断标志
 * @param[in] xwplwq: 管道等待队列结构体指针
 * @param[in] cpuirq: 本地CPU的中断标志
 */
static __xwmp_inline
void xwmp_plwq_unlock_cpuirqrs(struct xwmp_plwq * xwplwq, xwreg_t cpuirq)
{
        xwmp_splk_unlock_cpuirqrs(&xwplwq->lock, cpuirq);
}

/**
 * @brief 检测管道等待队列是否为空
 * @return 布尔值
 * @retval true: 空
 * @retval false: 非空
 */
static __xwmp_inline
bool xwmp_plwq_check_empty(struct xwmp_plwq * xwplwq)
{
        return xwlib_bclst_tst_empty_carefully(&xwplwq->head);
}

#endif /* xwos/mp/plwq.h */
