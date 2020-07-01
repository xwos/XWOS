/**
 * @file
 * @brief XuanWuOS内核：调度器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 函数suffix意义：
 *   1. _lc: Local Context，是指只能在“本地”CPU的中执行的代码；
 *   2. _lic: Local ISR Context，是指只能在“本地”CPU的中断上下文中执行的代码。
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/mm/kma.h>
#include <soc_sched.h>
#include <xwos/up/irq.h>
#include <xwos/up/thread.h>
#include <xwos/up/rtrq.h>
#include <xwos/up/tt.h>
#include <xwos/up/scheduler.h>
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
  #include <xwos/up/bh.h>
#endif /* XWUPCFG_SD_BH */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS调度器
 */
__xwos_data struct xwos_scheduler xwos_scheduler;

/**
 * @brief 调度器的空闲线程栈
 */
__xwos_data __aligned_l1cacheline
xwu8_t xwos_scheduler_idled_stack[XWUPCFG_SD_IDLE_STACK_SIZE];

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
/**
 * @brief 调度器的中断底半部栈
 */
__xwos_data __aligned_l1cacheline
xwu8_t xwos_scheduler_bhd_stack[XWUPCFG_SD_BH_STACK_SIZE];
#endif /* XWUPCFG_SD_BH */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwos_code
void bdl_xwsd_idle_hook(struct xwos_scheduler * xwsd);

static __xwos_code
struct xwos_tcb * xwos_scheduler_rtrq_choose(void);

static __xwos_code
xwer_t xwos_scheduler_idled(struct xwos_scheduler * xwsd);

static __xwos_code
void xwos_scheduler_init_idled(void);

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
static __xwos_code
xwer_t xwos_scheduler_bhd(struct xwos_scheduler * xwsd);

static __xwos_code
void xwos_scheduler_init_bhd(void);
#endif /* XWUPCFG_SD_BH */

static __xwos_code
bool xwos_scheduler_do_chkpmpt(struct xwos_tcb * t);

static __xwos_code
xwer_t xwos_scheduler_check_swcx(struct xwos_tcb * t,
                                 struct xwos_tcb ** pmtcb);

static __xwos_code
xwer_t xwos_scheduler_do_swcx(void);

#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
static __xwos_code
void xwos_scheduler_notify_allfrz_lc(void);

static __xwos_code
xwer_t xwos_scheduler_thaw_allfrz_lic(void);
#endif /* XWUPCFG_SD_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS INIT API：初始化本地CPU的调度器
 * @return 错误码
 * @note
 * - 重入性：本函数只可在系统初始化时调用一次
 */
__xwos_init_code
xwer_t xwos_scheduler_init(void)
{
        xwer_t rc;
        struct xwos_scheduler * xwsd;

        xwsd = &xwos_scheduler;
        xwsd->cstk = XWOS_SCHEDULER_IDLE_STK(xwsd);
        xwsd->pstk = NULL;
        xwsd->req_schedule_cnt = 0;
        xwsd->req_chkpmpt_cnt = 0;
        xwsd->dis_pmpt_cnt = 0;
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
        xwsd->req_bh_cnt = 0;
        xwsd->dis_bh_cnt = 0;
        xwos_bh_cb_init(&xwsd->bhcb);
        xwos_scheduler_init_bhd();
#endif /* XWUPCFG_SD_BH */
        xwlib_bclst_init_head(&xwsd->tcblist);
        xwsd->thrd_num = 0;
        rc = xwos_tt_init(&xwsd->tt);
        if (__unlikely(rc < 0)) {
                goto err_tt_init;
        }
        xwos_rtrq_init(&xwsd->rq.rt);
        xwos_scheduler_init_idled();
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
        xwsd->pm.wklkcnt = XWOS_SCHEDULER_WKLKCNT_UNLOCKED;
        xwsd->pm.frz_thrd_cnt = 0;
        xwlib_bclst_init_head(&xwsd->pm.frzlist);
        xwsd->pm.cb.resume = NULL;
        xwsd->pm.cb.suspend = NULL;
        xwsd->pm.cb.wakeup = NULL;
        xwsd->pm.cb.sleep = NULL;
        xwsd->pm.cb.arg = NULL;
#endif /* XWUPCFG_SD_PM */
        rc = soc_scheduler_init(xwsd);
err_tt_init:
        return rc;
}

/**
 * @brief XWOS INIT API：启动本地CPU的调度器
 * @return 错误码
 * @note
 * - 重入性：只可调用一次
 * - 此函数不会返回
 */
__xwos_init_code
xwer_t xwos_scheduler_start_lc(void)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        rc = xwos_syshwt_start(&xwsd->tt.hwt);
        if (__unlikely(OK == rc)) {
                soc_scheduler_start_lc(xwsd);
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：启动本地CPU调度器的系统定时器
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_scheduler_start_syshwt(void)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        rc = xwos_syshwt_start(&xwsd->tt.hwt);
        return rc;
}

/**
 * @brief 关闭本地CPU调度器的系统定时器
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_scheduler_stop_syshwt(void)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        rc = xwos_syshwt_stop(&xwsd->tt.hwt);
        return rc;
}

/**
 * @brief 返回本地CPU调度器的指针
 * @return 本地CPU调度器的指针
 */
__xwos_code
struct xwos_scheduler * xwos_scheduler_get_lc(void)
{
        return &xwos_scheduler;
}

/**
 * @brief 获得本地CPU的调度器中正在运行的线程的线程控制块指针
 * @return 本地CPU的调度器中正在运行的线程的线程控制块指针
 */
__xwos_code
struct xwos_tcb * xwos_scheduler_get_ctcb_lc(void)
{
        struct xwos_tcb * ctcb;

        ctcb = container_of(xwos_scheduler.cstk, struct xwos_tcb, stack);
        return ctcb;
}

/**
 * @brief 从XWOS调度器中选择一个就绪的线程
 * @retval pointer: 被选择线程的线程控制块对象指针
 * @retval NULL: 空闲任务
 * @note
 * - 此函数只能在临界区中调用。
 */
static __xwos_code
struct xwos_tcb * xwos_scheduler_rtrq_choose(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;
        struct xwos_tcb * t;

        xwsd = &xwos_scheduler;
        xwrtrq = &xwsd->rq.rt;
        t = xwos_rtrq_choose(xwrtrq);
        if (__likely(NULL != t)) {
                xwos_rtrq_remove(xwrtrq, t);
                xwbop(xwsq_t, c0m, &t->state, XWSDOBJ_DST_READY);
                xwbop(xwsq_t, s1m, &t->state, XWSDOBJ_DST_RUNNING);
        }/* else {} */
        return t;
}

/**
 * @brief 空闲任务的主函数
 * @param xwsd: (I) XWOS调度器的指针
 */
static __xwos_code
xwer_t xwos_scheduler_idled(struct xwos_scheduler * xwsd)
{
        XWOS_UNUSED(xwsd);

        while (true) {
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
                xwos_scheduler_notify_allfrz_lc();
#endif /* XWUPCFG_SD_PM */
#if (defined(BRDCFG_XWSD_IDLE_HOOK) && (1 == BRDCFG_XWSD_IDLE_HOOK))
                bdl_xwsd_idle_hook(xwsd);
#endif /* #if (defined(BRDCFG_XWSD_IDLE_HOOK) && (1 == BRDCFG_XWSD_IDLE_HOOK)) */
        }
        return OK;
}

/**
 * @brief 初始化空闲线程栈
 */
static __xwos_code
void xwos_scheduler_init_idled(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = &xwos_scheduler;
        xwsd->idle.name = NULL;
        xwsd->idle.main = (xwos_thrd_f)xwos_scheduler_idled;
        xwsd->idle.arg = xwsd;
        xwsd->idle.size = XWUPCFG_SD_IDLE_STACK_SIZE;
        xwsd->idle.base = (xwstk_t *)xwos_scheduler_idled_stack;
#if (defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK))
        xwsd->idle.sp = xwsd->idle.base + (xwsd->idle.size >> 2);
#elif (defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK))
        xwsd->idle.sp = xwsd->idle.base + (xwsd->idle.size >> 2) - 1;
#elif (defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK))
        xwsd->idle.sp = xwsd->idle.base - 1;
#elif (defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK))
        xwsd->idle.sp = xwsd->idle.base;
#else
  #error "Unknown stack type!"
#endif
        soc_scheduler_init_sdobj_stack(&xwsd->idle, XWSDOBJ_ATTR_PRIVILEGED);
}

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
/**
 * @brief 中断底半部的主函数
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwos_scheduler_bhd(struct xwos_scheduler * xwsd)
{
        struct xwos_bh_node * bhn;

        xwos_cpuirq_disable_lc();
        while (1) {
                if (!xwlib_bclst_tst_empty(&xwsd->bhcb.list)) {
                        bhn = xwlib_bclst_first_entry(&xwsd->bhcb.list,
                                                      struct xwos_bh_node,
                                                      node);
                        xwlib_bclst_del_init(&bhn->node);
                        xwos_cpuirq_enable_lc();
                        bhn->func(bhn->arg);
                        xwos_cpuirq_disable_lc();
                }/* else {} */
                xwsd->req_bh_cnt--;
                if (0 == xwsd->req_bh_cnt) {
                        xwsd->cstk = xwsd->pstk;
                        xwsd->pstk = XWOS_SCHEDULER_BH_STK(xwsd);
                        xwos_cpuirq_enable_lc();
                        soc_scheduler_req_swcx(xwsd);
                        xwos_cpuirq_disable_lc();
                }/* else {} */
        }
        xwos_cpuirq_enable_lc();
        return OK;
}

/**
 * @brief 初始化中断底半部栈
 */
static __xwos_code
void xwos_scheduler_init_bhd(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = &xwos_scheduler;
        xwsd->bh.name = NULL;
        xwsd->bh.main = (xwos_thrd_f)xwos_scheduler_bhd;
        xwsd->bh.arg = xwsd;
        xwsd->bh.size = XWUPCFG_SD_BH_STACK_SIZE;
        xwsd->bh.base = (xwstk_t *)xwos_scheduler_bhd_stack;
#if defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK)
        xwsd->bh.sp = xwsd->bh.base + (xwsd->bh.size >> 2);
#elif defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK)
        xwsd->bh.sp = xwsd->bh.base + (xwsd->bh.size >> 2) - 1;
#elif defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK)
        xwsd->bh.sp = xwsd->bh.base - 1;
#elif defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK)
        xwsd->bh.sp = xwsd->bh.base;
#else
  #error "Unknown stack type!"
#endif
        soc_scheduler_init_sdobj_stack(&xwsd->bh, XWSDOBJ_ATTR_PRIVILEGED);
}

/**
 * @brief XWOS API：禁止本地CPU调度器进入中断底半部
 * @return XWOS调度器的指针
 */
__xwos_api
struct xwos_scheduler * xwos_scheduler_dsbh_lc(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwsd->dis_bh_cnt++;
        xwos_cpuirq_restore_lc(cpuirq);
        return xwsd;
}

/**
 * @brief XWOS API：允许本地CPU调度器进入中断底半部
 * @return XWOS调度器的指针
 */
__xwos_api
struct xwos_scheduler * xwos_scheduler_enbh_lc(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        bool sched;

        sched = false;
        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwsd->dis_bh_cnt--;
        if (0 == xwsd->dis_bh_cnt) {
                if (0 != xwsd->req_bh_cnt) {
                        if (!xwos_scheduler_tst_in_bh_lc()) {
                                xwsd->pstk = xwsd->cstk;
                                xwsd->cstk = XWOS_SCHEDULER_BH_STK(xwsd);
                                sched = true;
                        }/* else {} */
                }/* else {} */
        }
        xwos_cpuirq_restore_lc(cpuirq);
        if (sched) {
                soc_scheduler_req_swcx(xwsd);
        }/* else {} */
        return xwsd;
}

/**
 * @brief 请求切换至中断底半部
 * @return 错误码
 * @retval OK: OK
 * @retval -EPERM: 中断底半部已被禁止
 * @retval -EINPROGRESS: 切换上下文的过程正在进行中
 * @retval -EALREADY: 当前上下文已经是中断底半部上下文
 */
__xwos_code
xwer_t xwos_scheduler_req_bh(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwsd->req_bh_cnt++;
        if (0 == xwsd->dis_bh_cnt) {
                if (__unlikely(NULL != xwsd->pstk)) {
                        rc = -EINPROGRESS;
                } else if (__unlikely(XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk)) {
                        rc = -EALREADY;
                } else {
                        xwsd->pstk = xwsd->cstk;
                        xwsd->cstk = XWOS_SCHEDULER_BH_STK(xwsd);
                        rc = OK;
                }
        } else {
                rc = -EPERM;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        if (OK == rc) {
                soc_scheduler_req_swcx(xwsd);
        }/* else {} */
        return rc;
}
#endif /* XWUPCFG_SD_BH */

/**
 * @brief XWOS API：禁止抢占
 * @return XWOS调度器的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
struct xwos_scheduler * xwos_scheduler_dspmpt_lc(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwsd->dis_pmpt_cnt++;
        xwos_cpuirq_restore_lc(cpuirq);
        return xwsd;
}

/**
 * @brief XWOS API：允许抢占
 * @return XWOS调度器的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
struct xwos_scheduler * xwos_scheduler_enpmpt_lc(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tcb * t;
        bool sched;
        xwreg_t cpuirq;

        sched = false;
        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwsd->dis_pmpt_cnt--;
        if (0 == xwsd->dis_pmpt_cnt) {
                if (0 != xwsd->req_chkpmpt_cnt) {
                        struct xwos_sdobj_stack_info * cstk;

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
                        if (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk) {
                                cstk = xwsd->pstk;
                        } else {
                                cstk = xwsd->cstk;
                        }
#else /* XWUPCFG_SD_BH */
                        cstk = xwsd->cstk;
#endif /* !XWUPCFG_SD_BH */
                        if (XWOS_SCHEDULER_IDLE_STK(xwsd) != cstk) {
                                t = container_of(cstk, struct xwos_tcb, stack);
                                sched = xwos_scheduler_do_chkpmpt(t);
                        } else {
                                sched = true;
                        }
                        xwsd->req_chkpmpt_cnt = 0;
                }
        }
        xwos_cpuirq_restore_lc(cpuirq);
        if (sched) {
                xwos_scheduler_req_swcx();
        }/* else {} */
        return xwsd;
}

/**
 * @brief 检查是否需要抢占
 * @param xwsd: (I) XWOS调度器的指针
 * @param t: (I) 线程控制块对象的指针
 * @return 布尔值
 * @retval true: 需要抢占
 * @retval false: 不需要抢占
 * @note
 * - 此函数被调用时需要关闭本地CPU的中断。
 */
static __xwos_code
bool xwos_scheduler_do_chkpmpt(struct xwos_tcb * t)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;
        bool sched;

        xwsd = &xwos_scheduler;
        xwrtrq = &xwsd->rq.rt;
        if (XWSDOBJ_DST_RUNNING & t->state) {
                if (t->prio.d >= xwrtrq->top) {
                        sched = false;
                } else {
                        sched = true;
                }
        } else {
                /* The thread will give up the CPU in a jiffy.
                   We don't check the preemption */
                sched = false;
        }
        return sched;
}

/**
 * @brief 调度器检测抢占
 */
__xwos_code
void xwos_scheduler_chkpmpt(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_sdobj_stack_info * cstk;
        struct xwos_tcb * t;
        xwreg_t cpuirq;
        bool sched;

        xwsd = &xwos_scheduler;
        sched = false;
        xwos_cpuirq_save_lc(&cpuirq);
        if (0 != xwsd->dis_pmpt_cnt) {
                xwsd->req_chkpmpt_cnt++;
        } else {
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
                if (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk) {
                        cstk = xwsd->pstk;
                } else {
                        cstk = xwsd->cstk;
                }
#else /* XWUPCFG_SD_BH */
                cstk = xwsd->cstk;
#endif /* !XWUPCFG_SD_BH */
                if (XWOS_SCHEDULER_IDLE_STK(xwsd) != cstk) {
                        t = container_of(cstk, struct xwos_tcb, stack);
                        sched = xwos_scheduler_do_chkpmpt(t);
                } else {
                        sched = true;
                }
                xwsd->req_chkpmpt_cnt = 0;

        }
        xwos_cpuirq_restore_lc(cpuirq);
        if (sched) {
                xwos_scheduler_req_swcx();
        }/* else {} */
}

/**
 * @brief 检测调度器是否需要切换上下文
 * @param t: (I) 被检测的线程的控制块对象的指针
 * @param pmtcb: (O) 指向缓冲区的指针，通过此缓冲区返回指向抢占线程控制块对象的指针
 * @return 错误码
 * @retval OK: 需要切换上下文
 * @retval <0: 不需要切换上下文
 * @note
 * - 此函数被调用时需要关闭本地CPU的中断。
 */
static __xwos_code
xwer_t xwos_scheduler_check_swcx(struct xwos_tcb * t, struct xwos_tcb ** pmtcb)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        xwrtrq = &xwsd->rq.rt;
        if (XWSDOBJ_DST_RUNNING & t->state) {
                if (t->prio.d >= xwrtrq->top) {
                        rc = -EPERM;
                } else {
                        xwbop(xwsq_t, c0m, &t->state, XWSDOBJ_DST_RUNNING);
                        rc = xwos_thrd_rq_add_head(t);
                        XWOS_BUG_ON(rc < 0);
                        *pmtcb = xwos_scheduler_rtrq_choose();
                        rc = OK;
                }
        } else {
                *pmtcb = xwos_scheduler_rtrq_choose();
                rc = OK;
        }
        return rc;
}

/**
 * @brief 执行上下文的切换
 * @param xwsd: (I) XWOS调度器的指针
 * @retval rc: @ref xwos_scheduler_req_swcx()
 */
static __xwos_code
xwer_t xwos_scheduler_do_swcx(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tcb * swt, * ctcb;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        if (XWOS_SCHEDULER_IDLE_STK(xwsd) == xwsd->cstk) {
                swt = xwos_scheduler_rtrq_choose();
                if (NULL != swt) {
                        xwsd->pstk = xwsd->cstk;
                        xwsd->cstk = &swt->stack;
                        rc = OK;
                } else {
                        rc = -EAGAIN;
                }
        } else {
                ctcb = xwos_scheduler_get_ctcb_lc();
                rc = xwos_scheduler_check_swcx(ctcb, &swt);
                if (OK == rc) {
                        if (ctcb == swt) {
                                /* 当前线程的状态可能在中断中被改变，
                                   并被加入到就绪队列，然后在这里又被重新选择。
                                   这种情况下，调度器只需要恢复这个线程的
                                   @ref XWSDOBJ_DST_RUNNING 状态，
                                   并不需要切换线程上下文。*/
                                rc = -EAGAIN;
                        } else {
                                if (NULL == swt) {
                                        xwsd->cstk = XWOS_SCHEDULER_IDLE_STK(xwsd);
                                } else {
                                        xwsd->cstk = &swt->stack;
                                }
                                xwsd->pstk = &ctcb->stack;
                                rc = OK;
                        }
                } else {
                        rc = -EINVAL;
                }
        }
        return rc;
}

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
/**
 * @brief 请求切换上下文
 * @return 错误码
 * @retval OK: OK，需要触发切换上下文中断执行切换上下文的过程
 * @retval -EBUSY: 当前上下文为中断底半部上下文
 * @retval -EINVAL: 正在运行的线程状态错误
 * @retval -EAGAIN: 不需要切换上下文
 * @retval -EEINPROGRESS: 切换上下文的过程正在进行
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwos_scheduler_req_swcx(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwsd->req_schedule_cnt++;
        if (__unlikely(NULL != xwsd->pstk)) {
                xwos_cpuirq_restore_lc(cpuirq);
                rc = -EINPROGRESS;
        } else if (__unlikely(XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk)) {
                xwos_cpuirq_restore_lc(cpuirq);
                rc = -EBUSY;
        } else {
                xwsd->pstk = err_ptr(-EINVAL); /* invalidate other caller. */
                rc = xwos_scheduler_do_swcx();
                xwos_cpuirq_restore_lc(cpuirq);
                if (OK == rc) {
                        soc_scheduler_req_swcx(xwsd);
                } else {
                        xwos_scheduler_finish_swcx();
                }
        }
        return rc;
}

/**
 * @brief 结束上下文的切换
 * @note
 * - 此函数需要在切换上下文的中断的最后调用。
 */
__xwos_code
void xwos_scheduler_finish_swcx(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        if (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk) {
                /* Finish switch context from thread to BH */
                xwos_cpuirq_restore_lc(cpuirq);
        } else if (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->pstk) {
                /* Finish switch context from BH to thread */
                xwsd->pstk = NULL;
                if (xwsd->req_bh_cnt >= 1) {
                        xwsd->pstk = xwsd->cstk;
                        xwsd->cstk = XWOS_SCHEDULER_BH_STK(xwsd);
                        xwos_cpuirq_restore_lc(cpuirq);
                        soc_scheduler_req_swcx(xwsd);
                } else if (xwsd->req_schedule_cnt > 0) {
                        xwsd->req_schedule_cnt = 0;
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwos_scheduler_req_swcx();
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                }
        } else {
                /* Finish switch context from thread to thread */
                xwsd->pstk = NULL;
                xwsd->req_schedule_cnt--;
                if (xwsd->req_bh_cnt >= 1) {
                        xwsd->pstk = xwsd->cstk;
                        xwsd->cstk = XWOS_SCHEDULER_BH_STK(xwsd);
                        xwos_cpuirq_restore_lc(cpuirq);
                        soc_scheduler_req_swcx(xwsd);
                } else if (xwsd->req_schedule_cnt > 0) {
                        xwsd->req_schedule_cnt = 0;
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwos_scheduler_req_swcx();
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                }
        }
}
#else /* XWUPCFG_SD_BH */
/**
 * @brief 请求切换上下文
 * @return 错误码
 * @retval OK: OK，需要触发上下文切换中断执行切换上下文的过程
 * @retval -EBUSY: 当前上下文为中断底半部上下文
 * @retval -EINVAL: 当前正在运行的线程状态错误
 * @retval -EAGAIN: 不需要切换上下文
 * @retval -EEINPROGRESS: 切换上下文的过程正在进行
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwos_scheduler_req_swcx(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwsd->req_schedule_cnt++;
        if (__unlikely(NULL != xwsd->pstk)) {
                xwos_cpuirq_restore_lc(cpuirq);
                rc = -EINPROGRESS;
        } else {
                xwsd->pstk = err_ptr(-EINVAL); /* invalidate other caller. */
                rc = xwos_scheduler_do_swcx();
                xwos_cpuirq_restore_lc(cpuirq);
                if (OK == rc) {
                        soc_scheduler_req_swcx(xwsd);
                } else {
                        xwos_scheduler_finish_swcx();
                }
        }
        return rc;
}

/**
 * @brief 结束上下文的切换
 * @note
 * - 此函数需在切换上下文的中断的最后调用。
 */
__xwos_code
void xwos_scheduler_finish_swcx(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        /* Finish switch context from thread to thread */
        xwsd->pstk = NULL;
        xwsd->req_schedule_cnt--;
        if (xwsd->req_schedule_cnt > 0) {
                xwsd->req_schedule_cnt = 0;
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_req_swcx();
        } else {
                xwos_cpuirq_restore_lc(cpuirq);
        }
}
#endif /* !XWUPCFG_SD_BH */

/**
 * @brief 中断调度器中所有阻塞/睡眠态的线程
 */
__xwos_code
void xwos_scheduler_intr_all(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tcb * c, * n;
        xwreg_t cpuirq;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwlib_bclst_itr_next_entry_safe(c, n, &xwsd->tcblist,
                                        struct xwos_tcb, tcbnode) {
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_thrd_intr(c);
                xwos_cpuirq_disable_lc();
        }
        xwos_cpuirq_restore_lc(cpuirq);
}

#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
__xwos_api
void xwos_scheduler_set_pm_cb(xwos_scheduler_pm_cb_f resume_cb,
                              xwos_scheduler_pm_cb_f suspend_cb,
                              xwos_scheduler_pm_cb_f wakeup_cb,
                              xwos_scheduler_pm_cb_f sleep_cb,
                              void * arg)
{
        struct xwos_scheduler * xwsd;

        xwsd = &xwos_scheduler;
        xwsd->pm.cb.resume = resume_cb;
        xwsd->pm.cb.suspend = suspend_cb;
        xwsd->pm.cb.wakeup = wakeup_cb;
        xwsd->pm.cb.sleep = sleep_cb;
        xwsd->pm.cb.arg = arg;
}

/**
 * @brief 将调度器的唤醒锁计数器加1
 * @retval OK: OK
 * @retval <0: 当前调度器正在进入低功耗
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwos_scheduler_inc_wklkcnt(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        if (xwsd->pm.wklkcnt >= XWOS_SCHEDULER_WKLKCNT_UNLOCKED) {
                xwsd->pm.wklkcnt++;
                rc = OK;
        } else {
                rc = -EACCES;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief 将调度器的唤醒锁计数器减1
 * @return 错误码
 * @retval OK: OK
 * @retval <0: 当前调度器正在进入低功耗
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwos_scheduler_dec_wklkcnt(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;
        bool lpm;

        lpm = false;
        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        if (xwsd->pm.wklkcnt >= XWOS_SCHEDULER_WKLKCNT_UNLOCKED) {
                xwsd->pm.wklkcnt--;
                if (XWOS_SCHEDULER_WKLKCNT_FREEZING == xwsd->pm.wklkcnt) {
                        lpm = true;
                }/* else {} */
                rc = OK;
        } else {
                rc = -EACCES;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        if (lpm) {
                soc_scheduler_suspend(xwsd);
        }/* else {} */
        return rc;
}

/**
 * @brief 暂停调度器（中断中执行）
 * @param xwsd: (I) 调度器对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在CPU自身的调度器服务中断中执行，可通过@ref soc_scheduler_suspend()
 *   触发并执行此函数。
 */
__xwos_code
xwer_t xwos_scheduler_suspend_lic(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;

        xwos_scheduler_intr_all();
        xwos_cpuirq_save_lc(&cpuirq);
        if (xwsd->thrd_num == xwsd->pm.frz_thrd_cnt) {
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_notify_allfrz_lic();
        } else {
                xwos_cpuirq_restore_lc(cpuirq);
        }
        return OK;
}

/**
 * @brief 通知所有线程已经冻结（中断上下文中执行的部分）
 * @return 错误码
 */
__xwos_code
xwer_t xwos_scheduler_notify_allfrz_lic(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        if (XWOS_SCHEDULER_WKLKCNT_FREEZING == xwsd->pm.wklkcnt) {
                xwsd->pm.wklkcnt--;
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_syshwt_stop(&xwsd->tt.hwt);
                rc = OK;
        } else {
                xwos_cpuirq_restore_lc(cpuirq);
                rc = -EINTR;
        }
        xwos_scheduler_req_swcx();
        return rc;
}

/**
 * @brief 通知所有线程已经冻结（空闲任务中执行的部分）
 */
static __xwos_code
void xwos_scheduler_notify_allfrz_lc(void)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        xwsd = &xwos_scheduler;
        if (XWOS_SCHEDULER_WKLKCNT_ALLFRZ == xwsd->pm.wklkcnt) {
                xwos_cpuirq_save_lc(&cpuirq);
                if (XWOS_SCHEDULER_WKLKCNT_ALLFRZ == xwsd->pm.wklkcnt) {
                        xwsd->pm.wklkcnt--;
                        if (xwsd->pm.cb.suspend) {
                                xwsd->pm.cb.suspend(xwsd->pm.cb.arg);
                        }/* else {} */
                        xwsd->pm.wklkcnt--;
                        xwos_cpuirq_restore_lc(cpuirq);
                        while (XWOS_SCHEDULER_WKLKCNT_SUSPENDED == xwsd->pm.wklkcnt) {
                                if (xwsd->pm.cb.sleep) {
                                        xwsd->pm.cb.sleep(xwsd->pm.cb.arg);
                                }/* else {} */
                        }
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                }
        }
}

/**
 * @brief 解冻所有线程
 * @return 错误码
 */
static __xwos_code
xwer_t xwos_scheduler_thaw_allfrz_lic(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tcb * c, * n;
        xwreg_t cpuirq;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        xwos_cpuirq_save_lc(&cpuirq);
        xwlib_bclst_itr_next_entry_safe(c, n, &xwsd->pm.frzlist,
                                        struct xwos_tcb, frznode) {
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_thrd_thaw_lic(c);
                xwos_cpuirq_disable_lc();
        }
        if (0 == xwsd->pm.frz_thrd_cnt) {
                rc = OK;
        } else {
                rc = -EBUG;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        xwos_scheduler_req_swcx();
        return rc;
}

__xwos_api
xwer_t xwos_scheduler_suspend(void)
{
        return xwos_scheduler_dec_wklkcnt();
}

/**
 * @brief 继续已经暂停的调度器（在中断中执行）
 * @param xwsd: (I) 调度器对象的指针
 * @return 错误码
 * @note
 * - 此函数只可由xwos_scheduler_resume()调用。
 *   在SMP架构中，SOC层soc_scheduler_resume会触发CPU自身的调度器服务中断来运行此
 *   函数，UP架构中复用了同一份BSP的适配代码。因此，此函数仅仅是为了保证编译顺利。
 */
__xwos_code
xwer_t xwos_scheduler_resume_lic(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_cpuirq_save_lc(&cpuirq);
        do {
                if (XWOS_SCHEDULER_WKLKCNT_SUSPENDED == xwsd->pm.wklkcnt) {
                        xwsd->pm.wklkcnt++;
                        xwos_cpuirq_restore_lc(cpuirq);
                        if (xwsd->pm.cb.wakeup) {
                                xwsd->pm.cb.wakeup(xwsd->pm.cb.arg);
                        }/* else {} */
                        xwos_cpuirq_save_lc(&cpuirq);
                }
                if (XWOS_SCHEDULER_WKLKCNT_RESUMING == xwsd->pm.wklkcnt) {
                        xwsd->pm.wklkcnt++;
                        if (xwsd->pm.cb.resume) {
                                xwsd->pm.cb.resume(xwsd->pm.cb.arg);
                        }/* else {} */
                }
                if (XWOS_SCHEDULER_WKLKCNT_ALLFRZ == xwsd->pm.wklkcnt) {
                        xwsd->pm.wklkcnt++;
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwos_syshwt_start(&xwsd->tt.hwt);
                        xwos_cpuirq_save_lc(&cpuirq);
                }
                if (XWOS_SCHEDULER_WKLKCNT_THAWING == xwsd->pm.wklkcnt) {
                        xwsd->pm.wklkcnt++;
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwos_scheduler_thaw_allfrz_lic();
                        xwos_cpuirq_save_lc(&cpuirq);
                        rc = OK;
                        break;
                }
                if (xwsd->pm.wklkcnt >= XWOS_SCHEDULER_WKLKCNT_UNLOCKED) {
                        rc = -EALREADY;
                        break;
                }
        } while (true);
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
}

__xwos_api
xwer_t xwos_scheduler_resume(void)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;

        xwsd = &xwos_scheduler;
        if (OK != xwos_irq_get_id(NULL)) {
                rc = soc_scheduler_resume(xwsd);
        } else {
                rc = xwos_scheduler_resume_lic(xwsd);
        }
        return rc;
}

__xwos_api
xwsq_t xwos_scheduler_get_pm_state(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = &xwos_scheduler;
        return xwsd->pm.wklkcnt;
}

#else /* XWUPCFG_SD_PM */
__xwos_code
xwer_t xwos_scheduler_suspend_lic(struct xwos_scheduler * xwsd)
{
        XWOS_UNUSED(xwsd);
        return -ENOSYS;
}

__xwos_code
xwer_t xwos_scheduler_resume_lic(struct xwos_scheduler * xwsd)
{
        XWOS_UNUSED(xwsd);
        return -ENOSYS;
}

__xwos_api
xwsq_t xwos_scheduler_get_pm_state(void)
{
        return XWOS_SCHEDULER_WKLKCNT_RUNNING;
}
#endif /* !XWUPCFG_SD_PM */
