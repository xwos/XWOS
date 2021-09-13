/**
 * @file
 * @brief 玄武OS移植实现层：SOC中断
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

#ifndef __xwosimpl_soc_irq_h__
#define __xwosimpl_soc_irq_h__

#ifndef __xwos_ospl_soc_irq_h__
#  error "This file should be included from <xwos/ospl/soc/irq.h>."
#endif

#include <arch_irq.h>

#define SOC_IRQC_MAX_PRIO       (0xFU)
#define SOC_IRQC_OS_PRIO        (0x8U)
#define SOC_IRQC_MIN_PRIO       (SOC_IRQC_OS_PRIO + 1U)
#define SOC_IRQC_ENBIT          (3U)

#define SOC_SYSHWT_PRIO         (SOC_IRQC_OS_PRIO)

#define SOC_SWCX_CH             0
#define SOC_SWCX_IRQN           IRQ_SWINT0
#define SOC_SWCX_PRIO           (SOC_IRQC_OS_PRIO)

#define SOC_SKD_SWI_CH          1
#define SOC_SKD_SWI_IRQN        IRQ_SWINT1
#define SOC_SKD_SWI_PRIO        (SOC_IRQC_OS_PRIO)
#define SOC_SKD_SWI_VALUE       0x00020000U /* SSI1 */

#define SOC_IRQN_NIL            XWIRQ_MAX

enum soc_irq_em {
        IRQ_SWINT0 = 0,
        IRQ_SWINT1,
        IRQ_SWINT2,
        IRQ_SWINT3,
        IRQ_SWINT4,
        IRQ_SWINT5,
        IRQ_SWINT6,
        IRQ_SWINT7,
        IRQ_RESERVED8,
        IRQ_FLASH,
        IRQ_EDMA_CMBERR,
        IRQ_EDMA_CH0,
        IRQ_EDMA_CH1,
        IRQ_EDMA_CH2,
        IRQ_EDMA_CH3,
        IRQ_EDMA_CH4,
        IRQ_EDMA_CH5,
        IRQ_EDMA_CH6,
        IRQ_EDMA_CH7,
        IRQ_EDMA_CH8,
        IRQ_EDMA_CH9,
        IRQ_EDMA_CH10,
        IRQ_EDMA_CH11,
        IRQ_EDMA_CH12,
        IRQ_EDMA_CH13,
        IRQ_EDMA_CH14,
        IRQ_EDMA_CH15,
        IRQ_RESERVED27,
        IRQ_SWT,
        IRQ_RESERVED29,
        IRQ_STM_CH0,
        IRQ_STM_CH1,
        IRQ_STM_CH2,
        IRQ_STM_CH3,
        IRQ_RESERVED34,
        IRQ_ECC_DBD,
        IRQ_ECC_SBD,
        IRQ_RESERVED37,
        IRQ_RTC,
        IRQ_API,
        IRQ_RESERVED40,
        IRQ_EXT_IRQ0,
        IRQ_EXT_IRQ1,
        IRQ_EXT_IRQ2,
        IRQ_RESERVED44,
        IRQ_RESERVED45,
        IRQ_WKUP_IRQ0,
        IRQ_WKUP_IRQ1,
        IRQ_WKUP_IRQ2,
        IRQ_WKUP_IRQ3,
        IRQ_RESERVED50,
        IRQ_ME_SAFE_MODE,
        IRQ_ME_MODE_TRANSITION,
        IRQ_INVALID_MODE,
        IRQ_INVALID_CFG,
        IRQ_RESERVED55,
        IRQ_RGM,
        IRQ_FXOSC,
        IRQ_RESERVED58,
        IRQ_PIT0,
        IRQ_PIT1,
        IRQ_PIT2,
        IRQ_ADC0_EOC,
        IRQ_ADC0_ER,
        IRQ_ADC0_WD,
        IRQ_FLEXCAN0_ESR,
        IRQ_FLEXCAN0_ESR_BOFF,
        IRQ_RESERVED67,
        IRQ_FLEXCAN0_BUF_00_03,
        IRQ_FLEXCAN0_BUF_04_07,
        IRQ_FLEXCAN0_BUF_08_11,
        IRQ_FLEXCAN0_BUF_12_15,
        IRQ_FLEXCAN0_BUF_16_31,
        IRQ_FLEXCAN0_BUF_32_63,
        IRQ_DSPI0_SR_TFUF_RFOF,
        IRQ_DSPI0_SR_EOQF,
        IRQ_DSPI0_SR_TFFF,
        IRQ_DSPI0_SR_TCF,
        IRQ_DSPI0_SR_RFDF,
        IRQ_LINFLEX0_RXI,
        IRQ_LINFLEX0_TXI,
        IRQ_LINFLEX0_ERR,
        IRQ_ADC1_EOC,
        IRQ_RESERVED83,
        IRQ_ADC1_WD,
        IRQ_FLEXCAN1_ESR,
        IRQ_FLEXCAN1_ESR_BOFF,
        IRQ_FLEXCAN1_ESR_WAK,
        IRQ_FLEXCAN1_BUF_00_03,
        IRQ_FLEXCAN1_BUF_04_07,
        IRQ_FLEXCAN1_BUF_08_11,
        IRQ_FLEXCAN1_BUF_12_15,
        IRQ_FLEXCAN1_BUF_16_31,
        IRQ_FLEXCAN1_BUF_32_63,
        IRQ_DSPI1_SR_TFUF_RFOF,
        IRQ_DSPI1_SR_EOQF,
        IRQ_DSPI1_SR_TFFF,
        IRQ_DSPI1_SR_TCF,
        IRQ_DSPI1_SR_RFDF,
        IRQ_LINFLEX1_RXI,
        IRQ_LINFLEX1_TXI,
        IRQ_LINFLEX1_ERR,
        IRQ_RESERVED102,
        IRQ_RESERVED103,
        IRQ_RESERVED104,
        IRQ_FLEXCAN2_ESR,
        IRQ_FLEXCAN2_ESR_BOFF,
        IRQ_RESERVED107,
        IRQ_FLEXCAN2_BUF_00_03,
        IRQ_FLEXCAN2_BUF_04_07,
        IRQ_FLEXCAN2_BUF_08_11,
        IRQ_FLEXCAN2_BUF_12_15,
        IRQ_FLEXCAN2_BUF_16_31,
        IRQ_FLEXCAN2_BUF_32_63,
        IRQ_DSPI2_SR_TFUF_RFOF,
        IRQ_DSPI2_SR_EOQF,
        IRQ_DSPI2_SR_TFFF,
        IRQ_DSPI2_SR_TCF,
        IRQ_DSPI2_SR_RFDF,
        IRQ_LINFLEX2_RXI,
        IRQ_LINFLEX2_TXI,
        IRQ_LINFLEX2_ERR,
        IRQ_LINFLEX3_RXI,
        IRQ_LINFLEX3_TXI,
        IRQ_LINFLEX3_ERR,
        IRQ_I2C0_SR,
        IRQ_RESERVED126,
        IRQ_PIT3,
        IRQ_PIT4,
        IRQ_PIT5,
        IRQ_PIT6,
        IRQ_PIT7,
        IRQ_RESERVED132,
        IRQ_RESERVED133,
        IRQ_RESERVED134,
        IRQ_RESERVED135,
        IRQ_RESERVED136,
        IRQ_RESERVED137,
        IRQ_RESERVED138,
        IRQ_RESERVED139,
        IRQ_RESERVED140,
        IRQ_EMIOS0_GFR_F0_F1,
        IRQ_EMIOS0_GFR_F2_F3,
        IRQ_EMIOS0_GFR_F4_F5,
        IRQ_EMIOS0_GFR_F6_F7,
        IRQ_EMIOS0_GFR_F8_F9,
        IRQ_EMIOS0_GFR_F10_F11,
        IRQ_EMIOS0_GFR_F12_F13,
        IRQ_EMIOS0_GFR_F14_F15,
        IRQ_EMIOS0_GFR_F16_F17,
        IRQ_EMIOS0_GFR_F18_F19,
        IRQ_EMIOS0_GFR_F20_F21,
        IRQ_EMIOS0_GFR_F22_F23,
        IRQ_EMIOS0_GFR_F24_F25,
        IRQ_EMIOS0_GFR_F26_F27,
        IRQ_EMIOS0_GFR_F28_F29,
        IRQ_EMIOS0_GFR_F30_F31,
        IRQ_EMIOS1_GFR_F0_F1,
        IRQ_EMIOS1_GFR_F2_F3,
        IRQ_EMIOS1_GFR_F4_F5,
        IRQ_EMIOS1_GFR_F6_F7,
        IRQ_EMIOS1_GFR_F8_F9,
        IRQ_EMIOS1_GFR_F10_F11,
        IRQ_EMIOS1_GFR_F12_F13,
        IRQ_EMIOS1_GFR_F14_F15,
        IRQ_EMIOS1_GFR_F16_F17,
        IRQ_EMIOS1_GFR_F18_F19,
        IRQ_EMIOS1_GFR_F20_F21,
        IRQ_EMIOS1_GFR_F22_F23,
        IRQ_EMIOS1_GFR_F24_F25,
        IRQ_EMIOS1_GFR_F26_F27,
        IRQ_EMIOS1_GFR_F28_F29,
        IRQ_EMIOS1_GFR_F30_F31,
        IRQ_FLEXCAN3_ESR,
        IRQ_FLEXCAN3_ESR_BOFF,
        IRQ_RESERVED175,
        IRQ_FLEXCAN3_BUF_00_03,
        IRQ_FLEXCAN3_BUF_04_07,
        IRQ_FLEXCAN3_BUF_08_11,
        IRQ_FLEXCAN3_BUF_12_15,
        IRQ_FLEXCAN3_BUF_16_31,
        IRQ_FLEXCAN3_BUF_32_63,
        IRQ_DSPI3_SR_TFUF_RFOF,
        IRQ_DSPI3_SR_EOQF,
        IRQ_DSPI3_SR_TFFF,
        IRQ_DSPI3_SR_TCF,
        IRQ_DSPI3_SR_RFDF,
        IRQ_LINFLEX4_RXI,
        IRQ_LINFLEX4_TXI,
        IRQ_LINFLEX4_ERR,
        IRQ_FLEXCAN4_ESR,
        IRQ_FLEXCAN4_ESR_BOFF,
        IRQ_RESERVED192,
        IRQ_FLEXCAN4_BUF_00_03,
        IRQ_FLEXCAN4_BUF_04_07,
        IRQ_FLEXCAN4_BUF_08_11,
        IRQ_FLEXCAN4_BUF_12_15,
        IRQ_FLEXCAN4_BUF_16_31,
        IRQ_FLEXCAN4_BUF_32_63,
        IRQ_LINFLEX5_RXI,
        IRQ_LINFLEX5_TXI,
        IRQ_LINFLEX5_ERR,
        IRQ_FLEXCAN5_ESR,
        IRQ_FLEXCAN5_ESR_BOFF,
        IRQ_RESERVED204,
        IRQ_FLEXCAN5_BUF_00_03,
        IRQ_FLEXCAN5_BUF_04_07,
        IRQ_FLEXCAN5_BUF_08_11,
        IRQ_FLEXCAN5_BUF_12_15,
        IRQ_FLEXCAN5_BUF_16_31,
        IRQ_FLEXCAN5_BUF_32_63,
        IRQ_DSPI4_SR_TFUF_RFOF,
        IRQ_DSPI4_SR_EOQF,
        IRQ_DSPI4_SR_TFFF,
        IRQ_DSPI4_SR_TCF,
        IRQ_DSPI4_SR_RFDF,
        IRQ_LINFLEX6_RXI,
        IRQ_LINFLEX6_TXI,
        IRQ_LINFLEX6_ERR,
        IRQ_DSPI5_SR_TFUF_RFOF,
        IRQ_DSPI5_SR_EOQF,
        IRQ_DSPI5_SR_TFFF,
        IRQ_DSPI5_SR_TCF,
        IRQ_DSPI5_SR_RFDF,
        IRQ_LINFLEX7_RXI,
        IRQ_LINFLEX7_TXI,
        IRQ_LINFLEX7_ERR,
        IRQ_LINFLEX8_RXI,
        IRQ_LINFLEX8_TXI,
        IRQ_LINFLEX8_ERR,
        IRQ_LINFLEX9_RXI,
        IRQ_LINFLEX9_TXI,
        IRQ_LINFLEX9_ERR,
        IRQ_SXOSC,
};

struct soc_irq_cfg {
        xwu8_t priority;
};

struct soc_irq_data {
        void * data;
};

struct soc_ivt {
        xwisr_f irq[SOCCFG_IRQ_NUM];
};

struct soc_idvt {
        void * irq[SOCCFG_IRQ_NUM];
};

void soc_irqc_init(void);
void soc_isr_noop(void);
void soc_skd_isr_swi(void);

#endif /* xwosimpl_soc_irq.h */
