/**
 * @file
 * @brief 操作系统抽象层：电源管理
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_pm_h__
#define __xwos_osal_pm_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/pm.h>

/**
 * @defgroup xwos_pm 电源管理
 * @ingroup xwos
 * 技术参考手册： [电源管理](../Docs/TechRefManual/Pm)
 *
 * ## 电源管理回调
 *
 * 用户可以通过 `xwos_pm_set_cb()` 设置电源管理回调函数。
 * 这个CAPI只影响当前运行代码的CPU，如果是多核系统，需要在每个CPU上都调用一次这个CAPI。
 *
 * + `resume` ：从暂停模式恢复的回调函数
 * + `suspend` ：进入暂停模式的回调函数
 * + `wakeup` ：从休眠中唤醒的回调函数
 * + `sleep` ：CPU进入休眠的回调函数
 *
 *
 * ## 系统休眠与唤醒
 *
 * 用户可以通过调用 `xwos_pm_suspend()` 使得操作系统进入休眠模式。
 * 休眠后可以在唤醒中断中调用 `xwos_pm_resume()` 唤醒操作系统。
 *
 * 休眠与唤醒是一个比较复杂的过程，可以通过 `xwos_pm_get_stage()` 获取电源管理
 * 目前所处的 @ref xwos_pm_stage 。
 *
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/pm.hxx
 * @{
 */

/**
 * @brief 电源管理回调函数指针类型
 */
typedef xwosdl_pm_cb_f xwos_pm_cb_f;

/**
 * @defgroup xwos_pm_stage 电源管理阶段
 * @{
 */

#define XWOS_PM_STAGE_SUSPENDED   XWOSDL_PM_STAGE_SUSPENDED /**< 已经暂停 */
#define XWOS_PM_STAGE_SUSPENDING  XWOSDL_PM_STAGE_SUSPENDING /**< 正在暂停 */
#define XWOS_PM_STAGE_RESUMING    XWOSDL_PM_STAGE_RESUMING /**< 正在恢复 */
#define XWOS_PM_STAGE_ALLFRZ      XWOSDL_PM_STAGE_ALLFRZ /**< 所有线程全部冻结 */
#define XWOS_PM_STAGE_FREEZING    XWOSDL_PM_STAGE_FREEZING /**< 正在冻结线程 */
#define XWOS_PM_STAGE_THAWING     XWOSDL_PM_STAGE_THAWING /**< 正在解冻线程 */
#define XWOS_PM_STAGE_RUNNING     XWOSDL_PM_STAGE_RUNNING /**< 正常运行 */

/**
 * @} end of xwos_pm_stage
 */

/**
 * @brief XWOS API：设置电源管理的回调函数
 * @param[in] resume: 从暂停模式恢复的回调函数
 * @param[in] suspend: 进入暂停模式的回调函数
 * @param[in] wakeup: 唤醒时回调函数
 * @param[in] sleep: 休眠时的回调函数
 * @param[in] arg: 电源管理系统调用回调函数时传递的参数
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
void xwos_pm_set_cb(xwos_pm_cb_f resume,
                    xwos_pm_cb_f suspend,
                    xwos_pm_cb_f wakeup,
                    xwos_pm_cb_f sleep,
                    void * arg)
{
        return xwosdl_pm_set_cb(resume, suspend, wakeup, sleep, arg);
}

/**
 * @brief XWOS API：将系统切换为低功耗状态
 * @note
 * + 上下文：任意
 * @details
 * 调用此方法后，所有线程都将开始冻结。冻结完成后，系统开始进入低功耗状态。
 */
static __xwos_inline_api
void xwos_pm_suspend(void)
{
        xwosdl_pm_suspend();
}

/**
 * @brief XWOS API：唤醒系统
 * @note
 * + 上下文：中断
 * @details
 * 只可在唤醒中断中调用。
 */
static __xwos_inline_api
void xwos_pm_resume(void)
{
        xwosdl_pm_resume();
}

/**
 * @brief XWOS API：获取当前电源管理阶段
 * @return 电源管理阶段 @ref xwos_pm_stage
 * @note
 * + 上下文：任意
 * @details
 * 电源管理是复杂的异步操作，当系统正在进入低功耗、
 * 或从低功耗唤醒时可通过此函数获取进展的阶段。
 * 返回值取值 @ref xwos_pm_stage
 */
static __xwos_inline_api
xwsq_t xwos_pm_get_stage(void)
{
        return xwosdl_pm_get_stage();
}

/**
 * @} xwos_pm
 */

#endif /* xwos/osal/pm.h */
