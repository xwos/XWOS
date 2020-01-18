/**
 * @file
 * @brief XuanWuOS内核：管道等待队列
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_plwq_h__
#define __xwos_smp_plwq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/wqn.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_tcb;

/**
 * @brief 管道等待队列
 */
struct xwos_plwq {
        struct xwlib_bclst_head head; /**< 链表头 */
        struct xwlk_splk lock; /**< 保护该结构体的锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 安全地遍历管道等待队列中所有节点，以防其被删除
 * @param c: (I) 作为循环光标的等待队列节点结构体(struct xwos_wqn)指针
 * @param n: (I) 作为临时变量的另一个等待队列节点结构体(struct xwos_wqn)指针
 * @param xwplwq: (I) 管道等待队列的指针
 */
#define xwos_plwq_itr_wqn_safe(c, n, xwplwq)                            \
        xwlib_bclst_itr_next_entry_safe(c, n, &((xwplwq)->head),        \
                                        struct xwos_wqn,                \
                                        cln.pl)

/**
 * @brief 以删除为目的，遍历管道等待队列中所有节点
 * @param c: (I) 作为循环光标的等待队列节点结构体(struct xwos_wqn)指针
 * @param xwplwq: (I) 管道等待队列的指针
 */
#define xwos_plwq_itr_wqn_rm(c, xwplwq)                                 \
        xwlib_bclst_itr_next_entry_del(c, &((xwplwq)->head),            \
                                       struct xwos_wqn,                 \
                                       cln.pl)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwos_plwq_init(struct xwos_plwq * xwplwq);

__xwos_code
void xwos_plwq_add_head_locked(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn);

__xwos_code
void xwos_plwq_add_tail_locked(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn);

__xwos_code
xwer_t xwos_plwq_remove_locked(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn);

__xwos_code
struct xwos_wqn * xwos_plwq_choose_locked(struct xwos_plwq * xwplwq);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 锁住管道等待队列
 * @param xwplwq: (I) 管道等待队列结构体指针
 */
static __xw_inline
void xwos_plwq_lock(struct xwos_plwq * xwplwq)
{
        xwlk_splk_lock(&xwplwq->lock);
}

/**
 * @brief 解锁管道等待队列
 * @param xwplwq: (I) 管道等待队列结构体指针
 */
static __xw_inline
void xwos_plwq_unlock(struct xwos_plwq * xwplwq)
{
        xwlk_splk_unlock(&xwplwq->lock);
}

/**
 * @brief 锁住管道等待队列，并关闭本地CPU的中断
 * @param xwplwq: (I) 管道等待队列结构体指针
 */
static __xw_inline
void xwos_plwq_lock_cpuirq(struct xwos_plwq * xwplwq)
{
        xwlk_splk_lock_cpuirq(&xwplwq->lock);
}

/**
 * @brief 解锁管道等待队列，并开启本地CPU的中断
 * @param xwplwq: (I) 管道等待队列结构体指针
 */
static __xw_inline
void xwos_plwq_unlock_cpuirq(struct xwos_plwq * xwplwq)
{
        xwlk_splk_unlock_cpuirq(&xwplwq->lock);
}

/**
 * @brief 锁住管道等待队列，保存本地CPU的中断标志，并关闭
 * @param xwplwq: (I) 管道等待队列结构体指针
 * @param cpuirq: (O) 保存本地CPU的中断标志的缓冲区的指针
 */
static __xw_inline
void xwos_plwq_lock_cpuirqsv(struct xwos_plwq * xwplwq, xwreg_t * cpuirq)
{
        xwlk_splk_lock_cpuirqsv(&xwplwq->lock, cpuirq);
}

/**
 * @brief 解锁管道等待队列，并恢复本地CPU的中断标志
 * @param xwplwq: (I) 管道等待队列结构体指针
 * @param cpuirq: (I) 本地CPU的中断标志
 */
static __xw_inline
void xwos_plwq_unlock_cpuirqrs(struct xwos_plwq * xwplwq, xwreg_t cpuirq)
{
        xwlk_splk_unlock_cpuirqrs(&xwplwq->lock, cpuirq);
}

/**
 * @brief 检测管道等待队列是否为空
 * @return 布尔值
 * @retval true: 空
 * @retval false: 非空
 */
static __xw_inline
bool xwos_plwq_check_empty(struct xwos_plwq * xwplwq)
{
        return xwlib_bclst_tst_empty_carefully(&xwplwq->head);
}

#endif /* xwos/smp/plwq.h */
