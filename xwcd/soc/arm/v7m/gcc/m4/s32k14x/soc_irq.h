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
