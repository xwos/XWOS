/**
 * @file
 * @brief XWOS UP内核：调度器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - 函数suffix意义：
 *   1. _lc: Local Context，是指只能在“本地”CPU的中执行的代码；
 *   2. _lic: Local ISR Context，是指只能在“本地”CPU的中断上下文中执行的代码。
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/ospl/tls.h>
#include <xwos/up/pm.h>
#include <xwos/up/thd.h>
#include <xwos/up/rtrq.h>
#include <xwos/up/tt.h>
#include <xwos/up/skd.h>
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
#  include <xwos/up/bh.h>
#endif

/* #define XWOS_SKDLOGF */ /**< 调试日志开关 */
#ifdef XWOS_SKDLOGF
#  define xwos_skdlogf(lv, xwskd, fmt, ...) \
          xwlogf(lv, "XWSKD.%d", fmt, xwskd->id, ##__VA_ARGS__)
#else
#  define xwos_skdlogf(lv, xwskd, fmt, ...)
#endif

/**
 * @brief XWOS UP调度器
 */
__xwup_data struct xwup_skd xwup_skd; // cppcheck-suppress [misra-c2012-8.4]

/**
 * @brief XWOS UP调度器的空闲线程栈
 */
static __xwup_data __xwcc_alignl1cache
xwu8_t xwup_skd_idled_stack[XWOSCFG_SKD_IDLE_STACK_SIZE];

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
/**
 * @brief XWOS UP调度器的中断底半部栈
 */
static __xwup_data __xwcc_alignl1cache
xwu8_t xwup_skd_bhd_stack[XWOSCFG_SKD_BH_STACK_SIZE];
#endif

#if defined(BRDCFG_XWSKD_IDLE_HOOK) && (1 == BRDCFG_XWSKD_IDLE_HOOK)
extern
void board_xwskd_idle_hook(struct xwup_skd * xwskd);
#endif

#if defined(BRDCFG_XWSKD_PRE_SWCX_HOOK) && (1 == BRDCFG_XWSKD_PRE_SWCX_HOOK)
extern
void board_xwskd_pre_swcx_hook(struct xwup_skd * xwskd);
#endif

#if defined(BRDCFG_XWSKD_POST_SWCX_HOOK) && (1 == BRDCFG_XWSKD_POST_SWCX_HOOK)
extern
void board_xwskd_post_swcx_hook(struct xwup_skd * xwskd);
#endif

static __xwup_code
struct xwup_thd * xwup_skd_rtrq_choose(void);

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
static __xwup_code
void xwup_skd_del_thd_lc(struct xwup_skd * xwskd);
#endif

static __xwup_code
xwer_t xwup_skd_idled(struct xwup_skd * xwskd);

static __xwup_code
void xwup_skd_init_idled(void);

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
static __xwup_code
xwer_t xwup_skd_bhd(struct xwup_skd * xwskd);

static __xwup_code
void xwup_skd_init_bhd(void);
#endif

static __xwup_code
bool xwup_skd_chkpmpt_thd(struct xwup_thd * t);

static __xwup_code
xwer_t xwup_skd_check_swcx(struct xwup_thd * t, struct xwup_thd ** pmthd);

static __xwup_code
xwer_t xwup_skd_swcx(void);

static __xwup_code
void xwup_skd_finish_swcx_lic(struct xwup_skd * xwskd);

#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
static __xwup_code
void xwup_skd_notify_allfrz_lc(struct xwup_skd * xwskd);

static __xwup_code
xwer_t xwup_skd_thaw_allfrz_lic(struct xwup_skd * xwskd);
#endif

/**
 * @brief XWUP INIT API：初始化本地CPU的调度器
 * @return 错误码
 * @note
 * - 重入性：本函数只可在系统初始化时调用一次
 */
__xwup_init_code
xwer_t xwup_skd_init_lc(void)
{
        xwer_t rc;
        struct xwup_skd * xwskd;

        xwskd = &xwup_skd;
        xwskd->cstk = XWUP_SKD_IDLE_STK(xwskd);
        xwskd->pstk = NULL;
        xwskd->state = XWUP_SKD_STATE_INIT;
        xwskd->req_schedule_cnt = (xwsq_t)0;
        xwskd->req_chkpmpt_cnt = (xwsq_t)0;
        xwskd->dis_pmpt_cnt = (xwsq_t)0;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwskd->req_bh_cnt = (xwsq_t)0;
        xwskd->dis_bh_cnt = (xwsq_t)0;
        xwup_bh_cb_init(&xwskd->bhcb);
        xwup_skd_init_bhd();
#endif
        xwskd->dis_th_cnt = (xwsq_t)0;
        xwlib_bclst_init_head(&xwskd->thdlist);
        xwskd->thd_num = (xwsz_t)0;
        xwlib_bclst_init_head(&xwskd->thdelist);
        rc = xwup_tt_init(&xwskd->tt);
        if (rc < 0) {
                goto err_tt_init;
        }
        xwup_rtrq_init(&xwskd->rq.rt);
        xwup_skd_init_idled();
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwskd->pm.wklkcnt = (xwsq_t)XWUP_SKD_WKLKCNT_UNLOCKED;
        xwskd->pm.frz_thd_cnt = (xwsq_t)0;
        xwlib_bclst_init_head(&xwskd->pm.frzlist);
        xwskd->pm.cb.resume = NULL;
        xwskd->pm.cb.suspend = NULL;
        xwskd->pm.cb.wakeup = NULL;
        xwskd->pm.cb.sleep = NULL;
        xwskd->pm.cb.arg = NULL;
#endif
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
err_tt_init:
        return rc;
}

/**
 * @brief XWUP INIT API：启动本地CPU的调度器
 * @return 错误码
 * @note
 * - 重入性：只可调用一次
 * - 此函数不会返回
 */
__xwup_init_code
xwer_t xwup_skd_start_lc(void)
{
        struct xwup_skd * xwskd;
        xwer_t rc;

        xwskd = &xwup_skd;
        if ((xwsq_t)XWUP_SKD_STATE_INIT == xwskd->state) {
                xwup_skd_dspmpt_lc();
                rc = xwup_syshwt_start(&xwskd->tt.hwt);
                if (XWOK == rc) {
                        rc = xwospl_skd_start(xwskd);
                        if (rc < 0) {
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwup_syshwt_stop(&xwskd->tt.hwt);
                        }
                }
        } else {
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief XWUP API：启动本地CPU调度器的系统定时器
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_skd_start_syshwt_lc(void)
{
        struct xwup_skd * xwskd;
        xwer_t rc;

        xwskd = &xwup_skd;
        rc = xwup_syshwt_start(&xwskd->tt.hwt);
        return rc;
}

/**
 * @brief XWUP API：关闭本地CPU调度器的系统定时器
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_skd_stop_syshwt_lc(void)
{
        struct xwup_skd * xwskd;
        xwer_t rc;

        xwskd = &xwup_skd;
        rc = xwup_syshwt_stop(&xwskd->tt.hwt);
        return rc;
}

/**
 * @brief 返回本地CPU调度器的指针
 * @return 本地CPU调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_get_lc(void)
{
        return &xwup_skd;
}

/**
 * @brief 获得本地CPU的调度器中正在运行的线程对象的指针
 * @return 本地CPU的调度器中正在运行的线程对象的指针
 */
__xwup_code
struct xwup_thd * xwup_skd_get_cthd_lc(void)
{
        struct xwup_thd * cthd;

        cthd = xwcc_derof(xwup_skd.cstk, struct xwup_thd, stack);
        return cthd;
}

/**
 * @brief 从XWOS UP调度器中选择一个就绪的线程
 * @retval pointer: 被选择的线程对象的指针
 * @retval NULL: 空闲任务
 * @note
 * - 此函数只能在临界区中调用。
 */
static __xwup_code
struct xwup_thd * xwup_skd_rtrq_choose(void)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        struct xwup_thd * t;

        xwskd = &xwup_skd;
        xwrtrq = &xwskd->rq.rt;
        t = xwup_rtrq_choose(xwrtrq);
        if (NULL != t) {
                xwup_rtrq_remove(xwrtrq, t);
                xwbop(xwsq_t, c0m, &t->state, (xwsq_t)XWUP_SKDOBJ_ST_READY);
                xwbop(xwsq_t, s1m, &t->state, (xwsq_t)XWUP_SKDOBJ_ST_RUNNING);
        }
        return t;
}

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
/**
 * @brief 删除 **删除列表** 中的线程
 * @param[in] xwskd: XWOS UP调度器的指针
 * @return 错误码
 */
static __xwup_code
void xwup_skd_del_thd_lc(struct xwup_skd * xwskd)
{
        if (!xwlib_bclst_tst_empty(&xwskd->thdelist)) {
                struct xwup_thd * thd;
                xwospl_cpuirq_disable_lc();
                xwlib_bclst_itr_next_entry_del(thd, &xwskd->thdelist,
                                               struct xwup_thd, thdnode) {
                        xwlib_bclst_del_init(&thd->thdnode);
                        xwospl_cpuirq_enable_lc();
                        xwup_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                        xwospl_cpuirq_disable_lc();
                }
                xwospl_cpuirq_enable_lc();
        }
}
#endif

/**
 * @brief 空闲任务的主函数
 * @param[in] xwskd: XWOS UP调度器的指针
 */
static __xwup_code
xwer_t xwup_skd_idled(struct xwup_skd * xwskd)
{
        XWOS_UNUSED(xwskd);

        while (true) {
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
                xwup_skd_del_thd_lc(xwskd);
#endif
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                xwup_skd_notify_allfrz_lc(xwskd);
#endif
#if (defined(BRDCFG_XWSKD_IDLE_HOOK) && (1 == BRDCFG_XWSKD_IDLE_HOOK))
                board_xwskd_idle_hook(xwskd);
#endif
        }
        return XWOK;
}

static __xwup_rodata
const char xwup_skd_idled_name[] = "IDLE";

/**
 * @brief 初始化空闲线程栈
 */
static __xwup_code
void xwup_skd_init_idled(void)
{
        struct xwup_skd * xwskd;

        xwskd = &xwup_skd;
        xwskd->idle.name = xwup_skd_idled_name;
        xwskd->idle.main = (xwup_thd_f)xwup_skd_idled;
        xwskd->idle.arg = xwskd;
        xwskd->idle.size = XWOSCFG_SKD_IDLE_STACK_SIZE;
        xwskd->idle.base = (xwstk_t *)xwup_skd_idled_stack;
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
        xwskd->idle.flag = (xwsq_t)XWUP_SKDOBJ_FLAG_PRIVILEGED;
        xwospl_skd_init_stack(&xwskd->idle, xwup_cthd_return);
#if (defined(XWOSCFG_SKD_IDLE_TLS) && (1 == XWOSCFG_SKD_IDLE_TLS))
        xwospl_tls_init(&xwskd->idle);
#endif
}

/**
 * @brief 关闭CPU的中断顶半部
 * @return XWOS MP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_dsth_lc(void)
{
        struct xwup_skd * xwskd = &xwup_skd;

        xwospl_cpuirq_disable_lc();
        xwskd->dis_th_cnt++;
        return xwskd;
}

/**
 * @brief 开启CPU的中断顶半部
 * @return XWOS MP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_enth_lc(void)
{
        struct xwup_skd * xwskd = &xwup_skd;

        xwskd->dis_th_cnt--;
        if ((xwsq_t)0 == xwskd->dis_th_cnt) {
                xwospl_cpuirq_enable_lc();
        }
        return xwskd;
}

/**
 * @brief 保存调度器的中断顶半部禁止计数器并关闭中断
 * @param[out] dis_th_cnt: 指向缓冲区的指针，通过此缓冲区返回计数器的值
 * @return XWOS MP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_svth_lc(xwsq_t * dis_th_cnt)
{
        struct xwup_skd * xwskd = &xwup_skd;

        xwospl_cpuirq_disable_lc();
        *dis_th_cnt = xwskd->dis_th_cnt;
        xwskd->dis_th_cnt++;
        return xwskd;
}

/**
 * @brief 恢复调度器的中断顶半部禁止计数器
 * @param[in] dis_th_cnt: 计数器的值
 * @return XWOS MP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_rsth_lc(xwsq_t dis_th_cnt)
{
        struct xwup_skd * xwskd = &xwup_skd;

        xwskd->dis_th_cnt = dis_th_cnt;
        if ((xwsq_t)0 == dis_th_cnt) {
                xwospl_cpuirq_enable_lc();
        }
        return xwskd;
}

/**
 * @brief 测试是否允许进入中断顶半部
 * @retval true: 允许
 * @retval false: 禁止
 */
__xwup_code
bool xwup_skd_tstth_lc(void)
{
        return ((xwsq_t)0 == xwup_skd.dis_th_cnt);
}

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
/**
 * @brief 中断底半部的主函数
 * @param[in] xwskd: XWOS UP调度器的指针
 * @return 错误码
 */
static __xwup_code
xwer_t xwup_skd_bhd(struct xwup_skd * xwskd)
{
        struct xwup_bh_node * bhn;

        while (true) {
                xwospl_cpuirq_disable_lc();
                if (!xwlib_bclst_tst_empty(&xwskd->bhcb.list)) {
                        bhn = xwlib_bclst_first_entry(&xwskd->bhcb.list,
                                                      struct xwup_bh_node,
                                                      node);
                        xwlib_bclst_del_init(&bhn->node);
                        xwospl_cpuirq_enable_lc();
                        bhn->func(bhn->arg);
                        xwospl_cpuirq_disable_lc();
                }
                if (xwskd->req_bh_cnt > (xwsq_t)0) {
                        xwskd->req_bh_cnt--;
                        if ((xwsq_t)0 == xwskd->req_bh_cnt) {
                                xwskd->cstk = xwskd->pstk;
                                xwskd->pstk = XWUP_SKD_BH_STK(xwskd);
                                xwospl_cpuirq_enable_lc();
                                xwospl_skd_req_swcx(xwskd);
                        } else {
                                xwospl_cpuirq_enable_lc();
                        }
                } else {
                        xwospl_cpuirq_enable_lc();
                }
        }
        return XWOK;
}

static __xwup_rodata
const char xwup_skd_bhd_name[] = "BH";

/**
 * @brief 初始化中断底半部栈
 */
static __xwup_code
void xwup_skd_init_bhd(void)
{
        struct xwup_skd * xwskd;

        xwskd = &xwup_skd;
        xwskd->bh.name = xwup_skd_bhd_name;
        xwskd->bh.main = (xwup_thd_f)xwup_skd_bhd;
        xwskd->bh.arg = xwskd;
        xwskd->bh.size = XWOSCFG_SKD_BH_STACK_SIZE;
        xwskd->bh.base = (xwstk_t *)xwup_skd_bhd_stack;
#if defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK)
        xwskd->bh.sp = &xwskd->bh.base[(xwskd->bh.size / sizeof(xwstk_t))];
        xwskd->bh.tls = &xwskd->bh.base[0];
#elif defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK)
        xwskd->bh.sp = &xwskd->bh.base[(xwskd->bh.size / sizeof(xwstk_t)) - 1];
        xwskd->bh.tls = &xwskd->bh.base[0];
#elif defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK)
        xwskd->bh.sp = &xwskd->bh.base[-1];
        xwskd->bh.tls = &xwskd->bh.base[(attr->stack_size / sizeof(xwstk_t))];
#elif defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK)
        xwskd->bh.sp = &xwskd->bh.base[0];
        xwskd->bh.tls = &xwskd->bh.base[(attr->stack_size / sizeof(xwstk_t))];
#else
#  error "Unknown stack type!"
#endif
        xwskd->bh.guard_base = xwskd->bh.tls;
        xwskd->bh.guard = (xwsz_t)XWMMCFG_STACK_GUARD_SIZE_DEFAULT;
        xwskd->bh.flag = (xwsq_t)XWUP_SKDOBJ_FLAG_PRIVILEGED;
        xwospl_skd_init_stack(&xwskd->bh, xwup_cthd_return);
#if (defined(XWOSCFG_SKD_BH_TLS) && (1 == XWOSCFG_SKD_BH_TLS))
        xwospl_tls_init(&xwskd->bh);
#endif
}

/**
 * @brief XWUP API：关闭本地CPU调度器的中断底半部
 * @return XWOS UP调度器的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
struct xwup_skd * xwup_skd_dsbh_lc(void)
{
        struct xwup_skd * xwskd = &xwup_skd;
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->dis_bh_cnt++;
        xwospl_cpuirq_restore_lc(cpuirq);
        return xwskd;
}

/**
 * @brief XWUP API：开启本地CPU调度器的中断底半部
 * @return XWOS UP调度器的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
struct xwup_skd * xwup_skd_enbh_lc(void)
{
        struct xwup_skd * xwskd = &xwup_skd;
        xwreg_t cpuirq;
        bool sched = false;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->dis_bh_cnt--;
        if ((xwsq_t)0 == xwskd->dis_bh_cnt) {
                if ((xwsq_t)0 != xwskd->req_bh_cnt) {
                        if (!xwup_skd_tst_in_bh_lc()) {
                                xwskd->pstk = xwskd->cstk;
                                xwskd->cstk = XWUP_SKD_BH_STK(xwskd);
                                sched = true;
                        }
                }
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        if (sched) {
                xwospl_skd_req_swcx(xwskd);
        }
        return xwskd;
}

/**
 * @brief 保存调度器的中断底半部禁止计数器并关闭中断底半部
 * @param[out] dis_bh_cnt: 指向缓冲区的指针，此缓冲区用于返回禁止中断底半部计数
 * @return XWOS UP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_svbh_lc(xwsq_t * dis_bh_cnt)
{
        struct xwup_skd * xwskd = &xwup_skd;
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        *dis_bh_cnt = xwskd->dis_bh_cnt;
        xwskd->dis_bh_cnt++;
        xwospl_cpuirq_restore_lc(cpuirq);
        return xwskd;
}

/**
 * @brief 恢复调度器的中断底半部禁止计数器
 * @param[in] dis_bh_cnt: 禁止中断底半部计数
 * @return XWOS UP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_rsbh_lc(xwsq_t dis_bh_cnt)
{
        struct xwup_skd * xwskd = &xwup_skd;
        xwreg_t cpuirq;
        bool sched = false;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->dis_bh_cnt = dis_bh_cnt;
        if ((xwsq_t)0 == xwskd->dis_bh_cnt) {
                if ((xwsq_t)0 != xwskd->req_bh_cnt) {
                        if (!xwup_skd_tst_in_bh_lc()) {
                                xwskd->pstk = xwskd->cstk;
                                xwskd->cstk = XWUP_SKD_BH_STK(xwskd);
                                sched = true;
                        }
                }
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        if (sched) {
                xwospl_skd_req_swcx(xwskd);
        }
        return xwskd;
}

/**
 * @brief 测试调度器是否允许进入中断底半部
 * @retval true: 允许
 * @retval false: 禁止
 */
__xwup_code
bool xwup_skd_tstbh_lc(void)
{
        struct xwup_skd * xwskd;

        xwskd = &xwup_skd;
        return ((xwsq_t)0 == xwskd->dis_bh_cnt);
}

/**
 * @brief 请求切换至中断底半部
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EPERM: 中断底半部已被禁止
 * @retval -EINPROGRESS: 切换上下文的过程正在进行中
 * @retval -EALREADY: 当前上下文已经是中断底半部上下文
 */
__xwup_code
xwer_t xwup_skd_req_bh(void)
{
        struct xwup_skd * xwskd = &xwup_skd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->req_bh_cnt++;
        if ((xwsq_t)0 == xwskd->dis_bh_cnt) {
                if (NULL != xwskd->pstk) {
                        rc = -EINPROGRESS;
                } else if (XWUP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                        rc = -EALREADY;
                } else {
                        xwskd->pstk = xwskd->cstk;
                        xwskd->cstk = XWUP_SKD_BH_STK(xwskd);
                        rc = XWOK;
                }
        } else {
                rc = -EPERM;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        if (XWOK == rc) {
                xwospl_skd_req_swcx(xwskd);
        }
        return rc;
}

/**
 * @brief XWUP API：测试当前上下文是否为中断底半部上下文
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
__xwup_api
bool xwup_skd_tst_in_bh_lc(void)
{
        struct xwup_skd * xwskd = &xwup_skd;
        return (XWUP_SKD_BH_STK(xwskd) == xwskd->cstk);
}
#endif

__xwup_api
struct xwup_skd * xwup_skd_dspmpt_lc(void)
{
        struct xwup_skd * xwskd = &xwup_skd;
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->dis_pmpt_cnt++;
        xwospl_cpuirq_restore_lc(cpuirq);
        return xwskd;
}

__xwup_api
struct xwup_skd * xwup_skd_enpmpt_lc(void)
{
        struct xwup_skd * xwskd = &xwup_skd;
        struct xwup_thd * t;
        bool sched = false;
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->dis_pmpt_cnt--;
        if ((xwsq_t)0 == xwskd->dis_pmpt_cnt) {
                if ((xwsq_t)0 != xwskd->req_chkpmpt_cnt) {
                        struct xwup_skdobj_stack * cstk;

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                        if (XWUP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                                cstk = xwskd->pstk;
                        } else {
                                cstk = xwskd->cstk;
                        }
#else
                        cstk = xwskd->cstk;
#endif
                        if (XWUP_SKD_IDLE_STK(xwskd) != cstk) {
                                t = xwcc_derof(cstk, struct xwup_thd, stack);
                                sched = xwup_skd_chkpmpt_thd(t);
                        } else {
                                sched = true;
                        }
                        xwskd->req_chkpmpt_cnt = (xwsq_t)0;
                }
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        if (sched) {
                xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        }
        return xwskd;
}

/**
 * @brief 保存调度器的抢占
 * @param[out] dis_pmpt_cnt: 指向缓冲区的指针，此缓冲区用于返回禁止抢占计数
 * @return XWOS UP调度器的指针
 */
__xwup_api
struct xwup_skd * xwup_skd_svpmpt_lc(xwsq_t * dis_pmpt_cnt)
{
        struct xwup_skd * xwskd = &xwup_skd;
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        *dis_pmpt_cnt = xwskd->dis_pmpt_cnt;
        xwskd->dis_pmpt_cnt++;
        xwospl_cpuirq_restore_lc(cpuirq);
        return xwskd;
}

/**
 * @brief 恢复调度器的抢占
 * @param[in] dis_pmpt_cnt: 禁止抢占计数
 * @return XWOS UP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_rspmpt_lc(xwsq_t dis_pmpt_cnt)
{
        struct xwup_skd * xwskd = &xwup_skd;
        struct xwup_thd * t;
        bool sched = false;
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->dis_pmpt_cnt = dis_pmpt_cnt;
        if ((xwsq_t)0 == xwskd->dis_pmpt_cnt) {
                if ((xwsq_t)0 != xwskd->req_chkpmpt_cnt) {
                        struct xwup_skdobj_stack * cstk;

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                        if (XWUP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                                cstk = xwskd->pstk;
                        } else {
                                cstk = xwskd->cstk;
                        }
#else
                        cstk = xwskd->cstk;
#endif
                        if (XWUP_SKD_IDLE_STK(xwskd) != cstk) {
                                t = xwcc_derof(cstk, struct xwup_thd, stack);
                                sched = xwup_skd_chkpmpt_thd(t);
                        } else {
                                sched = true;
                        }
                        xwskd->req_chkpmpt_cnt = (xwsq_t)0;
                }
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        if (sched) {
                xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        }
        return xwskd;
}

/**
 * @brief 测试调度器是否允许抢占
 * @retval true: 允许
 * @retval false: 禁止
 */
__xwup_code
bool xwup_skd_tstpmpt_lc(void)
{
        struct xwup_skd * xwskd = &xwup_skd;
        return ((xwsq_t)0 == xwskd->dis_pmpt_cnt);
}

/**
 * @brief 检查是否需要抢占
 * @param[in] xwskd: XWOS UP调度器的指针
 * @param[in] t: 线程对象的指针
 * @return 布尔值
 * @retval true: 需要抢占
 * @retval false: 不需要抢占
 * @note
 * - 此函数被调用时需要关闭本地CPU的中断。
 */
static __xwup_code
bool xwup_skd_chkpmpt_thd(struct xwup_thd * t)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        bool sched;

        xwskd = &xwup_skd;
        xwrtrq = &xwskd->rq.rt;
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_RUNNING & t->state)) {
                if (t->prio.d >= xwrtrq->top) {
                        sched = false;
                } else {
                        sched = true;
                }
        } else {
                /* 线程没有运行标志，马上就会发生调度。因此这里不必检测抢占。*/
                sched = false;
        }
        return sched;
}

/**
 * @brief 调度器检测抢占
 */
__xwup_code
void xwup_skd_chkpmpt(void)
{
        struct xwup_skd * xwskd;
        struct xwup_skdobj_stack * cstk;
        struct xwup_thd * t;
        xwreg_t cpuirq;
        bool sched;

        xwskd = &xwup_skd;
        sched = false;
        xwospl_cpuirq_save_lc(&cpuirq);
        if ((xwsq_t)0 != xwskd->dis_pmpt_cnt) {
                xwskd->req_chkpmpt_cnt++;
        } else {
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                if (XWUP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                        cstk = xwskd->pstk;
                } else {
                        cstk = xwskd->cstk;
                }
#else
                cstk = xwskd->cstk;
#endif
                if (XWUP_SKD_IDLE_STK(xwskd) != cstk) {
                        t = xwcc_derof(cstk, struct xwup_thd, stack);
                        sched = xwup_skd_chkpmpt_thd(t);
                } else {
                        sched = true;
                }
                xwskd->req_chkpmpt_cnt = (xwsq_t)0;

        }
        xwospl_cpuirq_restore_lc(cpuirq);
        if (sched) {
                xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        }
}

/**
 * @brief 检测调度器是否需要切换上下文
 * @param[in] t: 被检测的线程对象的指针
 * @param[out] pmthd: 指向缓冲区的指针，通过此缓冲区返回抢占线程对象的指针
 * @return 错误码
 * @retval XWOK: 需要切换上下文
 * @retval <0: 不需要切换上下文
 * @note
 * - 此函数被调用时需要关闭本地CPU的中断。
 */
static __xwup_code
xwer_t xwup_skd_check_swcx(struct xwup_thd * t, struct xwup_thd ** pmthd)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        xwer_t rc;

        xwskd = &xwup_skd;
        xwrtrq = &xwskd->rq.rt;
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_RUNNING & t->state)) {
                if (t->prio.d >= xwrtrq->top) {
                        rc = -EPERM;
                } else {
                        xwbop(xwsq_t, c0m, &t->state, (xwsq_t)XWUP_SKDOBJ_ST_RUNNING);
                        rc = xwup_thd_rq_add_head(t);
                        XWOS_BUG_ON(rc < 0);
                        *pmthd = xwup_skd_rtrq_choose();
                        rc = XWOK;
                }
        } else {
                *pmthd = xwup_skd_rtrq_choose();
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 执行上下文的切换
 * @param[in] xwskd: XWOS UP调度器的指针
 * @return 错误吗 @ref xwup_skd_req_swcx()
 */
static __xwup_code
xwer_t xwup_skd_swcx(void)
{
        struct xwup_skd * xwskd;
        struct xwup_thd * swt;
        struct xwup_thd * cthd;
        xwer_t rc;

        xwskd = &xwup_skd;
        if (XWUP_SKD_IDLE_STK(xwskd) == xwskd->cstk) {
                swt = xwup_skd_rtrq_choose();
                if (NULL != swt) {
                        xwskd->pstk = xwskd->cstk;
                        xwskd->cstk = &swt->stack;
                        rc = XWOK;
                } else {
                        rc = -EAGAIN;
                }
        } else {
                cthd = xwup_skd_get_cthd_lc();
                rc = xwup_skd_check_swcx(cthd, &swt);
                if (XWOK == rc) {
                        if (cthd == swt) {
                                /* 当前线程的状态可能在中断中被改变，
                                   并被加入到就绪队列，然后在这里又被重新选择。
                                   这种情况下，调度器只需要恢复这个线程的
                                   `XWUP_SKDOBJ_ST_RUNNING` 状态，
                                   并不需要切换线程上下文。*/
                                rc = -EAGAIN;
                        } else {
                                if (NULL == swt) {
                                        xwskd->cstk = XWUP_SKD_IDLE_STK(xwskd);
                                } else {
                                        xwskd->cstk = &swt->stack;
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
__xwup_code
struct xwup_skd * xwup_skd_post_start_lic(struct xwup_skd * xwskd)
{
        xwskd->state = (xwsq_t)XWUP_SKD_STATE_START;
        xwup_skd_enpmpt_lc();
        return xwskd;
}

/**
 * @brief 切换上下文之前的回调函数
 * @param[in] xwskd: XWOS UP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_pre_swcx_lic(struct xwup_skd * xwskd)
{
#if defined(BRDCFG_XWSKD_PRE_SWCX_HOOK) && (1 == BRDCFG_XWSKD_PRE_SWCX_HOOK)
        board_xwskd_pre_swcx_hook(xwskd);
#endif
        return xwskd;
}

/**
 * @brief 切换上下文之后的回调函数
 * @param[in] xwskd: XWOS UP调度器的指针
 */
__xwup_code
struct xwup_skd * xwup_skd_post_swcx_lic(struct xwup_skd * xwskd)
{
#if defined(BRDCFG_XWSKD_POST_SWCX_HOOK) && (1 == BRDCFG_XWSKD_POST_SWCX_HOOK)
        board_xwskd_post_swcx_hook(xwskd);
#endif
        xwup_skd_finish_swcx_lic(xwskd);
        return xwskd;
}

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
/**
 * @brief 请求切换上下文
 * @return 错误码
 * @retval XWOK: 没有错误，需要触发切换上下文中断执行切换上下文的过程
 * @retval -EBUSY: 当前上下文为中断底半部上下文
 * @retval -EINVAL: 正在运行的线程状态错误
 * @retval -EAGAIN: 不需要切换上下文
 * @retval -EEINPROGRESS: 切换上下文的过程正在进行
 */
__xwup_code
xwer_t xwup_skd_req_swcx(void)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = &xwup_skd;
        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->req_schedule_cnt++;
        if (NULL != xwskd->pstk) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINPROGRESS;
        } else if (XWUP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EBUSY;
        } else {
                xwskd->pstk = err_ptr(-EINVAL); /* invalidate other caller. */
                rc = xwup_skd_swcx();
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWOK == rc) {
                        xwospl_skd_req_swcx(xwskd);
                } else {
                        // cppcheck-suppress [misra-c2012-17.2]
                        xwup_skd_finish_swcx_lic(xwskd);
                }
        }
        return rc;
}

/**
 * @brief 结束上下文的切换
 * @param[in] xwskd: XWOS UP调度器的指针
 * @note
 * - 此函数需要在BSP切换上下文的中断的最后调用。
 */
static __xwup_code
void xwup_skd_finish_swcx_lic(struct xwup_skd * xwskd)
{
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                /* Finish switch context from thread to BH */
                xwospl_cpuirq_restore_lc(cpuirq);
        } else if (XWUP_SKD_BH_STK(xwskd) == xwskd->pstk) {
                /* Finish switch context from BH to thread */
                xwskd->pstk = NULL;
                if (xwskd->req_bh_cnt >= (xwsq_t)1) {
                        xwskd->pstk = xwskd->cstk;
                        xwskd->cstk = XWUP_SKD_BH_STK(xwskd);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwospl_skd_req_swcx(xwskd);
                } else if (xwskd->req_schedule_cnt > (xwsq_t)0) {
                        xwskd->req_schedule_cnt = (xwsq_t)0;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        // cppcheck-suppress [misra-c2012-17.2, misra-c2012-17.7]
                        xwup_skd_req_swcx();
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        } else {
                /* Finish switch context from thread to thread */
                xwskd->pstk = NULL;
                xwskd->req_schedule_cnt--;
                if (xwskd->req_bh_cnt >= (xwsq_t)1) {
                        xwskd->pstk = xwskd->cstk;
                        xwskd->cstk = XWUP_SKD_BH_STK(xwskd);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwospl_skd_req_swcx(xwskd);
                } else if (xwskd->req_schedule_cnt > (xwsq_t)0) {
                        xwskd->req_schedule_cnt = (xwsq_t)0;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        // cppcheck-suppress [misra-c2012-17.2, misra-c2012-17.7]
                        xwup_skd_req_swcx();
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        }
}
#else
/**
 * @brief 请求切换上下文
 * @return 错误码
 * @retval XWOK: 没有错误，需要触发上下文切换中断执行切换上下文的过程
 * @retval -EBUSY: 当前上下文为中断底半部上下文
 * @retval -EINVAL: 当前正在运行的线程状态错误
 * @retval -EAGAIN: 不需要切换上下文
 * @retval -EEINPROGRESS: 切换上下文的过程正在进行
 */
__xwup_code
xwer_t xwup_skd_req_swcx(void)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = &xwup_skd;
        xwospl_cpuirq_save_lc(&cpuirq);
        xwskd->req_schedule_cnt++;
        if (NULL != xwskd->pstk) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINPROGRESS;
        } else {
                xwskd->pstk = err_ptr(-EINVAL); /* invalidate other caller. */
                rc = xwup_skd_swcx();
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWOK == rc) {
                        xwospl_skd_req_swcx(xwskd);
                } else {
                        // cppcheck-suppress [misra-c2012-17.2]
                        xwup_skd_finish_swcx_lic(xwskd);
                }
        }
        return rc;
}

/**
 * @brief 结束上下文的切换
 * @param[in] xwskd: XWOS UP调度器的指针
 * @note
 * - 此函数需在BSP切换上下文的中断的最后调用。
 */
static __xwup_code
void xwup_skd_finish_swcx_lic(struct xwup_skd * xwskd)
{
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        /* Finish switch context from thread to thread */
        xwskd->pstk = NULL;
        xwskd->req_schedule_cnt--;
        if (xwskd->req_schedule_cnt > (xwsq_t)0) {
                xwskd->req_schedule_cnt = (xwsq_t)0;
                xwospl_cpuirq_restore_lc(cpuirq);
                // cppcheck-suppress [misra-c2012-17.2, misra-c2012-17.7]
                xwup_skd_req_swcx();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
        }
}
#endif

/**
 * @brief 中断调度器中所有阻塞/睡眠态的线程
 */
__xwup_code
void xwup_skd_intr_all(void)
{
        struct xwup_skd * xwskd;
        struct xwup_thd * c;
        struct xwup_thd * n;
        xwreg_t cpuirq;

        xwskd = &xwup_skd;
        xwospl_cpuirq_save_lc(&cpuirq);
        // cppcheck-suppress [misra-c2012-12.3, misra-c2012-14.2]
        xwlib_bclst_itr_next_entry_safe(c, n, &xwskd->thdlist,
                                        struct xwup_thd, thdnode) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_intr(c); //cppcheck-suppress [misra-c2012-17.7]
                xwospl_cpuirq_disable_lc();
        }
        xwospl_cpuirq_restore_lc(cpuirq);
}

#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
/**
 * @brief XWUP PM API：设置电源管理回调函数
 * @param[in] resume_cb: 从暂停模式恢复的回调函数
 * @param[in] suspend_cb: 进入暂停模式的回调函数
 * @param[in] wakeup_cb: 唤醒的回调函数
 * @param[in] sleep_cb: 休眠的回调函数
 * @param[in] arg: 回调函数调用时的参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_skd_set_pm_cb(xwup_skd_pm_cb_f resume_cb,
                        xwup_skd_pm_cb_f suspend_cb,
                        xwup_skd_pm_cb_f wakeup_cb,
                        xwup_skd_pm_cb_f sleep_cb,
                        void * arg)
{
        struct xwup_skd * xwskd;

        xwskd = &xwup_skd;
        xwskd->pm.cb.resume = resume_cb;
        xwskd->pm.cb.suspend = suspend_cb;
        xwskd->pm.cb.wakeup = wakeup_cb;
        xwskd->pm.cb.sleep = sleep_cb;
        xwskd->pm.cb.arg = arg;
}

/**
 * @brief 将调度器的唤醒锁计数器加1
 * @retval XWOK: 没有错误
 * @retval <0: 当前调度器正在进入低功耗
 */
__xwup_code
xwer_t xwup_skd_inc_wklkcnt(void)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = &xwup_skd;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (xwskd->pm.wklkcnt >= (xwsq_t)XWUP_SKD_WKLKCNT_UNLOCKED) {
                xwskd->pm.wklkcnt++;
                rc = XWOK;
        } else {
                rc = -EACCES;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief 将调度器的唤醒锁计数器减1
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: 当前调度器正在进入低功耗
 */
__xwup_code
xwer_t xwup_skd_dec_wklkcnt(void)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;
        bool lpm;

        lpm = false;
        xwskd = &xwup_skd;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (xwskd->pm.wklkcnt >= (xwsq_t)XWUP_SKD_WKLKCNT_UNLOCKED) {
                xwskd->pm.wklkcnt--;
                if ((xwsq_t)XWUP_SKD_WKLKCNT_FREEZING == xwskd->pm.wklkcnt) {
                        lpm = true;
                }
                rc = XWOK;
        } else {
                rc = -EACCES;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        if (lpm) {
                rc = xwospl_skd_suspend(xwskd);
        }
        return rc;
}

/**
 * @brief 禁止调度器进入暂停模式
 * @return 错误码
 */
__xwup_code
xwer_t xwup_skd_wakelock_lock(void)
{
        return xwup_skd_inc_wklkcnt();
}

/**
 * @brief 允许调度器进入暂停模式
 * @return 错误码
 */
__xwup_code
xwer_t xwup_skd_wakelock_unlock(void)
{
        return xwup_skd_dec_wklkcnt();
}

/**
 * @brief 暂停调度器（中断中执行）
 * @param[in] xwskd: XWOS UP调度器的指针
 * @return 错误码
 * @note
 * - 此函数只能在CPU自身的调度器服务中断中执行，可通过 @ref xwospl_skd_suspend()
 *   触发并执行此函数。
 */
__xwup_code
xwer_t xwup_skd_suspend_lic(struct xwup_skd * xwskd)
{
        xwreg_t cpuirq;

        xwup_skd_intr_all();
        xwospl_cpuirq_save_lc(&cpuirq);
        if (xwskd->thd_num == xwskd->pm.frz_thd_cnt) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_notify_allfrz_lic(); //cppcheck-suppress [misra-c2012-17.7]
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return XWOK;
}

/**
 * @brief 通知所有线程已经冻结（中断上下文中执行的部分）
 * @return 错误码
 */
__xwup_code
xwer_t xwup_skd_notify_allfrz_lic(void)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = &xwup_skd;
        xwospl_cpuirq_save_lc(&cpuirq);
        if ((xwsq_t)XWUP_SKD_WKLKCNT_FREEZING == xwskd->pm.wklkcnt) {
                xwskd->pm.wklkcnt--;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_syshwt_stop(&xwskd->tt.hwt); //cppcheck-suppress [misra-c2012-17.7]
                rc = XWOK;
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINTR;
        }
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        return rc;
}

/**
 * @brief 通知所有线程已经冻结（空闲任务中执行的部分）
 * @param[in] xwskd: XWOS UP调度器的指针
 */
static __xwup_code
void xwup_skd_notify_allfrz_lc(struct xwup_skd * xwskd)
{
        xwreg_t cpuirq;

        if ((xwsq_t)XWUP_SKD_WKLKCNT_ALLFRZ == xwskd->pm.wklkcnt) {
                xwospl_cpuirq_save_lc(&cpuirq);
                if ((xwsq_t)XWUP_SKD_WKLKCNT_ALLFRZ == xwskd->pm.wklkcnt) {
                        xwskd->pm.wklkcnt--;
                        if (NULL != xwskd->pm.cb.suspend) {
                                xwskd->pm.cb.suspend(xwskd->pm.cb.arg);
                        }
                        xwskd->pm.wklkcnt--;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        while ((xwsq_t)XWUP_SKD_WKLKCNT_SUSPENDED ==
                               xwskd->pm.wklkcnt) {
                                if (NULL != xwskd->pm.cb.sleep) {
                                        xwskd->pm.cb.sleep(xwskd->pm.cb.arg);
                                }
                        }
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        }
}

/**
 * @brief 解冻所有线程
 * @param[in] xwskd: XWOS UP调度器的指针
 * @return 错误码
 */
static __xwup_code
xwer_t xwup_skd_thaw_allfrz_lic(struct xwup_skd * xwskd)
{
        struct xwup_thd * c;
        struct xwup_thd * n;
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwlib_bclst_itr_next_entry_safe(c, n, &xwskd->pm.frzlist,
                                        struct xwup_thd, frznode) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_thaw_lic(c); //cppcheck-suppress [misra-c2012-17.7]
                xwospl_cpuirq_disable_lc();
        }
        if ((xwsq_t)0 == xwskd->pm.frz_thd_cnt) {
                rc = XWOK;
        } else {
                rc = -EBUG;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        return rc;
}

/**
 * @brief XWUP API：申请暂停调度器
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_skd_suspend(void)
{
        return xwup_skd_dec_wklkcnt();
}

/**
 * @brief 继续已经暂停的调度器（在中断中执行）
 * @param[in] xwskd: XWOS UP调度器的指针
 * @return 错误码
 * @note
 * - 此函数只可由 @ref xwup_skd_resume() 调用。
 *   在MP架构中，SOC层 `xwospl_skd_resume()` 会触发CPU自身的调度器服务中断来运行此
 *   函数，UP架构中复用了同一份BSP的适配代码。因此，此函数仅仅是为了保证编译顺利。
 */
__xwup_code
xwer_t xwup_skd_resume_lic(struct xwup_skd * xwskd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        do { //cppcheck-suppress [misra-c2012-15.4]
                if ((xwsq_t)XWUP_SKD_WKLKCNT_SUSPENDED == xwskd->pm.wklkcnt) {
                        xwskd->pm.wklkcnt++;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (NULL != xwskd->pm.cb.wakeup) {
                                xwskd->pm.cb.wakeup(xwskd->pm.cb.arg);
                        }
                        xwospl_cpuirq_save_lc(&cpuirq);
                }
                if ((xwsq_t)XWUP_SKD_WKLKCNT_RESUMING == xwskd->pm.wklkcnt) {
                        xwskd->pm.wklkcnt++;
                        if (NULL != xwskd->pm.cb.resume) {
                                xwskd->pm.cb.resume(xwskd->pm.cb.arg);
                        }
                }
                if ((xwsq_t)XWUP_SKD_WKLKCNT_ALLFRZ == xwskd->pm.wklkcnt) {
                        xwskd->pm.wklkcnt++;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        //cppcheck-suppress [misra-c2012-17.7]
                        xwup_syshwt_start(&xwskd->tt.hwt);
                        xwospl_cpuirq_save_lc(&cpuirq);
                }
                if ((xwsq_t)XWUP_SKD_WKLKCNT_THAWING == xwskd->pm.wklkcnt) {
                        xwskd->pm.wklkcnt++;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        //cppcheck-suppress [misra-c2012-17.7]
                        xwup_skd_thaw_allfrz_lic(xwskd);
                        xwospl_cpuirq_save_lc(&cpuirq);
                        rc = XWOK;
                        break;
                }
                if (xwskd->pm.wklkcnt >= (xwsq_t)XWUP_SKD_WKLKCNT_UNLOCKED) {
                        rc = -EALREADY;
                        break;
                }
        } while (true);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief XWUP API：继续已经暂停的调度器
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_skd_resume(void)
{
        struct xwup_skd * xwskd;
        xwer_t rc;

        xwskd = &xwup_skd;
        if (XWOK != xwup_irq_get_id(NULL)) {
                rc = xwospl_skd_resume(xwskd);
        } else {
                rc = xwup_skd_resume_lic(xwskd);
        }
        return rc;
}

/**
 * @brief XWUP API：获取调度器电源管理阶段
 * @return 状态值 @ref xwup_pm_stage_em
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwsq_t xwup_skd_get_pm_stage(void)
{
        struct xwup_skd * xwskd;

        xwskd = &xwup_skd;
        return xwskd->pm.wklkcnt;
}

#else
__xwup_code
xwer_t xwup_skd_suspend_lic(struct xwup_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        return -ENOSYS;
}

__xwup_code
xwer_t xwup_skd_resume_lic(struct xwup_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        return -ENOSYS;
}

__xwup_api
xwsq_t xwup_skd_get_pm_stage(void)
{
        return (xwsq_t)XWUP_PM_STAGE_RUNNING;
}
#endif

__xwup_api
void xwup_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        struct xwup_skd * xwskd;
        xwirq_t irqn;
        xwer_t rc;
        xwsq_t ctx;

        rc = xwup_irq_get_id(&irqn);
        if (XWOK == rc) {
                ctx = (xwsq_t)XWUP_SKD_CONTEXT_ISR;
                if (NULL != irqnbuf) {
                        *irqnbuf = irqn;
                }
        } else {
                xwskd = xwup_skd_get_lc();
                if ((xwsq_t)XWUP_SKD_STATE_START == xwskd->state) {
                        if (XWUP_SKD_IDLE_STK(xwskd) == xwskd->cstk) {
                                ctx = (xwsq_t)XWUP_SKD_CONTEXT_IDLE;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
                        } else if (XWUP_SKD_BH_STK(xwskd) == xwskd->cstk) {
                                ctx = (xwsq_t)XWUP_SKD_CONTEXT_BH;
#endif
                        } else {
                                ctx = (xwsq_t)XWUP_SKD_CONTEXT_THD;
                        }
                } else {
                        ctx = (xwsq_t)XWUP_SKD_CONTEXT_BOOT;
                }
        }
        if (NULL != ctxbuf) {
                *ctxbuf = ctx;
        }
}

__xwup_api
bool xwup_skd_prio_tst_valid(xwpr_t prio)
{
        return (prio <= XWUP_SKD_PRIORITY_INVALID);
}
