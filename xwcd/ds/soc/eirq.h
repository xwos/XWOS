/**
 * @file
 * @brief xwcd硬件抽象层：SOC外部中断
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwcd/ds/standard.h>
#include <xwcd/ds/soc/chip.h>

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
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwds_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid, xwsq_t eiflag,
                     xwds_eirq_f isr, xwds_eirq_arg_t arg);
xwer_t xwds_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid);

#endif /* xwcd/ds/soc/eirq.h */
