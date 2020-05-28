/**
 * @file
 * @brief xwmd硬件抽象层：SOC外部中断
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_soc_eirq_h__
#define __xwmd_ds_soc_eirq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWDS External IRQ Number
 */
#define XWDS_EIRQ(x)    ((xwid_t)x)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 外部中断标志
 */
enum xwds_soc_ei_flag_em {
        XWDS_SOC_EIF_TM_RISING = BIT(0), /**< 上升沿触发 */
        XWDS_SOC_EIF_TM_FALLING = BIT(1), /**< 下降沿触发 */
        XWDS_SOC_EIF_TM_EITHER = XWDS_SOC_EIF_TM_RISING |
                                 XWDS_SOC_EIF_TM_FALLING, /**< 任意边沿触发 */
        XWDS_SOC_EIF_TM_LOW = BIT(2), /**< 低电平触发*/
        XWDS_SOC_EIF_TM_HIGH = BIT(3), /**< 高电平触发 */
        XWDS_SOC_EIF_TM_MASK = XWDS_SOC_EIF_TM_RISING | XWDS_SOC_EIF_TM_FALLING |
                               XWDS_SOC_EIF_TM_LOW | XWDS_SOC_EIF_TM_HIGH,
        XWDS_SOC_EIF_WKUP = BIT(4), /**< 唤醒 */
        XWDS_SOC_EIF_DMA = BIT(5), /**< 触发DMA */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWDS API：申请外部中断
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口
 * @param pinmask: (I) GPIO PIN
 * @param eiid: (I) 外部中断ID
 * @param eiflag: (I) 触发标志
 * @param isr: (I) 中断响应函数
 * @param arg: (I) 中断响应函数参数
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 外部中断ID错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid, xwsq_t eiflag,
                     xwds_eirq_f isr, xwds_eirq_arg_t arg);

/**
 * @brief XWDS API：释放外部中断
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口
 * @param pinmask: (I) GPIO PIN
 * @param eiid: (I) 外部中断ID
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 外部中断ID错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/soc/eirq.h */
