/**
 * @file
 * @brief 玄武OS UP内核：内核电源管理
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_pm_h__
#define __xwos_up_pm_h__

#include <xwos/standard.h>

/**
 * @brief 调度器唤醒锁状态枚举
 */
enum xwup_skd_wakelock_cnt_em {
        XWUP_SKD_WKLKCNT_SUSPENDED = 0, /**< 调度器已暂停 */
        XWUP_SKD_WKLKCNT_SUSPENDING, /**< 调度器正在暂停 */
        XWUP_SKD_WKLKCNT_RESUMING = XWUP_SKD_WKLKCNT_SUSPENDING, /**< 调度器正在恢复 */
        XWUP_SKD_WKLKCNT_ALLFRZ, /**< 调度器所有线程已冻结 */
        XWUP_SKD_WKLKCNT_FREEZING, /**< 正在冻结线程 */
        XWUP_SKD_WKLKCNT_THAWING = XWUP_SKD_WKLKCNT_FREEZING, /**< 正在解冻线程 */
        XWUP_SKD_WKLKCNT_RUNNING, /**< 正常运行 */
        XWUP_SKD_WKLKCNT_UNLOCKED = XWUP_SKD_WKLKCNT_RUNNING, /**< 唤醒锁：未加锁 */
        XWUP_SKD_WKLKCNT_LOCKED, /**< 唤醒锁：已加锁 */
};

/**
 * @brief 电源管理阶段枚举
 */
enum xwup_skd_pm_stage_em {
        XWUP_PM_STAGE_SUSPENDED = XWUP_SKD_WKLKCNT_SUSPENDED, /**< 已经暂停 */
        XWUP_PM_STAGE_SUSPENDING = XWUP_SKD_WKLKCNT_SUSPENDING, /**< 正在暂停 */
        XWUP_PM_STAGE_RESUMING = XWUP_SKD_WKLKCNT_RESUMING, /**< 正在恢复 */
        XWUP_PM_STAGE_ALLFRZ = XWUP_SKD_WKLKCNT_ALLFRZ, /**< 所有线程已冻结 */
        XWUP_PM_STAGE_FREEZING = XWUP_SKD_WKLKCNT_FREEZING, /**< 正在冻结线程 */
        XWUP_PM_STAGE_THAWING = XWUP_SKD_WKLKCNT_THAWING, /**< 正在解冻线程 */
        XWUP_PM_STAGE_RUNNING = XWUP_SKD_WKLKCNT_RUNNING, /**< 正常运行 */
};

typedef void (* xwup_skd_pm_cb_f)(void *); /**< 电源管理回调函数 */

#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
void xwup_skd_set_pm_cb(xwup_skd_pm_cb_f resume_cb,
                        xwup_skd_pm_cb_f suspend_cb,
                        xwup_skd_pm_cb_f wakeup_cb,
                        xwup_skd_pm_cb_f sleep_cb,
                        void * arg);
xwer_t xwup_skd_suspend(void);
xwer_t xwup_skd_resume(void);
xwsq_t xwup_skd_get_pm_stage(void);
#endif

#endif /* xwos/up/pm.h */
