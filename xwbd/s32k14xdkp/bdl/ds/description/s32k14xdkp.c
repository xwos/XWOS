/**
 * @file
 * @brief 设备栈描述
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <smp_nvic.h>
#include <soc_irq.h>
#include <xwmd/ds/object.h>
#include <bdl/ds/description/s32k14xdkp.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwds s32k14x_ds;

__soc_isr_table_qualifier struct soc_irq_data_table soc_irq_data_table = {
        .arch = {
                [ARCH_SP_TOP + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RESET + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_NMI + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_HARDFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_MMFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_BUSFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_USGFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN9 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN8 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN7 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN6 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_SVCALL + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_DBGMON + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN3 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_PENDSV + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_SYSTICK + ARCHCFG_IRQ_NUM] = NULL,
        },
        .soc = {
                [SOC_IRQ_DMA0] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA1] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA2] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA3] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA4] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA5] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA6] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA7] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA8] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA9] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA10] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA11] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA12] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA13] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA14] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA15] = &s32k14x_soc_cb,
                [SOC_IRQ_DMA_Error] = &s32k14x_soc_cb,
                [SOC_IRQ_MCM] = NULL,
                [SOC_IRQ_FTFC] = NULL,
                [SOC_IRQ_Read_Collision] = NULL,
                [SOC_IRQ_LVD_LVW] = NULL,
                [SOC_IRQ_FTFC_Fault] = NULL,
                [SOC_IRQ_WDOG_EWM] = NULL,
                [SOC_IRQ_RCM] = NULL,
                [SOC_IRQ_LPI2C0_Master] = NULL,
                [SOC_IRQ_LPI2C0_Slave] = NULL,
                [SOC_IRQ_LPSPI0] = NULL,
                [SOC_IRQ_LPSPI1] = NULL,
                [SOC_IRQ_LPSPI2] = NULL,
                [SOC_IRQ_LPI2C1_Master] = NULL,
                [SOC_IRQ_LPI2C1_Slave] = NULL,
                [SOC_IRQ_LPUART0_RxTx] = NULL,
                [SOC_IRQ_LPUART1_RxTx] = NULL,
                [SOC_IRQ_LPUART2_RxTx] = NULL,
                [SOC_IRQ_ADC0] = NULL,
                [SOC_IRQ_ADC1] = NULL,
                [SOC_IRQ_CMP0] = NULL,
                [SOC_IRQ_ERM_single_fault] = NULL,
                [SOC_IRQ_ERM_double_fault] = NULL,
                [SOC_IRQ_RTC] = NULL,
                [SOC_IRQ_RTC_Seconds] = NULL,
                [SOC_IRQ_LPIT0_Ch0] = NULL,
                [SOC_IRQ_LPIT0_Ch1] = NULL,
                [SOC_IRQ_LPIT0_Ch2] = NULL,
                [SOC_IRQ_LPIT0_Ch3] = NULL,
                [SOC_IRQ_PDB0] = NULL,
                [SOC_IRQ_SAI1_Tx] = NULL,
                [SOC_IRQ_SAI1_Rx] = NULL,
                [SOC_IRQ_SCG] = NULL,
                [SOC_IRQ_LPTMR0] = NULL,
                [SOC_IRQ_PORTA] = &s32k14x_soc_cb,
                [SOC_IRQ_PORTB] = &s32k14x_soc_cb,
                [SOC_IRQ_PORTC] = &s32k14x_soc_cb,
                [SOC_IRQ_PORTD] = &s32k14x_soc_cb,
                [SOC_IRQ_PORTE] = &s32k14x_soc_cb,
                [SOC_IRQ_SWI] = NULL,
                [SOC_IRQ_QSPI] = NULL,
                [SOC_IRQ_PDB1] = NULL,
                [SOC_IRQ_FLEXIO] = NULL,
                [SOC_IRQ_SAI0_Tx] = NULL,
                [SOC_IRQ_SAI0_Rx] = NULL,
                [SOC_IRQ_ENET_TIMER] = NULL,
                [SOC_IRQ_ENET_TX] = NULL,
                [SOC_IRQ_ENET_RX] = NULL,
                [SOC_IRQ_ENET_ERR] = NULL,
                [SOC_IRQ_ENET_STOP] = NULL,
                [SOC_IRQ_ENET_WAKE] = NULL,
                [SOC_IRQ_CAN0_ORed] = NULL,
                [SOC_IRQ_CAN0_Error] = NULL,
                [SOC_IRQ_CAN0_Wake_Up] = NULL,
                [SOC_IRQ_CAN0_ORed_0_15_MB] = NULL,
                [SOC_IRQ_CAN0_ORed_16_31_MB] = NULL,
                [SOC_IRQ_CAN1_ORed] = NULL,
                [SOC_IRQ_CAN1_Error] = NULL,
                [SOC_IRQ_CAN1_ORed_0_15_MB] = NULL,
                [SOC_IRQ_CAN1_ORed_16_31_MB] = NULL,
                [SOC_IRQ_CAN2_ORed] = NULL,
                [SOC_IRQ_CAN2_Error] = NULL,
                [SOC_IRQ_CAN2_ORed_0_15_MB] = NULL,
                [SOC_IRQ_CAN2_ORed_16_31_MB] = NULL,
                [SOC_IRQ_FTM0_Ch0_Ch1] = NULL,
                [SOC_IRQ_FTM0_Ch2_Ch3] = NULL,
                [SOC_IRQ_FTM0_Ch4_Ch5] = NULL,
                [SOC_IRQ_FTM0_Ch6_Ch7] = NULL,
                [SOC_IRQ_FTM0_Fault] = NULL,
                [SOC_IRQ_FTM0_Ovf_Reload] = NULL,
                [SOC_IRQ_FTM1_Ch0_Ch1] = NULL,
                [SOC_IRQ_FTM1_Ch2_Ch3] = NULL,
                [SOC_IRQ_FTM1_Ch4_Ch5] = NULL,
                [SOC_IRQ_FTM1_Ch6_Ch7] = NULL,
                [SOC_IRQ_FTM1_Fault] = NULL,
                [SOC_IRQ_FTM1_Ovf_Reload] = NULL,
                [SOC_IRQ_FTM2_Ch0_Ch1] = NULL,
                [SOC_IRQ_FTM2_Ch2_Ch3] = NULL,
                [SOC_IRQ_FTM2_Ch4_Ch5] = NULL,
                [SOC_IRQ_FTM2_Ch6_Ch7] = NULL,
                [SOC_IRQ_FTM2_Fault] = NULL,
                [SOC_IRQ_FTM2_Ovf_Reload] = NULL,
                [SOC_IRQ_FTM3_Ch0_Ch1] = NULL,
                [SOC_IRQ_FTM3_Ch2_Ch3] = NULL,
                [SOC_IRQ_FTM3_Ch4_Ch5] = NULL,
                [SOC_IRQ_FTM3_Ch6_Ch7] = NULL,
                [SOC_IRQ_FTM3_Fault] = NULL,
                [SOC_IRQ_FTM3_Ovf_Reload] = NULL,
                [SOC_IRQ_FTM4_Ch0_Ch1] = NULL,
                [SOC_IRQ_FTM4_Ch2_Ch3] = NULL,
                [SOC_IRQ_FTM4_Ch4_Ch5] = NULL,
                [SOC_IRQ_FTM4_Ch6_Ch7] = NULL,
                [SOC_IRQ_FTM4_Fault] = NULL,
                [SOC_IRQ_FTM4_Ovf_Reload] = NULL,
                [SOC_IRQ_FTM5_Ch0_Ch1] = NULL,
                [SOC_IRQ_FTM5_Ch2_Ch3] = NULL,
                [SOC_IRQ_FTM5_Ch4_Ch5] = NULL,
                [SOC_IRQ_FTM5_Ch6_Ch7] = NULL,
                [SOC_IRQ_FTM5_Fault] = NULL,
                [SOC_IRQ_FTM5_Ovf_Reload] = NULL,
                [SOC_IRQ_FTM6_Ch0_Ch1] = NULL,
                [SOC_IRQ_FTM6_Ch2_Ch3] = NULL,
                [SOC_IRQ_FTM6_Ch4_Ch5] = NULL,
                [SOC_IRQ_FTM6_Ch6_Ch7] = NULL,
                [SOC_IRQ_FTM6_Fault] = NULL,
                [SOC_IRQ_FTM6_Ovf_Reload] = NULL,
                [SOC_IRQ_FTM7_Ch0_Ch1] = NULL,
                [SOC_IRQ_FTM7_Ch2_Ch3] = NULL,
                [SOC_IRQ_FTM7_Ch4_Ch5] = NULL,
                [SOC_IRQ_FTM7_Ch6_Ch7] = NULL,
                [SOC_IRQ_FTM7_Fault] = NULL,
                [SOC_IRQ_FTM7_Ovf_Reload] = NULL,
        },
};
