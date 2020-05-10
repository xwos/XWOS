/**
 * @file
 * @brief XuanWuOS内核：内核电源管理
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
#include <xwos/lib/xwaop.h>
#include <xwos/smp/cpu.h>
#include <xwos/smp/pm.h>
#include <xwos/smp/scheduler.h>

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
 * @brief XWOS PM API：初始化电源管理领域
 * @param pmdm: (I) 电源管理领域控制块指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
void xwos_pmdm_init(struct xwos_pmdm * pmdm)
{
        pmdm->stage = XWOS_PMDM_STAGE_RUNNING;
        pmdm->suspended_xwsd_cnt = 0;
}

/**
 * @brief XWOS PM API：得到当前CPU所属的电源管理领域控制块的指针
 * @param pmdm: (I) 电源管理领域控制块的指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
struct xwos_pmdm * xwos_pmdm_get_lc(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_get_lc();
        return xwsd->pm.xwpmdm;
}

/**
 * @brief XWOS PM API：设置电源管理领域的回调函数
 * @param pmdm: (I) 电源管理领域控制块指针
 * @param resume_cb: (I) 电源管理领域从暂停模式恢复的回调函数
 * @param suspend_cb: (I) 电源管理领域进入暂停模式的回调函数
 * @param wakeup_cb: (I) 唤醒电源管理领域的回调函数
 * @param sleep_cb: (I) 电源管理领域休眠的回调函数
 * @param arg: (I) 回调函数调用时的参数
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
void xwos_pmdm_set_cb(struct xwos_pmdm * pmdm,
                      xwos_pmdm_cb_f resume_cb,
                      xwos_pmdm_cb_f suspend_cb,
                      xwos_pmdm_cb_f wakeup_cb,
                      xwos_pmdm_cb_f sleep_cb,
                      void * arg)
{
        pmdm->cb.resume = resume_cb;
        pmdm->cb.suspend = suspend_cb;
        pmdm->cb.wakeup = wakeup_cb;
        pmdm->cb.sleep = sleep_cb;
        pmdm->cb.arg = arg;
}

/**
 * @brief XWOS PM API：暂停电源管理领域中所有调度器
 * @param pmdm: (I) 电源管理领域控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
xwer_t xwos_pmdm_suspend(struct xwos_pmdm * pmdm)
{
        xwid_t cpuid;
        xwsq_t nv;
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq_t, &pmdm->stage,
                                XWOS_PMDM_STAGE_RUNNING,
                                1,
                                &nv, NULL);
        if ((OK == rc) && (XWOS_PMDM_STAGE_FREEZING == nv)) {
                for (cpuid = 0; cpuid < pmdm->xwsd_num; cpuid++) {
                        rc = xwos_scheduler_suspend(cpuid);
                        if (__unlikely(rc < 0)) {
                                break;
                        }
                }
        }
        return rc;
}

/**
 * @brief XWOS PM API：继续电源管理领域中所有调度器
 * @param pmdm: (I) 电源管理领域控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 中断上下文：可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：不可以使用
 */
__xwos_api
xwer_t xwos_pmdm_resume(struct xwos_pmdm * pmdm)
{
        xwid_t cpuid;
        xwsq_t nv, ov;
        xwreg_t cpuirq;
        xwer_t rc;

        do {
                rc = xwaop_teq_then_add(xwsq_t, &pmdm->stage,
                                        XWOS_PMDM_STAGE_SUSPENDED,
                                        1,
                                        &nv, &ov);
                if ((OK == rc) && (XWOS_PMDM_STAGE_RESUMING == nv)) {
                        if (pmdm->cb.wakeup) {
                                pmdm->cb.wakeup(pmdm, pmdm->cb.arg);
                        }/* else {} */
                }

                rc = xwaop_teq_then_add(xwsq_t, &pmdm->stage,
                                        XWOS_PMDM_STAGE_RESUMING,
                                        1,
                                        &nv, &ov);
                if ((OK == rc) && (XWOS_PMDM_STAGE_THAWING == nv)) {
                        xwlk_splk_lock_cpuirqsv(&pmdm->rslock, &cpuirq);
                        if (pmdm->cb.resume) {
                                pmdm->cb.resume(pmdm, pmdm->cb.arg);
                        }/* else {} */
                        xwlk_splk_unlock_cpuirqrs(&pmdm->rslock, cpuirq);
                }

                rc = xwaop_teq_then_add(xwsq_t, &pmdm->stage,
                                        XWOS_PMDM_STAGE_THAWING,
                                        1,
                                        &nv, &ov);
                if ((OK == rc) && (XWOS_PMDM_STAGE_RUNNING == nv)) {
                        for (cpuid = 0; cpuid < pmdm->xwsd_num; cpuid++) {
                                rc = xwos_scheduler_resume(cpuid);
                                if (__unlikely(rc < 0)) {
                                        break;
                                }/* else {} */
                        }
                        break;
                }
                if (ov >= XWOS_PMDM_STAGE_RUNNING) {
                        break;
                }
        } while (true);
        return rc;
}

/**
 * @brief XWOS PM API：获取电源管理步骤
 * @param pmdm: (I) 电源管理领域控制块指针
 * @return 电源管理步骤 @ref xwos_pmdm_stage_em
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
xwsq_t xwos_pmdm_get_stage(struct xwos_pmdm * pmdm)
{
        xwsq_t stage;

        stage = xwaop_load(xwsq_t, &pmdm->stage, xwmb_modr_relaxed);
        return stage;
}

/**
 * @brief 增加已暂停的调度器的数量
 * @param pmdm: (I) 电源管理领域控制块指针
 */
__xwos_code
void xwos_pmdm_report_xwsd_suspended(struct xwos_pmdm * pmdm)
{
        xwsq_t nv;
        xwer_t rc;
        xwreg_t cpuirq;
        xwsz_t suspended_xwsd_cnt;

        xwaop_add(xwsz_t, &pmdm->suspended_xwsd_cnt, 1, &suspended_xwsd_cnt, NULL);
        if (pmdm->xwsd_num == suspended_xwsd_cnt) {
                xwlk_splk_lock_cpuirqsv(&pmdm->rslock, &cpuirq);
                rc = xwaop_teq_then_sub(xwsq_t, &pmdm->stage,
                                        XWOS_PMDM_STAGE_FREEZING,
                                        1,
                                        &nv, NULL);
                if ((OK == rc) && (XWOS_PMDM_STAGE_SUSPENDING == nv)) {
                        if (pmdm->cb.suspend) {
                                pmdm->cb.suspend(pmdm, pmdm->cb.arg);
                        }/* else {} */
                        xwlk_splk_unlock_cpuirqrs(&pmdm->rslock, cpuirq);

                        xwaop_teq_then_sub(xwsq_t, &pmdm->stage,
                                           XWOS_PMDM_STAGE_SUSPENDING,
                                           1,
                                           &nv, NULL);
                        while (XWOS_PMDM_STAGE_SUSPENDED == nv) {
                                if (pmdm->cb.sleep) {
                                        pmdm->cb.sleep(pmdm, pmdm->cb.arg);
                                }/* else {} */
                                nv = xwaop_load(xwsq_t, &pmdm->stage,
                                                xwmb_modr_relaxed);
                        }
                } else {
                        xwlk_splk_unlock_cpuirqrs(&pmdm->rslock, cpuirq);
                }
        }
}

/**
 * @brief 减少已暂停的调度器的数量
 * @param pmdm: (I) 电源管理领域控制块指针
 */
__xwos_code
void xwos_pmdm_report_xwsd_resuming(struct xwos_pmdm * pmdm)
{
        xwaop_sub(xwsz_t, &pmdm->suspended_xwsd_cnt, 1, NULL, NULL);
}
