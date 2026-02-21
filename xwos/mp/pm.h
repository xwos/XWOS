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

#ifndef __xwos_mp_pm_h__
#define __xwos_mp_pm_h__

#include <xwos/standard.h>
#include <xwos/mp/skd.h>

struct xwmp_pmdm;

/**
 * @brief 电源管理阶段枚举
 */
enum xwup_skd_pm_stage_em {
        XWMP_PM_STAGE_SUSPENDED = XWMP_SKD_WKLKCNT_SUSPENDED, /**< 已经暂停 */
        XWMP_PM_STAGE_SUSPENDING = XWMP_SKD_WKLKCNT_SUSPENDING, /**< 正在暂停 */
        XWMP_PM_STAGE_RESUMING = XWMP_SKD_WKLKCNT_RESUMING, /**< 正在恢复 */
        XWMP_PM_STAGE_ALLFRZ = XWMP_SKD_WKLKCNT_ALLFRZ, /**< 所有线程已冻结 */
        XWMP_PM_STAGE_FREEZING = XWMP_SKD_WKLKCNT_FREEZING, /**< 正在冻结线程 */
        XWMP_PM_STAGE_THAWING = XWMP_SKD_WKLKCNT_THAWING, /**< 正在解冻线程 */
        XWMP_PM_STAGE_RUNNING = XWMP_SKD_WKLKCNT_RUNNING, /**< 正常运行 */
};


void xwmp_pm_set_cb(xwmp_skd_pm_cb_f resume,
                    xwmp_skd_pm_cb_f suspend,
                    xwmp_skd_pm_cb_f wakeup,
                    xwmp_skd_pm_cb_f sleep,
                    void * arg);
void xwmp_pm_suspend(void);
void xwmp_pm_resume(void);
xwsq_t xwmp_pm_get_stage(void);

#endif /* xwos/mp/pm.h */
