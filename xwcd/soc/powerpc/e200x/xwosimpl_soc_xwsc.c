/**
 * @file
 * @brief 玄武OS移植实现层：SOC系统调用
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

#include <xwos/standard.h>
#include <e200x_core.h>
#include <arch_sc_trap.h>

/**
 * @brief Real entry to start supervisor call
 */
__xwbsp_isr
void soc_privilege_start_entry(__xwcc_unused xwreg_t arg1,
                               __xwcc_unused xwreg_t arg2,
                               __xwcc_unused xwreg_t * srr0,
                               xwreg_t * srr1)
{
        union msr_reg msr;

        msr.w = *srr1;
        msr.b.pr = 0;
        *srr1 = msr.w;
}

/**
 * @brief Real entry to end supervisor call
 */
__xwbsp_isr
void soc_privilege_end_entry(__xwcc_unused xwreg_t arg1,
                             __xwcc_unused xwreg_t arg2,
                             __xwcc_unused xwreg_t * srr0,
                             xwreg_t * srr1)
{
        union msr_reg msr;

        msr.w = *srr1;
        msr.b.pr = 1;
        *srr1 = msr.w;
}

/**
 * @brief Start supervisor call (change to privileged access temporarily)
 */
__xwbsp_code
void soc_privilege_start(void)
{
        arch_systemcall(ARCH_SC_PRIVILEGE_START,
                        XWOS_UNUSED_ARGUMENT,
                        XWOS_UNUSED_ARGUMENT);
}

/**
 * @brief End supervisor call (close the privileged access)
 */
__xwbsp_code
void soc_privilege_end(void)
{
        arch_systemcall(ARCH_SC_PRIVILEGE_END,
                        XWOS_UNUSED_ARGUMENT,
                        XWOS_UNUSED_ARGUMENT);
}
