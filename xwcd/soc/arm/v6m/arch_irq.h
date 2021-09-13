/**
 * @file
 * @brief 架构描述层：中断
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

#ifndef __arch_irq_h__
#define __arch_irq_h__

#include <xwos/standard.h>
#include <armv6m_nvic.h>

void arch_init_sysirqs(void);
void arch_isr_reset(void);
void arch_isr_nmi(void);
void arch_isr_hardfault(void);
void arch_isr_mm(void);
void arch_isr_busfault(void);
void arch_isr_usagefault(void);
void arch_isr_dbgmon(void);
void arch_isr_svc(void);
void arch_isr_noop(void);

/**
 * @brief Enable local CPU IRQ
 */
static __xwbsp_inline
void arch_cpuirq_enable_lc(void)
{
        cm_nvic_enable_interrupts();
}

/**
 * @brief Disable local CPU IRQ
 */
static __xwbsp_inline
void arch_cpuirq_disable_lc(void)
{
        cm_nvic_disable_interrupts();
}

/**
 * @brief Restore local CPU IRQ flag
 */
static __xwbsp_inline
void arch_cpuirq_restore_lc(xwreg_t cpuirq)
{
        cm_set_primask(cpuirq);
}

/**
 * @brief Save local CPU IRQ flag and disable local IRQ
 */
static __xwbsp_inline
void arch_cpuirq_save_lc(xwreg_t * cpuirq)
{
        cm_get_primask(cpuirq);
        cm_nvic_disable_interrupts();
}

#endif /* arch_irq.h */
