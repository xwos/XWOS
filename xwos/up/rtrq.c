/**
 * @file
 * @brief XuanWuOS内核：实时就绪队列
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
#include <xwos/lib/string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/thread.h>
#include <xwos/up/rtrq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化实时就绪队列
 * @param xwrtrq: (I) XWOS的实时就绪队列
 */
__xwos_code
void xwos_rtrq_init(struct xwos_rtrq * xwrtrq)
{
        xwsq_t i;

        memset(&xwrtrq->bmp, 0, sizeof(xwrtrq->bmp));
        for (i = 0; i < XWOS_RTRQ_QNUM; i++) {
                xwlib_bclst_init_head(&xwrtrq->q[i]);
        }
        xwrtrq->top = XWOS_SD_PRIORITY_INVALID;
}

/**
 * @brief 将线程加入到实时就绪队列的头部
 * @param xwrtrq: (I) XWOS的实时就绪队列
 * @param tcb: (I) 线程控制块的指针
 * @retval OK: OK
 * @retval -EPERM: 线程没有设置状态标志@ref XWSDOBJ_DST_READY
 * @note
 * - 当线程加入到就绪队列时，它不应该拥有下面的状态：
 *   XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_FROZEN | XWSDOBJ_DST_STANDBY
 * - 当线程加入到就绪队列时, 它不应该*同时*拥有下面的状态：
 *   XWSDOBJ_DST_BLOCKING & XWSDOBJ_DST_SLEEPING
 * - 这个函数只能在临界区中调用。
 */
__xwos_code
void xwos_rtrq_add_head(struct xwos_rtrq * xwrtrq, struct xwos_tcb * tcb)
{
        xwpr_t prio;

        prio = tcb->prio.d;
        xwlib_bclst_add_head(&xwrtrq->q[prio], &tcb->rqnode);
        if (!xwbmpop_t1i(xwrtrq->bmp, (xwsq_t)prio)) {
                xwbmpop_s1i(xwrtrq->bmp, (xwsq_t)prio);
                if (xwrtrq->top < prio) {
                        xwrtrq->top = prio;
                }/* else {} */
        }/* else {} */

}

/**
 * @brief 将线程加入到实时就绪队列的尾部
 * @param xwrtrq: (I) XWOS的实时就绪队列
 * @param tcb: (I) 线程控制块的指针
 * @retval OK: OK
 * @retval -EPERM: 线程没有设置状态标志@ref XWSDOBJ_DST_READY
 * @note
 * - 当线程加入到就绪队列时，它不应该拥有下面的状态：
 *   XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_FROZEN | XWSDOBJ_DST_STANDBY
 * - 当线程加入到就绪队列时, 它不应该*同时*拥有下面的状态：
 *   XWSDOBJ_DST_BLOCKING & XWSDOBJ_DST_SLEEPING
 * - 这个函数只能在临界区中调用。
 */
__xwos_code
void xwos_rtrq_add_tail(struct xwos_rtrq * xwrtrq, struct xwos_tcb * tcb)
{
        xwpr_t prio;

        prio = tcb->prio.d;
        xwlib_bclst_add_tail(&xwrtrq->q[prio], &tcb->rqnode);
        if (!xwbmpop_t1i(xwrtrq->bmp, (xwsq_t)prio)) {
                xwbmpop_s1i(xwrtrq->bmp, (xwsq_t)prio);
                if (xwrtrq->top < prio) {
                        xwrtrq->top = prio;
                }/* else {} */
        }/* else {} */
}

/**
 * @brief 将线程从实时就绪队列中删除
 * @param xwrtrq: (I) XWOS的实时就绪队列
 * @param tcb: (I) 线程控制块的指针
 * @note
 * - 这个函数只能在临界区中调用。
 */
__xwos_code
void xwos_rtrq_remove(struct xwos_rtrq * xwrtrq, struct xwos_tcb * tcb)
{
        xwpr_t prio;

        prio = tcb->prio.d;
        xwlib_bclst_del_init(&tcb->rqnode);
        if (xwlib_bclst_tst_empty(&xwrtrq->q[prio])) {
                xwbmpop_c0i(xwrtrq->bmp, (xwsq_t)prio);
                prio = (xwpr_t)xwbmpop_fls(xwrtrq->bmp, XWOS_RTRQ_QNUM);
                if (prio < 0) {
                        xwrtrq->top = XWOS_SD_PRIORITY_INVALID;
                } else {
                        xwrtrq->top = prio;
                }
        }
}

/**
 * @brief 从实时就绪队列中选择优先级最高的线程
 * @param xwrtrq: (I) XWOS的实时就绪队列
 * @return 被选择的线程控制块的指针
 * @note
 * - 这个函数只能在临界区中调用。
 */
__xwos_code
struct xwos_tcb * xwos_rtrq_choose(struct xwos_rtrq * xwrtrq)
{
        struct xwos_tcb * t;

        if (XWOS_SD_PRIORITY_INVALID == xwrtrq->top) {
                t = NULL;
        } else {
                t = xwlib_bclst_first_entry(&xwrtrq->q[xwrtrq->top],
                                            struct xwos_tcb,
                                            rqnode);
        }
        return t;
}
