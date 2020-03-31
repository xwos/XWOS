/**
 * @file
 * @brief XuanWuOS内核：调度器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - XWOS的调度器是一个每CPU变量，为了提高程序的效率，减少cache miss，
 *   通常只由CPU才可访问自己的调度器控制块结构体。
 * - 调度时锁的顺序：同级的锁不可同时获得
 *   + ① xwos_scheduler.cxlock
 *     + ② xwos_scheduler.rq.rt.lock
 *       + ③ xwos_tcb.stlock
 * - 休眠时锁的顺序：同级的锁不可同时获得
 *   + ① xwos_scheduler.lpm.lock
 *     + ② xwos_tcb.stlock
 *     + ② xwos_scheduler.tcblistlock
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
#include <xwos/lib/xwaop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <soc_sched.h>
#include <xwos/smp/irq.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/cpu.h>
#include <xwos/smp/pm.h>
#include <xwos/smp/thread.h>
#include <xwos/smp/rtrq.h>
#include <xwos/smp/tt.h>
#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
  #include <xwos/smp/bh.h>
#endif /* XWSMPCFG_SD_BH */
#include <xwos/smp/scheduler.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 每CPU的调度器
 */
__xwos_data
struct xwos_scheduler xwos_scheduler[CPUCFG_CPU_NUM];

/**
 * @brief 每CPU的调度器的空闲线程栈
 */
__xwos_data __aligned_l1cacheline
xwu8_t xwos_scheduler_idled_stack[CPUCFG_CPU_NUM][XWSMPCFG_SD_IDLE_STACK_SIZE];

#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
/**
 * @brief 每CPU的调度器的中断底半部栈
 */
__xwos_data __aligned_l1cacheline
xwu8_t xwos_scheduler_bhd_stack[CPUCFG_CPU_NUM][XWSMPCFG_SD_BH_STACK_SIZE];
#endif /* XWSMPCFG_SD_BH */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwos_code
void bdl_xwsd_idle_hook(struct xwos_scheduler * xwsd);

static __xwos_code
struct xwos_tcb * xwos_scheduler_rtrq_choose(struct xwos_scheduler * xwsd);

static __xwos_code
xwer_t xwos_scheduler_idled(struct xwos_scheduler * xwsd);

static __xwos_code
void xwos_scheduler_init_idled(struct xwos_scheduler * xwsd);

#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
static __xwos_code
xwer_t xwos_scheduler_bhd(struct xwos_scheduler * xwsd);

static __xwos_code
void xwos_scheduler_init_bhd(struct xwos_scheduler * xwsd);

static __xwos_code
xwer_t xwos_scheduler_sw_bh(struct xwos_scheduler * xwsd);

static __xwos_code
void xwos_scheduler_bh_yield(struct xwos_scheduler * xwsd);
#endif /* XWSMPCFG_SD_BH */

static __xwos_code
bool xwos_scheduler_do_chkpmpt(struct xwos_scheduler * xwsd, struct xwos_tcb * t);

static __xwos_code
xwer_t xwos_scheduler_check_swcx(struct xwos_scheduler * xwsd,
                                 struct xwos_tcb * t,
                                 struct xwos_tcb ** pmtcb);

static __xwos_code
xwer_t xwos_scheduler_do_swcx(struct xwos_scheduler * xwsd);

static __xwos_code
void xwos_scheduler_reqfrz_intr_all_lic(struct xwos_scheduler * xwsd);

static __xwos_code
xwer_t xwos_scheduler_thaw_allfrz_lic(struct xwos_scheduler * xwsd);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS INIT API：初始化本地CPU的调度器
 * @param xwpmdm: (I) 电源管理区域的指针
 * @return 错误码
 * @note
 * - 本函数只可在系统初始化时调用一次。
 */
__xwos_init_code
xwer_t xwos_scheduler_init_lc(struct xwos_pmdm * xwpmdm)
{
        xwid_t id;
        xwer_t rc;
        struct xwos_scheduler * xwsd;

        id = xwos_cpu_get_id();
        xwsd = &xwos_scheduler[id];

        xwsd->cstk = XWOS_SCHEDULER_IDLE_STK(xwsd);
        xwsd->pstk = NULL;
        xwsd->id = id;
        xwsd->req_schedule_cnt = 0;
        xwsd->req_chkpmpt_cnt = 0;
        xwsd->dis_pmpt_cnt = 0;
#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
        xwsd->req_bh_cnt = 0;
        xwsd->dis_bh_cnt = 0;
        xwos_bh_cb_init(&xwsd->bhcb);
        xwos_scheduler_init_bhd(xwsd);
#endif /* XWSMPCFG_SD_BH */
        xwlk_splk_init(&xwsd->cxlock);
        xwlk_splk_init(&xwsd->tcblistlock);
        xwlib_bclst_init_head(&xwsd->tcblist);
        xwsd->thrd_num = 0;
        rc = xwos_tt_init(&xwsd->tt);
        if (__unlikely(rc < 0)) {
                goto err_tt_init;
        }
        rc = xwos_rtrq_init(&xwsd->rq.rt);
        if (__unlikely(rc < 0)) {
                goto err_rtrq_init;
        }
        xwos_scheduler_init_idled(xwsd);
        xwsd->lpm.wklkcnt = XWOS_SCHEDULER_WKLKCNT_UNLOCKED;
        xwsd->lpm.frz_thrd_cnt = 0;
        xwlk_splk_init(&xwsd->lpm.lock);
        xwlib_bclst_init_head(&xwsd->lpm.frzlist);
        xwsd->lpm.xwpmdm = xwpmdm;
        rc = soc_scheduler_init(xwsd);
        if (__unlikely(rc < 0)) {
                goto err_sd_init;
        }
        return OK;

err_sd_init:
        XWOS_BUG();
err_rtrq_init:
        XWOS_BUG();
err_tt_init:
        XWOS_BUG();
        return rc;
}

/**
 * @brief XWOS INIT API：启动本地CPU的调度器
 * @return 错误码
 * @note
 * - 重入性：只可调用一次
 * - 此函数不会返回。
 */
__xwos_init_code
xwer_t xwos_scheduler_start_lc(void)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;
        xwid_t id;

        id = xwos_cpu_get_id();
        xwsd = &xwos_scheduler[id];
        rc = xwos_syshwt_start(&xwsd->tt.hwt);
        if (__unlikely(OK == rc)) {
                soc_scheduler_start_lc(xwsd);
        }/* else {} */
        return rc;
}

/**
 * @brief 得到本地CPU调度器的指针
 * @return 本地CPU调度器的指针
 */
__xwos_code
struct xwos_scheduler * xwos_scheduler_get_lc(void)
{
        struct xwos_scheduler * xwsd;
        xwid_t cpuid;

        cpuid = xwos_cpu_get_id();
        xwmb_smp_ddb();
        do {
                xwsd = &xwos_scheduler[cpuid];
                cpuid = xwos_cpu_get_id();
                xwmb_smp_ddb();
        } while (xwsd->id != cpuid);
        return xwsd;
}

/**
 * @brief 得到指定ID的CPU调度器的指针
 * @param cpuid: (I) CPU ID
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回指定ID的CPU调度器的指针
 * @return 错误码
 */
__xwos_code
xwer_t xwos_scheduler_get_by_cpuid(xwid_t cpuid, struct xwos_scheduler ** ptrbuf)
{
        xwer_t rc;

        if (cpuid < XWOS_CPU_NUM) {
                *ptrbuf = &xwos_scheduler[cpuid];
                rc = OK;
        } else {
                rc = -ENODEV;
        }
        return rc;
}

/**
 * @brief 获得调度器中正在运行的线程的线程控制块指针
 * @param xwsd: (I) XWOS调度器的指针
 * @return 调度器中正在运行的线程的线程控制块指针
 */
__xwos_code
struct xwos_tcb * xwos_scheduler_get_ctcb(struct xwos_scheduler * xwsd)
{
        struct xwos_tcb * ctcb;
        ctcb = container_of(xwsd->cstk, struct xwos_tcb, stack);
        return ctcb;
}

/**
 * @brief 获得本地CPU的调度器中正在运行的线程的线程控制块指针
 * @return 本地CPU的调度器中正在运行的线程的线程控制块指针
 */
__xwos_code
struct xwos_tcb * xwos_scheduler_get_ctcb_lc(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tcb * ctcb;
        xwid_t cpuid;

        cpuid = xwos_cpu_get_id();
        xwmb_smp_ddb();
        do {
                xwsd = &xwos_scheduler[cpuid];
                ctcb = container_of(xwsd->cstk, struct xwos_tcb, stack);
                cpuid = xwos_cpu_get_id();
                xwmb_smp_ddb();
        } while (xwsd->id != cpuid);
        return ctcb;
}

/**
 * @brief 从XWOS调度器中选择一个就绪的线程
 * @param xwsd: (I) XWOS调度器的指针
 * @return 线程控制块对象指针或NULL（代表空闲任务）
 * @note
 * - 此函数只能在锁xwsd->cxlock中调用。
 */
static __xwos_code
struct xwos_tcb * xwos_scheduler_rtrq_choose(struct xwos_scheduler * xwsd)
{
        struct xwos_rtrq * xwrtrq;
        struct xwos_tcb * t;
        xwpr_t prio;
        xwer_t rc;

        xwrtrq = &xwsd->rq.rt;
        xwlk_rawly_lock(&xwrtrq->lock);
        t = xwos_rtrq_choose_locked(xwrtrq);
        if (__likely(NULL != t)) {
                rc = xwos_rtrq_remove_locked(xwrtrq, t);
                XWOS_BUG_ON(rc);
                xwlk_rawly_lock(&t->stlock);
                prio = t->dprio.rq;
                xwbop_c0m(xwsq_t, &t->state, XWSDOBJ_DST_READY);
                t->dprio.rq = XWOS_SD_PRIORITY_INVALID;
                xwbop_s1m(xwsq_t, &t->state, XWSDOBJ_DST_RUNNING);
                t->dprio.r = prio;
                xwlk_rawly_unlock(&t->stlock);
                xwlk_rawly_unlock(&xwrtrq->lock);
        } else {
                xwlk_rawly_unlock(&xwrtrq->lock);
        }
        return t;
}

/**
 * @brief 空闲任务的主函数
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwos_scheduler_idled(struct xwos_scheduler * xwsd)
{
        XWOS_UNUSED(xwsd);

        while (true) {
#if defined(BRDCFG_XWSD_IDLE_HOOK) && (1 == BRDCFG_XWSD_IDLE_HOOK)
                bdl_xwsd_idle_hook(xwsd);
#endif /* BRDCFG_XWSD_IDLE_HOOK */
        }
        return OK;
}

/**
 * @brief 初始化空闲线程栈
 * @param xwsd: (I) XWOS调度器的指针
 */
static __xwos_code
void xwos_scheduler_init_idled(struct xwos_scheduler * xwsd)
{
        xwsd->idle.name = NULL;
        xwsd->idle.main = (xwos_thrd_f)xwos_scheduler_idled;
        xwsd->idle.arg = xwsd;
        xwsd->idle.size = XWSMPCFG_SD_IDLE_STACK_SIZE;
        xwsd->idle.base = (xwstk_t *)xwos_scheduler_idled_stack[xwsd->id];
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

#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
/**
 * @brief 中断底半部的主函数
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwos_scheduler_bhd(struct xwos_scheduler * xwsd)
{
        struct xwos_bh_node * bhn;
        xwsq_t nv;

        while (1) {
                xwlk_rawly_lock_cpuirq(&xwsd->bhcb.lock);
                if (!xwlib_bclst_tst_empty(&xwsd->bhcb.list)) {
                        bhn = xwlib_bclst_first_entry(&xwsd->bhcb.list,
                                                      struct xwos_bh_node,
                                                      node);
                        xwlib_bclst_del_init(&bhn->node);
                        xwlk_rawly_unlock_cpuirq(&xwsd->bhcb.lock);
                        bhn->func(bhn->arg);
                } else {
                        xwlk_rawly_unlock_cpuirq(&xwsd->bhcb.lock);
                }
                xwaop_sub(xwsq_t, &xwsd->req_bh_cnt, 1, &nv, NULL);
                if (0 == nv) {
                        xwos_scheduler_bh_yield(xwsd);
                }/* else {} */
        }
        return OK;
}

/**
 * @brief 初始化中断底半部栈
 * @param xwsd: (I) XWOS调度器的指针
 */
static __xwos_code
void xwos_scheduler_init_bhd(struct xwos_scheduler * xwsd)
{
        xwsd->bh.name = NULL;
        xwsd->bh.main = (xwos_thrd_f)xwos_scheduler_bhd;
        xwsd->bh.arg = xwsd;
        xwsd->bh.size = XWSMPCFG_SD_BH_STACK_SIZE;
        xwsd->bh.base = (xwstk_t *)xwos_scheduler_bhd_stack[xwsd->id];
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
        bool retry;

        do {
                xwsd = &xwos_scheduler[xwos_cpu_get_id()];
                xwaop_add(xwsq_t, &xwsd->dis_bh_cnt, 1, NULL, NULL);
                if (__unlikely(xwsd->id != xwos_cpu_get_id())) {
                        xwaop_sub(xwsq_t, &xwsd->dis_bh_cnt, 1, NULL, NULL);
                        if ((!xwos_scheduler_tst_in_bh(xwsd)) &&
                            (0 != xwaop_load(xwsq_t, &xwsd->req_bh_cnt,
                                             xwmb_modr_consume))) {
                                xwos_scheduler_sw_bh(xwsd);
                        }/* else {} */
                        retry = true;
                } else {
                        retry = false;
                }
        } while (retry);
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
        xwsq_t nv;

        xwsd = xwos_scheduler_get_lc();
        xwaop_sub(xwsq_t, &xwsd->dis_bh_cnt, 1, &nv, NULL);
        if (0 == nv) {
                if (0 != xwaop_load(xwsq_t, &xwsd->req_bh_cnt, xwmb_modr_consume)) {
                        xwos_scheduler_sw_bh(xwsd);
                }/* else {} */
        }
        return xwsd;
}

/**
 * @brief 禁止调度器进入中断底半部
 * @param xwsd: (I) XWOS调度器的指针
 * @return XWOS调度器的指针
 */
__xwos_code
struct xwos_scheduler * xwos_scheduler_dsbh(struct xwos_scheduler * xwsd)
{
        xwaop_add(xwsq_t, &xwsd->dis_bh_cnt, 1, NULL, NULL);
        return xwsd;
}

/**
 * @brief 允许调度器的中断底半部
 * @param xwsd: (I) XWOS调度器的指针
 * @return XWOS调度器的指针
 */
__xwos_code
struct xwos_scheduler * xwos_scheduler_enbh(struct xwos_scheduler * xwsd)
{
        xwsq_t nv;

        xwaop_sub(xwsq_t, &xwsd->dis_bh_cnt, 1, &nv, NULL);
        if (0 == nv) {
                if (0 != xwaop_load(xwsq_t, &xwsd->req_bh_cnt, xwmb_modr_consume)) {
                        xwos_scheduler_sw_bh(xwsd);
                }/* else {} */
        }
        return xwsd;
}

/**
 * @brief 切换至中断底半部上下文
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EPERM: 中断底半部已被禁止
 * @retval -EINPROGRESS: 切换上下文的过程正在进行中
 * @retval -EALREADY: 当前上下文已经是中断底半部上下文
 */
static __xwos_code
xwer_t xwos_scheduler_sw_bh(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        if (0 == xwaop_load(xwsq_t, &xwsd->dis_bh_cnt, xwmb_modr_consume)) {
                xwlk_rawly_lock_cpuirqsv(&xwsd->cxlock, &cpuirq);
                if (__unlikely(NULL != xwsd->pstk)) {
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                        rc = -EINPROGRESS;
                } else if (__unlikely(XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk)) {
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                        rc = -EALREADY;
                } else {
                        xwsd->pstk = xwsd->cstk;
                        xwsd->cstk = XWOS_SCHEDULER_BH_STK(xwsd);
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                        soc_scheduler_req_swcx(xwsd);
                        rc = OK;
                }
        } else {
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief 请求切至中断底半部
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码 @ref xwos_scheduler_sw_bh()
 */
__xwos_code
xwer_t xwos_scheduler_req_bh(struct xwos_scheduler * xwsd)
{
        xwaop_add(xwsq_t, &xwsd->req_bh_cnt, 1, NULL, NULL);
        return xwos_scheduler_sw_bh(xwsd);
}

/**
 * @brief 从中断底半部上下文中返回到线程上下文
 * @param xwsd: (I) XWOS调度器的指针
 */
static __xwos_code
void xwos_scheduler_bh_yield(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;

        xwlk_rawly_lock_cpuirqsv(&xwsd->cxlock, &cpuirq);
        xwsd->cstk = xwsd->pstk;
        xwsd->pstk = XWOS_SCHEDULER_BH_STK(xwsd);
        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
        soc_scheduler_req_swcx(xwsd);
}
#endif /* XWSMPCFG_SD_BH */

/**
 * @brief XWOS API：禁止本地CPU调度器的抢占
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
        bool retry;

        do {
                xwsd = &xwos_scheduler[xwos_cpu_get_id()];
                xwaop_add(xwsq_t, &xwsd->dis_pmpt_cnt, 1, NULL, NULL);
                if (__unlikely(xwsd->id != xwos_cpu_get_id())) {
                        xwaop_sub(xwsq_t, &xwsd->dis_pmpt_cnt, 1, NULL, NULL);
                        if (0 != xwaop_load(xwsq_t, &xwsd->req_chkpmpt_cnt,
                                            xwmb_modr_consume)) {
                                xwos_scheduler_chkpmpt(xwsd);
                        }/* else {} */
                        retry = true;
                } else {
                        retry = false;
                }
        } while (retry);
        return xwsd;
}

/**
 * @brief XWOS API：允许本地CPU调度器的抢占
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
        xwsq_t nv;

        xwsd = xwos_scheduler_get_lc();
        xwaop_sub(xwsq_t, &xwsd->dis_pmpt_cnt, 1, &nv, NULL);
        if (0 == nv) {
                if (0 != xwaop_load(xwsq_t, &xwsd->req_chkpmpt_cnt,
                                    xwmb_modr_consume)) {
                        xwos_scheduler_chkpmpt(xwsd);
                }/* else {} */
        }
        return xwsd;
}

/**
 * @brief 禁止调度器的抢占
 * @param xwsd: (I) XWOS调度器的指针
 * @return XWOS调度器的指针
 */
__xwos_code
struct xwos_scheduler * xwos_scheduler_dspmpt(struct xwos_scheduler * xwsd)
{
        xwaop_add(xwsq_t, &xwsd->dis_pmpt_cnt, 1, NULL, NULL);
        return xwsd;
}

/**
 * @brief 允许调度器的抢占
 * @param xwsd: (I) XWOS调度器的指针
 * @return XWOS调度器的指针
 */
__xwos_code
struct xwos_scheduler * xwos_scheduler_enpmpt(struct xwos_scheduler * xwsd)
{
        xwsq_t nv;

        xwaop_sub(xwsq_t, &xwsd->dis_pmpt_cnt, 1, &nv, NULL);
        if (0 == nv) {
                if (0 != xwaop_load(xwsq_t, &xwsd->req_chkpmpt_cnt,
                                    xwmb_modr_consume)) {
                        xwos_scheduler_chkpmpt(xwsd);
                }/* else {} */
        }
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
 * - 此函数被调用时需要获得锁xwsd->cxlock并且关闭本地CPU的中断。
 */
static __xwos_code
bool xwos_scheduler_do_chkpmpt(struct xwos_scheduler * xwsd, struct xwos_tcb * t)
{
        struct xwos_rtrq * xwrtrq;
        bool sched;

        xwrtrq = &xwsd->rq.rt;
        XWOS_BUG_ON(&t->stack == &xwsd->idle);
#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
        XWOS_BUG_ON(&t->stack == &xwsd->bh);
#endif /* XWSMPCFG_SD_BH */
        xwlk_rawly_lock(&xwrtrq->lock);
        xwlk_rawly_lock(&t->stlock);
        if (XWSDOBJ_DST_RUNNING & t->state) {
                if (t->dprio.r >= xwrtrq->top) {
                        sched = false;
                } else {
                        sched = true;
                }
        } else {
                /* The thread will give up the CPU in a jiffy.
                   We don't check the preemption */
                sched = false;
        }
        xwlk_rawly_unlock(&t->stlock);
        xwlk_rawly_unlock(&xwrtrq->lock);
        return sched;
}

/**
 * @brief 调度器检测抢占
 * @param xwsd: (I) XWOS调度器的指针
 */
__xwos_code
void xwos_scheduler_chkpmpt(struct xwos_scheduler * xwsd)
{
        struct xwos_tcb * t;
        struct xwos_sdobj_stack_info * cstk;
        xwreg_t cpuirq;
        bool sched;

        if (0 != xwaop_load(xwsq_t, &xwsd->dis_pmpt_cnt, xwmb_modr_consume)) {
                xwaop_add(xwsq_t, &xwsd->req_chkpmpt_cnt, 1, NULL, NULL);
        } else {
                xwlk_rawly_lock_cpuirqsv(&xwsd->cxlock, &cpuirq);
#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
                if (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk) {
                        cstk = xwsd->pstk;
                } else {
                        cstk = xwsd->cstk;
                }
#else /* XWSMPCFG_SD_BH */
                cstk = xwsd->cstk;
#endif /* !XWSMPCFG_SD_BH */
                if (XWOS_SCHEDULER_IDLE_STK(xwsd) != cstk) {
                        t = container_of(cstk, struct xwos_tcb, stack);
                        sched = xwos_scheduler_do_chkpmpt(xwsd, t);
                } else {
                        sched = true;
                }
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                xwaop_write(xwsq_t, &xwsd->req_chkpmpt_cnt, 0, NULL);
                if (sched) {
                        xwos_scheduler_req_swcx(xwsd);
                }/* else {} */
        }
}

/**
 * @brief 所有CPU的调度器检测抢占
 */
__xwos_code
void xwos_scheduler_chkpmpt_all(void)
{
        struct xwos_scheduler * xwsd;
        xwid_t cpuid;

        for (cpuid = 0; cpuid < (xwid_t)CPUCFG_CPU_NUM; cpuid++) {
                xwsd = &xwos_scheduler[cpuid];
                xwos_scheduler_chkpmpt(xwsd);
        }
}

/**
 * @brief 检测调度器是否需要切换上下文
 * @param xwsd: (I) XWOS调度器的指针
 * @param t: (I) 被检测的线程控制块对象的指针
 * @param pmtcb: (O) 指向缓冲区的指针，此缓冲区用于返回抢占线程的控制块的指针
 * @return 错误码
 * @retval OK: 需要切换上下文
 * @retval <0: 不需要切换上下文
 * @note
 * - 此函数被调用时需要获得锁xwsd->cxlock并且关闭本地CPU的中断。
 */
static __xwos_code
xwer_t xwos_scheduler_check_swcx(struct xwos_scheduler * xwsd,
                                 struct xwos_tcb * t,
                                 struct xwos_tcb ** pmtcb)
{
        struct xwos_rtrq * xwrtrq;
        xwpr_t prio;
        xwer_t rc;

        xwrtrq = &xwsd->rq.rt;
        xwlk_rawly_lock(&xwrtrq->lock);
        xwlk_rawly_lock(&t->stlock);
        if (XWSDOBJ_DST_RUNNING & t->state) {
                if (t->dprio.r >= xwrtrq->top) {
                        xwlk_rawly_unlock(&t->stlock);
                        xwlk_rawly_unlock(&xwrtrq->lock);
                        rc = -EPERM;
                } else {
                        prio = t->dprio.r;
                        xwbop_c0m(xwsq_t, &t->state, XWSDOBJ_DST_RUNNING);
                        t->dprio.r = XWOS_SD_PRIORITY_INVALID;
                        xwlk_rawly_unlock(&t->stlock);
                        xwlk_rawly_unlock(&xwrtrq->lock);
                        rc = xwos_thrd_rq_add_head(t, prio);
                        XWOS_BUG_ON(rc < 0);
                        *pmtcb = xwos_scheduler_rtrq_choose(xwsd);
                        rc = OK;
                }
        } else {
                xwlk_rawly_unlock(&t->stlock);
                xwlk_rawly_unlock(&xwrtrq->lock);
                *pmtcb = xwos_scheduler_rtrq_choose(xwsd);
                rc = OK;
        }
        return rc;
}

/**
 * @brief 执行上下文的切换
 * @param xwsd: (I) XWOS调度器的指针
 * @retval rc: @ref xwos_scheduler_req_swcx()
 * @note
 * - 此函数被调用时需要获得锁xwsd->cxlock并且关闭本地CPU的中断。
 */
static __xwos_code
xwer_t xwos_scheduler_do_swcx(struct xwos_scheduler * xwsd)
{
        struct xwos_tcb * swtcb, * ctcb;
        xwer_t rc;

        if (XWOS_SCHEDULER_IDLE_STK(xwsd) == xwsd->cstk) {
                swtcb = xwos_scheduler_rtrq_choose(xwsd);
                if (NULL != swtcb) {
                        xwsd->pstk = xwsd->cstk;
                        xwsd->cstk = &swtcb->stack;
                        rc = OK;
                } else {
                        rc = -EAGAIN;
                }
        } else {
                ctcb = xwos_scheduler_get_ctcb(xwsd);
                rc = xwos_scheduler_check_swcx(xwsd, ctcb, &swtcb);
                if (OK == rc) {
                        if (ctcb == swtcb) {
                                /* 当前线程的状态可能在中断或其他CPU中被改变，
                                   并又被加入到就绪队列中，然后在此处又被重新选择。
                                   这种情况下，调度器只需要恢复此线程的
                                   @ref XWSDOBJ_DST_RUNNING状态，
                                   并不需要切换线程上下文。*/
                                rc = -EAGAIN;
                        } else {
                                if (NULL == swtcb) {
                                        xwsd->cstk = XWOS_SCHEDULER_IDLE_STK(xwsd);
                                } else {
                                        xwsd->cstk = &swtcb->stack;
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

#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
/**
 * @brief 请求切换上下文
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码
 * @retval OK: 需要触发一个软中断执行切换上下文的过程
 * @retval -EBUSY: 当前上下文为中断底半部上下文
 * @retval -EINVAL: 当前正在运行的线程状态错误
 * @retval -EAGAIN: 不需要切换上下文
 * @retval -EEINPROGRESS: 切换上下文的过程正在进行
 * note
 * - 同步/异步：异步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_code
xwer_t xwos_scheduler_req_swcx(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwlk_rawly_lock_cpuirqsv(&xwsd->cxlock, &cpuirq);
        xwsd->req_schedule_cnt++;
        if (__unlikely(NULL != xwsd->pstk)) {
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                rc = -EINPROGRESS;
        } else if (__unlikely(XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk)) {
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                rc = -EBUSY;
        } else {
                xwsd->pstk = err_ptr(-EINVAL); /* Invalidate other caller. */
                rc = xwos_scheduler_do_swcx(xwsd);
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                if (OK == rc) {
                        soc_scheduler_req_swcx(xwsd);
                } else {
                        xwos_scheduler_finish_swcx(xwsd);
                }
        }
        return rc;
}

/**
 * @brief 结束上下文的切换
 * @param xwsd: (I) XWOS调度器的指针
 * @note
 * - 此函数需要在BSP中切换线程上下文的中断函数的最后一步调用。
 */
__xwos_code
void xwos_scheduler_finish_swcx(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;
        xwsq_t rbc;

        xwlk_rawly_lock_cpuirqsv(&xwsd->cxlock, &cpuirq);
        if (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk) {
                /* Finish switching context from thread to BH */
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
        } else if (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->pstk) {
                /* Finish switching context from BH to thread */
                xwsd->pstk = NULL;
                rbc = xwaop_load(xwsq_t, &xwsd->req_bh_cnt, xwmb_modr_consume);
                if (rbc >= 1) {
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                        xwos_scheduler_sw_bh(xwsd);
                } else if (xwsd->req_schedule_cnt > 0) {
                        xwsd->req_schedule_cnt = 0;
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                        xwos_scheduler_req_swcx(xwsd);
                } else {
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                }
        } else {
                /* Finish switching context from thread to thread */
                xwsd->pstk = NULL;
                xwsd->req_schedule_cnt--;
                rbc = xwaop_load(xwsq_t, &xwsd->req_bh_cnt, xwmb_modr_consume);
                if (rbc >= 1) {
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                        xwos_scheduler_sw_bh(xwsd);
                } else if (xwsd->req_schedule_cnt > 0) {
                        xwsd->req_schedule_cnt = 0;
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                        xwos_scheduler_req_swcx(xwsd);
                } else {
                        xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                }
        }
}
#else /* XWSMPCFG_SD_BH */
/**
 * @brief 请求切换上下文
 * @param xwsd: (I) XWOS调度器的指针
 * @return 错误码
 * @retval OK: 需要触发一个软中断执行切换上下文的过程
 * @retval -EINVAL: 当前正在运行的线程状态错误
 * @retval -EAGAIN: 不需要切换上下文
 * @retval -EEINPROGRESS: 切换上下文的过程正在进行
 * @note
 * - 同步/异步：异步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_code
xwer_t xwos_scheduler_req_swcx(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwlk_rawly_lock_cpuirqsv(&xwsd->cxlock, &cpuirq);
        xwsd->req_schedule_cnt++;
        if (__unlikely(NULL != xwsd->pstk)) {
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                rc = -EINPROGRESS;
        } else {
                xwsd->pstk = err_ptr(-EINVAL); /* Invalidate other caller. */
                rc = xwos_scheduler_do_swcx(xwsd);
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                if (OK == rc) {
                        soc_scheduler_req_swcx(xwsd);
                } else {
                        xwos_scheduler_finish_swcx(xwsd);
                }
        }
        return rc;
}

/**
 * @brief 结束上下文的切换
 * @param xwsd: (I) XWOS调度器的指针
 * @note
 * - 此函数需要在BSP中切换线程上下文的中断函数的最后一步调用。
 */
__xwos_code
void xwos_scheduler_finish_swcx(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;

        xwlk_rawly_lock_cpuirqsv(&xwsd->cxlock, &cpuirq);
        /* Finish switching context from thread to thread */
        xwsd->pstk = NULL;
        xwsd->req_schedule_cnt--;
        if (xwsd->req_schedule_cnt > 0) {
                xwsd->req_schedule_cnt = 0;
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
                xwos_scheduler_req_swcx(xwsd);
        } else {
                xwlk_rawly_unlock_cpuirqrs(&xwsd->cxlock, cpuirq);
        }
}
#endif /* !XWSMPCFG_SD_BH */

/**
 * @brief 将调度器的唤醒锁计数器加1
 * @param xwsd: (I) 调度器对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval <0: 当前调度器正在进入低功耗
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_code
xwer_t xwos_scheduler_inc_wklkcnt(struct xwos_scheduler * xwsd)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq_t, &xwsd->lpm.wklkcnt,
                                XWOS_SCHEDULER_WKLKCNT_UNLOCKED, 1,
                                NULL, NULL);
        return rc;
}

/**
 * @brief 将调度器的唤醒锁计数器减1
 * @param xwsd: (I) 调度器对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval <0: 当前调度器正在进入低功耗
 * @note
 * - 同步/异步：同步
 * - 中断上下文：在本地CPU的中断上下文中执行此函数时，xwsd不可指定为本地CPU的调度器
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_code
xwer_t xwos_scheduler_dec_wklkcnt(struct xwos_scheduler * xwsd)
{
        xwer_t rc;
        xwsq_t nv;

        rc = xwaop_tge_then_sub(xwsq_t, &xwsd->lpm.wklkcnt,
                                XWOS_SCHEDULER_WKLKCNT_UNLOCKED,
                                1,
                                &nv, NULL);
        if ((OK == rc) && (XWOS_SCHEDULER_WKLKCNT_LPM == nv)) {
                soc_scheduler_suspend(xwsd);
        }/* else {} */
        return rc;
}

/**
 * @brief 中断本地CPU中所有线程的阻塞或睡眠状态，并准备冻结
 * @param xwsd: (I) 调度器对象的指针
 * @note
 * - 此函数只能在本地CPU中执行。
 */
static __xwos_code
void xwos_scheduler_reqfrz_intr_all_lic(struct xwos_scheduler * xwsd)
{
        struct xwos_tcb * c, * n;
        xwreg_t cpuirq;

        xwlk_splk_lock_cpuirqsv(&xwsd->tcblistlock, &cpuirq);
        xwlib_bclst_itr_next_entry_safe(c, n, &xwsd->tcblist,
                                        struct xwos_tcb, tcbnode) {
                xwos_thrd_grab(c);
                xwlk_splk_unlock_cpuirqrs(&xwsd->tcblistlock, cpuirq);
                xwos_thrd_reqfrz_lic(c);
                xwos_thrd_intr(c);
                xwos_thrd_put(c);
                xwlk_splk_lock_cpuirqsv(&xwsd->tcblistlock, &cpuirq);
        }
        xwlk_splk_unlock_cpuirqrs(&xwsd->tcblistlock, cpuirq);
}

/**
 * @brief 通知所有线程已经冻结
 * @param xwsd: (I) 调度器对象的指针
 * @return 错误码
 */
__xwos_code
xwer_t xwos_scheduler_notify_allfrz(struct xwos_scheduler * xwsd)
{
        xwer_t rc;

        if (xwos_scheduler_tst_lpm(xwsd)) {
                xwos_syshwt_stop(&xwsd->tt.hwt);
                if (xwsd->lpm.xwpmdm) {
                        xwos_pmdm_inc_lpmxwsd_cnt(xwsd->lpm.xwpmdm);
                }/* else {} */
        }
        rc = xwos_scheduler_req_swcx(xwsd);
        return rc;
}

/**
 * @brief 解冻本地CPU中的所有线程
 * @param xwsd: (I) 本地CPU调度器对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在本地CPU中执行。
 */
static __xwos_code
xwer_t xwos_scheduler_thaw_allfrz_lic(struct xwos_scheduler * xwsd)
{
        struct xwos_tcb * c, * n;
        xwreg_t cpuirq;
        xwer_t rc;

        xwlk_splk_lock_cpuirqsv(&xwsd->lpm.lock, &cpuirq);
        xwlib_bclst_itr_next_entry_safe(c, n, &xwsd->lpm.frzlist,
                                        struct xwos_tcb, frznode) {
                xwos_thrd_grab(c);
                xwlk_splk_unlock_cpuirqrs(&xwsd->lpm.lock, cpuirq);
                xwos_thrd_thaw_lic(c);
                xwos_thrd_put(c);
                xwlk_splk_lock_cpuirqsv(&xwsd->lpm.lock, &cpuirq);
        }
        if (0 == xwsd->lpm.frz_thrd_cnt) {
                rc = OK;
        } else {
                rc = -EBUG;
        }
        xwlk_splk_unlock_cpuirqrs(&xwsd->lpm.lock, cpuirq);
        xwos_scheduler_req_swcx(xwsd);
        return rc;
}

/**
 * @brief 暂停本地CPU的调度器
 * @param xwsd: (I) 调度器对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在CPU自身的中断中执行，因此当电源管理的代码运行于其他CPU上时，
 *   可通过@ref xwos_scheduler_suspend(cpuid)发射一个软中断到对应CPU上，
 *   让它自己执行此函数。
 */
__xwos_code
xwer_t xwos_scheduler_suspend_lic(struct xwos_scheduler * xwsd)
{
        xwreg_t cpuirq;

        xwos_scheduler_reqfrz_intr_all_lic(xwsd);
        xwlk_splk_lock_cpuirqsv(&xwsd->lpm.lock, &cpuirq);
        xwlk_splk_lock(&xwsd->tcblistlock);
        if (xwsd->thrd_num == xwsd->lpm.frz_thrd_cnt) {
                xwlk_splk_unlock(&xwsd->tcblistlock);
                xwlk_splk_unlock_cpuirqrs(&xwsd->lpm.lock, cpuirq);
                xwos_scheduler_notify_allfrz(xwsd);
        } else {
                xwlk_splk_unlock(&xwsd->tcblistlock);
                xwlk_splk_unlock_cpuirqrs(&xwsd->lpm.lock, cpuirq);
        }
        return OK;
}

/**
 * @brief XWOS API：请求暂停调度器
 * @param cpuid: (I) CPU的ID
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 中断上下文：在本地CPU的中断上下文中执行此函数时，cpuid不可指定为本地CPU
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
xwer_t xwos_scheduler_suspend(xwid_t cpuid)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;

        rc = xwos_scheduler_get_by_cpuid(cpuid, &xwsd);
        if (OK == rc) {
                rc = xwos_scheduler_dec_wklkcnt(xwsd);
        }/* else {} */
        return rc;
}

/**
 * @brief 继续本地CPU调度器
 * @param xwsd: (I) 调度器对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在CPU自身的中断中执行，因此当电源管理的代码运行于其他CPU上时，
 *   可通过@ref xwos_scheduler_resume(cpuid)发射一个软中断到对应CPU，
 *   让其进入中断，并执行此函数。
 */
__xwos_code
xwer_t xwos_scheduler_resume_lic(struct xwos_scheduler * xwsd)
{
        xwer_t rc;

        rc = xwaop_teq_then_write(xwsq_t, &xwsd->lpm.wklkcnt,
                                  XWOS_SCHEDULER_WKLKCNT_LPM,
                                  XWOS_SCHEDULER_WKLKCNT_UNLOCKED,
                                  NULL);
        if (OK == rc) {
                rc = xwos_scheduler_thaw_allfrz_lic(xwsd);
                XWOS_BUG_ON(rc < 0);
                xwos_pmdm_dec_lpmxwsd_cnt(xwsd->lpm.xwpmdm);
                xwos_syshwt_start(&xwsd->tt.hwt);
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：请求继续调度器
 * @param cpuid: (I) CPU的ID
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 中断上下文：可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：不可以使用
 * - 说明：SMP系统中：
 *         1. 执行唤醒系统代码的CPU需直接在唤醒中断中执行xwos_scheduler_resume_lic()，
 *            不可触发软中断再调用。
 *         2. 其他CPU的唤醒需要通过触发软中断执行xwos_scheduler_resume_lic()。
 */
__xwos_api
xwer_t xwos_scheduler_resume(xwid_t cpuid)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;

        if (xwos_cpu_get_id() == cpuid) {
                if (OK != xwos_irq_get_id(NULL)) {
                        rc = -ENOTINISR;
                } else {
                        xwsd = xwos_scheduler_get_lc();
                        rc = xwos_scheduler_resume_lic(xwsd);
                }
        } else {
                rc = xwos_scheduler_get_by_cpuid(cpuid, &xwsd);
                if (OK == rc) {
                        rc = soc_scheduler_resume(xwsd);
                }/* else {} */
        }
        return rc;
}

/**
 * @brief XWOS API：测试调度器是否处于暂停状态
 * @param xwsd: (I) 调度器对象的指针
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
bool xwos_scheduler_tst_lpm(struct xwos_scheduler * xwsd)
{
        xwsq_t wklkcnt;

        wklkcnt = xwaop_load(xwsq_t, &xwsd->lpm.wklkcnt, xwmb_modr_consume);
        return !!(XWOS_SCHEDULER_WKLKCNT_LPM == wklkcnt);
}
