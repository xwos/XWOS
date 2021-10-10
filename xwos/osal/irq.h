/**
 * @file
 * @brief 操作系统抽象层：中断
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_irq_h__
#define __xwos_osal_irq_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/irq.h>

/**
 * @defgroup xwos_irq 中断
 * @ingroup xwos
 * - EXC：编号为负数，从-1开始编号
 * - IRQ：编号为正数或0，从0开始编号
 * @{
 */

/**
 * @brief XWOS API：申请中断
 * @param[in] irqn: 中断号
 * @param[in] isr: 中断处理函数
 * @param[in] data: 中断数据
 * @param[in] cfg: 中断配置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 中断号的枚举通常定义在芯片的头文件中；
 * - SOCCFG_RO_IVT配置为0时，参数isr无效，中断向量需预先写入vector.c
 *   的向量表中。
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_request(xwirq_t irqn, xwisr_f isr, void * data,
                        const struct soc_irq_cfg * cfg)
{
        return xwosdl_irq_request(irqn, isr, data, cfg);
}

/**
 * @brief XWOS API：释放中断
 * @param[in] irqn: 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_release(xwirq_t irqn)
{
        return xwosdl_irq_release(irqn);
}

/**
 * @brief XWOS API：开启中断
 * @param[in] irqn: 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        return xwosdl_irq_restore(irqn, flag);
}

/**
 * @brief XWOS API：挂起中断标志
 * @param[in] irqn: 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_pend(xwirq_t irqn)
{
        return xwosdl_irq_pend(irqn);
}

/**
 * @brief XWOS API：清除中断标志
 * @param[in] irqn: 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_clear(xwirq_t irqn)
{
        return xwosdl_irq_clear(irqn);
}

/**
 * @brief XWOS API：测试中断是否挂起
 * @param[in] irqn: 中断号
 * @param[out] pending: 指向缓冲区的指针，此缓冲区用于返回中断是否挂起
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_tst(xwirq_t irqn, bool * pending)
{
        return xwosdl_irq_tst(irqn, pending);
}

/**
 * @brief XWOS API：配置中断
 * @param[in] irqn: 中断号
 * @param[in] cfg: CPU私有的配置结构体指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        return xwosdl_irq_cfg(irqn, cfg);
}

/**
 * @brief XWOS API：获取中断的配置
 * @param[in] irqn: 中断号
 * @param[in] cfgbuf: 指向缓冲区的指针，此缓冲区用于返回SOC中断配置结构体
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        return xwosdl_irq_get_cfg(irqn, cfgbuf);
}

/**
 * @brief XWOS API：获取中断的数据
 * @param[in] irqn: 中断号
 * @param[in] databuf: 指向缓冲区的指针，此缓冲区用于返回SOC中断数据结构体
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        return xwosdl_irq_get_data(irqn, databuf);
}

/**
 * @brief XWOS API：判断当前的上下文是否为中断上下文，并取得当前中断的中断号
 * @param[out] irqnbuf: 指向缓冲区的指针，通过此缓冲区返回当前中断号：
 * + 返回结果仅当返回值为OK时有效
 * + 可为NULL，表示不需要返回中断号
 * @return 错误码
 * @retval OK: 当前上下文为中断
 * @retval -EINTHD: 当前上下文为线程
 * @retval -EINBH: 当前上下文为中断底半部
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_get_id(xwirq_t * irqnbuf)
{
        return xwosdl_irq_get_id(irqnbuf);
}

/**
 * @brief XWOS API：开启本地CPU的中断
 */
static __xwos_inline_api
void xwos_cpuirq_enable_lc(void)
{
        xwosdl_cpuirq_enable_lc();
}

/**
 * @brief XWOS API：关闭本地CPU的中断
 */
static __xwos_inline_api
void xwos_cpuirq_disable_lc(void)
{
        xwosdl_cpuirq_disable_lc();
}

/**
 * @brief XWOS API：恢复本地CPU的中断
 * @param[in] flag: 本地CPU的中断开关
 */
static __xwos_inline_api
void xwos_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwosdl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWOS API：保存然后关闭本地CPU的中断
 * @param[out] flag: 指向缓冲区的指针，此缓冲区用于返回本地CPU的中断开关
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
