/**
 * @file
 * @brief 架构描述层：中断
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#ifndef __xwcd_soc_powerpc_e200x_arch_irq_h__
#define __xwcd_soc_powerpc_e200x_arch_irq_h__

#include <xwos/standard.h>
#include <xwcd/soc/powerpc/e200x/e200x_isa.h>

#define SOC_EXC_CRITICAL_INPUT                  (-9)
#define SOC_EXC_MACHINE_CHECK                   (-8)
#define SOC_EXC_DATA_STORAGE                    (-7)
#define SOC_EXC_INSTRUCTION_STORAGE             (-6)
#define SOC_EXC_EXTERNAL_INPUT                  (-5)
#define SOC_EXC_ALIGNMENT                       (-4)
#define SOC_EXC_PROGRAM                         (-3)
#define SOC_EXC_SYSTEM_CALL                     (-2)
#define SOC_EXC_DEBUG                           (-1)

#define arch_cpuirq_enable_lc()         __asm__ volatile("wrteei 1")
#define arch_cpuirq_disable_lc()        __asm__ volatile("wrteei 0")

static __xwbsp_inline
void arch_cpuirq_save_lc(xwreg_t * cpuirq)
{
        union msr_reg msr;

        __asm__ volatile(
        "       mfmsr           %[__msr]"
        : [__msr] "=&r" (msr.w)
        :
        : "memory"
        );

        *cpuirq = msr.b.ee;
        __asm__ volatile("wrteei    0");
}

static __xwbsp_inline
void arch_cpuirq_restore_lc(xwreg_t cpuirq)
{
        union msr_reg msr;

        __asm__ volatile(
        "       mfmsr           %[__msr]"
        : [__msr] "=&r" (msr.w)
        :
        : "memory"
        );

        msr.b.ee = cpuirq;
        __asm__ volatile(
        "       mtmsr           %[__msr]"
        :
        : [__msr] "r" (msr.w)
        : "memory"
        );
}

static __xwbsp_inline
bool arch_cpuirq_test_lc(void)
{
        union msr_reg msr;

        __asm__ volatile(
        "       mfmsr           %[__msr]"
        : [__msr] "=&r" (msr.w)
        :
        : "memory"
        );
        return !!(msr.b.ee);
}

#endif  /* xwcd/soc/powerpc/e200x/arch_irq.h */
