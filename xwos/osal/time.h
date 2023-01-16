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
 * @brief XWOS API：获取当前CPU的系统时间点
 * @return 当前时间点
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 * @details
 * XWOS中，每个CPU都有一个私有的滴答定时器，产生周期性的定时中断。 **系统时间** 在每次中断时都会增加一次。
 *
 * **系统时间** 是每CPU私有变量。代码运行在哪个CPU，访问的就是哪个私有变量。
 *
 * `XWOS的系统时间 = 滴答时间定时器中断的次数 * 中断的周期（以纳秒为单位）`
 */
static __xwos_inline_api
xwtm_t xwtm_now(void)
{
        return xwosdl_skd_get_timetick_lc();
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
        xwtm_t from, to;

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
 * **系统时间戳** 类似于 **系统时间** 。但是 **系统时间** 只会在每次定时器中断时才增加一次，两次中断之间不会发生变化。
 *
 * **系统时间戳** 是通过把滴答定时器到下一次中断还剩多少时间计算出来，再累加到系统时间上获取的。
 *
 * **系统时间戳** 是每CPU私有变量。代码运行在哪个CPU，访问的就是哪个私有变量。
 *
 * `XWOS的系统时间戳 = XWOS的系统时间 + 滴答时间定时器中数值换算成纳秒的时间`
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
        xwtm_t from, to;

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
xwu64_t xwtm_nowtc(void)
{
        return xwosdl_skd_get_tickcount_lc();
}

/**
 * @} xwos_time
 */

#endif /* xwos/osal/time.h */
