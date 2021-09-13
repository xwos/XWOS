/**
 * @file
 * @brief 架构描述层：FPU
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
#include <armv7m_isa.h>
#include <arch_fpu.h>

/**
 * @brief Init architecture fpu
 */
__xwbsp_init_code
void arch_fpu_init(void)
{
        /* enable access of CP10 & CP11 */
        cm_scs.scb.cpacr.bit.cp10 = 3;
        cm_scs.scb.cpacr.bit.cp11 = 3;

        /* Hardware automatically preserves FP context on exception entry and restores
           it on exception return */
        cm_scs.scb.fpu.fpccr.bit.aspen = 1;
        /* cm_scs.scb.fpu.fpccr.bit.lspen = 1; */

        /* FPU can pend fault */
        cm_scs.scb.fpu.fpccr.bit.monrdy = 1;
        cm_scs.scb.fpu.fpccr.bit.bfrdy = 1;
        cm_scs.scb.fpu.fpccr.bit.mmrdy = 1;
        cm_scs.scb.fpu.fpccr.bit.hfrdy = 1;
}
