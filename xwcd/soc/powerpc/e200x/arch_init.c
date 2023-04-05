/**
 * @file
 * @brief 架构描述层：初始化
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

#include <xwos/standard.h>
#include <e200x_core.h>
#include <arch_image_flag.h>
#include <arch_init.h>

extern __xwos_isr
void arch_isr_reset(void);

extern __xwbsp_init_code
void arch_ivpr_init(void);

static __xwbsp_init_code
void arch_spr_init(void);

extern const xwu8_t image_tail_lma_base[];
extern const xwu8_t image_tail_lma_end[];

__image_header const struct arch_image_header arch_image_header = {
        .tailflag_addr = (void *)image_tail_lma_base,
        .end_addr = (void *)image_tail_lma_end,
        .prolog = NULL,
        .entry = arch_isr_reset,
        .epilog = NULL,
};

__image_tail const struct arch_image_tail arch_image_tail = {
        .flag1 = ARCHCFG_IMG_TAILFLAG1,
        .flag2 = ARCHCFG_IMG_TAILFLAG2,
        .aligned_bytes = {
                0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
        },
};

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
