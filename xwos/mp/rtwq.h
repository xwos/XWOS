/**
 * @file
 * @brief XWOS MP内核：实时等待队列
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_rtwq_h__
#define __xwos_mp_rtwq_h__

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/wqn.h>

/**
 * @brief 以删除为目的，遍历实时等待队列中所有节点
 * @param[in] c: 循环变量，类型只能是 `struct xwmp_wqn *`
 * @param[in] xwrtwq: 等待队列的指针
 */
// cppcheck-suppress [misra-c2012-20.7]
#define xwmp_rtwq_itr_thd_rm(c, xwrtwq) \
        for (c = (xwrtwq)->rightmost; c; c = (xwrtwq)->rightmost)

/**
 * @brief 实时等待队列
 */
struct xwmp_rtwq {
        struct xwlib_rbtree tree; /**< 红黑树（关键字：优先级） */
        xwpr_t max_prio; /**< 红黑树中最大关键字 */
        struct xwmp_wqn * rightmost; /**< 拥有最大关键字的节点 */
        struct xwmp_splk lock; /* 保护结构体的锁 */
};

void xwmp_rtwq_init(struct xwmp_rtwq * xwrtwq);
void xwmp_rtwq_add_locked(struct xwmp_rtwq * xwrtwq, struct xwmp_wqn * wqn,
                          xwpr_t prio);
xwer_t xwmp_rtwq_remove_locked(struct xwmp_rtwq * xwrtwq, struct xwmp_wqn * wqn);
struct xwmp_wqn * xwmp_rtwq_choose_locked(struct xwmp_rtwq * xwrtwq);

/**
 * @brief 上锁等待队列
 * @param[in] xwrtwq: 实时等待队列
 */
static __xwmp_inline
void xwmp_rtwq_lock(struct xwmp_rtwq * xwrtwq)
{
        xwmp_splk_lock(&xwrtwq->lock);
}

/**
 * @brief 解锁等待队列
 * @param[in] xwrtwq: 实时等待队列
 */
static __xwmp_inline
void xwmp_rtwq_unlock(struct xwmp_rtwq * xwrtwq)
{
        xwmp_splk_unlock(&xwrtwq->lock);
}

/**
 * @brief 上锁等待队列，并关闭本地CPU的中断
 * @param[in] xwrtwq: 实时等待队列
 */
static __xwmp_inline
void xwmp_rtwq_lock_cpuirq(struct xwmp_rtwq * xwrtwq)
{
        xwmp_splk_lock_cpuirq(&xwrtwq->lock);
}

/**
 * @brief 解锁等待队列，并开启本地CPU的中断
 * @param[in] xwrtwq: 实时等待队列
 */
static __xwmp_inline
void xwmp_rtwq_unlock_cpuirq(struct xwmp_rtwq * xwrtwq)
{
        xwmp_splk_unlock_cpuirq(&xwrtwq->lock);
}

/**
 * @brief 上锁等待队列，保存本地CPU的中断标志并关闭
 * @param[in] xwrtwq: 实时等待队列
 */
static __xwmp_inline
void xwmp_rtwq_lock_cpuirqsv(struct xwmp_rtwq * xwrtwq, xwreg_t * cpuirq)
{
        xwmp_splk_lock_cpuirqsv(&xwrtwq->lock, cpuirq);
}

/**
 * @brief 解锁等待队列，恢复本地CPU的中断标志
 * @param[in] xwrtwq: 实时等待队列
 */
static __xwmp_inline
void xwmp_rtwq_unlock_cpuirqrs(struct xwmp_rtwq * xwrtwq, xwreg_t cpuirq)
{
        xwmp_splk_unlock_cpuirqrs(&xwrtwq->lock, cpuirq);
}

/**
 * @brief 检测等待队列是否为空
 * @param[in] xwrtwq: 实时等待队列
 * @return 布尔值
 * @retval true: 空
 * @retval false: 非空
 */
static __xwmp_inline
bool xwmp_rtwq_check_empty(struct xwmp_rtwq * xwrtwq)
{
        return (!xwrtwq->rightmost);
}

#endif /* xwos/mp/rtwq.h */
