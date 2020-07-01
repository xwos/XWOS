/**
 * @file
 * @brief XuanWuOS内核：管道等待队列
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/up/wqn.h>
#include <xwos/up/plwq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化管道等待队列
 * @param xwplwq: (I) 管道等待队列结构体指针
 */
__xwos_code
void xwos_plwq_init(struct xwos_plwq * xwplwq)
{
        xwlib_bclst_init_head(&xwplwq->head);
}

/**
 * @brief 将等待队列节点加入到管道等待队列的头部
 * @param xwplwq: (I) 管道等待队列结构体指针
 * @param wqn: (I) 等待队列节点结构体指针
 * @note
 * - 这个函数只能在临界区被调用。
 */
__xwos_code
void xwos_plwq_add_head(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn)
{
        xwlib_bclst_add_head(&xwplwq->head, &wqn->cln.pl);
}

/**
 * @brief 将等待队列节点加入到管道等待队列的尾部
 * @param xwplwq: (I) 管道等待队列结构体指针
 * @param wqn: (I) 等待队列节点结构体指针
 * @note
 * - 这个函数只能在临界区被调用。
 */
__xwos_code
void xwos_plwq_add_tail(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn)
{
        xwlib_bclst_add_tail(&xwplwq->head, &wqn->cln.pl);
}

/**
 * @brief 将等待队列节点从管道等待队列中移除
 * @param xwplwq: (I) 管道等待队列结构体指针
 * @param wqn: (I) 等待队列节点结构体指针
 * @retval OK: OK
 * @retval -ESRCH: 没有这个节点
 * @note
 * - 这个函数只能在临界区被调用。
 */
__xwos_code
xwer_t xwos_plwq_remove(struct xwos_plwq * xwplwq, struct xwos_wqn * wqn)
{
        xwer_t rc;

        if (__unlikely((NULL == wqn->cb) || (xwplwq != wqn->wq))) {
                rc = -ESRCH;
        } else {
                xwlib_bclst_del_init(&wqn->cln.pl);
                rc = OK;
        }
        return rc;
}

/**
 * @brief 从管道等待队列中选择一个节点
 * @param xwplwq: (I) 管道等待队列结构体指针
 * @return 被选择的等待队列节点结构体指针
 * @retval NULL: 没有选中任何节点（等待队列为空）
 * @note
 * - 这个函数只能在临界区被调用。
 */
__xwos_code
struct xwos_wqn * xwos_plwq_choose(struct xwos_plwq * xwplwq)
{
        struct xwos_wqn * wqn;

        if (xwlib_bclst_tst_empty(&xwplwq->head)) {
                wqn = NULL;
        } else {
                wqn = xwlib_bclst_first_entry(&xwplwq->head,
                                              struct xwos_wqn,
                                              cln.pl);
                xwlib_bclst_del_init(&wqn->cln.pl);
        }
        return wqn;
}
