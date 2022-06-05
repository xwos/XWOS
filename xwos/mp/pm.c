/**
 * @file
 * @brief 玄武OS MP内核：电源管理
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwaop.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/pm.h>
#include <xwos/mp/lock/spinlock.h>

/**
 * @brief 玄武OS MP内核电源管理领域
 */
__xwmp_data struct xwmp_pmdm xwmp_pmdm = {
        .xwskd_num = CPUCFG_CPU_NUM,
};

/**
 * @brief 初始化电源管理领域
 * @param[in] pmdm: 电源管理领域控制块指针
 */
__xwmp_code
void xwmp_pmdm_init(void)
{
        xwmp_pmdm.stage = XWMP_PM_STAGE_RUNNING;
        xwmp_pmdm.suspended_xwskd_cnt = 0;
        xwmp_splk_init(&xwmp_pmdm.rslock);
}

/**
 * @brief XWMP PM API：设置电源管理领域的回调函数
 * @param[in] resume_cb: 电源管理领域从暂停模式恢复的回调函数
 * @param[in] suspend_cb: 电源管理领域进入暂停模式的回调函数
 * @param[in] wakeup_cb: 唤醒电源管理领域的回调函数
 * @param[in] sleep_cb: 电源管理领域休眠的回调函数
 * @param[in] arg: 回调函数调用时的参数
 * @note
 * - 上下文：任意
 */
__xwmp_api
void xwmp_pmdm_set_cb(xwmp_pmdm_cb_f resume_cb,
                      xwmp_pmdm_cb_f suspend_cb,
                      xwmp_pmdm_cb_f wakeup_cb,
                      xwmp_pmdm_cb_f sleep_cb,
                      void * arg)
{
        xwmp_pmdm.cb.resume = resume_cb;
        xwmp_pmdm.cb.suspend = suspend_cb;
        xwmp_pmdm.cb.wakeup = wakeup_cb;
        xwmp_pmdm.cb.sleep = sleep_cb;
        xwmp_pmdm.cb.arg = arg;
}

/**
 * @brief XWMP PM API：暂停电源管理领域
 * @note
 * - 上下文：任意
 */
__xwmp_api
xwer_t xwmp_pmdm_suspend(void)
{
        xwid_t cpuid;
        xwsq_t nv;
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq_t, &xwmp_pmdm.stage,
                                XWMP_PM_STAGE_RUNNING,
                                1,
                                &nv, NULL);
        if ((XWOK == rc) && (XWMP_PM_STAGE_FREEZING == nv)) {
                for (cpuid = 0; cpuid < xwmp_pmdm.xwskd_num; cpuid++) {
                        rc = xwmp_skd_suspend(cpuid);
                        if (__xwcc_unlikely(rc < 0)) {
                                break;
                        }
                }
        }
        return rc;
}

/**
 * @brief XWMP PM API：继续电源管理领域
 * @note
 * - 上下文：中断
 */
__xwmp_api
xwer_t xwmp_pmdm_resume(void)
{
        xwid_t cpuid;
        xwsq_t nv, ov;
        xwreg_t cpuirq;
        xwer_t rc;

        do {
                rc = xwaop_teq_then_add(xwsq_t, &xwmp_pmdm.stage,
                                        XWMP_PM_STAGE_SUSPENDED,
                                        1,
                                        &nv, &ov);
                if ((XWOK == rc) && (XWMP_PM_STAGE_RESUMING == nv)) {
                        if (xwmp_pmdm.cb.wakeup) {
                                xwmp_pmdm.cb.wakeup(xwmp_pmdm.cb.arg);
                        }/* else {} */
                }

                rc = xwaop_teq_then_add(xwsq_t, &xwmp_pmdm.stage,
                                        XWMP_PM_STAGE_RESUMING,
                                        2,
                                        &nv, &ov);
                if ((XWOK == rc) && (XWMP_PM_STAGE_THAWING == nv)) {
                        xwmp_splk_lock_cpuirqsv(&xwmp_pmdm.rslock, &cpuirq);
                        if (xwmp_pmdm.cb.resume) {
                                xwmp_pmdm.cb.resume(xwmp_pmdm.cb.arg);
                        }/* else {} */
                        xwmp_splk_unlock_cpuirqrs(&xwmp_pmdm.rslock, cpuirq);
                }

                rc = xwaop_teq_then_add(xwsq_t, &xwmp_pmdm.stage,
                                        XWMP_PM_STAGE_THAWING,
                                        1,
                                        &nv, &ov);
                if ((XWOK == rc) && (XWMP_PM_STAGE_RUNNING == nv)) {
                        for (cpuid = 0; cpuid < xwmp_pmdm.xwskd_num; cpuid++) {
                                rc = xwmp_skd_resume(cpuid);
                                if (__xwcc_unlikely(rc < 0)) {
                                        break;
                                }/* else {} */
                        }
                        break;
                }
                if (ov >= XWMP_PM_STAGE_RUNNING) {
                        break;
                }
        } while (true);
        return rc;
}

/**
 * @brief XWMP PM API：获取当前电源管理阶段
 * @return 电源管理阶段 @ref xwmp_pm_stage_em
 */
__xwmp_api
xwsq_t xwmp_pmdm_get_stage(void)
{
        xwsq_t stage;

        stage = xwaop_load(xwsq_t, &xwmp_pmdm.stage, xwaop_mo_relaxed);
        return stage;
}

/**
 * @brief 增加已暂停的调度器的数量
 * @param[in] pmdm: 电源管理领域控制块指针
 */
__xwmp_code
void xwmp_pmdm_report_xwskd_suspended(struct xwmp_pmdm * pmdm)
{
        xwsq_t nv;
        xwer_t rc;
        xwreg_t cpuirq;
        xwsz_t suspended_xwskd_cnt;

        xwaop_add(xwsz_t, &pmdm->suspended_xwskd_cnt, 1, &suspended_xwskd_cnt, NULL);
        if (pmdm->xwskd_num == suspended_xwskd_cnt) {
                xwmp_splk_lock_cpuirqsv(&pmdm->rslock, &cpuirq);
                rc = xwaop_teq_then_sub(xwsq_t, &pmdm->stage,
                                        XWMP_PM_STAGE_FREEZING,
                                        2,
                                        &nv, NULL);
                if ((XWOK == rc) && (XWMP_PM_STAGE_SUSPENDING == nv)) {
                        if (pmdm->cb.suspend) {
                                pmdm->cb.suspend(pmdm->cb.arg);
                        }/* else {} */
                        xwmp_splk_unlock_cpuirqrs(&pmdm->rslock, cpuirq);

                        xwaop_teq_then_sub(xwsq_t, &pmdm->stage,
                                           XWMP_PM_STAGE_SUSPENDING,
                                           1,
                                           &nv, NULL);
                        while (XWMP_PM_STAGE_SUSPENDED == nv) {
                                if (pmdm->cb.sleep) {
                                        pmdm->cb.sleep(pmdm->cb.arg);
                                }/* else {} */
                                nv = xwaop_load(xwsq_t, &pmdm->stage, xwaop_mo_relaxed);
                        }
                } else {
                        xwmp_splk_unlock_cpuirqrs(&pmdm->rslock, cpuirq);
                }
        }
}

/**
 * @brief 减少已暂停的调度器的数量
 * @param[in] pmdm: 电源管理领域控制块指针
 */
__xwmp_code
void xwmp_pmdm_report_xwskd_resuming(struct xwmp_pmdm * pmdm)
{
        xwaop_sub(xwsz_t, &pmdm->suspended_xwskd_cnt, 1, NULL, NULL);
}
