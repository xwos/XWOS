/**
 * @file
 * @brief 架构描述层：ARMv8A的架构指令
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#include <xwcd/soc/arm64/v8a/arch_isa.h>

/**
 * @brief ARMv8A从EL3回到EL2.AArch64
 */
__xwbsp_code
void armv8a_switch_el3_to_el2_aarch64(void)
{
        __asm__ volatile(
        "       adr     x0, 1f\n"
        "       msr     elr_el3, x0\n"
        "       mov     x0, sp\n"
        "       msr     sp_el2, x0\n"
        "       msr     spsr_el3, %[__spsr]\n"
        "       isb\n"
        "       eret\n"
        "1:\n"
        :
        : [__spsr] "r" (((xwu64_t)0xF << (xwu64_t)6) /* DAIF */ |
                        ((xwu64_t)0x9 << (xwu64_t)0) /* EL2h */ )
        : "x0");
}

__xwbsp_code
void armv8a_switch_el2_to_el1_aarch64(void)
{
        __asm__ volatile(
        "       adr     x0, 1f\n"
        "       msr     elr_el2, x0\n"
        "       mov     x0, sp\n"
        "       msr     sp_el1, x0\n"
        "       msr     spsr_el2, %[__spsr]\n"
        "       isb\n"
        "       eret\n"
        "1:\n"
        :
        : [__spsr] "r" (((xwu64_t)0xF << (xwu64_t)6) /* DAIF */ |
                        ((xwu64_t)0x5 << (xwu64_t)0) /* EL1h */ )
        : "x0");
}
