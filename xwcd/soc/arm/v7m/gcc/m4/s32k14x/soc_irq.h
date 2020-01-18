/**
 * @file
 * @brief SOC Adapter Code：IRQ
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

#ifndef __soc_irq_h__
#define __soc_irq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <arch_irq.h>
#include <soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum soc_irq_em {
        SOC_IRQ_DMA0 = 0U, /**< DMA channel 0 transfer complete */
        SOC_IRQ_DMA1 = 1U, /**< DMA channel 1 transfer complete */
        SOC_IRQ_DMA2 = 2U, /**< DMA channel 2 transfer complete */
        SOC_IRQ_DMA3 = 3U, /**< DMA channel 3 transfer complete */
        SOC_IRQ_DMA4 = 4U, /**< DMA channel 4 transfer complete */
        SOC_IRQ_DMA5 = 5U, /**< DMA channel 5 transfer complete */
        SOC_IRQ_DMA6 = 6U, /**< DMA channel 6 transfer complete */
        SOC_IRQ_DMA7 = 7U, /**< DMA channel 7 transfer complete */
        SOC_IRQ_DMA8 = 8U, /**< DMA channel 8 transfer complete */
        SOC_IRQ_DMA9 = 9U, /**< DMA channel 9 transfer complete */
        SOC_IRQ_DMA10 = 10U, /**< DMA channel 10 transfer complete */
        SOC_IRQ_DMA11 = 11U, /**< DMA channel 11 transfer complete */
        SOC_IRQ_DMA12 = 12U, /**< DMA channel 12 transfer complete */
        SOC_IRQ_DMA13 = 13U, /**< DMA channel 13 transfer complete */
        SOC_IRQ_DMA14 = 14U, /**< DMA channel 14 transfer complete */
        SOC_IRQ_DMA15 = 15U, /**< DMA channel 15 transfer complete */
        SOC_IRQ_DMA_Error = 16U, /**< DMA error interrupt channels 0-15 */
        SOC_IRQ_MCM = 17U, /**< FPU sources */
        SOC_IRQ_FTFC = 18U, /**< FTFC Command complete */
        SOC_IRQ_Read_Collision = 19U, /**< FTFC Read collision */
        SOC_IRQ_LVD_LVW = 20U, /**< PMC Low voltage detect interrupt */
        SOC_IRQ_FTFC_Fault = 21U, /**< FTFC Double bit fault detect */
        SOC_IRQ_WDOG_EWM = 22U, /**< Single interrupt vector for WDOG and EWM */
        SOC_IRQ_RCM = 23U, /**< RCM Asynchronous Interrupt */
        SOC_IRQ_LPI2C0_Master = 24U, /**< LPI2C0 Master Interrupt */
        SOC_IRQ_LPI2C0_Slave = 25U, /**< LPI2C0 Slave Interrupt */
        SOC_IRQ_LPSPI0 = 26U, /**< LPSPI0 Interrupt */
        SOC_IRQ_LPSPI1 = 27U, /**< LPSPI1 Interrupt */
        SOC_IRQ_LPSPI2 = 28U, /**< LPSPI2 Interrupt */
        SOC_IRQ_LPI2C1_Master = 29U, /**< LPI2C1 Master Interrupt */
        SOC_IRQ_LPI2C1_Slave = 30U, /**< LPI2C1 Slave Interrupt */
        SOC_IRQ_LPUART0_RxTx = 31U, /**< LPUART0 Transmit / Receive Interrupt */
        SOC_IRQ_LPUART1_RxTx = 33U, /**< LPUART1 Transmit / Receive  Interrupt */
        SOC_IRQ_LPUART2_RxTx = 35U, /**< LPUART2 Transmit / Receive  Interrupt */
        SOC_IRQ_ADC0 = 39U, /**< ADC0 interrupt request. */
        SOC_IRQ_ADC1 = 40U, /**< ADC1 interrupt request. */
        SOC_IRQ_CMP0 = 41U, /**< CMP0 interrupt request */
        SOC_IRQ_ERM_single_fault = 44U, /**< ERM single bit error correction */
        SOC_IRQ_ERM_double_fault = 45U, /**< ERM double bit error non-correctable */
        SOC_IRQ_RTC = 46U, /**< RTC alarm interrupt */
        SOC_IRQ_RTC_Seconds = 47U, /**< RTC seconds interrupt */
        SOC_IRQ_LPIT0_Ch0 = 48U, /**< LPIT0 channel 0 overflow interrupt */
        SOC_IRQ_LPIT0_Ch1 = 49U, /**< LPIT0 channel 1 overflow interrupt */
        SOC_IRQ_LPIT0_Ch2 = 50U, /**< LPIT0 channel 2 overflow interrupt */
        SOC_IRQ_LPIT0_Ch3 = 51U, /**< LPIT0 channel 3 overflow interrupt */
        SOC_IRQ_PDB0 = 52U, /**< PDB0 interrupt */
        SOC_IRQ_SAI1_Tx = 55U, /**< SAI1 Transmit Synchronous interrupt (for interrupt controller) */
        SOC_IRQ_SAI1_Rx = 56U, /**< SAI1 Receive Synchronous interrupt (for interrupt controller) */
        SOC_IRQ_SCG = 57U, /**< SCG bus interrupt request */
        SOC_IRQ_LPTMR0 = 58U, /**< LPTIMER interrupt request */
        SOC_IRQ_PORTA = 59U, /**< Port A pin detect interrupt */
        SOC_IRQ_PORTB = 60U, /**< Port B pin detect interrupt */
        SOC_IRQ_PORTC = 61U, /**< Port C pin detect interrupt */
        SOC_IRQ_PORTD = 62U, /**< Port D pin detect interrupt */
        SOC_IRQ_PORTE = 63U, /**< Port E pin detect interrupt */
        SOC_IRQ_SWI = 64U, /**< Software interrupt */
        SOC_IRQ_QSPI = 65U, /**< QSPI All interrupts ORed output */
        SOC_IRQ_PDB1 = 68U, /**< PDB1 interrupt */
        SOC_IRQ_FLEXIO = 69U, /**< FlexIO Interrupt */
        SOC_IRQ_SAI0_Tx = 70U, /**< SAI0 Transmit Synchronous interrupt (for interrupt controller) */
        SOC_IRQ_SAI0_Rx = 71U, /**< SAI0 Receive Synchronous interrupt (for interrupt controller) */
        SOC_IRQ_ENET_TIMER = 72U, /**< ENET 1588 Timer Interrupt - synchronous */
        SOC_IRQ_ENET_TX = 73U, /**< ENET Data transfer done */
        SOC_IRQ_ENET_RX = 74U, /**< ENET Receive Buffer Done for Ring/Queue 0 */
        SOC_IRQ_ENET_ERR = 75U, /**< ENET Payload receive error. */
        SOC_IRQ_ENET_STOP = 76U, /**< ENET Graceful stop */
        SOC_IRQ_ENET_WAKE = 77U, /**< ENET Wake from sleep. */
        SOC_IRQ_CAN0_ORed = 78U, /**< CAN0 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
        SOC_IRQ_CAN0_Error = 79U, /**< CAN0 Interrupt indicating that errors were detected on the CAN bus */
        SOC_IRQ_CAN0_Wake_Up = 80U,              /**< CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode */
        SOC_IRQ_CAN0_ORed_0_15_MB = 81U, /**< CAN0 OR'ed Message buffer (0-15) */
        SOC_IRQ_CAN0_ORed_16_31_MB = 82U, /**< CAN0 OR'ed Message buffer (16-31) */
        SOC_IRQ_CAN1_ORed = 85U, /**< CAN1 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
        SOC_IRQ_CAN1_Error = 86U, /**< CAN1 Interrupt indicating that errors were detected on the CAN bus */
        SOC_IRQ_CAN1_ORed_0_15_MB = 88U, /**< CAN1 OR'ed Interrupt for Message buffer (0-15) */
        SOC_IRQ_CAN1_ORed_16_31_MB = 89U, /**< CAN1 OR'ed Interrupt for Message buffer (16-31) */
        SOC_IRQ_CAN2_ORed = 92U, /**< CAN2 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
        SOC_IRQ_CAN2_Error = 93U, /**< CAN2 Interrupt indicating that errors were detected on the CAN bus */
        SOC_IRQ_CAN2_ORed_0_15_MB = 95U, /**< CAN2 OR'ed Message buffer (0-15) */
        SOC_IRQ_CAN2_ORed_16_31_MB = 96U, /**< CAN2 OR'ed Message buffer (16-31) */
        SOC_IRQ_FTM0_Ch0_Ch1 = 99U, /**< FTM0 Channel 0 and 1 interrupt */
        SOC_IRQ_FTM0_Ch2_Ch3 = 100U, /**< FTM0 Channel 2 and 3 interrupt */
        SOC_IRQ_FTM0_Ch4_Ch5 = 101U, /**< FTM0 Channel 4 and 5 interrupt */
        SOC_IRQ_FTM0_Ch6_Ch7 = 102U, /**< FTM0 Channel 6 and 7 interrupt */
        SOC_IRQ_FTM0_Fault = 103U, /**< FTM0 Fault interrupt */
        SOC_IRQ_FTM0_Ovf_Reload = 104U, /**< FTM0 Counter overflow and Reload interrupt */
        SOC_IRQ_FTM1_Ch0_Ch1 = 105U, /**< FTM1 Channel 0 and 1 interrupt */
        SOC_IRQ_FTM1_Ch2_Ch3 = 106U, /**< FTM1 Channel 2 and 3 interrupt */
        SOC_IRQ_FTM1_Ch4_Ch5 = 107U, /**< FTM1 Channel 4 and 5 interrupt */
        SOC_IRQ_FTM1_Ch6_Ch7 = 108U, /**< FTM1 Channel 6 and 7 interrupt */
        SOC_IRQ_FTM1_Fault = 109U, /**< FTM1 Fault interrupt */
        SOC_IRQ_FTM1_Ovf_Reload = 110U, /**< FTM1 Counter overflow and Reload interrupt */
        SOC_IRQ_FTM2_Ch0_Ch1 = 111U, /**< FTM2 Channel 0 and 1 interrupt */
        SOC_IRQ_FTM2_Ch2_Ch3 = 112U, /**< FTM2 Channel 2 and 3 interrupt */
        SOC_IRQ_FTM2_Ch4_Ch5 = 113U, /**< FTM2 Channel 4 and 5 interrupt */
        SOC_IRQ_FTM2_Ch6_Ch7 = 114U, /**< FTM2 Channel 6 and 7 interrupt */
        SOC_IRQ_FTM2_Fault = 115U, /**< FTM2 Fault interrupt */
        SOC_IRQ_FTM2_Ovf_Reload = 116U, /**< FTM2 Counter overflow and Reload interrupt */
        SOC_IRQ_FTM3_Ch0_Ch1 = 117U, /**< FTM3 Channel 0 and 1 interrupt */
        SOC_IRQ_FTM3_Ch2_Ch3 = 118U, /**< FTM3 Channel 2 and 3 interrupt */
        SOC_IRQ_FTM3_Ch4_Ch5 = 119U, /**< FTM3 Channel 4 and 5 interrupt */
        SOC_IRQ_FTM3_Ch6_Ch7 = 120U, /**< FTM3 Channel 6 and 7 interrupt */
        SOC_IRQ_FTM3_Fault = 121U, /**< FTM3 Fault interrupt */
        SOC_IRQ_FTM3_Ovf_Reload = 122U, /**< FTM3 Counter overflow and Reload interrupt */
        SOC_IRQ_FTM4_Ch0_Ch1 = 123U, /**< FTM4 Channel 0 and 1 interrupt */
        SOC_IRQ_FTM4_Ch2_Ch3 = 124U, /**< FTM4 Channel 2 and 3 interrupt */
        SOC_IRQ_FTM4_Ch4_Ch5 = 125U, /**< FTM4 Channel 4 and 5 interrupt */
        SOC_IRQ_FTM4_Ch6_Ch7 = 126U, /**< FTM4 Channel 6 and 7 interrupt */
        SOC_IRQ_FTM4_Fault = 127U, /**< FTM4 Fault interrupt */
        SOC_IRQ_FTM4_Ovf_Reload = 128U, /**< FTM4 Counter overflow and Reload interrupt */
        SOC_IRQ_FTM5_Ch0_Ch1 = 129U, /**< FTM5 Channel 0 and 1 interrupt */
        SOC_IRQ_FTM5_Ch2_Ch3 = 130U, /**< FTM5 Channel 2 and 3 interrupt */
        SOC_IRQ_FTM5_Ch4_Ch5 = 131U, /**< FTM5 Channel 4 and 5 interrupt */
        SOC_IRQ_FTM5_Ch6_Ch7 = 132U, /**< FTM5 Channel 6 and 7 interrupt */
        SOC_IRQ_FTM5_Fault = 133U, /**< FTM5 Fault interrupt */
        SOC_IRQ_FTM5_Ovf_Reload = 134U, /**< FTM5 Counter overflow and Reload interrupt */
        SOC_IRQ_FTM6_Ch0_Ch1 = 135U, /**< FTM6 Channel 0 and 1 interrupt */
        SOC_IRQ_FTM6_Ch2_Ch3 = 136U, /**< FTM6 Channel 2 and 3 interrupt */
        SOC_IRQ_FTM6_Ch4_Ch5 = 137U, /**< FTM6 Channel 4 and 5 interrupt */
        SOC_IRQ_FTM6_Ch6_Ch7 = 138U, /**< FTM6 Channel 6 and 7 interrupt */
        SOC_IRQ_FTM6_Fault = 139U, /**< FTM6 Fault interrupt */
        SOC_IRQ_FTM6_Ovf_Reload = 140U, /**< FTM6 Counter overflow and Reload interrupt */
        SOC_IRQ_FTM7_Ch0_Ch1 = 141U, /**< FTM7 Channel 0 and 1 interrupt */
        SOC_IRQ_FTM7_Ch2_Ch3 = 142U, /**< FTM7 Channel 2 and 3 interrupt */
        SOC_IRQ_FTM7_Ch4_Ch5 = 143U, /**< FTM7 Channel 4 and 5 interrupt */
        SOC_IRQ_FTM7_Ch6_Ch7 = 144U, /**< FTM7 Channel 6 and 7 interrupt */
        SOC_IRQ_FTM7_Fault = 145U, /**< FTM7 Fault interrupt */
        SOC_IRQ_FTM7_Ovf_Reload = 146U, /**< FTM7 Counter overflow and Reload interrupt */
        SOC_IRQ_NUM,
};

/**
 * @brief Configurations of SOC IRQ
 */
struct soc_irq_cfg {
        struct cortexm_nvic_irq_cfg irqcfg;
};

/**
 * @brief Data of SOC IRQ
 */
struct soc_irq_data {
        void * data;
};

/**
 * @brief SOC ISR Table
 */
struct soc_isr_table {
        xwisr_f arch[ARCHCFG_IRQ_NUM];
        xwisr_f soc[SOCCFG_IRQ_NUM];
};

/**
 * @brief SOC ISR Data Table
 */
struct soc_irq_data_table {
        void * arch[ARCHCFG_IRQ_NUM];
        void * soc[SOCCFG_IRQ_NUM];
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      XWOS IRQ Adapter Functions     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SOC Adapter Function：Get Current IRQ Number
 */
static __xwbsp_inline
xwer_t soc_irq_get_id(xwirq_t * irqnbuf)
{
        return arch_irq_get_id(irqnbuf);
}

/**
 * @brief SOC Adaptation Function：Enable local CPU IRQ
 */
static __xwbsp_inline
void soc_cpuirq_enable_lc(void)
{
        arch_cpuirq_enable_lc();
}

/**
 * @brief SOC Adaptation Function：Disable local CPU IRQ
 */
static __xwbsp_inline
void soc_cpuirq_disable_lc(void)
{
        arch_cpuirq_disable_lc();
}

/**
 * @brief SOC Adaptation Function：Restore local CPU IRQ flag
 */
static __xwbsp_inline
void soc_cpuirq_restore_lc(xwreg_t cpuirq)
{
        arch_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief SOC Adaptation Function：Save local CPU IRQ flag and disable local IRQ
 */
static __xwbsp_inline
void soc_cpuirq_save_lc(xwreg_t * cpuirq)
{
        arch_cpuirq_save_lc(cpuirq);
}

#endif /* soc_irq.h */
