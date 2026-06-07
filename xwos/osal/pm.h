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
 * ## 电源管理操作函数
 *
 * 用户可以通过 `xwos_pm_set_op()` 设置电源管理操作函数。
 * + 如果是多核系统，需要通过参数 `cpuid` 为每个CPU的都设置操作函数。
 *   此CAPI通常在系统初始化阶段调用，CPU0可以为所有CPU设置操作函数。
 * + 如果是单核系统，参数 `cpuid` 无意义，可取任意值，但建议设置为 `0` 。
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
 * @{
 */

/**
 * @brief 电源管理操作函数指针类型
 */
typedef xwosdl_pm_op_f xwos_pm_op_f;

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
 * @brief XWOS API：设置电源管理的操作函数
 * @param[in] resume_periph: 恢复外设
 * @param[in] suspend_periph: 暂停外设
 * @param[in] wakeup_cpu: 唤醒CPU
 * @param[in] sleep_cpu: 休眠CPU
 * @param[in] arg: 调用各操作函数时传递的参数
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
void xwos_pm_set_op(xwid_t cpuid,
                    xwos_pm_op_f resume_periph,
                    xwos_pm_op_f suspend_periph,
                    xwos_pm_op_f wakeup_cpu,
                    xwos_pm_op_f sleep_cpu,
                    void * arg)
{
        return xwosdl_pm_set_op(cpuid,
                                resume_periph,
                                suspend_periph,
                                wakeup_cpu,
                                sleep_cpu,
                                arg);
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
