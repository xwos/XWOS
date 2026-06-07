/**
 * @file
 * @brief XWOS MP内核：电源管理
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>
#include <xwos/mp/pm.h>

/**
 * @brief XWMP PM API：设置当前CPU的电源管理操作函数
 * @details
 * 此CAPI只能设置当前CPU的电源管理操作函数，需要在每个CPU都执行一次。
 */
__xwmp_api
void xwmp_pm_set_op(xwid_t cpuid,
                    xwmp_skd_pm_op_f resume_periph,
                    xwmp_skd_pm_op_f suspend_periph,
                    xwmp_skd_pm_op_f wakeup_cpu,
                    xwmp_skd_pm_op_f sleep_cpu,
                    void * arg)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_get_by_cpuid(cpuid);
        if (NULL != xwskd) {
                xwskd->pm.op.resume_periph = resume_periph;
                xwskd->pm.op.suspend_periph = suspend_periph;
                xwskd->pm.op.wakeup_cpu = wakeup_cpu;
                xwskd->pm.op.sleep_cpu = sleep_cpu;
                xwskd->pm.op.arg = arg;
        }
}

/**
 * @brief XWMP PM API：申请进入低功耗
 * @note
 * - 上下文：任意
 */
__xwmp_api
void xwmp_pm_suspend(void)
{
        struct xwmp_skd * local;

        local = xwmp_skd_get_lc();
        xwmp_skd_dec_wklkcnt_lc(local);
}

/**
 * @brief XWMP PM API：从低功耗状态恢复运行状态
 * @note
 * - 上下文：中断
 */
__xwmp_api
void xwmp_pm_resume(void)
{
        struct xwmp_skd * local;
        xwsq_t ctx;
        xwirq_t irqn;

        local = xwmp_skd_get_lc();
        xwmp_skd_get_context_lc(&ctx, &irqn);
        if (XWMP_SKD_CONTEXT_ISR == ctx) {
                xwmp_skd_resume_lc(local);
        } else {
                xwospl_skd_resume(local);
        }
}

/**
 * @brief XWMP PM API：获取当前CPU的电源管理阶段
 * @return 电源管理阶段 @ref xwmp_pm_stage_em
 * @details
 * 此CAPI只能获取当前CPU的电源管理阶段，
 * 其他CPU的电源管理阶段只能由其他CPU上运行的代码获取。
 */
__xwmp_api
xwsq_t xwmp_pm_get_stage(void)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_get_lc();
        return xwmp_skd_get_pm_state(xwskd);
}
