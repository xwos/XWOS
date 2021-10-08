/**
 * @file
 * @brief 玄武OS通用库：无锁队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_lfq_h__
#define __xwos_lib_lfq_h__

#include <xwos/standard.h>

/**
 * @defgroup lfq 无锁队列
 * @{
 */

#include <xwos/ospl/soc/lfq.h>

/**
 * @brief 初始化无锁队列节点
 * @param[in] n: 队列节点指针
 */
static __xwlib_inline
void xwlib_lfq_init(xwlfq_a * n)
{
        *n = (xwlfq_t)0;
}

/**
 * @brief 将一个节点推入无锁队列（栈式队列）
 * @param[in] h: 无锁队列头的指针
 * @param[in] n: 入队节点的指针
 */
void xwlib_lfq_push(xwlfq_a * h, xwlfq_a * n);

/**
 * @brief 弹出无锁队列中的第一个节点（栈式队列）
 * @param[in] h: 无锁队列头的指针
 * @return 被弹出的节点
 */
xwlfq_t * xwlib_lfq_pop(xwlfq_a * h);

/**
 * @} lfq
 */

#endif /* xwos/lib/lfq.h */
