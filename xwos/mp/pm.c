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
#include <xwos/mp/pm.h>

/**
 * @brief XWMP PM API：设置当前CPU的电源管理回调函数
 * @details
 * 此CAPI只能设置当前CPU的电源管理回调函数，需要在每个CPU都执行一次。
 */
__xwmp_api
void xwmp_pm_set_cb(xwmp_skd_pm_cb_f resume,
                    xwmp_skd_pm_cb_f suspend,
                    xwmp_skd_pm_cb_f wakeup,
                    xwmp_skd_pm_cb_f sleep,
                    void * arg)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_get_lc();
        xwskd->pm.cb.resume = resume;
        xwskd->pm.cb.suspend = suspend;
        xwskd->pm.cb.wakeup = wakeup;
        xwskd->pm.cb.sleep = sleep;
        xwskd->pm.cb.arg = arg;
}

/**
 * @brief XWMP PM API：申请进入低功耗
 * @note
 * - 上下文：任意
 */
__xwmp_api
void xwmp_pm_suspend(void)
{
        xwid_t cpuid;

        for (cpuid = (xwid_t)0; cpuid < (xwid_t)CPUCFG_CPU_NUM; cpuid++) {
                xwmp_skd_suspend(cpuid);
        }
}

/**
 * @brief XWMP PM API：从低功耗状态恢复运行状态
 * @note
 * - 上下文：中断
 */
__xwmp_api
void xwmp_pm_resume(void)
{
        xwid_t cpuid;

        for (cpuid = (xwid_t)0; cpuid < (xwid_t)CPUCFG_CPU_NUM; cpuid++) {
                xwmp_skd_resume(cpuid);
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
