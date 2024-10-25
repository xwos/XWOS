/**
 * @file
 * @brief XWOS UP内核：调度器的时间树
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
#include <xwos/ospl/syshwt.h>
#include <xwos/up/irq.h>
#include <xwos/up/skd.h>
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
#  include <xwos/up/bh.h>
#endif
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/tt.h>

#if defined(BRDCFG_XWSKD_SYSHWT_HOOK) && (1 == BRDCFG_XWSKD_SYSHWT_HOOK)
extern
void board_xwskd_syshwt_hook(struct xwup_skd * xwskd);
#endif

static __xwup_code
void xwup_tt_rmrbb_locked(struct xwup_tt * xwtt, struct xwup_ttn * ttn);

static __xwup_code
void xwup_tt_rmrbn_locked(struct xwup_tt * xwtt, struct xwup_ttn * ttn);

static __xwup_bh
void xwup_tt_bh(struct xwup_tt * xwtt);

/**
 * @brief 初始化时间树节点
 * @param[in] ttn: 时间树节点的指针
 */
__xwup_code
void xwup_ttn_init(struct xwup_ttn * ttn)
{
        xwlib_bclst_init_node(&ttn->rbb);
        xwlib_rbtree_init_node(&ttn->rbn);
        ttn->wkup_xwtm = (xwtm_t)0;
        ttn->wkuprs = (xwsq_t)XWUP_TTN_WKUPRS_UNKNOWN;
        ttn->cb = NULL;
        ttn->xwtt = NULL;
}

/**
 * @brief 初始化时间树
 * @param[in] xwtt: 时间树的指针
 */
__xwup_code
xwer_t xwup_tt_init(struct xwup_tt * xwtt)
{
        xwer_t rc;

        xwup_sqlk_init(&xwtt->lock);
        xwlib_rbtree_init(&xwtt->rbtree);
        xwtt->deadline = (xwtm_t)0;
        xwtt->leftmost = NULL;
        xwlib_bclst_init_head(&xwtt->timeout);
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwup_bh_node_init(&xwtt->bhn, (xwup_bh_f)xwup_tt_bh, xwtt);
#endif
        rc = xwup_syshwt_init(&xwtt->hwt);
        return rc;
}

/**
 * @brief 将节点加入到时间树
 * @param[in] xwtt: 时间树的指针
 * @param[in] ttn: 时间树节点的指针
 * @param[in] cpuirq: CPU中断开关标志
 * @retval XWOK: 没有错误
 * @retval -EINTR: 被中断
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
__xwup_code
xwer_t xwup_tt_add_locked(struct xwup_tt * xwtt, struct xwup_ttn * ttn,
                          xwreg_t cpuirq)
{
        struct xwlib_rbtree_node ** pos;
        struct xwlib_rbtree_node * rbn;
        struct xwup_ttn * n;
        struct xwup_ttn * leftmost;
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
                        seq = xwup_sqlk_get_seq(&xwtt->lock);
                        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        /* IRQs may happen */
                        seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
                        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                        seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
                        if (xwup_sqlk_rd_retry(&xwtt->lock, seq)) {
                                goto retry; // cppcheck-suppress [misra-c2012-15.2]
                        }
                        while (NULL != rbn) { // cppcheck-suppress [misra-c2012-15.4]
                                n = xwlib_rbtree_entry(rbn, struct xwup_ttn, rbn);
                                nt = n->wkup_xwtm;
                                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                                /* IRQs may happen */
                                seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
                                rc = xwtm_cmp(ttn->wkup_xwtm, nt);
                                if (rc < 0) {
                                        pos = &rbn->left;
                                        lpc = (xwptr_t)pos;
                                        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
                                        if (xwup_sqlk_rd_retry(&xwtt->lock, seq)) {
                                                // cppcheck-suppress [misra-c2012-15.2]
                                                goto retry;
                                        }
                                        rbn = rbn->left;
                                } else if (rc > 0) {
                                        pos = &rbn->right;
                                        lpc = ((xwptr_t)pos |
                                               (xwptr_t)XWLIB_RBTREE_POS_RIGHT);
                                        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
                                        if (xwup_sqlk_rd_retry(&xwtt->lock, seq)) {
                                                // cppcheck-suppress [misra-c2012-15.2]
                                                goto retry;
                                        }
                                        rbn = rbn->right;
                                } else {
                                        lpc = (xwptr_t)0;
                                        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
                                        if (xwup_sqlk_rd_retry(&xwtt->lock, seq)) {
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
static __xwup_code
void xwup_tt_rmrbb_locked(struct xwup_tt * xwtt, struct xwup_ttn * ttn)
{
        struct xwup_ttn * n;

        n = xwlib_bclst_first_entry(&ttn->rbb, struct xwup_ttn, rbb);
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
static __xwup_code
void xwup_tt_rmrbn_locked(struct xwup_tt * xwtt, struct xwup_ttn * ttn)
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
                        xwtt->leftmost = xwlib_rbtree_entry(s, struct xwup_ttn, rbn);
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
__xwup_code
xwer_t xwup_tt_remove_locked(struct xwup_tt * xwtt, struct xwup_ttn * ttn)
{
        xwer_t rc;

        if ((NULL == ttn->cb) || (xwtt != ttn->xwtt)) {
                rc = -ESRCH;
        } else {
                if (xwlib_rbtree_tst_node_unlinked(&ttn->rbn)) {
                        xwlib_bclst_del_init(&ttn->rbb);
                } else if (!xwlib_bclst_tst_empty(&ttn->rbb)) {
                        xwup_tt_rmrbb_locked(xwtt, ttn);
                } else {
                        xwup_tt_rmrbn_locked(xwtt, ttn);
                }
                ttn->xwtt = NULL;
                ttn->wkuprs = (xwsq_t)XWUP_TTN_WKUPRS_INTR;
                ttn->cb = NULL;
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 检查时间树中超时的节点
 * @param[in] xwtt: 时间树的指针
 */
__xwup_isr
xwer_t xwup_tt_check_deadline(struct xwup_tt * xwtt)
{
        struct xwup_ttn * leftmost;
        xwreg_t cpuirq;
        xwtm_t tick;
        xwer_t rc;

        rc = XWOK;
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        for (tick = xwup_syshwt_get_time(&xwtt->hwt);
             ((NULL != xwtt->leftmost) && (xwtm_cmp(xwtt->deadline, tick) <= 0));
             tick = xwup_syshwt_get_time(&xwtt->hwt)) {
                leftmost = xwtt->leftmost;
                xwup_tt_rmrbn_locked(xwtt, leftmost);
                xwlib_bclst_insseg_tail(&xwtt->timeout, &leftmost->rbb);
                rc = -ETIMEDOUT;
        }
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return rc;
}

/**
 * @brief 时间树的中断底半部
 * @param[in] xwtt: 时间树的指针
 */
static __xwup_bh
void xwup_tt_bh(struct xwup_tt * xwtt)
{
        struct xwup_ttn * ttn;
        xwreg_t cpuirq;
        xwup_tt_cb_f cb;

        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        xwlib_bclst_itr_prev_entry_del(ttn, &xwtt->timeout, struct xwup_ttn, rbb) {
                xwlib_bclst_del_init(&ttn->rbb);
                cb = ttn->cb;
                ttn->wkuprs = (xwsq_t)XWUP_TTN_WKUPRS_TIMEDOUT;
                ttn->cb = NULL;
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                cb(ttn);
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
        }
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        xwup_skd_chkpmpt();
}

/**
 * @brief 获取时间树所属的调度器
 * @param[in] xwtt: 时间树的指针
 * @return 调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_tt_get_skd(struct xwup_tt * xwtt)
{
        return xwcc_derof(xwtt, struct xwup_skd, tt);
}

/**
 * @brief 初始化硬件定时器结构体
 * @param[in] hwt: 硬件定时器结构体的指针
 * @return 错误码
 */
__xwup_code
xwer_t xwup_syshwt_init(struct xwup_syshwt * hwt)
{
        xwer_t rc;

        hwt->timetick = (xwtm_t)(-(XWOSCFG_SYSHWT_PERIOD));
        hwt->irqrsc = NULL;
        hwt->irqs_num = (xwsz_t)0;
        xwup_sqlk_init(&hwt->lock);
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
__xwup_code
xwer_t xwup_syshwt_start(struct xwup_syshwt * hwt)
{
        return xwospl_syshwt_start(hwt);
}

/**
 * @brief 关闭硬件定时器
 * @param[in] hwt: 硬件定时器结构体的指针
 */
__xwup_code
xwer_t xwup_syshwt_stop(struct xwup_syshwt * hwt)
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
__xwup_code
xwtm_t xwup_syshwt_get_time(struct xwup_syshwt * hwt)
{
        xwsq_t seq;
        xwtm_t timetick;

        do {
                seq = xwup_sqlk_rd_begin(&hwt->lock);
                timetick = hwt->timetick;
        } while (xwup_sqlk_rd_retry(&hwt->lock, seq));
        return timetick;
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
__xwup_code
xwtm_t xwup_syshwt_get_timestamp(struct xwup_syshwt * hwt)
{
        xwsq_t seq;
        xwtm_t ts;
        xwtm_t timeconfetti;

        do {
                seq = xwup_sqlk_rd_begin(&hwt->lock);
                ts = hwt->timetick;
                timeconfetti = xwospl_syshwt_get_timeconfetti(hwt);
                ts = xwtm_add(ts, timeconfetti);
        } while (xwup_sqlk_rd_retry(&hwt->lock, seq));
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
__xwup_code
xwtk_t xwup_syshwt_get_tick(struct xwup_syshwt * hwt)
{
        return (xwtk_t)xwup_syshwt_get_time(hwt) / (xwtk_t)XWOSCFG_SYSHWT_PERIOD;
}

/**
 * @brief 系统滴答中断任务
 * @param[in] hwt: 硬件定时器结构体的指针
 */
__xwup_isr
void xwup_syshwt_task(struct xwup_syshwt * hwt)
{
        struct xwup_tt * xwtt;
        xwer_t rc;
        xwreg_t flags[hwt->irqs_num];

        xwtt = xwup_syshwt_get_tt(hwt);
        xwup_sqlk_wr_lock_irqssv(&hwt->lock, hwt->irqrsc, flags, hwt->irqs_num);
        hwt->timetick = xwtm_add(hwt->timetick, XWOSCFG_SYSHWT_PERIOD);
        xwup_sqlk_wr_unlock_irqsrs(&hwt->lock, hwt->irqrsc, flags, hwt->irqs_num);
        rc = xwup_tt_check_deadline(xwtt);
        if (-ETIMEDOUT == rc) {
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                struct xwup_skd * xwskd;

                xwskd = xwup_tt_get_skd(xwtt);
                xwup_bh_node_eq(&xwskd->bhcb, &xwtt->bhn);
                xwup_skd_req_bh(); // cppcheck-suppress [misra-c2012-17.7]
#else
                xwup_tt_bh(xwtt);
#endif
        }
        xwup_skd_chkpmpt();
#if defined(BRDCFG_XWSKD_SYSHWT_HOOK) && (1 == BRDCFG_XWSKD_SYSHWT_HOOK)
        board_xwskd_syshwt_hook(xwup_skd_get_lc());
#endif
}

/**
 * @brief 获取硬件定时器的时间树
 * @param[in] hwt: 硬件定时器结构体的指针
 * @return 时间树的指针
 */
__xwup_code
struct xwup_tt * xwup_syshwt_get_tt(struct xwup_syshwt * hwt)
{
        return xwcc_derof(hwt, struct xwup_tt, hwt);
}
