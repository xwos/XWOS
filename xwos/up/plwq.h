/**
 * @file
 * @brief XWOS UP内核：管道等待队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_plwq_h__
#define __xwos_up_plwq_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>

struct xwup_wqn;

/**
 * @brief 管道等待队列结构体
 */
struct xwup_plwq {
        struct xwlib_bclst_head head; /**< 链表头 */
};

/**
 * @brief 安全地遍历管道等待队列中所有节点，以防其被删除
 * @param[in] c: 循环变量，类型只能是 `struct xwup_wqn *`
 * @param[in] n: 临时变量，类型只能是 `struct xwup_wqn *`
 * @param[in] xwplwq: 管道等待队列的指针
 */
// cppcheck-suppress [misra-c2012-20.7]
#define xwup_plwq_itr_wqn_safe(c, n, xwplwq)                            \
        xwlib_bclst_itr_next_entry_safe(c, n, &((xwplwq)->head),        \
                                        struct xwup_wqn,                \
                                        cln.pl)

/**
 * @brief 以删除为目的，遍历管道等待队列中所有节点
 * @param[in] c: 循环变量，类型只能是 `struct xwup_wqn *`
 * @param[in] xwplwq: 管道等待队列的指针
 */
// cppcheck-suppress [misra-c2012-20.7]
#define xwup_plwq_itr_wqn_rm(c, xwplwq)                                 \
        xwlib_bclst_itr_next_entry_del(c, &((xwplwq)->head),            \
                                       struct xwup_wqn,                 \
                                       cln.pl)

void xwup_plwq_init(struct xwup_plwq * xwplwq);
void xwup_plwq_add_head(struct xwup_plwq * xwplwq, struct xwup_wqn * wqn);
void xwup_plwq_add_tail(struct xwup_plwq * xwplwq, struct xwup_wqn * wqn);
xwer_t xwup_plwq_remove(struct xwup_plwq * xwplwq, struct xwup_wqn * wqn);
struct xwup_wqn * xwup_plwq_choose(struct xwup_plwq * xwplwq);

/**
 * @brief 检测管道等待队列是否为空
 * @return 布尔值
 * @retval true: 空
 * @retval false: 非空
 */
static __xwcc_inline
bool xwup_plwq_check_empty(struct xwup_plwq * xwplwq)
{
        return xwlib_bclst_tst_empty_carefully(&xwplwq->head);
}

#endif /* xwos/up/plwq.h */
