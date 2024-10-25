/**
 * @file
 * @brief XWOS MP内核：调度器的时间树
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/xwaop.h>
#include <xwos/ospl/syshwt.h>
#include <xwos/mp/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/lock/seqlock.h>
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
#  include <xwos/mp/bh.h>
#endif
#include <xwos/mp/tt.h>

#if defined(BRDCFG_XWSKD_SYSHWT_HOOK) && (1 == BRDCFG_XWSKD_SYSHWT_HOOK)
extern
void board_xwskd_syshwt_hook(struct xwmp_skd * xwskd);
#endif

static __xwmp_code
void xwmp_tt_rmrbb_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn);

static __xwmp_code
void xwmp_tt_rmrbn_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn);

static __xwmp_bh
void xwmp_tt_bh(struct xwmp_tt * xwtt);

/**
 * @brief 初始化时间树节点
 * @param[in] ttn: 时间树节点的指针
 */
__xwmp_code
void xwmp_ttn_init(struct xwmp_ttn * ttn)
{
        xwlib_bclst_init_node(&ttn->rbb);
        xwlib_rbtree_init_node(&ttn->rbn);
        ttn->wkup_xwtm = (xwtm_t)0;
        xwaop_write(xwsq_t, &ttn->wkuprs, (xwsq_t)XWMP_TTN_WKUPRS_UNKNOWN, NULL);
        ttn->cb = NULL;
        ttn->xwtt = NULL;
}

/**
 * @brief 初始化时间树
 * @param[in] xwtt: 时间树的指针
 */
__xwmp_code
xwer_t xwmp_tt_init(struct xwmp_tt * xwtt)
{
        xwer_t rc;

        xwmp_sqlk_init(&xwtt->lock);
        xwlib_rbtree_init(&xwtt->rbtree);
        xwtt->deadline = (xwtm_t)0;
        xwtt->leftmost = NULL;
        xwlib_bclst_init_head(&xwtt->timeout);
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwmp_bh_node_init(&xwtt->bhn, (xwmp_bh_f)xwmp_tt_bh, xwtt);
#endif
        rc = xwmp_syshwt_init(&xwtt->hwt);
        return rc;
}

/**
 * @brief 加入节点到时间树
 * @param[in] xwtt: 时间树的指针
 * @param[in] ttn: 时间树节点的指针
 * @param[in] cpuirq: CPU中断开关标志
 * @retval XWOK: 没有错误
 * @retval -EINTR: 被中断
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
__xwmp_code
xwer_t xwmp_tt_add_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn, xwreg_t cpuirq)
{
        struct xwlib_rbtree_node ** pos;
        struct xwlib_rbtree_node * rbn;
        struct xwmp_ttn * n;
        struct xwmp_ttn * leftmost;
        xwptr_t lpc;
        xwsq_t seq;
        xwer_t rc;
        xwtm_t nt;

retry:
        /* the state of thread may be change in IRQ */
        if ((NULL == ttn->cb) || (xwtt != ttn->xwtt)) {
                rc = -EINTR;
        } else {
                pos = &xwtt->rbtree.root;
                lpc = (xwptr_t)pos;
                n = NULL;
                leftmost = xwtt->leftmost;
                if (NULL == leftmost) {
                        /* rbtree is empty. */
                        xwtt->deadline = ttn->wkup_xwtm;
                        xwtt->leftmost = ttn;
                } else if (xwtm_cmp(ttn->wkup_xwtm, xwtt->deadline) < 0) {
                        pos = &leftmost->rbn.left;
                        lpc = (xwptr_t)pos;
                        xwtt->deadline = ttn->wkup_xwtm;
                        xwtt->leftmost = ttn;
                } else if (0 == xwtm_cmp(ttn->wkup_xwtm, xwtt->deadline)) {
                        lpc = (xwptr_t)0;
                        n = leftmost;
                } else {
                        rbn = *pos;
                        seq = xwmp_sqlk_get_seq(&xwtt->lock);
                        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        /* IRQs may happen */
                        seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                        seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                        if (xwmp_sqlk_rd_retry(&xwtt->lock, seq)) {
                                goto retry; // cppcheck-suppress [misra-c2012-15.2]
                        }
                        while (NULL != rbn) { // cppcheck-suppress [misra-c2012-15.4]
                                n = xwlib_rbtree_entry(rbn, struct xwmp_ttn, rbn);
                                nt = n->wkup_xwtm;
                                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                                /* IRQs may happen */
                                seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                                rc = xwtm_cmp(ttn->wkup_xwtm, nt);
                                if (rc < 0) {
                                        pos = &rbn->left;
                                        lpc = (xwptr_t)pos;
                                        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                                        if (xwmp_sqlk_rd_retry(&xwtt->lock, seq)) {
                                                // cppcheck-suppress [misra-c2012-15.2]
                                                goto retry;
                                        }
                                        rbn = rbn->left;
                                } else if (rc > 0) {
                                        pos = &rbn->right;
                                        lpc = ((xwptr_t)pos |
                                               (xwptr_t)XWLIB_RBTREE_POS_RIGHT);
                                        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                                        if (xwmp_sqlk_rd_retry(&xwtt->lock, seq)) {
                                                // cppcheck-suppress [misra-c2012-15.2]
                                                goto retry;
                                        }
                                        rbn = rbn->right;
                                } else {
                                        lpc = (xwptr_t)0;
                                        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                                        if (xwmp_sqlk_rd_retry(&xwtt->lock, seq)) {
                                                // cppcheck-suppress [misra-c2012-15.2]
                                                goto retry;
                                        }
                                        break;
                                }
                        }
                }
                if ((xwptr_t)0 != lpc) {
                        xwlib_rbtree_link(&ttn->rbn, lpc);
                        xwlib_rbtree_insert_color(&xwtt->rbtree, &ttn->rbn);
                } else {
                        xwlib_bclst_add_head(&n->rbb, &ttn->rbb);
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 从时间树中删除节点（节点有伙伴）
 * @param[in] xwtt: 时间树的指针
 * @param[in] ttn: 时间树节点的指针
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
static __xwmp_code
void xwmp_tt_rmrbb_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn)
{
        struct xwmp_ttn * n;

        n = xwlib_bclst_first_entry(&ttn->rbb, struct xwmp_ttn, rbb);
        xwlib_bclst_del_init(&ttn->rbb);
        xwlib_rbtree_replace(&n->rbn, &ttn->rbn);
        xwlib_rbtree_init_node(&ttn->rbn);
        if (ttn == xwtt->leftmost) {
                xwtt->leftmost = n;
        }
}

/**
 * @brief 从时间树中删除节点（节点无伙伴）
 * @param[in] xwtt: 时间树的指针
 * @param[in] ttn: 时间树节点的指针
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
static __xwmp_code
void xwmp_tt_rmrbn_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn)
{
        struct xwlib_rbtree_node * s;

        if (ttn == xwtt->leftmost) {
                s = ttn->rbn.right; /* The successor of a min node is its
                                       right child due to the property 5 of
                                       red-black tree. Or if there is no
                                       right child, the successor is its
                                       parent. */
                if (NULL == s) {
                        s = xwlib_rbtree_get_parent(&ttn->rbn);
                }
                if (s != (struct xwlib_rbtree_node *)&xwtt->rbtree.root) {
                        xwtt->leftmost = xwlib_rbtree_entry(s, struct xwmp_ttn, rbn);
                        xwtt->deadline = xwtt->leftmost->wkup_xwtm;
                } else {
                        xwtt->leftmost = NULL;
                }
        }
        xwlib_rbtree_remove(&xwtt->rbtree, &ttn->rbn);
        xwlib_rbtree_init_node(&ttn->rbn);
}

/**
 * @brief 从时间树中删除节点
 * @param[in] xwtt: 时间树的指针
 * @param[in] ttn: 时间树节点的指针
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 时间树中不存在该节点
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
__xwmp_code
xwer_t xwmp_tt_remove_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn)
{
        xwer_t rc;

        if ((NULL == ttn->cb) || (xwtt != ttn->xwtt)) {
                rc = -ESRCH;
        } else {
                if (xwlib_rbtree_tst_node_unlinked(&ttn->rbn)) {
                        xwlib_bclst_del_init(&ttn->rbb);
                } else if (!xwlib_bclst_tst_empty(&ttn->rbb)) {
                        xwmp_tt_rmrbb_locked(xwtt, ttn);
                } else {
                        xwmp_tt_rmrbn_locked(xwtt, ttn);
                }
                ttn->xwtt = NULL;
                xwaop_write(xwsq_t, &ttn->wkuprs, (xwsq_t)XWMP_TTN_WKUPRS_INTR, NULL);
                ttn->cb = NULL;
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 检查时间树中的超时节点
 * @param[in] xwtt: 时间树的指针
 */
__xwmp_isr
xwer_t xwmp_tt_check_deadline(struct xwmp_tt * xwtt)
{
        struct xwmp_ttn * leftmost;
        xwreg_t cpuirq;
        xwtm_t tick;
        xwer_t rc;

        rc = XWOK;
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        for (tick = xwmp_syshwt_get_time(&xwtt->hwt);
             ((NULL != xwtt->leftmost) && (xwtm_cmp(xwtt->deadline, tick) <= 0));
             tick = xwmp_syshwt_get_time(&xwtt->hwt)) {
                leftmost = xwtt->leftmost;
                xwmp_tt_rmrbn_locked(xwtt, leftmost);
                xwlib_bclst_insseg_tail(&xwtt->timeout, &leftmost->rbb);
                rc = -ETIMEDOUT;
        }
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return rc;
}

/**
 * @brief 时间树的中断底半部
 * @param[in] xwtt: 时间树的指针
 */
static __xwmp_bh
void xwmp_tt_bh(struct xwmp_tt * xwtt)
{
        struct xwmp_skd * xwskd;
        struct xwmp_ttn * ttn;
        xwmp_tt_cb_f cb;
        xwreg_t cpuirq;

        xwskd = xwmp_tt_get_skd(xwtt);
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        xwlib_bclst_itr_prev_entry_del(ttn, &xwtt->timeout, struct xwmp_ttn, rbb) {
                xwlib_bclst_del_init(&ttn->rbb);
                cb = ttn->cb;
                xwaop_write(xwsq_t, &ttn->wkuprs, (xwsq_t)XWMP_TTN_WKUPRS_TIMEDOUT,
                            NULL);
                ttn->cb = NULL;
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                cb(ttn);
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
        }
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        xwmp_skd_chkpmpt(xwskd);
}

/**
 * @brief 获取时间树所属的调度器
 * @param[in] xwtt: 时间树的指针
 * @return 调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_tt_get_skd(struct xwmp_tt * xwtt)
{
        return xwcc_derof(xwtt, struct xwmp_skd, tt);
}

/**
 * @brief 初始化硬件定时器结构体
 * @param[in] hwt: 硬件定时器结构体的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_syshwt_init(struct xwmp_syshwt * hwt)
{
        xwer_t rc;

        hwt->timetick = (xwtm_t)(-(XWOSCFG_SYSHWT_PERIOD));
        hwt->irqrsc = NULL;
        hwt->irqs_num = (xwsz_t)0;
        xwmp_sqlk_init(&hwt->lock);
        rc = xwospl_syshwt_init(hwt);
        if (XWOK == rc) {
                XWOS_BUG_ON(NULL == hwt->irqrsc);
                XWOS_BUG_ON((xwsz_t)0 == hwt->irqs_num);
        }
        return rc;
}

/**
 * @brief 开启硬件定时器
 * @param[in] hwt: 硬件定时器结构体的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_syshwt_start(struct xwmp_syshwt * hwt)
{
        return xwospl_syshwt_start(hwt);
}

/**
 * @brief 关闭硬件定时器
 * @param[in] hwt: 硬件定时器结构体的指针
 */
__xwmp_code
xwer_t xwmp_syshwt_stop(struct xwmp_syshwt * hwt)
{
        return xwospl_syshwt_stop(hwt);
}

/**
 * @brief 获取系统滴答时间
 * @param[in] hwt: 硬件定时器结构体的指针
 * @return 系统滴答时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_code
xwtm_t xwmp_syshwt_get_time(struct xwmp_syshwt * hwt)
{
        xwsq_t seq;
        xwtm_t time;

        do {
                seq = xwmp_sqlk_rd_begin(&hwt->lock);
                time = hwt->timetick;
        } while (xwmp_sqlk_rd_retry(&hwt->lock, seq));
        return time;
}

/**
 * @brief 获取系统定时器的时间戳
 * @param[in] hwt: 硬件定时器结构体的指针
 * @return 时间戳
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_code
xwtm_t xwmp_syshwt_get_timestamp(struct xwmp_syshwt * hwt)
{
        xwsq_t seq;
        xwtm_t ts;
        xwtm_t timeconfetti;

        do {
                seq = xwmp_sqlk_rd_begin(&hwt->lock);
                ts = hwt->timetick;
                timeconfetti = xwospl_syshwt_get_timeconfetti(hwt);
                ts = xwtm_add(ts, timeconfetti);
        } while (xwmp_sqlk_rd_retry(&hwt->lock, seq));
        return ts;
}

/**
 * @brief 获取系统滴答计数
 * @param[in] hwt: 硬件定时器结构体的指针
 * @return 系统滴答计数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_code
xwtk_t xwmp_syshwt_get_tick(struct xwmp_syshwt * hwt)
{
        return (xwtk_t)xwmp_syshwt_get_time(hwt) / (xwtk_t)XWOSCFG_SYSHWT_PERIOD;
}

/**
 * @brief 系统滴答中断任务
 * @param[in] hwt: 硬件定时器结构体的指针
 */
__xwmp_isr
void xwmp_syshwt_task(struct xwmp_syshwt * hwt)
{
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        xwer_t rc;
        xwreg_t flags[hwt->irqs_num];

        xwtt = xwmp_syshwt_get_tt(hwt);
        xwskd = xwmp_tt_get_skd(xwtt);
        xwmp_sqlk_wr_lock_irqssv(&hwt->lock, hwt->irqrsc, flags, hwt->irqs_num);
        hwt->timetick = xwtm_add(hwt->timetick, XWOSCFG_SYSHWT_PERIOD);
        xwmp_sqlk_wr_unlock_irqsrs(&hwt->lock, hwt->irqrsc, flags, hwt->irqs_num);
        rc = xwmp_tt_check_deadline(xwtt);
        if (-ETIMEDOUT == rc) {
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                xwmp_bh_node_eq(&xwskd->bhcb, &xwtt->bhn);
                xwmp_skd_req_bh(xwskd); // cppcheck-suppress [misra-c2012-17.7]
#else
                xwmp_tt_bh(xwtt);
#endif
        }
        xwmp_skd_chkpmpt(xwskd);
#if defined(BRDCFG_XWSKD_SYSHWT_HOOK) && (1 == BRDCFG_XWSKD_SYSHWT_HOOK)
        board_xwskd_syshwt_hook(xwskd);
#endif
}

/**
 * @brief 获取硬件定时器的时间树
 * @param[in] hwt: 硬件定时器结构体的指针
 * @return 时间树的指针
 */
__xwmp_code
struct xwmp_tt * xwmp_syshwt_get_tt(struct xwmp_syshwt * hwt)
{
        return xwcc_derof(hwt, struct xwmp_tt, hwt);
}
