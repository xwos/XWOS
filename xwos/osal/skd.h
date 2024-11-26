/**
 * @file
 * @brief 操作系统抽象层：调度器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_skd_h__
#define __xwos_osal_skd_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/skd.h>
#include <xwos/osal/time.h>

/**
 * @defgroup xwos_skd 调度器
 * @ingroup xwos
 * 技术参考手册： [调度器](../Docs/TechRefManual/Skd)
 *
 *
 * ## 调度优先级
 *
 * XWOS的优先级，用类型 @ref xwpr_t 表示：
 *
 * + 值越 **小** ，优先级越 **低** ，优先级的值越 **大** ，优先级越 **高** ；
 * + 为了保证今后的扩展性，应该使用模块 @ref xwos_skd_priority 定义的宏，
 *   而不要直接使用 @ref xwpr_t 的数值。
 * + `xwos_skd_prio_tst_valid()` ：测试优先级是否有效
 *
 * ## 上下文
 *
 * XWOS将上下文分为五种，定义可以参考 @ref xwos_skd_context ，
 * 当前代码的上下文可以通过 `xwos_skd_get_context_lc()` 获取。
 *
 * ## CPU ID
 *
 * + `xwos_skd_get_cpuid_lc()` ：获取CPU ID。
 *
 * ## 启动调度
 *
 * 系统启动时，每个CPU都需要调用一次 `xwos_skd_start_lc()` 启动调度。
 *
 * ## 抢占
 *
 * + `xwos_skd_dspmpt_lc()` ：关闭本地CPU调度器的抢占
 * + `xwos_skd_enpmpt_lc()` ：开启本地CPU调度器的抢占
 *
 * 需要注意，关闭多少次抢占，就要打开相同次数的抢占。
 *
 * ## 中断底半部
 *
 * 当配置 `XWOSCFG_SKD_BH` 为 **1** ，XWOS的某些系统函数运行在中断底半部。
 * 例如定时器任务。
 *
 * + `xwos_skd_dsbh_lc()` ：关闭本地CPU的中断底半部
 * + `xwos_skd_enbh_lc()` ：开启本地CPU的中断底半部
 *
 * 需要注意，关闭多少次中断底半部，就要打开相同次数的中断底半部。
 *
 *
 * 当配置 `XWOSCFG_SKD_BH` 为 **0** ，中断底半部被彻底禁用，
 * `xwos_skd_dsbh_lc()` 与 `xwos_skd_enbh_lc()` **不** 可被调用。
 *
 *
 * ## 调度器的暂停与继续
 *
 * + `xwos_skd_pause_lc()` ：暂停调度器
 * + `xwos_skd_continue_lc()` ：继续已暂停的调度器
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/skd.hxx
 * @{
 */

/**
 * @defgroup xwos_skd_priority 调度优先级
 * @{
 */

/**
 * @brief XWOS API：最小实时优先级
 */
#define XWOS_SKD_PRIORITY_RT_MIN        XWOSDL_SKD_PRIORITY_RT_MIN

/**
 * @brief XWOS API：最大实时优先级
 */
#define XWOS_SKD_PRIORITY_RT_MAX        XWOSDL_SKD_PRIORITY_RT_MAX

/**
 * @brief XWOS API：无效优先级
 */
#define XWOS_SKD_PRIORITY_INVALID       XWOSDL_SKD_PRIORITY_INVALID

/**
 * @brief XWOS API：优先级在base基础上提高inc
 */
#define XWOS_SKD_PRIORITY_RAISE(base, inc)      XWOSDL_SKD_PRIORITY_RAISE(base, inc)

/**
 * @brief XWOS API：优先级在base基础上降低dec
 */
#define XWOS_SKD_PRIORITY_DROP(base, dec)       XWOSDL_SKD_PRIORITY_DROP(base, dec)

/** @} */ // xwos_skd_priority


/**
 * @defgroup xwos_skd_context 上下文类型
 * @{
 */

/**
 * @brief XWOS API：上下文：启动
 */
#define XWOS_SKD_CONTEXT_BOOT           XWOSDL_SKD_CONTEXT_BOOT

/**
 * @brief XWOS API：上下文：线程
 */
#define XWOS_SKD_CONTEXT_THD            XWOSDL_SKD_CONTEXT_THD

/**
 * @brief XWOS API：上下文：中断
 */
#define XWOS_SKD_CONTEXT_ISR            XWOSDL_SKD_CONTEXT_ISR

/**
 * @brief XWOS API：上下文：中断底半部
 */
#define XWOS_SKD_CONTEXT_BH             XWOSDL_SKD_CONTEXT_BH

/**
 * @brief XWOS API：上下文：空闲任务
 */
#define XWOS_SKD_CONTEXT_IDLE           XWOSDL_SKD_CONTEXT_IDLE

/** @} */ // xwos_skd_context

/**
 * @brief XWOS API：线程本地数据指针的数量
 */
#define XWOS_THD_LOCAL_DATA_NUM         XWOSDL_THD_LOCAL_DATA_NUM


/**
 * @brief XWOS API：检查优先级是否有效
 * @param[in] prio: 优先级
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
bool xwos_skd_prio_tst_valid(xwpr_t prio)
{
        return xwosdl_skd_prio_tst_valid(prio);
}

/**
 * @brief XWOS API：启动当前CPU的调度器
 * @return 此函数不会返回
 * @note
 * + 上下文：启动
 * @details
 * 此函数被调用后，调度器将开始调度，上下文(Context)也从 **启动** 切换为 **线程** 。
 * 调度器启动后，此函数不会返回。
 * 此函数只能在 `xwos_init()` 之后被调用。
 */
static __xwos_inline_api
xwer_t xwos_skd_start_lc(void)
{
        return xwosdl_skd_start_lc();
}

/**
 * @brief XWOS API：获取当前CPU的ID
 * @return 当前CPU的ID
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 */
static __xwos_inline_api
xwid_t xwos_skd_get_cpuid_lc(void)
{
        return xwosdl_skd_get_cpuid_lc();
}

/**
 * @brief XWOS API：获取当前代码的上下文
 * @param[out] ctxbuf: 指向缓冲区的指针，通过此缓冲区返回当前上下文，
 *  返回值 @ref xwos_skd_context
 * @param[out] irqnbuf: 指向缓冲区的指针，通过此缓冲区返回中断号
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
void xwos_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        xwosdl_skd_get_context_lc(ctxbuf, irqnbuf);
}

/**
 * @brief XWOS API：关闭本地CPU调度器的抢占
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 */
static __xwos_inline_api
void xwos_skd_dspmpt_lc(void)
{
        xwosdl_skd_dspmpt_lc();
}

/**
 * @brief XWOS API：开启本地CPU调度器的抢占
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 */
static __xwos_inline_api
void xwos_skd_enpmpt_lc(void)
{
        xwosdl_skd_enpmpt_lc();
}

/**
 * @brief XWOS API：关闭本地CPU调度器的中断底半部
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 */
static __xwos_inline_api
void xwos_skd_dsbh_lc(void)
{
        xwosdl_skd_dsbh_lc();
}

/**
 * @brief XWOS API：开启本地CPU调度器的中断底半部
 * @note
 * + 上下文：线程、中断、中断底半部、空闲任务
 */
static __xwos_inline_api
void xwos_skd_enbh_lc(void)
{
        xwosdl_skd_enbh_lc();
}

/**
 * @brief XWOS API：继续运行本地CPU调度器
 * @return 错误码
 * @note
 * + 上下文：任意
 * @details
 * 继续运行调度器包括几个操作：
 * + 1. 启动本地CPU的系统定时器；
 * + 2. 打开本地CPU调度器的中断底半部；
 * + 3. 打开本地CPU调度器的抢占。
 */
static __xwos_inline_api
xwer_t xwos_skd_continue_lc(void)
{
        return xwosdl_skd_continue_lc();
}

/**
 * @brief XWOS API：暂停本地CPU调度器
 * @return 错误码
 * @note
 * + 上下文：任意
 * @details
 * 暂停调度器包括几个操作：
 * + 1. 关闭本地CPU调度器的抢占；
 * + 2. 关闭本地CPU调度器的中断底半部；
 * + 3. 关闭本地CPU的系统定时器。
 */
static __xwos_inline_api
xwer_t xwos_skd_pause_lc(void)
{
        return xwosdl_skd_pause_lc();
}

/** @} */ // xwos_skd


#endif /* xwos/osal/skd.h */
