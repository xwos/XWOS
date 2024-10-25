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
 * XWOS的 **时间** 以 @ref xwtm_t 表示。系统滴答定时器中断的次数用 @ref xwtk_t 表示。
 * 参考 @ref xwos_lib_type_time 。
 *
 * + `xwtk_t` 是一个无符号的64位整数。
 * + `xwtm_t` 是一个有符号的64位整数，单位为 **纳秒** 。
 *
 * ## 获取当前的系统时间
 *
 * + 通过 `xwtm_now()` 可以获取当前CPU的系统时间点；
 * + 通过 `xwtm_nowts()` 可以获取当前CPU的系统时间戳；
 * + 通过 `xwtm_nowtc()` 获取当前CPU的系统滴答计数。
 *
 * ## 获取未来的系统时间
 *
 * + 通过 `xwtm_ft()` 可以计算出CPU的未来系统时间点；
 * + 通过 `xwtm_fts()` 可以计算出CPU的未来系统时间戳。
 * @{
 */

#define XWTM_TC_PER_S XWOSDL_SKD_TC_PER_S /**< 每秒滴答定时器中断的次数 */

/**
 * @brief XWOS API：获取当前CPU的系统时间点
 * @return 当前时间点
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 * @details
 * XWOS中，每个CPU都有一个私有的滴答定时器，产生周期性的定时中断。
 * **系统时间** 在每次中断时都会增加一次。
 *
 * **系统时间** 是每CPU私有变量。代码运行在哪个CPU，访问的就是哪个私有变量。
 *
 * `XWOS的系统时间 = 滴答时间定时器中断的次数 * 中断的周期（以纳秒为单位）`
 */
static __xwos_inline_api
xwtm_t xwtm_now(void)
{
        return xwosdl_skd_get_time_lc();
}

/**
 * @brief XWOS API：获取当前CPU的未来 **系统时间** 点
 * @param[in] dur: 从现在开始到未来的时间
 * @return 未来时间点
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 * @details
 * `未来系统时间点 = 当前系统时间点 + dur`
 */
static __xwos_inline_api
xwtm_t xwtm_ft(xwtm_t dur)
{
        xwtm_t from;
        xwtm_t to;

        from = xwtm_now();
        to = xwtm_add_safely(from, dur);
        return to;
}

/**
 * @brief XWOS API：获取当前CPU的系统时间戳
 * @return 当前系统时间戳
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 * @details
 * **系统时间戳** 类似于 **系统时间** 。但是 **系统时间** 只会在滴答定时器的
 * 每次中断时才增加一次，两次中断之间不会发生变化。
 *
 * 通过获取滴答定时器的计数器的值，计算出上一次中断后经过了多少时间，
 * 再累加到 **系统时间** 上，即为 **系统时间戳** 。
 *
 * **系统时间戳** 是每CPU私有变量。代码运行在哪个CPU，访问的就是哪个私有变量。
 *
 * `XWOS的系统时间戳 = XWOS的系统时间 + 滴答定时器距离下一次中断还有多少时间`
 */
static __xwos_inline_api
xwtm_t xwtm_nowts(void)
{
        return xwosdl_skd_get_timestamp_lc();
}

/**
 * @brief XWOS API：获取当前CPU的未来 **系统时间戳**
 * @param[in] dur: 从现在开始到未来的时间
 * @return 未来时间
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 * @details
 * `未来时间戳 = 当前时间时间戳 + dur`
 */
static __xwos_inline_api
xwtm_t xwtm_fts(xwtm_t dur)
{
        xwtm_t from;
        xwtm_t to;

        from = xwtm_nowts();
        to = xwtm_add_safely(from, dur);
        return to;
}

/**
 * @brief XWOS API：获取当前CPU的系统滴答计数
 * @return 滴答计数
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 * @details
 * **系统滴答计数** 是每CPU私有变量。代码运行在哪个CPU，访问的就是哪个私有变量。
 *
 * `XWOS的系统滴答计数 = 滴答时间定时器中断的次数`
 */
static __xwos_inline_api
xwtk_t xwtm_nowtc(void)
{
        return xwosdl_skd_get_tick_lc();
}

/**
 * @} xwos_time
 */

#endif /* xwos/osal/time.h */
