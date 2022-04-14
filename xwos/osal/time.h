/**
 * @file
 * @brief 操作系统抽象层：时间
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_time_h__
#define __xwos_osal_time_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/skd.h>

/**
 * @defgroup xwos_time 时间
 * @ingroup xwos
 * @{
 */

/**
 * @brief XWOS API：获取当前CPU调度器的系统滴答时间
 * @return 当前时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * XWOS的系统滴答时间 = 滴答时间定时器中断的次数 * 中断的周期（以纳秒为单位）
 */
static __xwos_inline_api
xwtm_t xwtm_now()
{
        return xwosdl_skd_get_timetick_lc();
}

/**
 * @brief XWOS API：获取当前CPU调度器的未来系统滴答时间
 * @param[in] xwtm: 从现在开始到未来的时间
 * @return 未来时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * 未来时间 = 当前时间 + xwtm
 */
static __xwos_inline_api
xwtm_t xwtm_ft(xwtm_t xwtm)
{
        xwtm_t from, to;

        from = xwtm_now();
        to = xwtm_add_safely(from, xwtm);
        return to;
}

/**
 * @brief XWOS API：获取当前CPU调度器的系统时间戳
 * @return 当前时间戳
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * XWOS的系统时间戳 = XWOS的系统滴答时间 + 滴答时间定时器中数值换算成纳秒的时间
 */
static __xwos_inline_api
xwtm_t xwtm_nowts()
{
        return xwosdl_skd_get_timestamp_lc();
}

/**
 * @brief XWOS API：获取当前CPU调度器的未来系统时间戳
 * @param[in] xwtm: 未来的什么时间
 * @return 未来时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * 未来时间 = 当前时间 + xwtm
 */
static __xwos_inline_api
xwtm_t xwtm_fts(xwtm_t xwtm)
{
        xwtm_t from, to;

        from = xwtm_nowts();
        to = xwtm_add_safely(from, xwtm);
        return to;
}

/**
 * @brief XWOS API：获取当前CPU调度器的系统滴答计数
 * @return 滴答计数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * XWOS的系统滴答计数 = 滴答时间定时器中断的次数
 */
static __xwos_inline_api
xwu64_t xwtm_nowtc(void)
{
        return xwosdl_skd_get_tickcount_lc();
}

/**
 * @} xwos_time
 */

#endif /* xwos/osal/time.h */
