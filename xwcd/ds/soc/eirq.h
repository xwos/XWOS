/**
 * @file
 * @brief 玄武设备栈：SOC：外部中断
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#ifndef __xwcd_ds_soc_eirq_h__
#define __xwcd_ds_soc_eirq_h__

#include <xwcd/ds/standard.h>
#include <xwcd/ds/soc/chip.h>

/**
 * @ingroup xwcd_ds_soc
 * @{
 */

/**
 * @brief XWDS External IRQ Number
 */
#define XWDS_EIRQ(x)    ((xwid_t)x)

/**
 * @brief 外部中断标志
 */
enum xwds_soc_ei_flag_em {
        XWDS_SOC_EIF_TM_RISING = XWBOP_BIT(0), /**< 上升沿触发 */
        XWDS_SOC_EIF_TM_FALLING = XWBOP_BIT(1), /**< 下降沿触发 */
        XWDS_SOC_EIF_TM_EITHER = (XWDS_SOC_EIF_TM_RISING |
                                  XWDS_SOC_EIF_TM_FALLING), /**< 任意边沿触发 */
        XWDS_SOC_EIF_TM_LOW = XWBOP_BIT(2), /**< 低电平触发*/
        XWDS_SOC_EIF_TM_HIGH = XWBOP_BIT(3), /**< 高电平触发 */
        XWDS_SOC_EIF_TM_MASK = (XWDS_SOC_EIF_TM_RISING | XWDS_SOC_EIF_TM_FALLING |
                                XWDS_SOC_EIF_TM_LOW | XWDS_SOC_EIF_TM_HIGH),
        XWDS_SOC_EIF_WKUP = XWBOP_BIT(4), /**< 唤醒 */
        XWDS_SOC_EIF_DMA = XWBOP_BIT(5), /**< 触发DMA */
};

/**
 * @brief XWDS API：申请外部中断
 * @param[in] soc: SOC对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: GPIO PIN
 * @param[in] eiid: 外部中断ID
 * @param[in] eiflag: 触发标志
 * @param[in] isr: 中断响应函数
 * @param[in] arg: 中断响应函数参数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 外部中断ID错误
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid, xwsq_t eiflag,
                     xwds_eirq_f isr, xwds_eirq_arg_t arg);

/**
 * @brief XWDS API：释放外部中断
 * @param[in] soc: SOC对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: GPIO PIN
 * @param[in] eiid: 外部中断ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 外部中断ID错误
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid);

/**
 * @} xwcd_ds_soc
 */

#endif /* xwcd/ds/soc/eirq.h */
