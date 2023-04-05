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


/**
 * @defgroup xwos_skd_context_em 上下文类型枚举
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

/** @} */ // xwos_skd_context_em

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
 * @brief XWOS API：初始化当前CPU的调度器
 * @return 错误码
 * @note
 * + 上下文：启动
 * @details
 * 此函数只能在 `xwos_init()` 之后， `xwos_skd_start_lc()` 之前被调用 。
 */
static __xwos_inline_api
xwer_t xwos_skd_init_lc(void)
{
        return xwosdl_skd_init_lc();
}

/**
 * @brief XWOS API：启动当前CPU的调度器
 * @return 此函数不会返回
 * @note
 * + 上下文：启动
 * @details
 * 此函数调用后，调度器将开始调度，且不会返回，
 * 上下文(Context)也从 **启动** 切换为 **线程** 。
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
xwid_t xwos_skd_id_lc(void)
{
        return xwosdl_skd_id_lc();
}

/**
 * @brief XWOS API：获取当前代码的上下文
 * @param[out] ctxbuf: 指向缓冲区的指针，通过此缓冲区返回当前上下文，
 *  返回值 @ref xwos_skd_context_em
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

/** @} */ // xwos_skd


#endif /* xwos/osal/skd.h */
