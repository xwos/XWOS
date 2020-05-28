/**
 * @file
 * @brief 操作系统接口抽象：调度器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_scheduler_h__
#define __xwos_osal_scheduler_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief [XWOSAL] 最小实时优先级
 */
#define XWOSAL_SD_PRIORITY_RT_MIN               XWOSDL_SD_PRIORITY_RT_MIN

/**
 * @brief [XWOSAL] 最大实时优先级
 */
#define XWOSAL_SD_PRIORITY_RT_MAX               XWOSDL_SD_PRIORITY_RT_MAX

/**
 * @brief [XWOSAL] 无效优先级
 */
#define XWOSAL_SD_PRIORITY_INVALID              XWOSDL_SD_PRIORITY_INVALID

/**
 * @brief [XWOSAL] 优先级在base基础上提高inc
 */
#define XWOSAL_SD_PRIORITY_RAISE(base, inc)     XWOSDL_SD_PRIORITY_RAISE(base, inc)

/**
 * @brief [XWOSAL] 优先级在base基础上降低dec
 */
#define XWOSAL_SD_PRIORITY_DROP(base, dec)      XWOSDL_SD_PRIORITY_DROP(base, dec)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOSAL API：检查优先级是否有效
 * @param prio: (I) 优先级
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
bool xwosal_scheduler_prio_tst_valid(xwpr_t prio)
{
        return xwosdl_scheduler_prio_tst_valid(prio);
}

/**
 * @brief XWOSAL API：启动当前CPU的调度器
 * @return 此函数不会返回
 * @note
 * - 同步/异步：同步
 * - 上下文：只可在系统复位后初始化流程中调用
 * - 重入性：只可调用一次
 */
static __xwos_inline_api
xwer_t xwosal_scheduler_start_lc(void)
{
        return xwosdl_scheduler_start_lc();
}

/**
 * @brief XWOSAL API：获取当前CPU调度器的系统滴答时间
 * @return 系统时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwtm_t xwosal_scheduler_get_timetick_lc(void)
{
        return xwosdl_scheduler_get_timetick_lc();
}

/**
 * @brief XWOSAL API：获取当前CPU调度器的系统滴答计数
 * @return 滴答计数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwu64_t xwosal_scheduler_get_tickcount_lc(void)
{
        return xwosdl_scheduler_get_tickcount_lc();
}

/**
 * @brief XWOSAL API：获取当前CPU调度器的系统时间戳
 * @return 系统时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwtm_t xwosal_scheduler_get_timestamp_lc(void)
{
        return xwosdl_scheduler_get_timestamp_lc();
}

/**
 * @brief XWOSAL API：关闭本地CPU调度器的抢占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
void xwosal_scheduler_dspmpt_lc(void)
{
        xwosdl_scheduler_dspmpt_lc();
}

/**
 * @brief XWOSAL API：开启本地CPU调度器的抢占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
void xwosal_scheduler_enpmpt_lc(void)
{
        xwosdl_scheduler_enpmpt_lc();
}

#endif /* xwos/osal/scheduler.h */
