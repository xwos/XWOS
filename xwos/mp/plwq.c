/**
 * @file
 * @brief XWOS MP内核：管道等待队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① xwmp_plwq.lock
 *     + ② xwmp_wqn.lock
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/wqn.h>
#include <xwos/mp/plwq.h>

/**
 * @brief 初始化管道等待队列
 * @param[in] xwplwq: 管道等待队列结构体指针
 */
__xwmp_code
void xwmp_plwq_init(struct xwmp_plwq * xwplwq)
{
        xwlib_bclst_init_head(&xwplwq->head);
        xwmp_splk_init(&xwplwq->lock);
}

/**
 * @brief 将等待队列节点加入到管道等待队列的头部
 * @param[in] xwplwq: 管道等待队列结构体指针
 * @param[in] wqn: 等待队列节点结构体指针
 * @note
 * - 这个函数只能在取得锁xwplwq->lock时被调用。
 */
__xwmp_code
void xwmp_plwq_add_head_locked(struct xwmp_plwq * xwplwq, struct xwmp_wqn * wqn)
{
        xwlib_bclst_add_head(&xwplwq->head, &wqn->cln.pl);
}

/**
 * @brief 将等待队列节点加入到管道等待队列的尾部
 * @param[in] xwplwq: 管道等待队列结构体指针
 * @param[in] wqn: 等待队列节点结构体指针
 * @note
 * - 这个函数只能在取得锁xwplwq->lock时被调用。
 */
__xwmp_code
void xwmp_plwq_add_tail_locked(struct xwmp_plwq * xwplwq, struct xwmp_wqn * wqn)
{
        xwlib_bclst_add_tail(&xwplwq->head, &wqn->cln.pl);
}

/**
 * @brief 将等待队列节点从管道等待队列中移除
 * @param[in] xwplwq: 管道等待队列结构体指针
 * @param[in] wqn: 等待队列节点结构体指针
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 没有这个节点
 * @note
 * - 这个函数只能在取得锁xwplwq->lock时被调用。
 */
__xwmp_code
xwer_t xwmp_plwq_remove_locked(struct xwmp_plwq * xwplwq, struct xwmp_wqn * wqn)
{
        xwer_t rc;

        if (__xwcc_unlikely((NULL == wqn->cb) || (xwplwq != wqn->wq))) {
                rc = -ESRCH;
        } else {
                xwlib_bclst_del_init(&wqn->cln.pl);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 从管道等待队列中选择一个节点
 * @param[in] xwplwq: 管道等待队列结构体指针
 * @return 被选择的等待队列节点结构体指针
 * @retval NULL: 没有选中任何节点（等待队列为空）
 * @note
 * - 这个函数只能在取得锁xwplwq->lock时被调用。
 * - 这个函数会将选中的节点锁住（wqn->lock）。
 */
__xwmp_code
struct xwmp_wqn * xwmp_plwq_choose_locked(struct xwmp_plwq * xwplwq)
{
        struct xwmp_wqn * wqn;

        if (xwlib_bclst_tst_empty(&xwplwq->head)) {
                wqn = NULL;
        } else {
                wqn = xwlib_bclst_first_entry(&xwplwq->head, struct xwmp_wqn,
                                              cln.pl);
                xwmp_splk_lock(&wqn->lock);
                xwlib_bclst_del_init(&wqn->cln.pl);
        }
        return wqn;
}
