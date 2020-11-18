/**
 * @file
 * @brief SOC描述层：Clock of MPC560xB
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

#ifndef __soc_clk_h__
#define __soc_clk_h__

#include <xwos/standard.h>
#include <soc.h>

enum mpc560xb_clk_em {
        MPC560XB_CLK_FXOSC = 0,
        MPC560XB_CLK_FIRC,
        MPC560XB_CLK_SCOSC,
        MPC560XB_CLK_SIRC,
        MPC560XB_CLK_FMPLL,
        MPC560XB_CLK_SYSCLK,
        MPC560XB_CLK_PC1,
        MPC560XB_CLK_PC2,
        MPC560XB_CLK_PC3,
        MPC560XB_CLK_ADC_0,
#if defined(MPC5607B)
        MPC560XB_CLK_ADC_1,
#endif /* #if defined(MPC5607B) */
        MPC560XB_CLK_CAN_SAMPLER,
        MPC560XB_CLK_CMU,
        MPC560XB_CLK_CTU,
        MPC560XB_CLK_DMA_MUX,
        MPC560XB_CLK_DSPI_0,
        MPC560XB_CLK_DSPI_1,
        MPC560XB_CLK_DSPI_2,
        MPC560XB_CLK_DSPI_3,
#if defined(MPC5607B)
        MPC560XB_CLK_DSPI_4,
        MPC560XB_CLK_DSPI_5,
#endif /* #if defined(MPC5607B) */
        MPC560XB_CLK_EMIOS_0,
        MPC560XB_CLK_EMIOS_1,
        MPC560XB_CLK_FLEXCAN_0,
        MPC560XB_CLK_FLEXCAN_1,
        MPC560XB_CLK_FLEXCAN_2,
        MPC560XB_CLK_FLEXCAN_3,
        MPC560XB_CLK_FLEXCAN_4,
        MPC560XB_CLK_FLEXCAN_5,
        MPC560XB_CLK_I2C,
        MPC560XB_CLK_LINFLEX_0,
        MPC560XB_CLK_LINFLEX_1,
        MPC560XB_CLK_LINFLEX_2,
        MPC560XB_CLK_LINFLEX_3,
#if defined(MPC5607B)
        MPC560XB_CLK_LINFLEX_4,
        MPC560XB_CLK_LINFLEX_5,
        MPC560XB_CLK_LINFLEX_6,
        MPC560XB_CLK_LINFLEX_7,
        MPC560XB_CLK_LINFLEX_8,
        MPC560XB_CLK_LINFLEX_9,
#endif /* #if defined(MPC5607B) */
        MPC560XB_CLK_PIT,
        MPC560XB_CLK_RTC,
        MPC560XB_CLK_SIUL,
        MPC560XB_CLK_WKUP,
        MPC560XB_CLK_NUM,
};

#endif /* soc_clk.h */
