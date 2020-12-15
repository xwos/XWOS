/**
 * @file
 * @brief 玄武OS MP内核：实时就绪队列
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① xwmp_mtxtree.lock
 *   + ① xwmp_skd.pm.lock
 *     + ② xwmp_rtrq.lock
 *       + ③ xwmp_thd.stlock
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/rtrq.h>

/**
 * @brief 初始化实时就绪队列
 * @param xwrtrq: (I) XWOS MP内核的实时就绪队列
 */
__xwmp_code
xwer_t xwmp_rtrq_init(struct xwmp_rtrq * xwrtrq)
{
        xwsq_t i;

        xwmp_splk_init(&xwrtrq->lock);
        memset(&xwrtrq->bmp, 0, sizeof(xwrtrq->bmp));
        for (i = 0; i < XWMP_RTRQ_QNUM; i++) {
                xwlib_bclst_init_head(&xwrtrq->q[i]);
        }
        xwrtrq->top = XWMP_SKD_PRIORITY_INVALID;
        return XWOK;
}

/**
 * @brief 将线程加入到实时就绪队列的头部
 * @param xwrtrq: (I) XWOS MP内核的实时就绪队列
 * @param thd: (I) 线程控制块的指针
 * @retval XWOK: 没有错误
 * @retval -EPERM: 线程没有设置状态标志@ref XWMP_SKDOBJ_DST_READY
 * @note
 * - 当线程加入到就绪队列时，它不应该拥有下面的状态：
 *   XWMP_SKDOBJ_DST_RUNNING | XWMP_SKDOBJ_DST_FROZEN | XWMP_SKDOBJ_DST_STANDBY
 * - 当线程加入到就绪队列时, 它不应该*同时*拥有下面的状态：
 *   XWMP_SKDOBJ_DST_BLOCKING & XWMP_SKDOBJ_DST_SLEEPING
 * - 此函数必须在持有锁xwrtrq->lock时才可调用。
 */
__xwmp_code
xwer_t xwmp_rtrq_add_head_locked(struct xwmp_rtrq * xwrtrq, struct xwmp_thd * thd)
{
        xwpr_t prio;
        xwer_t rc;

        if (!(XWMP_SKDOBJ_DST_READY & thd->state)) {
                rc = -EPERM;
        } else {
                prio = thd->dprio.rq;
                xwlib_bclst_add_head(&xwrtrq->q[prio], &thd->rqnode);
                if (!xwbmpop_t1i(xwrtrq->bmp, (xwsq_t)prio)) {
                        xwbmpop_s1i(xwrtrq->bmp, (xwsq_t)prio);
                        if (xwrtrq->top < prio) {
                                xwrtrq->top = prio;
                        }
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程加入到实时就绪队列的尾部
 * @param xwrtrq: (I) XWOS MP内核的实时就绪队列
 * @param thd: (I) 线程控制块的指针
 * @retval XWOK: 没有错误
 * @retval -EPERM: 线程没有设置状态标志@ref XWMP_SKDOBJ_DST_READY
 * @note
 * - 当线程加入到就绪队列时，它不应该拥有下面的状态：
 *   XWMP_SKDOBJ_DST_RUNNING | XWMP_SKDOBJ_DST_FROZEN | XWMP_SKDOBJ_DST_STANDBY
 * - 当线程加入到就绪队列时, 它不应该*同时*拥有下面的状态：
 *   XWMP_SKDOBJ_DST_BLOCKING & XWMP_SKDOBJ_DST_SLEEPING
 * - 此函数必须在持有锁xwrtrq->lock时才可调用。
 */
__xwmp_code
xwer_t xwmp_rtrq_add_tail_locked(struct xwmp_rtrq * xwrtrq, struct xwmp_thd * thd)
{
        xwpr_t prio;
        xwer_t rc;

        if (!(XWMP_SKDOBJ_DST_READY & thd->state)) {
                rc = -EPERM;
        } else {
                prio = thd->dprio.rq;
                xwlib_bclst_add_tail(&xwrtrq->q[prio], &thd->rqnode);
                if (!xwbmpop_t1i(xwrtrq->bmp, (xwsq_t)prio)) {
                        xwbmpop_s1i(xwrtrq->bmp, (xwsq_t)prio);
                        if (xwrtrq->top < prio) {
                                xwrtrq->top = prio;
                        }
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程从实时就绪队列中删除
 * @param xwrtrq: (I) XWOS MP内核的实时就绪队列
 * @param thd: (I) 线程控制块的指针
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 就绪队列中没有这个线程
 * @note
 * - 此函数必须在持有锁xwrtrq->lock时才可调用。
 */
__xwmp_code
xwer_t xwmp_rtrq_remove_locked(struct xwmp_rtrq * xwrtrq, struct xwmp_thd * thd)
{
        xwpr_t prio;
        xwer_t rc;

        if (!(XWMP_SKDOBJ_DST_READY & thd->state)) {
                rc = -ESRCH;
        } else {
                prio = thd->dprio.rq;
                xwlib_bclst_del_init(&thd->rqnode);
                if (xwlib_bclst_tst_empty(&xwrtrq->q[prio])) {
                        xwbmpop_c0i(xwrtrq->bmp, (xwsq_t)prio);
                        prio = (xwpr_t)xwbmpop_fls(xwrtrq->bmp, XWMP_RTRQ_QNUM);
                        if (prio < 0) {
                                xwrtrq->top = XWMP_SKD_PRIORITY_INVALID;
                        } else {
                                xwrtrq->top = prio;
                        }
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 从实时就绪队列中选择优先级最高的线程
 * @param xwrtrq: (I) XWOS MP内核的实时就绪队列
 * @return 被选择的线程控制块的指针
 * @note
 * - 此函数必须在持有锁xwrtrq->lock时才可调用。
 */
__xwmp_code
struct xwmp_thd * xwmp_rtrq_choose_locked(struct xwmp_rtrq * xwrtrq)
{
        struct xwmp_thd * t;

        if (XWMP_SKD_PRIORITY_INVALID == xwrtrq->top) {
                t = NULL;
        } else {
                t = xwlib_bclst_first_entry(&xwrtrq->q[xwrtrq->top],
                                            struct xwmp_thd,
                                            rqnode);
        }
        return t;
}
