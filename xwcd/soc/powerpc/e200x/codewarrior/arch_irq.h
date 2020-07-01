/**
 * @file
 * @brief interrupt controller
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
#include <e200x_core.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define ARCH_IRQ_CRITICAL_INPUT                 (-9)
#define ARCH_IRQ_MACHINE_CHECK                  (-8)
#define ARCH_IRQ_DATA_STORAGE                   (-7)
#define ARCH_IRQ_INSTRUCTION_STORAGE            (-6)
#define ARCH_IRQ_EXTERNAL_INPUT                 (-5)
#define ARCH_IRQ_ALIGNMENT                      (-4)
#define ARCH_IRQ_PROGRAM                        (-3)
#define ARCH_IRQ_SYSTEM_CALL                    (-2)
#define ARCH_IRQ_DEBUG                          (-1)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  macro functions  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define arch_cpuirq_enable_lc()         asm volatile("wrteei 1")

#define arch_cpuirq_disable_lc()        asm volatile("wrteei 0")

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ******** inline functions  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_inline
void arch_cpuirq_save_lc(xwreg_t * cpuirq)
{
        union msr_reg msr;

        asm volatile(
        "       mfmsr           %[__msr]"
        : [__msr] "=&r" (msr.w)
        :
        : "memory"
        );

        *cpuirq = msr.b.ee;
        asm volatile("wrteei    0");
}

static __xwbsp_inline
void arch_cpuirq_restore_lc(xwreg_t cpuirq)
{
        union msr_reg msr;

        asm volatile(
        "       mfmsr           %[__msr]"
        : [__msr] "=&r" (msr.w)
        :
        : "memory"
        );

        msr.b.ee = cpuirq;
        asm volatile(
        "       mtmsr           %[__msr]"
        :
        : [__msr] "r" (msr.w)
        : "memory"
        );
}

#endif  /* arch_irq.h */
