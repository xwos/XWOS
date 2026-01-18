/**
 * @file
 * @brief 架构描述层：初始化
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

#include <xwcd/soc/powerpc/e200x/arch_init.h>
#include <xwcd/soc/powerpc/e200x/e200x_isa.h>
#include <xwcd/soc/powerpc/e200x/arch_firmware.h>

extern __xwbsp_init_code
void arch_ivpr_init(void);

static __xwbsp_init_code
void arch_spr_init(void);

/**
 * @brief init special registers of arch
 */
static __xwbsp_init_code
void arch_spr_init(void)
{
        xwu32_t val;

        val = 0x00001000;
        /* Machine State Register */
        /* Execution of SPE APU vector instructions : Disabled */
        /* Critical Interrupt : Disabled */
        /* Wait State : Disabled */
        /* Load lower 16 bits of register value */
        /* External Interrupt : Disabled */
        /* Problem State : Disabled */
        /* FP available : Disabled */
        /* Machine Check : Enabled */
        /* Debug Interrupt : Disabled */
        /* Instruction Address Space : Disabled */
        /* Data Address Space : Disabled */
        /* Recoverable Interrupt : Disabled */
        asm volatile(
        "       mtmsr          %[__val]\n"
        :
        : [__val] "r" (val)
        : "memory"
        );

        val = 0x00000000;
        /* HID0 Setup */
        /* Debug Interrupt Clears (DCLREE) : Disabled */
        /* Debug Interrupt Clears (DCLRCE) : Disabled */
        /* Critical Interrupt Clears : Disabled */
        /* Machine Check Interrupt Clears : Disabled */
        /* Debug APU : Disabled */
        /* Interrupt Inputs Clear Reservation : Disabled */
        /* Machine Check Exception : Disabled */
        asm volatile(
        "       mtspr          1008, %[__val]\n"
        :
        : [__val] "r" (val)
        : "memory"
        );
}

/**
 * @brief lowlevel init architecture
 */
__xwbsp_init_code
void arch_init(void)
{
        arch_spr_init();
        arch_ivpr_init();
}
