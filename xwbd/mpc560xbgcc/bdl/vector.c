/**
 * @file
 * @brief 板级描述层：中断向量表
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc_irq.h>
#include <soc_irqc.h>
#include <soc_sched.h>
#include <soc_syshwt.h>
#include <driver/ds/soc.h>
#include <driver/ds/i2cm.h>
#include <driver/ds/dmauart0.h>
#include <bdl/ds/description/mpc560xbdkp.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 中断向量表
 */
__xwos_vctbl __soc_isr_table_qualifier struct soc_isr_table soc_isr_table = {
        .soc = {
                (xwisr_f)soc_scheduler_isr_swcx, /* Vector - 0 SWINT0 */
                (xwisr_f)soc_scheduler_isr_swi, /* Vector - 1 SWINT1 */
                (xwisr_f)soc_isr_nop, /* Vector - 2 SWINT2 */
                (xwisr_f)soc_isr_nop, /* Vector - 3 SWINT3 */
                (xwisr_f)soc_isr_nop, /* Vector - 4 SWINT4 */
                (xwisr_f)soc_isr_nop, /* Vector - 5 SWINT5 */
                (xwisr_f)soc_isr_nop, /* Vector - 6 SWINT6 */
                (xwisr_f)soc_isr_nop, /* Vector - 7 SWINT7 */
                (xwisr_f)soc_isr_nop, /* Vector - 8 */
                (xwisr_f)soc_isr_nop, /* Vector - 9 FLASH */
                (xwisr_f)mpc560xb_edma_cmberr_isr, /* Vector - 10 EDMA_CMBERR */
                (xwisr_f)mpc560xb_edma_ch0_isr, /* Vector - 11 EDMA_CH0 */
                (xwisr_f)mpc560xb_edma_ch1_isr, /* Vector - 12 EDMA_CH1 */
                (xwisr_f)mpc560xb_edma_ch2_isr, /* Vector - 13 EDMA_CH2 */
                (xwisr_f)mpc560xb_edma_ch3_isr, /* Vector - 14 EDMA_CH3 */
                (xwisr_f)mpc560xb_edma_ch4_isr, /* Vector - 15 EDMA_CH4 */
                (xwisr_f)mpc560xb_edma_ch5_isr, /* Vector - 16 EDMA_CH5 */
                (xwisr_f)mpc560xb_edma_ch6_isr, /* Vector - 17 EDMA_CH6 */
                (xwisr_f)mpc560xb_edma_ch7_isr, /* Vector - 18 EDMA_CH7 */
                (xwisr_f)mpc560xb_edma_ch8_isr, /* Vector - 19 EDMA_CH8 */
                (xwisr_f)mpc560xb_edma_ch9_isr, /* Vector - 20 EDMA_CH9 */
                (xwisr_f)mpc560xb_edma_ch10_isr, /* Vector - 21 EDMA_CH10 */
                (xwisr_f)mpc560xb_edma_ch11_isr, /* Vector - 22 EDMA_CH11 */
                (xwisr_f)mpc560xb_edma_ch12_isr, /* Vector - 23 EDMA_CH12 */
                (xwisr_f)mpc560xb_edma_ch13_isr, /* Vector - 24 EDMA_CH13 */
                (xwisr_f)mpc560xb_edma_ch14_isr, /* Vector - 25 EDMA_CH14 */
                (xwisr_f)mpc560xb_edma_ch15_isr, /* Vector - 26 EDMA_CH15 */
                (xwisr_f)soc_isr_nop, /* Vector - 27 */
                (xwisr_f)soc_isr_nop, /* Vector - 28 SWT */
                (xwisr_f)soc_isr_nop, /* Vector - 29 */
                (xwisr_f)soc_isr_nop, /* Vector - 30 STM_CH0 */
                (xwisr_f)soc_isr_nop, /* Vector - 31 STM_CH1 */
                (xwisr_f)soc_isr_nop, /* Vector - 32 STM_CH2 */
                (xwisr_f)soc_isr_nop, /* Vector - 33 STM_CH3 */
                (xwisr_f)soc_isr_nop, /* Vector - 34 */
                (xwisr_f)soc_isr_nop, /* Vector - 35 ECC_DBD */
                (xwisr_f)soc_isr_nop, /* Vector - 36 IRQ_ECC_SBD */
                (xwisr_f)soc_isr_nop, /* Vector - 37 */
                (xwisr_f)soc_isr_nop, /* Vector - 38 RTC */
                (xwisr_f)soc_isr_nop, /* Vector - 39 API*/
                (xwisr_f)soc_isr_nop, /* Vector - 40 */
                (xwisr_f)mpc560xb_eirq0_7_isr, /* Vector - 41 EXT_IRQ_0_7 */
                (xwisr_f)mpc560xb_eirq8_15_isr, /* Vector - 42 EXT_IRQ_8_15 */
                (xwisr_f)mpc560xb_eirq16_23_isr, /* Vector - 43 EXT_IRQ_16_23 */
                (xwisr_f)soc_isr_nop, /* Vector - 44 */
                (xwisr_f)soc_isr_nop, /* Vector - 45 */
                (xwisr_f)mpc560xb_wkup_irq0_7_isr, /* Vector - 46 WKUP_IRQ_0_7 */
                (xwisr_f)mpc560xb_wkup_irq8_15_isr, /* Vector - 47 WKUP_IRQ_8_15 */
                (xwisr_f)mpc560xb_wkup_irq16_23_isr, /* Vector - 48 WKUP_IRQ_16_23 */
                (xwisr_f)mpc560xb_wkup_irq24_28_isr, /* Vector - 49 WKUP_IRQ_24_28 */
                (xwisr_f)soc_isr_nop, /* Vector - 50 */
                (xwisr_f)soc_isr_nop, /* Vector - 51 ME_SAFE_MODE */
                (xwisr_f)soc_isr_nop, /* Vector - 52 ME_MODE_TRANSITION */
                (xwisr_f)soc_isr_nop, /* Vector - 53 INVALID_MODE */
                (xwisr_f)soc_isr_nop, /* Vector - 54 INVALID_CFG */
                (xwisr_f)soc_isr_nop, /* Vector - 55 */
                (xwisr_f)soc_isr_nop, /* Vector - 56 RGM */
                (xwisr_f)soc_isr_nop, /* Vector - 57 FXOSC */
                (xwisr_f)soc_isr_nop, /* Vector - 58 */
                (xwisr_f)soc_syshwt_isr, /* Vector - 59 PIT0 */
                (xwisr_f)soc_isr_nop, /* Vector - 60 PIT1 */
                (xwisr_f)soc_isr_nop, /* Vector - 61 PIT2 */
                (xwisr_f)soc_isr_nop, /* Vector - 62 ADC0_EOC */
                (xwisr_f)soc_isr_nop, /* Vector - 63 ADC0_ER */
                (xwisr_f)soc_isr_nop, /* Vector - 64 ADC0_WD */
                (xwisr_f)soc_isr_nop, /* Vector - 65 FLEXCAN0_ESR */
                (xwisr_f)soc_isr_nop, /* Vector - 66 FLEXCAN0_ESR_BOFF */
                (xwisr_f)soc_isr_nop, /* Vector - 67 */
                (xwisr_f)soc_isr_nop, /* Vector - 68 FLEXCAN0_BUF_00_03 */
                (xwisr_f)soc_isr_nop, /* Vector - 69 FLEXCAN0_BUF_04_07 */
                (xwisr_f)soc_isr_nop, /* Vector - 70 FLEXCAN0_BUF_08_11 */
                (xwisr_f)soc_isr_nop, /* Vector - 71 FLEXCAN0_BUF_12_15 */
                (xwisr_f)soc_isr_nop, /* Vector - 72 FLEXCAN0_BUF_16_31 */
                (xwisr_f)soc_isr_nop, /* Vector - 73 FLEXCAN0_BUF_32_63 */
                (xwisr_f)soc_isr_nop, /* Vector - 74 DSPI0_SR_TFUF_RFOF */
                (xwisr_f)soc_isr_nop, /* Vector - 75 DSPI0_SR_EOQF */
                (xwisr_f)soc_isr_nop, /* Vector - 76 DSPI0_SR_TFFF */
                (xwisr_f)soc_isr_nop, /* Vector - 77 DSPI0_SR_TCF */
                (xwisr_f)soc_isr_nop, /* Vector - 78 DSPI0_SR_RFDF */
                (xwisr_f)soc_isr_nop, /* Vector - 79 LINFLEX0_RX */
                (xwisr_f)soc_isr_nop, /* Vector - 80 LINFLEX0_TX */
                (xwisr_f)mpc560xb_dmauart0_err_isr, /* Vector - 81 LINFLEX0_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 82 ADC1_EOC */
                (xwisr_f)soc_isr_nop, /* Vector - 83 */
                (xwisr_f)soc_isr_nop, /* Vector - 84 ADC1_WD */
                (xwisr_f)soc_isr_nop, /* Vector - 85 FLEXCAN1_ESR */
                (xwisr_f)soc_isr_nop, /* Vector - 86 FLEXCAN1_ESR_BOFF */
                (xwisr_f)soc_isr_nop, /* Vector - 87 FLEXCAN1_ESR_WAK */
                (xwisr_f)soc_isr_nop, /* Vector - 88 FLEXCAN1_BUF_00_03 */
                (xwisr_f)soc_isr_nop, /* Vector - 89 FLEXCAN1_BUF_04_07 */
                (xwisr_f)soc_isr_nop, /* Vector - 90 FLEXCAN1_BUF_08_11 */
                (xwisr_f)soc_isr_nop, /* Vector - 91 FLEXCAN1_BUF_12_15 */
                (xwisr_f)soc_isr_nop, /* Vector - 92 FLEXCAN1_BUF_16_31 */
                (xwisr_f)soc_isr_nop, /* Vector - 93 FLEXCAN1_BUF_32_63 */
                (xwisr_f)soc_isr_nop, /* Vector - 94 DSPI1_SR_TFUF_RFOF */
                (xwisr_f)soc_isr_nop, /* Vector - 95 DSPI1_SR_EOQF */
                (xwisr_f)soc_isr_nop, /* Vector - 96 DSPI1_SR_TFFF */
                (xwisr_f)soc_isr_nop, /* Vector - 97 DSPI1_SR_TCF */
                (xwisr_f)soc_isr_nop, /* Vector - 98 DSPI1_SR_RFDF */
                (xwisr_f)soc_isr_nop, /* Vector - 99 LINFLEX1_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 100 LINFLEX1_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 101 LINFLEX1_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 102 */
                (xwisr_f)soc_isr_nop, /* Vector - 103 */
                (xwisr_f)soc_isr_nop, /* Vector - 104 */
                (xwisr_f)soc_isr_nop, /* Vector - 105 FLEXCAN2_ESR */
                (xwisr_f)soc_isr_nop, /* Vector - 106 FLEXCAN2_ESR_BOFF */
                (xwisr_f)soc_isr_nop, /* Vector - 107 */
                (xwisr_f)soc_isr_nop, /* Vector - 108 FLEXCAN2_BUF_00_03 */
                (xwisr_f)soc_isr_nop, /* Vector - 109 FLEXCAN2_BUF_04_07 */
                (xwisr_f)soc_isr_nop, /* Vector - 110 FLEXCAN2_BUF_08_11 */
                (xwisr_f)soc_isr_nop, /* Vector - 111 FLEXCAN2_BUF_12_15 */
                (xwisr_f)soc_isr_nop, /* Vector - 112 FLEXCAN2_BUF_16_31 */
                (xwisr_f)soc_isr_nop, /* Vector - 113 FLEXCAN2_BUF_32_63 */
                (xwisr_f)soc_isr_nop, /* Vector - 114 DSPI2_SR_TFUF_RFOF */
                (xwisr_f)soc_isr_nop, /* Vector - 115 DSPI2_SR_EOQF */
                (xwisr_f)soc_isr_nop, /* Vector - 116 DSPI2_SR_TFFF */
                (xwisr_f)soc_isr_nop, /* Vector - 117 DSPI2_SR_TCF */
                (xwisr_f)soc_isr_nop, /* Vector - 118 DSPI2_SR_RFDF */
                (xwisr_f)soc_isr_nop, /* Vector - 119 LINFLEX2_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 120 LINFLEX2_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 121 LINFLEX2_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 122 LINFLEX3_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 123 LINFLEX3_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 124 LINFLEX3_ERR */
                (xwisr_f)mpc560xb_i2cm_isr, /* Vector - 125 I2C0_SR */
                (xwisr_f)soc_isr_nop, /* Vector - 126 PIT3 */
                (xwisr_f)soc_isr_nop, /* Vector - 127 PIT4 */
                (xwisr_f)soc_isr_nop, /* Vector - 128 PIT5 */
                (xwisr_f)soc_isr_nop, /* Vector - 129 PIT6 */
                (xwisr_f)soc_isr_nop, /* Vector - 130 PIT7 */
                (xwisr_f)soc_isr_nop, /* Vector - 131 */
                (xwisr_f)soc_isr_nop, /* Vector - 132 */
                (xwisr_f)soc_isr_nop, /* Vector - 133 */
                (xwisr_f)soc_isr_nop, /* Vector - 134 */
                (xwisr_f)soc_isr_nop, /* Vector - 135 */
                (xwisr_f)soc_isr_nop, /* Vector - 136 */
                (xwisr_f)soc_isr_nop, /* Vector - 137 */
                (xwisr_f)soc_isr_nop, /* Vector - 138 */
                (xwisr_f)soc_isr_nop, /* Vector - 139 */
                (xwisr_f)soc_isr_nop, /* Vector - 140 */
                (xwisr_f)soc_isr_nop, /* Vector - 141 EMIOS0_GFR_F0_F1 */
                (xwisr_f)soc_isr_nop, /* Vector - 142 EMIOS0_GFR_F2_F3 */
                (xwisr_f)soc_isr_nop, /* Vector - 143 EMIOS0_GFR_F4_F5 */
                (xwisr_f)soc_isr_nop, /* Vector - 144 EMIOS0_GFR_F6_F7 */
                (xwisr_f)soc_isr_nop, /* Vector - 145 EMIOS0_GFR_F8_F9 */
                (xwisr_f)soc_isr_nop, /* Vector - 146 EMIOS0_GFR_F10_F11 */
                (xwisr_f)soc_isr_nop, /* Vector - 147 EMIOS0_GFR_F12_F13 */
                (xwisr_f)soc_isr_nop, /* Vector - 148 EMIOS0_GFR_F14_F15 */
                (xwisr_f)soc_isr_nop, /* Vector - 149 EMIOS0_GFR_F16_F17 */
                (xwisr_f)soc_isr_nop, /* Vector - 150 EMIOS0_GFR_F18_F19 */
                (xwisr_f)soc_isr_nop, /* Vector - 151 EMIOS0_GFR_F20_F21 */
                (xwisr_f)soc_isr_nop, /* Vector - 152 EMIOS0_GFR_F22_F23 */
                (xwisr_f)soc_isr_nop, /* Vector - 153 EMIOS0_GFR_F24_F25 */
                (xwisr_f)soc_isr_nop, /* Vector - 154 EMIOS0_GFR_F26_F27 */
                (xwisr_f)soc_isr_nop, /* Vector - 155 EMIOS0_GFR_F28_F29 */
                (xwisr_f)soc_isr_nop, /* Vector - 156 EMIOS0_GFR_F30_F31 */
                (xwisr_f)soc_isr_nop, /* Vector - 157 EMIOS1_GFR_F0_F1 */
                (xwisr_f)soc_isr_nop, /* Vector - 158 EMIOS1_GFR_F2_F3 */
                (xwisr_f)soc_isr_nop, /* Vector - 159 EMIOS1_GFR_F4_F5 */
                (xwisr_f)soc_isr_nop, /* Vector - 160 EMIOS1_GFR_F6_F7 */
                (xwisr_f)soc_isr_nop, /* Vector - 161 EMIOS1_GFR_F8_F9 */
                (xwisr_f)soc_isr_nop, /* Vector - 162 EMIOS1_GFR_F10_F11 */
                (xwisr_f)soc_isr_nop, /* Vector - 163 EMIOS1_GFR_F12_F13 */
                (xwisr_f)soc_isr_nop, /* Vector - 164 EMIOS1_GFR_F14_F15 */
                (xwisr_f)soc_isr_nop, /* Vector - 165 EMIOS1_GFR_F16_F17 */
                (xwisr_f)soc_isr_nop, /* Vector - 166 EMIOS1_GFR_F18_F19 */
                (xwisr_f)soc_isr_nop, /* Vector - 167 EMIOS1_GFR_F20_F21 */
                (xwisr_f)soc_isr_nop, /* Vector - 168 EMIOS1_GFR_F22_F23 */
                (xwisr_f)soc_isr_nop, /* Vector - 169 EMIOS1_GFR_F24_F25 */
                (xwisr_f)soc_isr_nop, /* Vector - 170 EMIOS1_GFR_F26_F27 */
                (xwisr_f)soc_isr_nop, /* Vector - 171 EMIOS1_GFR_F28_F29 */
                (xwisr_f)soc_isr_nop, /* Vector - 172 EMIOS1_GFR_F30_F31 */
                (xwisr_f)soc_isr_nop, /* Vector - 173 FLEXCAN3_ESR */
                (xwisr_f)soc_isr_nop, /* Vector - 174 FLEXCAN3_ESR_BOFF */
                (xwisr_f)soc_isr_nop, /* Vector - 175 */
                (xwisr_f)soc_isr_nop, /* Vector - 176 FLEXCAN3_BUF_00_03 */
                (xwisr_f)soc_isr_nop, /* Vector - 177 FLEXCAN3_BUF_04_07 */
                (xwisr_f)soc_isr_nop, /* Vector - 178 FLEXCAN3_BUF_08_11 */
                (xwisr_f)soc_isr_nop, /* Vector - 179 FLEXCAN3_BUF_12_15 */
                (xwisr_f)soc_isr_nop, /* Vector - 180 FLEXCAN3_BUF_16_31 */
                (xwisr_f)soc_isr_nop, /* Vector - 181 FLEXCAN3_BUF_32_63 */
                (xwisr_f)soc_isr_nop, /* Vector - 182 DSPI3_SR_TFUF_RFOF */
                (xwisr_f)soc_isr_nop, /* Vector - 183 DSPI3_SR_EOQF */
                (xwisr_f)soc_isr_nop, /* Vector - 184 DSPI3_SR_TFFF */
                (xwisr_f)soc_isr_nop, /* Vector - 185 DSPI3_SR_TCF */
                (xwisr_f)soc_isr_nop, /* Vector - 186 DSPI3_SR_RFDF */
                (xwisr_f)soc_isr_nop, /* Vector - 187 LINFLEX4_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 188 LINFLEX4_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 189 LINFLEX4_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 190 FLEXCAN4_ESR */
                (xwisr_f)soc_isr_nop, /* Vector - 191 FLEXCAN4_ESR_BOFF*/
                (xwisr_f)soc_isr_nop, /* Vector - 192 */
                (xwisr_f)soc_isr_nop, /* Vector - 193 FLEXCAN4_BUF_00_03 */
                (xwisr_f)soc_isr_nop, /* Vector - 194 FLEXCAN4_BUF_04_07 */
                (xwisr_f)soc_isr_nop, /* Vector - 195 FLEXCAN4_BUF_08_11 */
                (xwisr_f)soc_isr_nop, /* Vector - 196 FLEXCAN4_BUF_12_15 */
                (xwisr_f)soc_isr_nop, /* Vector - 197 FLEXCAN4_BUF_16_31 */
                (xwisr_f)soc_isr_nop, /* Vector - 198 FLEXCAN4_BUF_32_63 */
                (xwisr_f)soc_isr_nop, /* Vector - 199 LINFLEX5_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 200 LINFLEX5_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 201 LINFLEX5_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 202 FLEXCAN5_ESR */
                (xwisr_f)soc_isr_nop, /* Vector - 203 FLEXCAN5_ESR_BOFF */
                (xwisr_f)soc_isr_nop, /* Vector - 204 */
                (xwisr_f)soc_isr_nop, /* Vector - 205 FLEXCAN5_BUF_00_03 */
                (xwisr_f)soc_isr_nop, /* Vector - 206 FLEXCAN5_BUF_04_07 */
                (xwisr_f)soc_isr_nop, /* Vector - 207 FLEXCAN5_BUF_08_11 */
                (xwisr_f)soc_isr_nop, /* Vector - 208 FLEXCAN5_BUF_12_15 */
                (xwisr_f)soc_isr_nop, /* Vector - 209 FLEXCAN5_BUF_16_31 */
                (xwisr_f)soc_isr_nop, /* Vector - 210 FLEXCAN5_BUF_32_63 */
                (xwisr_f)soc_isr_nop, /* Vector - 211 DSPI4_SR_TFUF_RFOF */
                (xwisr_f)soc_isr_nop, /* Vector - 212 DSPI4_SR_EOQF */
                (xwisr_f)soc_isr_nop, /* Vector - 213 DSPI4_SR_TFFF */
                (xwisr_f)soc_isr_nop, /* Vector - 214 DSPI4_SR_TCF */
                (xwisr_f)soc_isr_nop, /* Vector - 215 DSPI4_SR_RFDF */
                (xwisr_f)soc_isr_nop, /* Vector - 216 LINFLEX6_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 217 LINFLEX6_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 218 LINFLEX6_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 219 DSPI5_SR_TFUF_RFOF */
                (xwisr_f)soc_isr_nop, /* Vector - 220 DSPI5_SR_EOQF */
                (xwisr_f)soc_isr_nop, /* Vector - 221 DSPI5_SR_TFFF */
                (xwisr_f)soc_isr_nop, /* Vector - 222 DSPI5_SR_TCF */
                (xwisr_f)soc_isr_nop, /* Vector - 223 DSPI5_SR_RFDF */
                (xwisr_f)soc_isr_nop, /* Vector - 224 LINFLEX7_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 225 LINFLEX7_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 226 LINFLEX7_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 227 LINFLEX8_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 228 LINFLEX8_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 229 LINFLEX8_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 230 LINFLEX9_RXI */
                (xwisr_f)soc_isr_nop, /* Vector - 231 LINFLEX9_TXI */
                (xwisr_f)soc_isr_nop, /* Vector - 232 LINFLEX9_ERR */
                (xwisr_f)soc_isr_nop, /* Vector - 233 SXOSC */
        },
};

__soc_isr_table_qualifier struct soc_irq_data_table soc_irq_data_table = {
        .soc = {
                [IRQ_EDMA_CMBERR] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH0] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH1] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH2] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH3] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH4] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH5] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH6] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH7] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH8] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH9] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH10] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH11] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH12] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH13] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH14] = &mpc560xb_soc_cb,
                [IRQ_EDMA_CH15] = &mpc560xb_soc_cb,
                [IRQ_EXT_IRQ0] = &mpc560xb_soc_cb,
                [IRQ_EXT_IRQ1] = &mpc560xb_soc_cb,
                [IRQ_EXT_IRQ2] = &mpc560xb_soc_cb,
                [IRQ_WKUP_IRQ0] = &mpc560xb_soc_cb,
                [IRQ_WKUP_IRQ1] = &mpc560xb_soc_cb,
                [IRQ_WKUP_IRQ2] = &mpc560xb_soc_cb,
                [IRQ_WKUP_IRQ3] = &mpc560xb_soc_cb,
                [IRQ_ADC0_EOC] = &mpc560xb_misc_cb,
                [IRQ_ADC0_ER] = &mpc560xb_misc_cb,
                [IRQ_ADC0_WD] = &mpc560xb_misc_cb,
                [IRQ_I2C0_SR] = &mpc560xb_i2cm_cb,
                [IRQ_LINFLEX0_ERR] = &mpc560xb_uart0_cb,
        }
};
