/**
 * @file
 * @brief XWOS移植实现层：中断
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

#ifndef __xwosimpl_irq_h__
#define __xwosimpl_irq_h__

#ifndef __xwos_ospl_irq_h__
#  error "This file should be included from <xwos/ospl/irq.h>."
#endif

#include <xwcd/soc/arm/v7m/arch_irq.h>

static __xwbsp_inline
void xwospl_cpuirq_enable_lc(void)
{
        arch_cpuirq_enable_lc();
}

static __xwbsp_inline
void xwospl_cpuirq_disable_lc(void)
{
        arch_cpuirq_disable_lc();
}

static __xwbsp_inline
void xwospl_cpuirq_restore_lc(xwreg_t cpuirq)
{
        arch_cpuirq_restore_lc(cpuirq);
}

static __xwbsp_inline
void xwospl_cpuirq_save_lc(xwreg_t * cpuirq)
{
        arch_cpuirq_save_lc(cpuirq);
}

static __xwbsp_inline
bool xwospl_cpuirq_test_lc(void)
{
        return arch_cpuirq_test_lc();
}

#endif /* xwcd/soc/arm/v7m/m3/ch32/xwosimpl_irq.h */
