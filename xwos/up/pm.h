/**
 * @file
 * @brief XWOS UP内核：内核电源管理
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_pm_h__
#define __xwos_up_pm_h__

#include <xwos/standard.h>
#include <xwos/up/skd.h>

/**
 * @brief 电源管理阶段枚举
 */
enum xwup_pm_stage_em {
        XWUP_PM_STAGE_SUSPENDED = XWUP_SKD_WKLKCNT_SUSPENDED, /**< 已经暂停 */
        XWUP_PM_STAGE_SUSPENDING = XWUP_SKD_WKLKCNT_SUSPENDING, /**< 正在暂停 */
        XWUP_PM_STAGE_RESUMING = XWUP_SKD_WKLKCNT_RESUMING, /**< 正在恢复 */
        XWUP_PM_STAGE_ALLFRZ = XWUP_SKD_WKLKCNT_ALLFRZ, /**< 所有线程已冻结 */
        XWUP_PM_STAGE_FREEZING = XWUP_SKD_WKLKCNT_FREEZING, /**< 正在冻结线程 */
        XWUP_PM_STAGE_THAWING = XWUP_SKD_WKLKCNT_THAWING, /**< 正在解冻线程 */
        XWUP_PM_STAGE_RUNNING = XWUP_SKD_WKLKCNT_RUNNING, /**< 正常运行 */
};

typedef void (* xwup_skd_pm_cb_f)(void *); /**< 电源管理回调函数 */

#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
void xwup_pm_set_cb(xwup_skd_pm_cb_f resume,
                    xwup_skd_pm_cb_f suspend,
                    xwup_skd_pm_cb_f wakeup,
                    xwup_skd_pm_cb_f sleep,
                    void * arg);
#endif
void xwup_pm_suspend(void);
void xwup_pm_resume(void);
xwsq_t xwup_pm_get_stage(void);

#endif /* xwos/up/pm.h */
