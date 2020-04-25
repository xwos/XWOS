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
 * @brief XWOSAL API：开启本地CPU的中断
 * @note
 * - 上下文：中断、中断底半部、线程
 */
static __xwos_inline_api
void xwosal_cpuirq_enable_lc(void)
{
        xwosdl_cpuirq_enable_lc();
}

/**
 * @brief XWOSAL API：关闭本地CPU的中断
 * @note
 * - 上下文：中断、中断底半部、线程
 */
static __xwos_inline_api
void xwosal_cpuirq_disable_lc(void)
{
        xwosdl_cpuirq_disable_lc();
}

/**
 * @brief XWOSAL API：恢复本地CPU的中断标志
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
 * @brief XWOSAL API：保存然后关闭本地CPU的中断标志
 * @param cpuirq: (O) 返回CPU中断标志的缓冲区的指针
 * @note
 * - 上下文：中断、中断底半部、线程
 */
static __xwos_inline_api
void xwosal_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwosdl_cpuirq_save_lc(cpuirq);
}

/**
 * @brief XWOSAL API：测试当前上下文是否为中断上下文，并返回中断号
 * @param irqnbuf: (O) 指向缓冲区的指针，通过此缓冲区返回当前中断号：
 *                     - 在XuanWuOS中，可返回中断号；在Linux内核中，返回的中断号无效
 *                     - 可为NULL，表示不需要返回中断号
 * @return 错误码
 * @retval OK: 当前上下文为中断
 * @retval -EINTHRD: 当前上下文为线程
 * @retval -EINBH: 当前上下文为中断底半部
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_irq_get_id(xwirq_t * irqnbuf)
{
        return xwosdl_irq_get_id(irqnbuf);
}

#endif /* xwos/osal/irq.h */
