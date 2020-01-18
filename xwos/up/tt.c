/**
 * @file
 * @brief XuanWuOS内核：调度器的时间树
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/irq.h>
#include <xwos/up/scheduler.h>
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
  #include <xwos/up/bh.h>
#endif /* XWUPCFG_SD_BH */
#include <xwos/up/lock/seqlock.h>
#include <soc_syshwt.h>
#include <xwos/up/tt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
void xwos_tt_rmrbb_locked(struct xwos_tt * xwtt, struct xwos_ttn * ttn);

static __xwos_code
void xwos_tt_rmrbn_locked(struct xwos_tt * xwtt, struct xwos_ttn * ttn);

static __xwos_bh
void xwos_tt_bh(struct xwos_tt * xwtt);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化时间树节点
 * @param ttn: (I) 时间树节点的指针
 */
__xwos_code
void xwos_ttn_init(struct xwos_ttn * ttn, xwptr_t entry, xwptr_t type)
{
        xwlib_bclst_init_node(&ttn->rbb);
        xwlib_rbtree_init_node(&ttn->rbn);
        ttn->wkup_xwtm = 0;
        ttn->wkuprs = XWOS_TTN_WKUPRS_UNKNOWN,
        ttn->cb = NULL;
        ttn->xwtt = NULL;
        ttn->entry.addr = entry & (~XWOS_TTN_TYPE_MASK);
        ttn->entry.type |= type & XWOS_TTN_TYPE_MASK;
}

/**
 * @brief 初始化时间树
 * @param xwtt: (I) 时间树的指针
 */
__xwos_code
xwer_t xwos_tt_init(struct xwos_tt * xwtt)
{
        xwer_t rc;

        xwlk_sqlk_init(&xwtt->lock);
        xwlib_rbtree_init(&xwtt->rbtree);
        xwtt->deadline = 0;
        xwtt->leftmost = NULL;
        xwlib_bclst_init_head(&xwtt->timeout);
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
        xwos_bh_node_init(&xwtt->bhn, (xwos_bh_f)xwos_tt_bh, xwtt);
#endif /* XWUPCFG_SD_BH */
        rc = xwos_syshwt_init(&xwtt->hwt);
        return rc;
}

/**
 * @brief 将节点加入到时间树
 * @param xwtt: (I) 时间树的指针
 * @param ttn: (I) 时间树节点的指针
 * @param cpuirq: (I) CPU中断开关标志
 * @retval OK: OK
 * @retval -EINTR: 被中断
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
__xwos_code
xwer_t xwos_tt_add_locked(struct xwos_tt * xwtt, struct xwos_ttn * ttn,
                          xwreg_t cpuirq)
{
        struct xwlib_rbtree_node ** pos;
        struct xwlib_rbtree_node * rbn;
        struct xwos_ttn * n, * leftmost;
        xwptr_t lpc;
        xwsq_t seq;
        xwer_t rc;
        xwtm_t nt;

retry:
        /* the state of thread may be change in IRQ */
        if (__unlikely((NULL == ttn->cb) || (xwtt != ttn->xwtt))) {
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
                        seq = xwlk_sqlk_get_seq(&xwtt->lock);
                        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        /* IRQs may happen */
                        seq += XWLK_SQLK_GRANULARITY;
                        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                        seq += XWLK_SQLK_GRANULARITY;
                        if (xwlk_sqlk_rd_retry(&xwtt->lock, seq)) {
                                goto retry;
                        }
                        while (rbn) {
                                n = xwlib_rbtree_entry(rbn, struct xwos_ttn, rbn);
                                nt = n->wkup_xwtm;
                                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                                /* IRQs may happen */
                                seq += XWLK_SQLK_GRANULARITY;
                                rc = xwtm_cmp(ttn->wkup_xwtm, nt);
                                if (rc < 0) {
                                        pos = &rbn->left;
                                        lpc = (xwptr_t)pos;
                                        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += XWLK_SQLK_GRANULARITY;
                                        if (xwlk_sqlk_rd_retry(&xwtt->lock, seq)) {
                                                goto retry;
                                        }
                                        rbn = rbn->left;
                                } else if (rc > 0) {
                                        pos = &rbn->right;
                                        lpc = (xwptr_t)pos | XWLIB_RBTREE_POS_RIGHT;
                                        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += XWLK_SQLK_GRANULARITY;
                                        if (xwlk_sqlk_rd_retry(&xwtt->lock, seq)) {
                                                goto retry;
                                        }
                                        rbn = rbn->right;
                                } else {
                                        lpc = (xwptr_t)0;
                                        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                                        seq += XWLK_SQLK_GRANULARITY;
                                        if (xwlk_sqlk_rd_retry(&xwtt->lock, seq)) {
                                                goto retry;
                                        }
                                        break;
                                }
                        }
                }
                if (lpc) {
                        xwlib_rbtree_link(&ttn->rbn, lpc);
                        xwlib_rbtree_insert_color(&xwtt->rbtree, &ttn->rbn);
                } else {
                        xwlib_bclst_add_head(&n->rbb, &ttn->rbb);
                }
                rc = OK;
        }
        return rc;
}

/**
 * @brief 从时间树中删除节点（节点有伙伴）
 * @param xwtt: (I) 时间树的指针
 * @param ttn: (I) 时间树节点的指针
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
static __xwos_code
void xwos_tt_rmrbb_locked(struct xwos_tt * xwtt, struct xwos_ttn * ttn)
{
        struct xwos_ttn * n;

        n = xwlib_bclst_first_entry(&ttn->rbb, struct xwos_ttn, rbb);
        xwlib_bclst_del_init(&ttn->rbb);
        xwlib_rbtree_replace(&n->rbn, &ttn->rbn);
        xwlib_rbtree_init_node(&ttn->rbn);
        if (ttn == xwtt->leftmost) {
                xwtt->leftmost = n;
        }/* else {} */
}

/**
 * @brief 从时间树中删除节点（节点无伙伴）
 * @param xwtt: (I) 时间树的指针
 * @param ttn: (I) 时间树节点的指针
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
static __xwos_code
void xwos_tt_rmrbn_locked(struct xwos_tt * xwtt, struct xwos_ttn * ttn)
{
        struct xwlib_rbtree_node * s;

        if (ttn == xwtt->leftmost) {
                s = ttn->rbn.right; /* The successor of a min node is its
                                       right child due to the property 5 of
                                       red-black tree. Or if there is no
                                       right child, the successor is its
                                       parent. */
                if (!s) {
                        s = xwlib_rbtree_get_parent(&ttn->rbn);
                }/* else {} */
                if (s != (struct xwlib_rbtree_node *)&xwtt->rbtree.root) {
                        xwtt->leftmost = xwlib_rbtree_entry(s, struct xwos_ttn, rbn);
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
 * @param xwtt: (I) 时间树的指针
 * @param ttn: (I) 时间树节点的指针
 * @retval OK: OK
 * @retval -ESRCH: 时间树中不存在该节点
 * @note
 * - 此函数只能在获得写锁xwtt->lock，且CPU中断被关闭时调用。
 */
__xwos_code
xwer_t xwos_tt_remove_locked(struct xwos_tt * xwtt, struct xwos_ttn * ttn)
{
        xwer_t rc;

        if (__unlikely((NULL == ttn->cb) || (xwtt != ttn->xwtt))) {
                rc = -ESRCH;
        } else {
                if (xwlib_rbtree_tst_node_unlinked(&ttn->rbn)) {
                        xwlib_bclst_del_init(&ttn->rbb);
                } else if (!xwlib_bclst_tst_empty(&ttn->rbb)) {
                        xwos_tt_rmrbb_locked(xwtt, ttn);
                } else {
                        xwos_tt_rmrbn_locked(xwtt, ttn);
                }
                ttn->xwtt = NULL;
                ttn->wkuprs = XWOS_TTN_WKUPRS_INTR;
                ttn->cb = NULL;
                rc = OK;
        }
        return rc;
}

/**
 * @brief 检查时间树中超时的节点
 * @param xwtt: (I) 时间树的指针
 */
__xwos_isr
xwer_t xwos_tt_check_deadline(struct xwos_tt * xwtt)
{
        struct xwos_ttn * leftmost;
        xwtm_t tick;
        xwer_t rc;

        rc = OK;
        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
        for (tick = xwos_syshwt_get_timetick(&xwtt->hwt);
             ((NULL != xwtt->leftmost) && (xwtm_cmp(xwtt->deadline, tick) <= 0));
             tick = xwos_syshwt_get_timetick(&xwtt->hwt)) {
                leftmost = xwtt->leftmost;
                xwos_tt_rmrbn_locked(xwtt, leftmost);
                xwlib_bclst_insseg_tail(&xwtt->timeout, &leftmost->rbb);
                rc = -ETIMEDOUT;
        }
        xwlk_sqlk_wr_unlock_cpuirq(&xwtt->lock);
        return rc;
}

/**
 * @brief 时间树的中断底半部
 * @param xwtt: (I) 时间树的指针
 */
static __xwos_bh
void xwos_tt_bh(struct xwos_tt * xwtt)
{
        struct xwos_ttn * ttn;
        xwos_tt_cb_f cb;

        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwlib_bclst_itr_prev_entry_del(ttn, &xwtt->timeout, struct xwos_ttn, rbb) {
                xwlib_bclst_del_init(&ttn->rbb);
                cb = ttn->cb;
                ttn->wkuprs = XWOS_TTN_WKUPRS_TIMEDOUT;
                ttn->cb = NULL;
                xwlk_sqlk_wr_unlock_cpuirq(&xwtt->lock);
                cb(xwos_ttn_get_entry(ttn));
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
        }
        xwlk_sqlk_wr_unlock_cpuirq(&xwtt->lock);
        xwos_scheduler_chkpmpt();
}

/**
 * @brief 得到时间树所属的调度器
 * @param xwtt: (I) 时间树的指针
 * @return 调度器的指针
 */
__xwos_code
struct xwos_scheduler * xwos_tt_get_scheduler(struct xwos_tt * xwtt)
{
        return container_of(xwtt, struct xwos_scheduler, tt);
}

/**
 * @brief 初始化硬件定时器结构体
 * @param hwt: (I) 硬件定时器结构体的指针
 * @return 错误码
 */
__xwos_code
xwer_t xwos_syshwt_init(struct xwos_syshwt * hwt)
{
        xwer_t rc;

        hwt->timetick = (xwtm_t)(-(XWUPCFG_SYSHWT_PERIOD));
        hwt->irqrsc = NULL;
        hwt->irqs_num = 0;
        xwlk_sqlk_init(&hwt->lock);
        /* port code of CPU must set the irqc & irqrsc */
        rc = soc_syshwt_init(hwt);
        if (__likely(OK == rc)) {
                XWOS_BUG_ON(NULL == hwt->irqrsc);
                XWOS_BUG_ON(0 == hwt->irqs_num);
        }/* else {} */
        return rc;
}

/**
 * @brief 开启硬件定时器
 * @param hwt: (I) 硬件定时器结构体的指针
 * @return 错误码
 */
__xwos_code
xwer_t xwos_syshwt_start(struct xwos_syshwt * hwt)
{
        return soc_syshwt_start(hwt);
}

/**
 * @brief 关闭硬件定时器
 * @param hwt: (I) 硬件定时器结构体的指针
 */
__xwos_code
xwer_t xwos_syshwt_stop(struct xwos_syshwt * hwt)
{
        return soc_syshwt_stop(hwt);
}

/**
 * @brief XWOS API：得到系统滴答时间
 * @param hwt: (I) 硬件定时器结构体的指针
 * @return 系统滴答时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwtm_t xwos_syshwt_get_timetick(struct xwos_syshwt * hwt)
{
        xwsq_t seq;
        xwtm_t timetick;

        do {
                seq = xwlk_sqlk_rd_begin(&hwt->lock);
                timetick = hwt->timetick;
        } while (xwlk_sqlk_rd_retry(&hwt->lock, seq));
        return timetick;
}

/**
 * @brief XWOS API：得到系统滴答计数
 * @param hwt: (I) 硬件定时器结构体的指针
 * @return 系统滴答计数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwu64_t xwos_syshwt_get_tickcount(struct xwos_syshwt * hwt)
{
        return (xwu64_t)xwos_syshwt_get_timetick(hwt) / XWUPCFG_SYSHWT_PERIOD;
}

/**
 * @brief XWOS API：得到系统定时器的时间戳
 * @param hwt: (I) 硬件定时器结构体的指针
 * @return 时间戳
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwtm_t xwos_syshwt_get_timestamp(struct xwos_syshwt * hwt)
{
        xwsq_t seq;
        xwtm_t ts;
        xwtm_t timeconfetti;

        do {
                seq = xwlk_sqlk_rd_begin(&hwt->lock);
                ts = hwt->timetick;
                timeconfetti = soc_syshwt_get_timeconfetti(hwt);
                ts = xwtm_add(ts, timeconfetti);
        } while (xwlk_sqlk_rd_retry(&hwt->lock, seq));
        return ts;
}

/**
 * @brief 系统滴答中断任务
 * @param hwt: (I) 硬件定时器结构体的指针
 */
__xwos_isr
void xwos_syshwt_task(struct xwos_syshwt * hwt)
{
        struct xwos_tt * xwtt;
        xwer_t rc;
        xwreg_t flags[hwt->irqs_num];

        xwtt = xwos_syshwt_get_tt(hwt);
        xwlk_sqlk_wr_lock_irqssv(&hwt->lock, hwt->irqrsc, flags, hwt->irqs_num);
        hwt->timetick = xwtm_add(hwt->timetick, XWUPCFG_SYSHWT_PERIOD);
        xwlk_sqlk_wr_unlock_irqsrs(&hwt->lock, hwt->irqrsc, flags, hwt->irqs_num);
        rc = xwos_tt_check_deadline(xwtt);
        if (-ETIMEDOUT == rc) {
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
                struct xwos_scheduler * xwsd;

                xwsd = xwos_tt_get_scheduler(xwtt);
                xwos_bh_node_eq(&xwsd->bhcb, &xwtt->bhn);
                xwos_scheduler_req_bh();
#else /* XWUPCFG_SD_BH */
                xwos_tt_bh(xwtt);
#endif /* !XWUPCFG_SD_BH */
        }
        xwos_scheduler_chkpmpt();
}

/**
 * @brief 得到硬件定时器的时间树
 * @param hwt: (I) 硬件定时器结构体的指针
 * @return 时间树的指针
 */
__xwos_code
struct xwos_tt * xwos_syshwt_get_tt(struct xwos_syshwt * hwt)
{
        return container_of(hwt, struct xwos_tt, hwt);
}
