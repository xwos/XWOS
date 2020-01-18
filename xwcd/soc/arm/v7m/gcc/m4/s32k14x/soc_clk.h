/**
 * @file
 * @brief S32K14x Clock
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum s32k14x_clk_em {
        /* system clocks */
        S32K14X_SCLK_PREDIV_SYS,
        S32K14X_SCLK_CORE,
        S32K14X_SCLK_SYS,
        S32K14X_SCLK_BUS,
        S32K14X_SCLK_FLASH,
        S32K14X_SCLK_SPLL,
        S32K14X_SCLK_FIRC,
        S32K14X_SCLK_SIRC,
        S32K14X_SCLK_SOSC,
        S32K14X_SCLK_LPO_RTC_CLK,
        S32K14X_SCLK_RTC_CLKOUT,
        S32K14X_SCLK_LPO32K,
        S32K14X_SCLK_LPO128K,
        S32K14X_SCLK_SCG_CLKOUT,
        S32K14X_SCLK_LPO,
        S32K14X_SCLK_CLKOUT,
        S32K14X_SCLK_VCO,
        S32K14X_SCLK_SPLLDIV1_CLK,
        S32K14X_SCLK_SPLLDIV2_CLK,
        S32K14X_SCLK_FIRCDIV1_CLK,
        S32K14X_SCLK_FIRCDIV2_CLK,
        S32K14X_SCLK_SIRCDIV1_CLK,
        S32K14X_SCLK_SIRCDIV2_CLK,
        S32K14X_SCLK_SOSCDIV1_CLK,
        S32K14X_SCLK_SOSCDIV2_CLK,

        /* module clocks */
        S32K14X_MCLK_SRAM,
        S32K14X_MCLK_CMU,
        S32K14X_MCLK_PMC,
        S32K14X_MCLK_SIM,
        S32K14X_MCLK_RCM,
        S32K14X_MCLK_MSCM,
        S32K14X_MCLK_MPU,
        S32K14X_MCLK_DMA,
        S32K14X_MCLK_ERM,
        S32K14X_MCLK_EIM,
        S32K14X_MCLK_FTFC,
        S32K14X_MCLK_TRGMUX,
        S32K14X_MCLK_DMAMUX,
        S32K14X_MCLK_FlexCAN0,
        S32K14X_MCLK_FlexCAN1,
        S32K14X_MCLK_FTM3,
        S32K14X_MCLK_ADC1,
        S32K14X_MCLK_FlexCAN2,
        S32K14X_MCLK_LPSPI0,
        S32K14X_MCLK_LPSPI1,
        S32K14X_MCLK_LPSPI2,
        S32K14X_MCLK_PDB1,
        S32K14X_MCLK_CRC,
        S32K14X_MCLK_PDB0,
        S32K14X_MCLK_LPIT,
        S32K14X_MCLK_FTM0,
        S32K14X_MCLK_FTM1,
        S32K14X_MCLK_FTM2,
        S32K14X_MCLK_ADC0,
        S32K14X_MCLK_RTC,
        S32K14X_MCLK_LPTMR0,
        S32K14X_MCLK_PORTA,
        S32K14X_MCLK_PORTB,
        S32K14X_MCLK_PORTC,
        S32K14X_MCLK_PORTD,
        S32K14X_MCLK_PORTE,
        S32K14X_MCLK_SAI0,
        S32K14X_MCLK_SAI1,
        S32K14X_MCLK_FlexIO,
        S32K14X_MCLK_WDOG,
        S32K14X_MCLK_EWM,
        S32K14X_MCLK_LPI2C0,
        S32K14X_MCLK_LPI2C1,
        S32K14X_MCLK_LPUART0,
        S32K14X_MCLK_LPUART1,
        S32K14X_MCLK_LPUART2,
        S32K14X_MCLK_FTM4,
        S32K14X_MCLK_FTM5,
        S32K14X_MCLK_FTM6,
        S32K14X_MCLK_FTM7,
        S32K14X_MCLK_CMP0,
        S32K14X_MCLK_QSPI,
        S32K14X_MCLK_ENET,

        S32K14X_CLK_NUM,
};

enum soc_clk_scg_xccr_scs_em {
        SOC_CLK_SCG_XCCR_SCS_RESERVED0 = 0,
        SOC_CLK_SCG_XCCR_SCS_SOSC,
        SOC_CLK_SCG_XCCR_SCS_SIRC,
        SOC_CLK_SCG_XCCR_SCS_FIRC,
        SOC_CLK_SCG_XCCR_SCS_RESERVED4,
        SOC_CLK_SCG_XCCR_SCS_RESERVED5,
        SOC_CLK_SCG_XCCR_SCS_SPLL,
        SOC_CLK_SCG_XCCR_SCS_RESERVED7,
};

enum soc_clk_sim_lpoclks_rtcclksel_em {
        SOC_CLK_SIM_LPOCLKS_RTCCLKSEL_SOSCDIV1_CLK = 0,
        SOC_CLK_SIM_LPOCLKS_RTCCLKSEL_LPO32K_CLK,
        SOC_CLK_SIM_LPOCLKS_RTCCLKSEL_RTC_CLKIN,
        SOC_CLK_SIM_LPOCLKS_RTCCLKSEL_FIRCDIV1_CLK,
};

enum soc_clk_scg_clkoutcnfg_clkoutsel_em {
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_SLOW = 0,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_SOSC,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_SIRC,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_FIRC,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED4,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED5,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_SPLL,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED6,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED7,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED8,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED9,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED10,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED11,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED12,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED13,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED14,
        SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_RESERVED15,
};

enum soc_clk_sim_chipctl_clkoutsel_em {
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SCG_CLKOUT = 0,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_RESERVED1,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SOSC_DIV2_CLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_RESERVED3,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SIRC_DIV2_CLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_RESERVED5,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_FIRC_DIV2_CLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_HCLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SPLL_DIV2_CLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_BUS_CLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_LPO128K_CLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_RESERVED11,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_LPO_CLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_RESERVED13,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_RTC_CLK,
        SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_RESERVED15,
};

enum soc_clk_sai_xcr2_msel_em {
        SOC_CLK_SAI_xCR2_MSEL_BUS_CLK = 0,
        SOC_CLK_SAI_xCR2_MSEL_SAI0_MCLK_IN,
        SOC_CLK_SAI_xCR2_MSEL_SOSCDIV1_CLK,
        SOC_CLK_SAI_xCR2_MSEL_SAI1_MCLK_IN,
};

enum soc_clk_frequency_index_em {
        SOC_CLK_FQCYIDX_BUS = 0,
        SOC_CLK_FQCYIDX_FUNCTION,
        SOC_CLK_FQCYIDX_ADDITION,
        SOC_CLK_FQCYIDX_NUM,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_init_code
void soc_clk_lpo_init(void);

__xwbsp_init_code
void soc_clk_sirc_enable(void);

__xwbsp_init_code
void soc_clk_sirc_disable(void);

__xwbsp_init_code
void soc_clk_firc_enable(void);

__xwbsp_init_code
void soc_clk_firc_disable(void);

__xwbsp_init_code
void soc_clk_xwsc_init_8mhz(void);

__xwbsp_init_code
void soc_clk_xwsc_disable(void);

__xwbsp_init_code
void soc_clk_spll_init_160mhz(void);

__xwbsp_init_code
void soc_clk_spll_init_112mhz(void);

__xwbsp_init_code
void soc_clk_spll_disable(void);

__xwbsp_init_code
void soc_clk_nrun_init_80mhz(void);

__xwbsp_init_code
void soc_clk_nrun_init_8mhz(void);

__xwbsp_init_code
void soc_clk_hsrun_init_112mhz(void);

__xwbsp_init_code
void soc_clk_vlpr_init_4mhz(void);

#endif /* soc_clk.h */
