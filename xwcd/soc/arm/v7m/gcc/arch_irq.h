/**
 * @file
 * @brief Architecture IRQs
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <armv7m_nvic.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/* high */
#define ARCH_IRQ_PRIO_7         0x0
#define ARCH_IRQ_PRIO_6         0x2
#define ARCH_IRQ_PRIO_5         0x4
#define ARCH_IRQ_PRIO_4         0x6
#define ARCH_IRQ_PRIO_3         0x8
#define ARCH_IRQ_PRIO_2         0xA
#define ARCH_IRQ_PRIO_1         0xC
#define ARCH_IRQ_PRIO_0         0xE
/* low */

#define ARCH_IRQ_SUBPRIO_HIGH   0x0
#define ARCH_IRQ_SUBPRIO_LOW    0x1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_init_code
void arch_init_sysirqs(void);

__xwbsp_isr
void arch_isr_reset(void);

__xwbsp_isr
void arch_isr_nmi(void);

__xwbsp_isr
void arch_isr_hardfault(void);

__xwbsp_isr
void arch_isr_mm(void);

__xwbsp_isr
void arch_isr_busfault(void);

__xwbsp_isr
void arch_isr_usagefault(void);

__xwbsp_isr
void arch_isr_dbgmon(void);

__xwbsp_isr
void arch_isr_svc(void);

__xwbsp_isr
void arch_isr_nop(void);

__xwbsp_code
xwer_t arch_irq_get_id(xwirq_t * irqnbuf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
