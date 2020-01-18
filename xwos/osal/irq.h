/**
 * @file
 * @brief 操作系统抽象层：IRQ
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_irq_h__
#define __xwos_osal_irq_h__

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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层API：开启本地CPU的中断
 * @note
 * - 上下文：中断、中断底半部、线程
 */
static __xwos_inline_api
void xwosal_cpuirq_enable_lc(void)
{
        xwosdl_cpuirq_enable_lc();
}

/**
 * @brief 操作系统抽象层API：关闭本地CPU的中断
 * @note
 * - 上下文：中断、中断底半部、线程
 */
static __xwos_inline_api
void xwosal_cpuirq_disable_lc(void)
{
        xwosdl_cpuirq_disable_lc();
}

/**
 * @brief 操作系统抽象层API：恢复本地CPU的中断标志
 * @param cpuirq: (I) 中断标志
 * @note
 * - 上下文：中断、中断底半部、线程
 */
static __xwos_inline_api
void xwosal_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwosdl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief 操作系统抽象层API：保存然后关闭本地CPU的中断标志
 * @param cpuirq: (O) 返回CPU中断标志的缓冲区的指针
 * @note
 * - 上下文：中断、中断底半部、线程
 */
static __xwos_inline_api
void xwosal_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwosdl_cpuirq_save_lc(cpuirq);
}

#endif /* xwos/osal/irq.h */
