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

/* high */
#define ARCH_IRQ_PRIO_HIGHEST           0x0
#define ARCH_IRQ_PRIO_HIGH              0x1
#define ARCH_IRQ_PRIO_LOW               0x2
#define ARCH_IRQ_PRIO_LOWEST            0x3
/* low */

enum arch_irq_em {
        ARCH_SP_TOP = -16, /**< initial value of stack point */
        ARCH_IRQ_RESET = -15, /**< Reset Interrupt */
        ARCH_IRQ_NMI = -14, /**< 2 Non Maskable Interrupt */
        ARCH_IRQ_HARDFAULT = -13, /**< Cortex-M Hard Fault */
        ARCH_IRQ_MMFAULT = -12, /**< Cortex-M Memory Management Fault */
        ARCH_IRQ_BUSFAULT = -11, /**< Cortex-M Bus Fault */
        ARCH_IRQ_USGFAULT = -10, /**< Cortex-M Usage Fault */
        ARCH_IRQ_RSVN9 = -9,
        ARCH_IRQ_RSVN8 = -8,
        ARCH_IRQ_RSVN7 = -7,
        ARCH_IRQ_RSVN6 = -6,
        ARCH_IRQ_SVCALL = -5, /**< Cortex-M SV Call Interrupt */
        ARCH_IRQ_DBGMON = -4, /**< Cortex-M Debug Monitor Interrupt */
        ARCH_IRQ_RSVN3 = -3,
        ARCH_IRQ_PENDSV = -2, /**< Cortex-M Pend SV Interrupt */
        ARCH_IRQ_SYSTICK = -1, /**< Cortex-M System Tick Interrupt */
};

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
xwer_t arch_irq_get_id(xwirq_t * irqnbuf);

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
