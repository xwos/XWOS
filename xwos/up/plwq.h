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

#ifndef __xwos_up_plwq_h__
#define __xwos_up_plwq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_wqn;

/**
 * @brief 管道等待队列结构体
 */
struct xwos_plwq {
        struct xwlib_bclst_head head; /**< 链表头 */
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
void xwos_plwq_add_head(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn);

__xwos_code
void xwos_plwq_add_tail(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn);

__xwos_code
xwer_t xwos_plwq_remove(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn);

__xwos_code
struct xwos_wqn * xwos_plwq_choose(struct xwos_plwq * xwplwq);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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

#endif /* xwos/up/plwq.h */
