/**
 * @file
 * @brief XWOS MP内核：调度器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - XWOS MP的调度器是一个每CPU变量，为了提高程序的效率，减少cache miss，
 *   通常只由CPU才可访问自己的调度器控制块结构体。其他CPU需要操作当前
 *   CPU的调度器时，需要挂起当前CPU的调度器服务中断，由当前CPU在ISR
 *   中进行操作。
 * - 调度器服务中断处理以下操作：
 *   + 调度器暂停：xwmp_skd_suspend_lic()
 *                   |--> xwmp_skd_notify_allfrz_lic()
 *   + 线程退出：xwmp_thd_exit_lic()
 *                 |--> xwmp_skd_notify_allfrz_lic()
 *   + 线程冻结：xwmp_thd_freeze_lic()
 *                 |--> xwmp_skd_notify_allfrz_lic()
 *   + 线程解冻：xwmp_thd_thaw_lic()
 *   + 线程迁出：xwmp_thd_outmigrate_lic()
 *   + 线程迁入：xwmp_thd_immigrate_lic()
 * - 调度器服务中断的中断优先级必须为系统最高优先级。
 * - 调度时锁的顺序：同级的锁不可同时获得
 *   + ① xwmp_skd.cxlock
 *     + ② xwmp_skd.rq.rt.lock
 *       + ③ xwmp_thd.stlock
 * - 休眠时锁的顺序：同级的锁不可同时获得
 *   + ① xwmp_skd.pm.lock
 *     + ② xwmp_thd.stlock
 *     + ② xwmp_skd.thdlistlock
 * - 函数suffix意义：
 *   1. _lc: Local Context，是指只能在“本地”CPU的中执行的代码。
 *   2. _lic: Local ISR Context，是指只能在“本地”CPU的中断上下文中执行的代码。
 *   3. _pmlk: 是只持有锁xwmp_skd.pm.lock才可调用的函数。
 *   3. _tllk: 是只持有锁xwmp_skd.thdlistlock才可调用的函数。
 */

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwaop.h>
#include <string.h>
#include <xwos/mm/common.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/ospl/tls.h>
#include <xwos/mp/irq.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/pm.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/rtrq.h>
#include <xwos/mp/tt.h>
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
#  include <xwos/mp/bh.h>
#endif

/* #define XWOS_SKDLOGF */ /**< 调试日志开关 */
#ifdef XWOS_SKDLOGF
#  define xwos_skdlogf(lv, xwskd, fmt, ...) \
          xwlogf(lv, "XWSKD.%d", fmt, xwskd->id, ##__VA_ARGS__)
#else
#  define xwos_skdlogf(lv, xwskd, fmt, ...)
#endif

/**
 * @brief 每CPU的调度器
 */
__xwmp_data __xwcc_alignl1cache
struct xwmp_skd xwmp_skd[CPUCFG_CPU_NUM];

/**
 * @brief 每CPU的调度器的空闲线程栈
 */
static __xwmp_data __xwcc_alignl1cache
xwu8_t xwmp_skd_idled_stack[CPUCFG_CPU_NUM][XWOSCFG_SKD_IDLE_STACK_SIZE];

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
/**
 * @brief 每CPU的调度器的中断底半部栈
 */
static __xwmp_data __xwcc_alignl1cache
xwu8_t xwmp_skd_bhd_stack[CPUCFG_CPU_NUM][XWOSCFG_SKD_BH_STACK_SIZE];
#endif

#if defined(BRDCFG_XWSKD_IDLE_HOOK) && (1 == BRDCFG_XWSKD_IDLE_HOOK)
extern
void board_xwskd_idle_hook(struct xwmp_skd * xwskd);
#endif

#if defined(BRDCFG_XWSKD_PRE_SWCX_HOOK) && (1 == BRDCFG_XWSKD_PRE_SWCX_HOOK)
extern
void board_xwskd_pre_swcx_hook(struct xwmp_skd * xwskd);
#endif

#if defined(BRDCFG_XWSKD_POST_SWCX_HOOK) && (1 == BRDCFG_XWSKD_POST_SWCX_HOOK)
extern
void board_xwskd_post_swcx_hook(struct xwmp_skd * xwskd);
#endif

static __xwmp_code
struct xwmp_thd * xwmp_skd_rtrq_choose(struct xwmp_skd * xwskd);

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
static __xwmp_code
void xwmp_skd_del_thd_lc(struct xwmp_skd * xwskd);
#endif

static __xwmp_code
xwer_t xwmp_skd_idled(struct xwmp_skd * xwskd);

static __xwmp_code
void xwmp_skd_init_idled(struct xwmp_skd * xwskd);

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
static __xwmp_code
xwer_t xwmp_skd_bhd(struct xwmp_skd * xwskd);

static __xwmp_code
void xwmp_skd_init_bhd(struct xwmp_skd * xwskd);

static __xwmp_code
xwer_t xwmp_skd_sw_bh(struct xwmp_skd * xwskd);

static __xwmp_code
void xwmp_skd_bh_yield(struct xwmp_skd * xwskd);
#endif

static __xwmp_code
bool xwmp_skd_chkpmpt_thd(struct xwmp_skd * xwskd, struct xwmp_thd * t);

static __xwmp_code
xwer_t xwmp_skd_check_swcx(struct xwmp_skd * xwskd,
                           struct xwmp_thd * t,
                           struct xwmp_thd ** pmthd);

static __xwmp_code
xwer_t xwmp_skd_swcx(struct xwmp_skd * xwskd);

static __xwmp_code
void xwmp_skd_finish_swcx_lic(struct xwmp_skd * xwskd);

static __xwmp_code
void xwmp_skd_reqfrz_intr_all_lic(struct xwmp_skd * xwskd);

static __xwmp_code
void xwmp_skd_notify_allfrz_lc(struct xwmp_skd * xwskd);

static __xwmp_code
void xwmp_skd_thaw_allfrz_lic(struct xwmp_skd * xwskd);

/**
 * @brief XWMP INIT API：初始化本地CPU的调度器
 * @return 错误码
 * @note
 * - 重入性：本函数只可在系统初始化时调用一次
 */
__xwmp_init_code
xwer_t xwmp_skd_init_lc(void)
{
        xwid_t id;
        xwer_t rc;
        struct xwmp_skd * xwskd;

        id = xwmp_skd_get_cpuid_lc();
        xwskd = &xwmp_skd[id];

        xwskd->cstk = XWMP_SKD_IDLE_STK(xwskd);
        xwskd->pstk = NULL;
        xwskd->id = id;
        xwskd->state = (xwsq_t)XWMP_SKD_STATE_INIT;
        xwskd->req_schedule_cnt = (xwsq_t)0;
        xwskd->req_chkpmpt_cnt = (xwsq_t)0;
        xwskd->dis_pmpt_cnt = (xwsq_t)0;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwskd->req_bh_cnt = (xwsq_t)0;
        xwskd->dis_bh_cnt = (xwsq_t)0;
        xwmp_bh_cb_init(&xwskd->bhcb);
        xwmp_skd_init_bhd(xwskd);
#endif
        xwskd->dis_th_cnt = (xwsq_t)0;
        xwmp_splk_init(&xwskd->cxlock);
        xwmp_splk_init(&xwskd->thdlistlock);
        xwlib_bclst_init_head(&xwskd->thdlist);
        xwskd->thd_num = (xwsz_t)0;
        xwlib_bclst_init_head(&xwskd->thdelist);
        rc = xwmp_tt_init(&xwskd->tt);
        if (rc < 0) {
                goto err_tt_init;
        }
        xwmp_rtrq_init(&xwskd->rq.rt);
        xwmp_skd_init_idled(xwskd);
        xwskd->pm.wklkcnt = (xwsq_t)XWMP_SKD_WKLKCNT_RUNNING;
        xwskd->pm.frz_thd_cnt = (xwsq_t)0;
        xwmp_splk_init(&xwskd->pm.lock);
        xwlib_bclst_init_head(&xwskd->pm.frzlist);
        xwskd->pm.xwpmdm = &xwmp_pmdm;
        rc = xwospl_skd_init(xwskd);
        if (rc < 0) {
                goto err_skd_init;
        }
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwos_skdlogf(INFO, xwskd,
                     "Init scheduler@CPU%d, "
                     "IDLE Stack: {.base = 0x%lX, .size = 0x%lX}, "
                     "BH Stack: {.base = 0x%lX, .size = 0x%lX}\r\n",
                     xwskd->id,
                     (xwptr_t)xwskd->idle.base, xwskd->idle.size,
                     (xwptr_t)xwskd->bh.base, xwskd->bh.size);
#else
        xwos_skdlogf(INFO, xwskd,
                     "Init scheduler@CPU%d, "
                     "IDLE Stack: {.base = 0x%lX, .size = 0x%lX}\r\n",
                     xwskd->id,
                     (xwptr_t)xwskd->idle.base, xwskd->idle.size);
#endif
        return XWOK;

err_skd_init:
        XWOS_BUG();
err_tt_init:
        XWOS_BUG();
        return rc;
}

/**
 * @brief XWMP INIT API：启动本地CPU的调度器
 * @return 错误码
 * @note
 * - 重入性：只可调用一次
 * - 此函数不会返回
 */
__xwmp_init_code
xwer_t xwmp_skd_start_lc(void)
{
        struct xwmp_skd * xwskd;
        xwer_t rc;
        xwid_t id;

        id = xwmp_skd_get_cpuid_lc();
        xwskd = &xwmp_skd[id];
        if ((xwsq_t)XWMP_SKD_STATE_INIT == xwskd->state) {
                xwmp_skd_dspmpt(xwskd);
                rc = xwmp_syshwt_start(&xwskd->tt.hwt);
                if (XWOK == rc) {
                        rc = xwospl_skd_start(xwskd);
                        if (rc < 0) {
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwmp_syshwt_stop(&xwskd->tt.hwt);
                        }
                }
        } else {
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief XWMP API：启动本地CPU调度器的系统定时器
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_skd_start_syshwt_lc(void)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_get_lc();
        return xwmp_syshwt_start(&xwskd->tt.hwt);
}

/**
 * @brief XWMP API：关闭本地CPU调度器的系统定时器
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_skd_stop_syshwt_lc(void)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_get_lc();
        return xwmp_syshwt_stop(&xwskd->tt.hwt);
}

__xwmp_api
xwid_t xwmp_skd_get_cpuid_lc(void)
{
        return xwospl_skd_get_cpuid_lc();
}

/**
 * @brief 得到本地CPU调度器的指针
 * @return 本地CPU调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_get_lc(void)
{
        struct xwmp_skd * xwskd;
        xwid_t cpuid;

        cpuid = xwmp_skd_get_cpuid_lc();
        xwmb_ddb();
        do {
                xwskd = &xwmp_skd[cpuid];
                cpuid = xwmp_skd_get_cpuid_lc();
                xwmb_ddb();
        } while (xwskd->id != cpuid);
        return xwskd;
}

/**
 * @brief 得到指定ID的CPU调度器的指针
 * @param[in] cpuid: CPU ID
 * @param[out] ptrbuf: 指向缓冲区的指针，此缓冲区用于返回指定ID的CPU调度器的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_skd_get_by_cpuid(xwid_t cpuid, struct xwmp_skd ** ptrbuf)
{
        xwer_t rc;

        if (cpuid < (xwid_t)XWMP_CPU_NUM) {
                *ptrbuf = &xwmp_skd[cpuid];
                rc = XWOK;
        } else {
                rc = -ENODEV;
        }
        return rc;
}

/**
 * @brief 获得调度器中正在运行的线程对象的指针
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 调度器中正在运行的线程对象的指针
 */
__xwmp_code
struct xwmp_thd * xwmp_skd_get_cthd(struct xwmp_skd * xwskd)
{
        struct xwmp_thd * cthd;

        cthd = xwcc_derof(xwskd->cstk, struct xwmp_thd, stack);
        return cthd;
}

/**
 * @brief 获得本地CPU的调度器中正在运行的线程对象的指针
 * @return 本地CPU的调度器中正在运行的线程对象的指针
 */
__xwmp_code
struct xwmp_thd * xwmp_skd_get_cthd_lc(void)
{
        struct xwmp_skd * xwskd;
        struct xwmp_thd * cthd;
        xwid_t cpuid;

        cpuid = xwmp_skd_get_cpuid_lc();
        xwmb_ddb();
        do {
                xwskd = &xwmp_skd[cpuid];
                cthd = xwcc_derof(xwskd->cstk, struct xwmp_thd, stack);
                cpuid = xwmp_skd_get_cpuid_lc();
                xwmb_ddb();
        } while (xwskd->id != cpuid);
        return cthd;
}

/**
 * @brief 从XWMP调度器中选择一个就绪的线程
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 线程对象指针或NULL（代表空闲任务）
 * @note
 * - 此函数只能在锁xwskd->cxlock中调用。
 */
static __xwmp_code
struct xwmp_thd * xwmp_skd_rtrq_choose(struct xwmp_skd * xwskd)
{
        struct xwmp_rtrq * xwrtrq;
        struct xwmp_thd * t;
        xwpr_t prio;
        xwer_t rc;

        xwrtrq = &xwskd->rq.rt;
        xwmp_rawly_lock(&xwrtrq->lock);
        t = xwmp_rtrq_choose_locked(xwrtrq);
        if (NULL != t) {
                rc = xwmp_rtrq_remove_locked(xwrtrq, t);
                XWOS_BUG_ON(rc < 0);
                xwmp_rawly_lock(&t->stlock);
                prio = t->dprio.rq;
                xwbop_c0m(xwsq_t, &t->state, (xwsq_t)XWMP_SKDOBJ_ST_READY);
                t->dprio.rq = XWMP_SKD_PRIORITY_INVALID;
                xwbop_s1m(xwsq_t, &t->state, (xwsq_t)XWMP_SKDOBJ_ST_RUNNING);
                t->dprio.r = prio;
                xwmp_rawly_unlock(&t->stlock);
                xwmp_rawly_unlock(&xwrtrq->lock);
        } else {
                xwmp_rawly_unlock(&xwrtrq->lock);
        }
        return t;
}

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
/**
 * @brief 删除 **删除列表** 中的线程
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 */
static __xwmp_code
void xwmp_skd_del_thd_lc(struct xwmp_skd * xwskd)
{
        if (!xwlib_bclst_tst_empty(&xwskd->thdelist)) {
                struct xwmp_thd * thd;
                xwmp_splk_lock_cpuirq(&xwskd->thdlistlock);
                xwlib_bclst_itr_next_entry_del(thd, &xwskd->thdelist,
                                               struct xwmp_thd, thdnode) {
                        xwlib_bclst_del_init(&thd->thdnode);
                        xwmp_splk_unlock_cpuirq(&xwskd->thdlistlock);
                        xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_splk_lock_cpuirq(&xwskd->thdlistlock);
                }
                xwmp_splk_unlock_cpuirq(&xwskd->thdlistlock);
        }
}
#endif

/**
 * @brief 空闲任务的主函数
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_skd_idled(struct xwmp_skd * xwskd)
{
        XWOS_UNUSED(xwskd);

        while (true) {
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
                xwmp_skd_del_thd_lc(xwskd);
#endif
                xwmp_skd_notify_allfrz_lc(xwskd);
#if defined(BRDCFG_XWSKD_IDLE_HOOK) && (1 == BRDCFG_XWSKD_IDLE_HOOK)
                board_xwskd_idle_hook(xwskd);
#endif
        }
        return XWOK;
}

static __xwmp_rodata
const char xwmp_skd_idled_name[] = "IDLE";

/**
 * @brief 初始化空闲线程栈
 * @param[in] xwskd: XWOS MP调度器的指针
 */
static __xwmp_code
void xwmp_skd_init_idled(struct xwmp_skd * xwskd)
{
        xwskd->idle.name = xwmp_skd_idled_name;
        xwskd->idle.main = (xwmp_thd_f)xwmp_skd_idled;
        xwskd->idle.arg = xwskd;
        xwskd->idle.size = XWOSCFG_SKD_IDLE_STACK_SIZE;
        xwskd->idle.base = (xwstk_t *)xwmp_skd_idled_stack[xwskd->id];
#if (defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK))
        xwskd->idle.sp = &xwskd->idle.base[(xwskd->idle.size / sizeof(xwstk_t))];
        xwskd->idle.tls = &xwskd->idle.base[0];
#elif (defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK))
        xwskd->idle.sp = &xwskd->idle.base[(xwskd->idle.size / sizeof(xwstk_t)) - 1U];
        xwskd->idle.tls = &xwskd->idle.base[0];
#elif (defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK))
        xwskd->idle.sp = &xwskd->idle.base[-1];
        xwskd->idle.tls = &xwskd->idle.base[(attr->stack_size / sizeof(xwstk_t))];
#elif (defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK))
        xwskd->idle.sp = &xwskd->idle.base[0];
        xwskd->idle.tls = &xwskd->idle.base[(attr->stack_size / sizeof(xwstk_t))];
#else
#  error "Unknown stack type!"
#endif
        xwskd->idle.guard_base = xwskd->idle.tls;
        xwskd->idle.guard = (xwsz_t)XWMMCFG_STACK_GUARD_SIZE_DEFAULT;
        xwskd->idle.flag = (xwsq_t)XWMP_SKDOBJ_FLAG_PRIVILEGED;
        xwospl_skd_init_stack(&xwskd->idle, xwmp_cthd_return);
#if (defined(XWOSCFG_SKD_IDLE_TLS) && (1 == XWOSCFG_SKD_IDLE_TLS))
        xwospl_tls_init(&xwskd->idle);
#endif
}

/**
 * @brief 关闭CPU的中断顶半部
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_dsth(struct xwmp_skd * xwskd)
{
        xwospl_cpuirq_disable_lc();
        xwaop_add(xwsq_t, &xwskd->dis_th_cnt, 1, NULL, NULL);
        return xwskd;
}

/**
 * @brief 开启CPU的中断顶半部
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_enth(struct xwmp_skd * xwskd)
{
        xwsq_t nv;

        xwaop_sub(xwsq_t, &xwskd->dis_th_cnt, 1, &nv, NULL);
        if ((xwsq_t)0 == nv) {
                xwospl_cpuirq_enable_lc();
        }
        return xwskd;
}

/**
 * @brief 保存调度器的中断顶半部禁止计数器并关闭中断
 * @param[in] xwskd: XWOS MP调度器的指针
 * @param[out] dis_th_cnt: 指向缓冲区的指针，通过此缓冲区返回计数器的值
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_svth(struct xwmp_skd * xwskd, xwsq_t * dis_th_cnt)
{
        xwospl_cpuirq_disable_lc();
        xwaop_add(xwsq_t, &xwskd->dis_th_cnt, 1, NULL, dis_th_cnt);
        return xwskd;
}

/**
 * @brief 恢复调度器的中断顶半部禁止计数器
 * @param[in] xwskd: XWOS MP调度器的指针
 * @param[in] dis_th_cnt: 计数器的值
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_rsth(struct xwmp_skd * xwskd, xwsq_t dis_th_cnt)
{
        xwaop_write(xwsq_t, &xwskd->dis_th_cnt, dis_th_cnt, NULL);
        if ((xwsq_t)0 == dis_th_cnt) {
                xwospl_cpuirq_enable_lc();
        }
        return xwskd;
}

/**
 * @brief 测试是否允许进入中断顶半部
 * @param[in] xwskd: XWOS MP调度器的指针
 * @retval true: 允许
 * @retval false: 禁止
 */
__xwmp_code
bool xwmp_skd_tstth(struct xwmp_skd * xwskd)
{
        xwsq_t cnt;

        cnt = xwaop_load(xwsq_t, &xwskd->dis_th_cnt, xwaop_mo_acquire);
        return ((xwsq_t)0 == cnt);
}

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
/**
 * @brief 中断底半部的主函数
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_skd_bhd(struct xwmp_skd * xwskd)
{
        struct xwmp_bh_node * bhn;
        xwsq_t nv;

        while (true) {
                xwmp_rawly_lock_cpuirq(&xwskd->bhcb.lock);
                if (!xwlib_bclst_tst_empty(&xwskd->bhcb.list)) {
                        bhn = xwlib_bclst_first_entry(&xwskd->bhcb.list,
                                                      struct xwmp_bh_node,
                                                      node);
                        xwlib_bclst_del_init(&bhn->node);
                        xwmp_rawly_unlock_cpuirq(&xwskd->bhcb.lock);
                        bhn->func(bhn->arg);
                } else {
                        xwmp_rawly_unlock_cpuirq(&xwskd->bhcb.lock);
                }
                xwaop_sub(xwsq_t, &xwskd->req_bh_cnt, 1, &nv, NULL);
                if ((xwsq_t)0 == nv) {
                        xwmp_skd_bh_yield(xwskd);
                }
        }
        return XWOK;
}

static __xwmp_rodata
const char xwmp_skd_bhd_name[] = "BH";

/**
 * @brief 初始化中断底半部栈
 * @param[in] xwskd: XWOS MP调度器的指针
 */
static __xwmp_code
void xwmp_skd_init_bhd(struct xwmp_skd * xwskd)
{
        xwskd->bh.name = xwmp_skd_bhd_name;
        xwskd->bh.main = (xwmp_thd_f)xwmp_skd_bhd;
        xwskd->bh.arg = xwskd;
        xwskd->bh.size = XWOSCFG_SKD_BH_STACK_SIZE;
        xwskd->bh.base = (xwstk_t *)xwmp_skd_bhd_stack[xwskd->id];
#  if defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK)
        xwskd->bh.sp = &xwskd->bh.base[(xwskd->bh.size / sizeof(xwstk_t))];
        xwskd->bh.tls = &xwskd->bh.base[0];
#  elif defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK)
        xwskd->bh.sp = &xwskd->bh.base[(xwskd->bh.size / sizeof(xwstk_t)) - 1U];
        xwskd->bh.tls = &xwskd->bh.base[0];
#  elif defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK)
        xwskd->bh.sp = &xwskd->bh.base[-1];
        xwskd->bh.tls = &xwskd->bh.base[(attr->stack_size / sizeof(xwstk_t))];
#  elif defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK)
        xwskd->bh.sp = &xwskd->bh.base[0];
        xwskd->bh.tls = &xwskd->bh.base[(attr->stack_size / sizeof(xwstk_t))];
#  else
#    error "Unknown stack type!"
#  endif
        xwskd->bh.guard_base = xwskd->bh.tls;
        xwskd->bh.guard = (xwsz_t)XWMMCFG_STACK_GUARD_SIZE_DEFAULT;
        xwskd->bh.flag = (xwsq_t)XWMP_SKDOBJ_FLAG_PRIVILEGED;
        xwospl_skd_init_stack(&xwskd->bh, xwmp_cthd_return);
#if (defined(XWOSCFG_SKD_BH_TLS) && (1 == XWOSCFG_SKD_BH_TLS))
        xwospl_tls_init(&xwskd->bh);
#endif
}

/**
 * @brief 关闭调度器的中断底半部
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_dsbh(struct xwmp_skd * xwskd)
{
        xwaop_add(xwsq_t, &xwskd->dis_bh_cnt, 1, NULL, NULL);
        return xwskd;
}

/**
 * @brief 开启调度器的中断底半部
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_enbh(struct xwmp_skd * xwskd)
{
        xwsq_t nv;

        xwaop_sub(xwsq_t, &xwskd->dis_bh_cnt, 1, &nv, NULL);
        if ((xwsq_t)0 == nv) {
                if ((xwsq_t)0 != xwaop_load(xwsq_t, &xwskd->req_bh_cnt,
                                            xwaop_mo_acquire)) {
                        xwmp_skd_sw_bh(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                }
        }
        return xwskd;
}

/**
 * @brief 保存调度器的中断底半部禁止计数器并关闭中断底半部
 * @param[in] xwskd: XWOS MP调度器的指针
 * @param[out] dis_bh_cnt: 指向缓冲区的指针，通过此缓冲区返回计数器的值
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_svbh(struct xwmp_skd * xwskd, xwsq_t * dis_bh_cnt)
{
        xwaop_add(xwsq_t, &xwskd->dis_bh_cnt, 1, NULL, dis_bh_cnt);
        return xwskd;
}

/**
 * @brief 恢复调度器的中断底半部禁止计数器
 * @param[in] xwskd: XWOS MP调度器的指针
 * @param[in] dis_bh_cnt: 计数器的值
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_rsbh(struct xwmp_skd * xwskd, xwsq_t dis_bh_cnt)
{
        xwaop_write(xwsq_t, &xwskd->dis_bh_cnt, dis_bh_cnt, NULL);
        if ((xwsq_t)0 == dis_bh_cnt) {
                if ((xwsq_t)0 != xwaop_load(xwsq_t, &xwskd->req_bh_cnt,
                                            xwaop_mo_acquire)) {
                        xwmp_skd_sw_bh(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                }
        }
        return xwskd;
}

/**
 * @brief 测试调度器是否允许进入中断底半部
 * @param[in] xwskd: XWOS MP调度器的指针
 * @retval true: 允许
 * @retval false: 禁止
 */
__xwmp_code
bool xwmp_skd_tstbh(struct xwmp_skd * xwskd)
{
        xwsq_t cnt;

        cnt = xwaop_load(xwsq_t, &xwskd->dis_bh_cnt, xwaop_mo_acquire);
        return ((xwsq_t)0 == cnt);
}

/**
 * @brief 切换至中断底半部上下文
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EPERM: 中断底半部已被禁止
 * @retval -EINPROGRESS: 切换上下文的过程正在进行中
 * @retval -EALREADY: 当前上下文已经是中断底半部上下文
 */
static __xwmp_code
xwer_t xwmp_skd_sw_bh(struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        if ((xwsq_t)0 == xwaop_load(xwsq_t, &xwskd->dis_bh_cnt, xwaop_mo_relaxed)) {
                xwmp_rawly_lock_cpuirqsv(&xwskd->cxlock, &cpuirq);
                if (NULL != xwskd->pstk) {
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                        rc = -EINPROGRESS;
                } else if (XWMP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                        rc = -EALREADY;
                } else {
                        xwskd->pstk = xwskd->cstk;
                        xwskd->cstk = XWMP_SKD_BH_STK(xwskd);
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                        xwospl_skd_req_swcx(xwskd);
                        rc = XWOK;
                }
        } else {
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief 请求切至中断底半部
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码 @ref xwmp_skd_sw_bh()
 */
__xwmp_code
xwer_t xwmp_skd_req_bh(struct xwmp_skd * xwskd)
{
        xwaop_add(xwsq_t, &xwskd->req_bh_cnt, 1, NULL, NULL);
        return xwmp_skd_sw_bh(xwskd);
}

/**
 * @brief 从中断底半部上下文中返回到线程上下文
 * @param[in] xwskd: XWOS MP调度器的指针
 */
static __xwmp_code
void xwmp_skd_bh_yield(struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;

        xwmp_rawly_lock_cpuirqsv(&xwskd->cxlock, &cpuirq);
        xwskd->cstk = xwskd->pstk;
        xwskd->pstk = XWMP_SKD_BH_STK(xwskd);
        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
        xwospl_skd_req_swcx(xwskd);
}

/**
 * @brief 测试调度器的当前上下文是否为中断底半部上下文
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
__xwmp_code
bool xwmp_skd_tst_in_bh(struct xwmp_skd * xwskd)
{
        return (XWMP_SKD_BH_STK(xwskd) == xwskd->cstk);
}
#endif

/**
 * @brief 关闭调度器的抢占
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_dspmpt(struct xwmp_skd * xwskd)
{
        xwaop_add(xwsq_t, &xwskd->dis_pmpt_cnt, 1, NULL, NULL);
        return xwskd;
}

/**
 * @brief 开启调度器的抢占
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_enpmpt(struct xwmp_skd * xwskd)
{
        xwsq_t nv;

        xwaop_sub(xwsq_t, &xwskd->dis_pmpt_cnt, 1, &nv, NULL);
        if ((xwsq_t)0 == nv) {
                if ((xwsq_t)0 != xwaop_load(xwsq_t, &xwskd->req_chkpmpt_cnt,
                                            xwaop_mo_acquire)) {
                        xwmp_skd_chkpmpt(xwskd);
                }
        }
        return xwskd;
}

/**
 * @brief 保存调度器的抢占禁止计数器并关闭抢占
 * @param[in] xwskd: XWOS MP调度器的指针
 * @param[out] dis_pmpt_cnt: 指向缓冲区的指针，此缓冲区用于返回禁止抢占计数
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_svpmpt(struct xwmp_skd * xwskd, xwsq_t * dis_pmpt_cnt)
{
        xwaop_add(xwsq_t, &xwskd->dis_pmpt_cnt, 1, NULL, dis_pmpt_cnt);
        return xwskd;
}

/**
 * @brief 恢复调度器的抢占禁止计数器
 * @param[in] xwskd: XWOS MP调度器的指针
 * @param[in] dis_pmpt_cnt: 禁止抢占计数
 * @return XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_rspmpt(struct xwmp_skd * xwskd, xwsq_t dis_pmpt_cnt)
{
        xwaop_write(xwsq_t, &xwskd->dis_pmpt_cnt, dis_pmpt_cnt, NULL);
        if ((xwsq_t)0 == dis_pmpt_cnt) {
                if ((xwsq_t)0 != xwaop_load(xwsq_t, &xwskd->req_chkpmpt_cnt,
                                            xwaop_mo_acquire)) {
                        xwmp_skd_chkpmpt(xwskd);
                }
        }
        return xwskd;
}

/**
 * @brief 测试调度器是否允许抢占
 * @param[in] xwskd: XWOS MP调度器的指针
 * @retval true: 允许
 * @retval false: 禁止
 */
__xwmp_code
bool xwmp_skd_tstpmpt(struct xwmp_skd * xwskd)
{
        xwsq_t cnt;

        cnt = xwaop_load(xwsq_t, &xwskd->dis_pmpt_cnt, xwaop_mo_acquire);
        return ((xwsq_t)0 == cnt);
}

/**
 * @brief 检查是否需要抢占
 * @param[in] xwskd: XWOS MP调度器的指针
 * @param[in] t: 线程对象的指针
 * @return 布尔值
 * @retval true: 需要抢占
 * @retval false: 不需要抢占
 * @note
 * - 此函数被调用时需要获得锁xwskd->cxlock并且关闭本地CPU的中断。
 */
static __xwmp_code
bool xwmp_skd_chkpmpt_thd(struct xwmp_skd * xwskd, struct xwmp_thd * t)
{
        struct xwmp_rtrq * xwrtrq;
        bool sched;

        xwrtrq = &xwskd->rq.rt;
        XWOS_BUG_ON(&t->stack == &xwskd->idle);
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        XWOS_BUG_ON(&t->stack == &xwskd->bh);
#endif
        xwmp_rawly_lock(&xwrtrq->lock);
        xwmp_rawly_lock(&t->stlock);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_RUNNING & t->state)) {
                if (t->dprio.r >= xwrtrq->top) {
                        sched = false;
                } else {
                        sched = true;
                }
        } else {
                /* 线程没有运行标志，马上就会发生调度。因此这里不必检测抢占。*/
                sched = false;
        }
        xwmp_rawly_unlock(&t->stlock);
        xwmp_rawly_unlock(&xwrtrq->lock);
        return sched;
}

/**
 * @brief 调度器检测抢占
 * @param[in] xwskd: XWOS MP调度器的指针
 */
__xwmp_code
void xwmp_skd_chkpmpt(struct xwmp_skd * xwskd)
{
        struct xwmp_thd * t;
        struct xwmp_skdobj_stack * cstk;
        xwreg_t cpuirq;
        bool sched;

        if ((xwsq_t)0 != xwaop_load(xwsq_t, &xwskd->dis_pmpt_cnt, xwaop_mo_acquire)) {
                xwaop_add(xwsq_t, &xwskd->req_chkpmpt_cnt, 1, NULL, NULL);
        } else {
                xwmp_rawly_lock_cpuirqsv(&xwskd->cxlock, &cpuirq);
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                if (XWMP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                        cstk = xwskd->pstk;
                } else {
                        cstk = xwskd->cstk;
                }
#else
                cstk = xwskd->cstk;
#endif
                if (XWMP_SKD_IDLE_STK(xwskd) != cstk) {
                        t = xwcc_derof(cstk, struct xwmp_thd, stack);
                        sched = xwmp_skd_chkpmpt_thd(xwskd, t);
                } else {
                        sched = true;
                }
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                xwaop_write(xwsq_t, &xwskd->req_chkpmpt_cnt, 0, NULL);
                if (sched) {
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_skd_req_swcx(xwskd);
                }
        }
}

/**
 * @brief 所有CPU的调度器检测抢占
 */
__xwmp_code
void xwmp_skd_chkpmpt_all(void)
{
        struct xwmp_skd * xwskd;
        xwid_t cpuid;

        for (cpuid = (xwid_t)0; cpuid < (xwid_t)CPUCFG_CPU_NUM; cpuid++) {
                xwskd = &xwmp_skd[cpuid];
                xwmp_skd_chkpmpt(xwskd);
        }
}

/**
 * @brief 检测调度器是否需要切换上下文
 * @param[in] xwskd: XWOS MP调度器的指针
 * @param[in] t: 被检测的线程对象的指针
 * @param[out] pmthd: 指向缓冲区的指针，通过此缓冲区返回抢占线程对象的指针
 * @return 错误码
 * @retval OK: 需要切换上下文
 * @retval <0: 不需要切换上下文
 * @note
 * - 此函数被调用时需要获得锁xwskd->cxlock并且关闭本地CPU的中断。
 */
static __xwmp_code
xwer_t xwmp_skd_check_swcx(struct xwmp_skd * xwskd,
                           struct xwmp_thd * t,
                           struct xwmp_thd ** pmthd)
{
        struct xwmp_rtrq * xwrtrq;
        xwpr_t prio;
        xwer_t rc;

        xwrtrq = &xwskd->rq.rt;
        xwmp_rawly_lock(&xwrtrq->lock);
        xwmp_rawly_lock(&t->stlock);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_RUNNING & t->state)) {
                if (t->dprio.r >= xwrtrq->top) {
                        xwmp_rawly_unlock(&t->stlock);
                        xwmp_rawly_unlock(&xwrtrq->lock);
                        rc = -EPERM;
                } else {
                        prio = t->dprio.r;
                        xwbop_c0m(xwsq_t, &t->state, (xwsq_t)XWMP_SKDOBJ_ST_RUNNING);
                        t->dprio.r = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_rawly_unlock(&t->stlock);
                        xwmp_rawly_unlock(&xwrtrq->lock);
                        rc = xwmp_thd_rq_add_head(t, prio);
                        XWOS_BUG_ON(rc < 0);
                        *pmthd = xwmp_skd_rtrq_choose(xwskd);
                        rc = XWOK;
                }
        } else {
                xwmp_rawly_unlock(&t->stlock);
                xwmp_rawly_unlock(&xwrtrq->lock);
                *pmthd = xwmp_skd_rtrq_choose(xwskd);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 执行上下文的切换
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误吗 @ref xwmp_skd_req_swcx()
 * @note
 * - 此函数被调用时需要获得锁xwskd->cxlock并且关闭本地CPU的中断。
 */
static __xwmp_code
xwer_t xwmp_skd_swcx(struct xwmp_skd * xwskd)
{
        struct xwmp_thd * swthd;
        struct xwmp_thd * cthd;
        xwer_t rc;

        if (XWMP_SKD_IDLE_STK(xwskd) == xwskd->cstk) {
                swthd = xwmp_skd_rtrq_choose(xwskd);
                if (NULL != swthd) {
                        xwskd->pstk = xwskd->cstk;
                        xwskd->cstk = &swthd->stack;
                        rc = XWOK;
                } else {
                        rc = -EAGAIN;
                }
        } else {
                cthd = xwmp_skd_get_cthd(xwskd);
                rc = xwmp_skd_check_swcx(xwskd, cthd, &swthd);
                if (XWOK == rc) {
                        if (cthd == swthd) {
                                /* 当前线程的状态可能在中断或其他CPU中被改变，
                                   并又被加入到就绪队列中，然后在此处又被重新选择。
                                   这种情况下，调度器只需要恢复此线程的
                                   `XWMP_SKDOBJ_ST_RUNNING` 状态，
                                   并不需要切换线程上下文。*/
                                rc = -EAGAIN;
                        } else {
                                if (NULL == swthd) {
                                        xwskd->cstk = XWMP_SKD_IDLE_STK(xwskd);
                                } else {
                                        xwskd->cstk = &swthd->stack;
                                }
                                xwskd->pstk = &cthd->stack;
                                rc = XWOK;
                        }
                } else {
                        rc = -EINVAL;
                }
        }
        return rc;
}

/**
 * @brief 切换上下文之前的回调函数
 * @param[in] xwskd: XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_post_start_lic(struct xwmp_skd * xwskd)
{
        xwskd->state = (xwsq_t)XWMP_SKD_STATE_START;
        xwmp_skd_enpmpt(xwskd);
        return xwskd;
}

/**
 * @brief 切换上下文之前的回调函数
 * @param[in] xwskd: XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_pre_swcx_lic(struct xwmp_skd * xwskd)
{
#if defined(BRDCFG_XWSKD_PRE_SWCX_HOOK) && (1 == BRDCFG_XWSKD_PRE_SWCX_HOOK)
        board_xwskd_pre_swcx_hook(xwskd);
#endif
        return xwskd;
}

/**
 * @brief 切换上下文之后的回调函数
 * @param[in] xwskd: XWOS MP调度器的指针
 */
__xwmp_code
struct xwmp_skd * xwmp_skd_post_swcx_lic(struct xwmp_skd * xwskd)
{
#if defined(BRDCFG_XWSKD_POST_SWCX_HOOK) && (1 == BRDCFG_XWSKD_POST_SWCX_HOOK)
        board_xwskd_post_swcx_hook(xwskd);
#endif
        xwmp_skd_finish_swcx_lic(xwskd);
        return xwskd;
}

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
/**
 * @brief 请求切换上下文
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 * @retval XWOK: 需要触发切换上下文的中断执行切换上下文的过程
 * @retval -EBUSY: 当前上下文为中断底半部上下文
 * @retval -EINVAL: 当前正在运行的线程状态错误
 * @retval -EAGAIN: 不需要切换上下文
 * @retval -EEINPROGRESS: 切换上下文的过程正在进行
 */
__xwmp_code
xwer_t xwmp_skd_req_swcx(struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_rawly_lock_cpuirqsv(&xwskd->cxlock, &cpuirq);
        xwskd->req_schedule_cnt++;
        if (NULL != xwskd->pstk) {
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                rc = -EINPROGRESS;
        } else if (XWMP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                rc = -EBUSY;
        } else {
                xwskd->pstk = err_ptr(-EINVAL); /* Invalidate other caller. */
                rc = xwmp_skd_swcx(xwskd);
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                if (XWOK == rc) {
                        xwospl_skd_req_swcx(xwskd);
                } else {
                        // cppcheck-suppress [misra-c2012-17.2]
                        xwmp_skd_finish_swcx_lic(xwskd);
                }
        }
        return rc;
}

/**
 * @brief 结束上下文的切换
 * @param[in] xwskd: XWOS MP调度器的指针
 * @note
 * - 此函数需要在BSP切换线程上下文的中断函数的最后一步调用。
 */
static __xwmp_code
void xwmp_skd_finish_swcx_lic(struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;
        xwsq_t rbc;

        xwmp_rawly_lock_cpuirqsv(&xwskd->cxlock, &cpuirq);
        if (XWMP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                /* Finish switching context from thread to BH */
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
        } else if (XWMP_SKD_BH_STK(xwskd) == xwskd->pstk) {
                /* Finish switching context from BH to thread */
                xwskd->pstk = NULL;
                rbc = xwaop_load(xwsq_t, &xwskd->req_bh_cnt, xwaop_mo_relaxed);
                if (rbc >= (xwsq_t)1) {
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                        xwmp_skd_sw_bh(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                } else if (xwskd->req_schedule_cnt > (xwsq_t)0) {
                        xwskd->req_schedule_cnt = (xwsq_t)0;
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                        // cppcheck-suppress [misra-c2012-17.2, misra-c2012-17.7]
                        xwmp_skd_req_swcx(xwskd);
                } else {
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                }
        } else {
                /* Finish switching context from thread to thread */
                xwskd->pstk = NULL;
                xwskd->req_schedule_cnt--;
                rbc = xwaop_load(xwsq_t, &xwskd->req_bh_cnt, xwaop_mo_relaxed);
                if (rbc >= (xwsq_t)1) {
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                        xwmp_skd_sw_bh(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                } else if (xwskd->req_schedule_cnt > (xwsq_t)0) {
                        xwskd->req_schedule_cnt = (xwsq_t)0;
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                        // cppcheck-suppress [misra-c2012-17.2, misra-c2012-17.7]
                        xwmp_skd_req_swcx(xwskd);
                } else {
                        xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                }
        }
}
#else
/**
 * @brief 请求切换上下文
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 * @retval XWOK: 需要触发切换上下文的中断执行切换上下文的过程
 * @retval -EINVAL: 当前正在运行的线程状态错误
 * @retval -EAGAIN: 不需要切换上下文
 * @retval -EEINPROGRESS: 切换上下文的过程正在进行
 */
__xwmp_code
xwer_t xwmp_skd_req_swcx(struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_rawly_lock_cpuirqsv(&xwskd->cxlock, &cpuirq);
        xwskd->req_schedule_cnt++;
        if (NULL != xwskd->pstk) {
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                rc = -EINPROGRESS;
        } else {
                xwskd->pstk = err_ptr(-EINVAL); /* Invalidate other caller. */
                rc = xwmp_skd_swcx(xwskd);
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                if (XWOK == rc) {
                        xwospl_skd_req_swcx(xwskd);
                } else {
                        // cppcheck-suppress [misra-c2012-17.2]
                        xwmp_skd_finish_swcx_lic(xwskd);
                }
        }
        return rc;
}

/**
 * @brief 结束上下文的切换
 * @param[in] xwskd: XWOS MP调度器的指针
 * @note
 * - 此函数需要在BSP切换线程上下文的中断函数的最后一步调用。
 */
static __xwmp_code
void xwmp_skd_finish_swcx_lic(struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;

        xwmp_rawly_lock_cpuirqsv(&xwskd->cxlock, &cpuirq);
        /* Finish switching context from thread to thread */
        xwskd->pstk = NULL;
        xwskd->req_schedule_cnt--;
        if (xwskd->req_schedule_cnt > (xwsq_t)0) {
                xwskd->req_schedule_cnt = (xwsq_t)0;
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
                // cppcheck-suppress [misra-c2012-17.2]
                xwmp_skd_req_swcx(xwskd);
        } else {
                xwmp_rawly_unlock_cpuirqrs(&xwskd->cxlock, cpuirq);
        }
}
#endif

/**
 * @brief 将调度器的唤醒锁计数器加1
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: 当前调度器正在进入低功耗模式
 */
__xwmp_code
xwer_t xwmp_skd_inc_wklkcnt(struct xwmp_skd * xwskd)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq_t, &xwskd->pm.wklkcnt,
                                (xwsq_t)XWMP_SKD_WKLKCNT_UNLOCKED,
                                (xwsq_t)1,
                                NULL, NULL);
        return rc;
}

/**
 * @brief 将调度器的唤醒锁计数器减1
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: 当前调度器正在进入低功耗
 */
__xwmp_code
xwer_t xwmp_skd_dec_wklkcnt(struct xwmp_skd * xwskd)
{
        xwer_t rc;
        xwsq_t nv;

        rc = xwaop_tge_then_sub(xwsq_t, &xwskd->pm.wklkcnt,
                                (xwsq_t)XWMP_SKD_WKLKCNT_UNLOCKED,
                                (xwsq_t)1,
                                &nv, NULL);
        if ((XWOK == rc) && ((xwsq_t)XWMP_SKD_WKLKCNT_FREEZING == nv)) {
                rc = xwospl_skd_suspend(xwskd);
        }
        return rc;
}

/**
 * @brief 禁止调度器进入暂停模式
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_skd_wakelock_lock(struct xwmp_skd * xwskd)
{
        return xwmp_skd_inc_wklkcnt(xwskd);
}

/**
 * @brief 允许调度器进入暂停模式
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_skd_wakelock_unlock(struct xwmp_skd * xwskd)
{
        return xwmp_skd_dec_wklkcnt(xwskd);
}

/**
 * @brief 中断本地CPU中所有线程的阻塞或睡眠状态，并准备冻结
 * @param[in] xwskd: XWOS MP调度器的指针
 */
static __xwmp_code
void xwmp_skd_reqfrz_intr_all_lic(struct xwmp_skd * xwskd)
{
        struct xwmp_thd * c;
        struct xwmp_thd * n;
        xwreg_t cpuirq;

        xwmp_splk_lock_cpuirqsv(&xwskd->thdlistlock, &cpuirq);
        // cppcheck-suppress [misra-c2012-12.3, misra-c2012-14.2]
        xwlib_bclst_itr_next_entry_safe(c, n, &xwskd->thdlist,
                                        struct xwmp_thd, thdnode) {
                xwmp_thd_grab(c); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_splk_unlock_cpuirqrs(&xwskd->thdlistlock, cpuirq);
                xwmp_thd_reqfrz_lic(c); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_thd_intr(c); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_thd_put(c); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_splk_lock_cpuirqsv(&xwskd->thdlistlock, &cpuirq);
        }
        xwmp_splk_unlock_cpuirqrs(&xwskd->thdlistlock, cpuirq);
}

/**
 * @brief 通知所有线程已经冻结（中断上下文中执行的部分）
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_skd_notify_allfrz_lic(struct xwmp_skd * xwskd)
{
        xwer_t rc;
        xwsq_t nv;

        rc = xwaop_teq_then_sub(xwsq_t, &xwskd->pm.wklkcnt,
                                (xwsq_t)XWMP_SKD_WKLKCNT_FREEZING,
                                (xwsq_t)1,
                                &nv, NULL);
        if ((XWOK == rc) && ((xwsq_t)XWMP_SKD_WKLKCNT_ALLFRZ == nv)) {
                // cppcheck-suppress [misra-c2012-17.7]
                xwmp_syshwt_stop(&xwskd->tt.hwt);
        } else {
                rc = -EINTR;
        }
        xwmp_skd_req_swcx(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        return rc;
}

/**
 * @brief 通知所有线程已经冻结（空闲任务中执行的部分）
 * @param[in] xwskd: XWOS MP调度器的指针
 */
static __xwmp_code
void xwmp_skd_notify_allfrz_lc(struct xwmp_skd * xwskd)
{
        xwsq_t nv;
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq_t, &xwskd->pm.wklkcnt,
                                (xwsq_t)XWMP_SKD_WKLKCNT_ALLFRZ,
                                (xwsq_t)1,
                                &nv, NULL);
        if ((XWOK == rc) && ((xwsq_t)XWMP_SKD_WKLKCNT_SUSPENDED == nv)) {
                if (NULL != xwskd->pm.xwpmdm) {
                        xwmp_pmdm_report_xwskd_suspended(xwskd->pm.xwpmdm);
                }
        }
}

/**
 * @brief 解冻本地CPU中的所有线程
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 */
static __xwmp_code
void xwmp_skd_thaw_allfrz_lic(struct xwmp_skd * xwskd)
{
        struct xwmp_thd * c;
        struct xwmp_thd * n;
        xwreg_t cpuirq;

        xwmp_splk_lock_cpuirqsv(&xwskd->pm.lock, &cpuirq);
        xwmp_splk_lock(&xwskd->thdlistlock);
        // cppcheck-suppress [misra-c2012-12.3, misra-c2012-14.2]
        xwlib_bclst_itr_next_entry_safe(c, n, &xwskd->thdlist,
                                        struct xwmp_thd, thdnode) {
                xwmp_thd_grab(c); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_splk_unlock(&xwskd->thdlistlock);
                xwmp_thd_thaw_lic_pmlk(c); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_thd_put(c); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_splk_lock(&xwskd->thdlistlock);
        }
        xwmp_splk_unlock(&xwskd->thdlistlock);
        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
        xwmp_skd_req_swcx(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        XWOS_BUG_ON(xwskd->pm.frz_thd_cnt != (xwsq_t)0);
}

/**
 * @brief 暂停本地CPU的调度器
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 * @note
 * - 此函数只能在CPU自身的调度器服务中断中执行，当电源管理的代码运行于
 *   其他CPU上时，可通过 @ref xwmp_skd_suspend() 向对应CPU申请
 *   调度器服务中断并执行此函数。
 */
__xwmp_code
xwer_t xwmp_skd_suspend_lic(struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;

        xwmp_skd_reqfrz_intr_all_lic(xwskd);
        xwmp_splk_lock_cpuirqsv(&xwskd->pm.lock, &cpuirq);
        xwmp_splk_lock(&xwskd->thdlistlock);
        if (xwskd->thd_num == xwskd->pm.frz_thd_cnt) {
                xwmp_splk_unlock(&xwskd->thdlistlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                // cppcheck-suppress [misra-c2012-17.7]
                xwmp_skd_notify_allfrz_lic(xwskd);
        } else {
                xwmp_splk_unlock(&xwskd->thdlistlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
        }
        return XWOK;
}

/**
 * @brief XWMP API：申请暂停调度器
 * @param[in] cpuid: CPU的ID
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * + 调度器暂停时会通知所有线程进入冻结状态，以便执行低功耗的代码流程。
 */
__xwmp_api
xwer_t xwmp_skd_suspend(xwid_t cpuid)
{
        struct xwmp_skd * xwskd;
        xwer_t rc;

        rc = xwmp_skd_get_by_cpuid(cpuid, &xwskd);
        if (XWOK == rc) {
                rc = xwmp_skd_dec_wklkcnt(xwskd);
        }
        return rc;
}

/**
 * @brief 继续本地CPU调度器
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 错误码
 * @details
 * 此函数只能在CPU自身的调度器服务中断中执行，因此当电源管理的代码运行在其他CPU上时，
 * 可通过 @ref xwmp_skd_resume() 向其他CPU申请执行此函数。
 */
__xwmp_code
xwer_t xwmp_skd_resume_lic(struct xwmp_skd * xwskd)
{
        xwer_t rc;
        xwsq_t nv;
        xwsq_t ov;

        do { // cppcheck-suppress [misra-c2012-15.4]
                rc = xwaop_teq_then_add(xwsq_t, &xwskd->pm.wklkcnt,
                                        (xwsq_t)XWMP_SKD_WKLKCNT_SUSPENDED,
                                        (xwsq_t)1,
                                        &nv, &ov);
                if ((XWOK == rc) && ((xwsq_t)XWMP_SKD_WKLKCNT_ALLFRZ == nv)) {
                        xwmp_pmdm_report_xwskd_resuming(xwskd->pm.xwpmdm);
                }

                rc = xwaop_teq_then_add(xwsq_t, &xwskd->pm.wklkcnt,
                                        (xwsq_t)XWMP_SKD_WKLKCNT_ALLFRZ,
                                        (xwsq_t)1,
                                        &nv, &ov);
                if ((XWOK == rc) && ((xwsq_t)XWMP_SKD_WKLKCNT_THAWING == nv)) {
                        rc = xwmp_syshwt_start(&xwskd->tt.hwt);
                        XWOS_BUG_ON(rc < 0);
                }

                rc = xwaop_teq_then_add(xwsq_t, &xwskd->pm.wklkcnt,
                                        (xwsq_t)XWMP_SKD_WKLKCNT_THAWING,
                                        (xwsq_t)1,
                                        &nv, &ov);
                if ((XWOK == rc) && ((xwsq_t)XWMP_SKD_WKLKCNT_UNLOCKED == nv)) {
                        xwmp_skd_thaw_allfrz_lic(xwskd);
                        rc = XWOK;
                        break;
                } else if (ov >= (xwsq_t)XWMP_SKD_WKLKCNT_UNLOCKED) {
                        rc = -EALREADY;
                        break;
                } else {}
        } while (true);
        return rc;
}

/**
 * @brief XWMP API：从低功耗状态恢复调度器
 * @param[in] cpuid: CPU的ID
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_skd_resume(xwid_t cpuid)
{
        struct xwmp_skd * xwskd;
        xwid_t localid;
        xwer_t rc;

        localid = xwmp_skd_get_cpuid_lc();
        if (localid == cpuid) {
                xwskd = xwmp_skd_get_lc();
                if (XWOK == xwmp_irq_get_id(NULL)) {
                        rc = xwmp_skd_resume_lic(xwskd);
                } else {
                        rc = xwospl_skd_resume(xwskd);
                }
        } else {
                rc = xwmp_skd_get_by_cpuid(cpuid, &xwskd);
                if (XWOK == rc) {
                        rc = xwospl_skd_resume(xwskd);
                }
        }
        return rc;
}

/**
 * @brief XWMP API：获取调度器电源管理状态
 * @param[in] xwskd: XWOS MP调度器的指针
 * @return 状态值 @ref xwmp_skd_wakelock_cnt_em
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwsq_t xwmp_skd_get_pm_state(struct xwmp_skd * xwskd)
{
        xwsq_t wklkcnt;

        wklkcnt = xwaop_load(xwsq_t, &xwskd->pm.wklkcnt, xwaop_mo_acquire);
        return wklkcnt;
}

__xwmp_api
void xwmp_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        struct xwmp_skd * xwskd;
        xwirq_t irqn;
        xwer_t rc;
        xwsq_t ctx;

        rc = xwmp_irq_get_id(&irqn);
        if (XWOK == rc) {
                ctx = (xwsq_t)XWMP_SKD_CONTEXT_ISR;
                if (NULL != irqnbuf) {
                        *irqnbuf = irqn;
                }
        } else {
                xwskd = xwmp_skd_get_lc();
                if ((xwsq_t)XWMP_SKD_STATE_START == xwskd->state) {
                        if (XWMP_SKD_IDLE_STK(xwskd) == xwskd->cstk) {
                                ctx = (xwsq_t)XWMP_SKD_CONTEXT_IDLE;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                        } else if (XWMP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                                ctx = (xwsq_t)XWMP_SKD_CONTEXT_BH;
#endif
                        } else {
                                ctx = (xwsq_t)XWMP_SKD_CONTEXT_THD;
                        }
                } else {
                        ctx = (xwsq_t)XWMP_SKD_CONTEXT_BOOT;
                }
        }
        if (NULL != ctxbuf) {
                *ctxbuf = ctx;
        }
}

__xwmp_api
bool xwmp_skd_prio_tst_valid(xwpr_t prio)
{
        return (prio <= XWMP_SKD_PRIORITY_INVALID);
}
