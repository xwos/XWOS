/**
 * @file
 * @brief 操作系统抽象层：电源管理
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_pm_h__
#define __xwos_osal_pm_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/pm.h>

/**
 * @brief 电源管理回调函数指针类型
 * @note
 * - 定义：
 * ``` C
 * void (* xwos_pmdm_cb_f)(void * arg);
 * ```
 * - 电源管理要求用户定义四个回调函数，通过API @ref xwos_pm_set_cb()指定：
 *   - resume_cb: 从暂停模式恢复的回调函数
 *   - suspend_cb: 进入暂停模式的回调函数
 *   - wakeup_cb: 唤醒时回调函数
 *   - sleep_cb: 休眠时的回调函数
 */
typedef xwosdl_pm_cb_f xwos_pm_cb_f;

/**
 * @defgroup xwos_pm_stage_em
 * @{
 */

#define XWOS_PM_STAGE_SUSPENDED   XWOSDL_PM_STAGE_SUSPENDED /**< 已经暂停 */
#define XWOS_PM_STAGE_SUSPENDING  XWOSDL_PM_STAGE_SUSPENDING /**< 正在暂停 */
#define XWOS_PM_STAGE_RESUMING    XWOSDL_PM_STAGE_RESUMING /**< 正在恢复 */
#define XWOS_PM_STAGE_FREEZING    XWOSDL_PM_STAGE_FREEZING /**< 正在冻结线程 */
#define XWOS_PM_STAGE_THAWING     XWOSDL_PM_STAGE_THAWING /**< 正在解冻线程 */
#define XWOS_PM_STAGE_RUNNING     XWOSDL_PM_STAGE_RUNNING /**< 正常运行 */

/**
 * @}
 */

/**
 * @brief XWOS API：设置电源管理的回调函数
 * @param resume_cb: (I) 从暂停模式恢复的回调函数
 * @param suspend_cb: (I) 进入暂停模式的回调函数
 * @param wakeup_cb: (I) 唤醒时回调函数
 * @param sleep_cb: (I) 休眠时的回调函数
 * @param arg: (I) 电源管理系统调用回调函数时传递的参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwos_pm_set_cb(xwos_pm_cb_f resume_cb,
                    xwos_pm_cb_f suspend_cb,
                    xwos_pm_cb_f wakeup_cb,
                    xwos_pm_cb_f sleep_cb,
                    void * arg)
{
        return xwosdl_pm_set_cb(resume_cb, suspend_cb, wakeup_cb, sleep_cb, arg);
}

/**
 * @brief XWOS API：暂停玄武OS，并将系统切换为低功耗状态
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_pm_suspend(void)
{
        return xwosdl_pm_suspend();
}

/**
 * @brief XWOS API：唤醒系统，并继续运行玄武OS
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_pm_resume(void)
{
        return xwosdl_pm_resume();
}

/**
 * @brief XWOS API：获取当前电源管理阶段
 * @return 电源管理阶段 @ref xwos_pm_stage_em
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwos_pm_get_stage(void)
{
        return xwosdl_pm_get_stage();
}

#endif /* xwos/osal/pm.h */
