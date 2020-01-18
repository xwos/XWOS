/**
 * @file
 * @brief SOC Adapter Code: IRQ
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
        IRQ_RESERVED16 = 0, /**< Reserved interrupt 16 */
        IRQ_RESERVED17 = 1, /**< Reserved interrupt 17 */
        IRQ_RESERVED18 = 2, /**< Reserved interrupt 18 */
        IRQ_RESERVED19 = 3, /**< Reserved interrupt 19 */
        IRQ_RESERVED20 = 4, /**< Reserved interrupt 20 */
        IRQ_FTMRE = 5, /**< FTMRE command complete/read collision interrupt */
        IRQ_LVD_LVW = 6, /**< Low Voltage Detect, Low Voltage Warning */
        IRQ_IRQ = 7, /**< External interrupt */
        IRQ_I2C0 = 8, /**< I2C0 interrupt */
        IRQ_I2C1 = 9, /**< I2C1 interrupt */
        IRQ_SPI0 = 10, /**< SPI0 interrupt */
        IRQ_SPI1 = 11, /**< SPI1 interrupt */
        IRQ_UART0 = 12, /**< UART0 status/error interrupt */
        IRQ_UART1 = 13, /**< UART1 status/error interrupt */
        IRQ_UART2 = 14, /**< UART2 status/error interrupt */
        IRQ_ADC0 = 15, /**< ADC0 interrupt */
        IRQ_ACMP0 = 16, /**< ACMP0 interrupt */
        IRQ_FTM0 = 17, /**< FTM0 Single interrupt vector for all sources */
        IRQ_FTM1 = 18, /**< FTM1 Single interrupt vector for all sources */
        IRQ_FTM2 = 19, /**< FTM2 Single interrupt vector for all sources */
        IRQ_RTC = 20, /**< RTC overflow */
        IRQ_ACMP1 = 21, /**< ACMP1 interrupt */
        IRQ_PIT_CH0 = 22, /**< PIT CH0 overflow */
        IRQ_PIT_CH1_IRQn = 23, /**< PIT CH1 overflow */
        IRQ_KBI0 = 24, /**< Keyboard interrupt 0 */
        IRQ_KBI1 = 25, /**< Keyboard interrupt 1 */
        IRQ_Reserved42 = 26, /**< Reserved interrupt 42 */
        IRQ_ICS = 27, /**< ICS interrupt */
        IRQ_Watchdog = 28, /**< WDOG Interrupt */
        IRQ_PWT = 29, /**< Pulse Width Timer Interrupt */
        IRQ_MSCAN_RX = 30, /**< MSCAN Rx Interrupt */
        IRQ_MSCAN_TX = 31, /**< MSCAN Tx, Err and Wake-up interrupt */
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
 ********               XWOS IRQ Adapter Functions              ********
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
 * @brief SOC Adapter Function：Enable local CPU IRQ
 */
static __xwbsp_inline
void soc_cpuirq_enable_lc(void)
{
        arch_cpuirq_enable_lc();
}

/**
 * @brief SOC Adapter Function：Disable local CPU IRQ
 */
static __xwbsp_inline
void soc_cpuirq_disable_lc(void)
{
        arch_cpuirq_disable_lc();
}

/**
 * @brief SOC Adapter Function：Restore local CPU IRQ flag
 */
static __xwbsp_inline
void soc_cpuirq_restore_lc(xwreg_t cpuirq)
{
        arch_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief SOC Adapter Function：Save local CPU IRQ flag and disable local IRQ
 */
static __xwbsp_inline
void soc_cpuirq_save_lc(xwreg_t * cpuirq)
{
        arch_cpuirq_save_lc(cpuirq);
}

#endif /* soc_irq.h */
