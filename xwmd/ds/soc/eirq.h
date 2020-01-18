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
#include <xwos/standard.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SODS External IRQ Number
 */
#define SODS_EIRQ(x)    ((xwid_t)x)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 外部中断标志
 */
enum xwds_soc_ei_flag_em {
        SODS_SOC_EIF_TM_RISING = BIT(0), /**< 上升沿触发 */
        SODS_SOC_EIF_TM_FALLING = BIT(1), /**< 下降沿触发 */
        SODS_SOC_EIF_TM_EITHER = SODS_SOC_EIF_TM_RISING |
                                 SODS_SOC_EIF_TM_FALLING, /**< 任意边沿触发 */
        SODS_SOC_EIF_TM_LOW = BIT(2), /**< 低电平触发*/
        SODS_SOC_EIF_TM_HIGH = BIT(3), /**< 高电平触发 */
        SODS_SOC_EIF_TM_MASK = SODS_SOC_EIF_TM_RISING | SODS_SOC_EIF_TM_FALLING |
                               SODS_SOC_EIF_TM_LOW | SODS_SOC_EIF_TM_HIGH,
        SODS_SOC_EIF_WKUP = BIT(4), /**< 唤醒 */
        SODS_SOC_EIF_DMA = BIT(5), /**< 触发DMA */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_api
xwer_t xwds_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid, xwsq_t eiflag,
                     xwds_eirq_f isr, xwds_eirq_arg_t arg);

__xwds_api
xwer_t xwds_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/soc/eirq.h */
