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

#ifndef __xwos_up_pm_h__
#define __xwos_up_pm_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 调度器唤醒锁状态枚举
 */
enum xwos_scheduler_wakelock_cnt_em {
        XWOS_SCHEDULER_WKLKCNT_SUSPENDED = 0, /**< 调度器已暂停 */
        XWOS_SCHEDULER_WKLKCNT_SUSPENDING, /**< 调度器正在暂停 */
        XWOS_SCHEDULER_WKLKCNT_RESUMING = XWOS_SCHEDULER_WKLKCNT_SUSPENDING,
        XWOS_SCHEDULER_WKLKCNT_ALLFRZ, /**< 调度器所有线程已冻结 */
        XWOS_SCHEDULER_WKLKCNT_FREEZING, /**< 正在冻结线程 */
        XWOS_SCHEDULER_WKLKCNT_THAWING = XWOS_SCHEDULER_WKLKCNT_FREEZING,
        XWOS_SCHEDULER_WKLKCNT_RUNNING, /**< 正常运行 */
        XWOS_SCHEDULER_WKLKCNT_UNLOCKED = XWOS_SCHEDULER_WKLKCNT_RUNNING,
                                        /**< 唤醒锁：未加锁 */
        XWOS_SCHEDULER_WKLKCNT_LOCKED, /**< 唤醒锁：已加锁 */
};

typedef void (* xwos_scheduler_pm_cb_f)(void *); /**< 电源管理回调函数 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
/**
 * @brief XWOS PM API：设置电源管理回调函数
 * @param resume_cb: (I) 从暂停模式恢复的回调函数
 * @param suspend_cb: (I) 进入暂停模式的回调函数
 * @param wakeup_cb: (I) 唤醒的回调函数
 * @param sleep_cb: (I) 休眠的回调函数
 * @param arg: (I) 回调函数调用时的参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwos_scheduler_set_pm_cb(xwos_scheduler_pm_cb_f resume_cb,
                              xwos_scheduler_pm_cb_f suspend_cb,
                              xwos_scheduler_pm_cb_f wakeup_cb,
                              xwos_scheduler_pm_cb_f sleep_cb,
                              void * arg);

/**
 * @brief XWOS PM API：申请暂停调度器
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_scheduler_suspend(void);

/**
 * @brief XWOS PM API：继续已经暂停的调度器
 * @return 错误码
 * @retval OK: OK
 * @retval -ENOTINISR: 不在中断中
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_scheduler_resume(void);

/**
 * @brief XWOS PM API：获取调度器电源管理状态
 * @return 状态值 @ref xwos_scheduler_wakelock_cnt_em
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwsq_t xwos_scheduler_get_pm_state(void);
#endif /* XWUPCFG_SD_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/up/pm.h */
