/**
 * @file
 * @brief 玄武OS内核适配代码：中断
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
 * @brief 玄武OS内核中断适配函数：获取当前中断上下文的中断号
 */
static __xwbsp_inline
xwer_t soc_irq_get_id(xwirq_t * irqnbuf)
{
        return arch_irq_get_id(irqnbuf);
}

/**
 * @brief 玄武OS内核中断适配函数：开启本地CPU中断
 */
static __xwbsp_inline
void soc_cpuirq_enable_lc(void)
{
        arch_cpuirq_enable_lc();
}

/**
 * @brief 玄武OS内核中断适配函数：关闭本地CPU中断
 */
static __xwbsp_inline
void soc_cpuirq_disable_lc(void)
{
        arch_cpuirq_disable_lc();
}

/**
 * @brief 玄武OS内核中断适配函数：恢复本地CPU中断
 */
static __xwbsp_inline
void soc_cpuirq_restore_lc(xwreg_t cpuirq)
{
        arch_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief 玄武OS内核中断适配函数：保存并关闭本地CPU中断
 */
static __xwbsp_inline
void soc_cpuirq_save_lc(xwreg_t * cpuirq)
{
        arch_cpuirq_save_lc(cpuirq);
}

#endif /* soc_irq.h */
