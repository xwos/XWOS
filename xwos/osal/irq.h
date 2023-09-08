/**
 * @file
 * @brief 操作系统抽象层：异常与中断
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_irq_h__
#define __xwos_osal_irq_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/irq.h>

/**
 * @defgroup xwos_irq 中断
 * @ingroup xwos
 * 技术参考手册： [中断管理](../docs/TechRefManual/Irq)
 *
 * ## CPU中断开关
 *
 * + `xwos_cpuirq_enable_lc()` ：开启CPU的中断开关
 * + `xwos_cpuirq_disable_lc()` ：关闭CPU的中断开关
 * + `xwos_cpuirq_save_lc()` ：保存当前CPU的中断开关状态后关闭
 * + `xwos_cpuirq_restore_lc()` ：恢复之前保存的CPU的中断开关状态
 *
 *
 * ## 外设中断
 *
 * ### 获取当前代码的中断号
 *
 * + `xwos_irq_get_id()` 可以用来获取当前中断的中断号和判断上下文。
 *
 * ### 外设中断的其他CAPI
 *
 * + `xwos_irq_enable()` ：开启某个外设中断
 * + `xwos_irq_disable()` ：关闭某个外设中断
 * + `xwos_irq_save()` ：保存某个外设中断的开关，然后将其关闭
 * + `xwos_irq_restore()` ：恢复某个外设中断的开关
 *
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/irq.hxx
 * @{
 */

/**
 * @brief XWOS API：开启中断
 * @param[in] irqn: 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_irq_enable(xwirq_t irqn)
{
        return xwosdl_irq_enable(irqn);
}

/**
 * @brief XWOS API：关闭中断
 * @param[in] irqn: 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_irq_disable(xwirq_t irqn)
{
        return xwosdl_irq_disable(irqn);
}

/**
 * @brief XWOS API：保存中断的开关，然后将其关闭
 * @param[in] irqn: 中断号
 * @param[out] flag: 指向缓冲区的指针，此缓冲区用于返回中断开关
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        return xwosdl_irq_save(irqn, flag);
}

/**
 * @brief XWOS API：恢复中断的开关
 * @param[in] irqn: 中断号
 * @param[in] flag: 中断开关
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        return xwosdl_irq_restore(irqn, flag);
}

/**
 * @brief XWOS API：判断当前的上下文是否为中断上下文，并取得当前中断的中断号
 * @param[out] irqnbuf: 指向缓冲区的指针，通过此缓冲区返回当前中断号：
 * + 返回结果仅当返回值为OK时有效
 * + 可为NULL，表示不需要返回中断号
 * @return 错误码
 * @retval XWOK: 当前上下文为中断
 * @retval -ETHDCTX: 当前上下文为线程
 * @retval -EBHCTX: 当前上下文为中断底半部
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_irq_get_id(xwirq_t * irqnbuf)
{
        return xwosdl_irq_get_id(irqnbuf);
}

/**
 * @brief XWOS API：开启本地CPU的中断
 * @note
 * + 上下文：任意
 * @details
 * 此函数运行在哪个CPU上，开启的就是哪个CPU的中断。
 */
static __xwos_inline_api
void xwos_cpuirq_enable_lc(void)
{
        xwosdl_cpuirq_enable_lc();
}

/**
 * @brief XWOS API：关闭本地CPU的中断
 * @note
 * + 上下文：任意
 * @details
 * 此函数运行在哪个CPU上，关闭的就是哪个CPU的中断。
 */
static __xwos_inline_api
void xwos_cpuirq_disable_lc(void)
{
        xwosdl_cpuirq_disable_lc();
}

/**
 * @brief XWOS API：恢复本地CPU的中断
 * @param[in] cpuirq: 本地CPU的中断开关
 * @note
 * + 上下文：任意
 * @details
 * 此函数运行在哪个CPU上，恢复的就是哪个CPU的中断。
 */
static __xwos_inline_api
void xwos_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwosdl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWOS API：保存然后关闭本地CPU的中断
 * @param[out] cpuirq: 指向缓冲区的指针，此缓冲区用于返回本地CPU的中断开关
 * @note
 * + 上下文：任意
 * @details
 * 此函数运行在哪个CPU上，保存然后关闭的就是哪个CPU的中断。
 */
static __xwos_inline_api
void xwos_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwosdl_cpuirq_save_lc(cpuirq);
}

/**
 * @} xwos_irq
 */

#endif /* xwos/osal/irq.h */
