/**
 * @file
 * @brief 架构描述层：初始化
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
#include <arch_image_flag.h>
#include <arch_init.h>

extern __xwos_isr
void arch_isr_reset(void);

extern __xwbsp_init_code
void arch_ivpr_init(void);

static __xwbsp_init_code
void arch_spr_init(void);

extern xwu8_t data_lma_base[];
extern xwu8_t data_vma_base[];
extern xwu8_t data_vma_end[];

extern xwu8_t sdata_lma_base[];
extern xwu8_t sdata_vma_base[];
extern xwu8_t sdata_vma_end[];

extern xwu8_t sbss_vma_base[];
extern xwu8_t sbss_vma_end[];

extern xwu8_t sdata2_lma_base[];
extern xwu8_t sdata2_vma_base[];
extern xwu8_t sdata2_vma_end[];

extern xwu8_t sbss2_vma_base[];
extern xwu8_t sbss2_vma_end[];

extern xwu8_t bss_vma_base[];
extern xwu8_t bss_vma_end[];

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
void arch_lowlevel_init(void)
{
        arch_spr_init();
        arch_ivpr_init();
}

/**
 * @brief init architecture
 */
__xwbsp_init_code
void arch_init(void)
{
}

/**
 * @brief relocate data to RAM
 */
__xwbsp_init_code
void arch_relocate(void)
{
        xwsz_t count, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = data_lma_base;
        if (data_vma_base != src) {
                dst = data_vma_base;
                count = (xwsz_t)data_vma_end - (xwsz_t)data_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        src = sdata_lma_base;
        if (sdata_vma_base != src) {
                dst = sdata_vma_base;
                count = (xwsz_t)sdata_vma_end - (xwsz_t)sdata_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        src = sdata2_lma_base;
        if (sdata2_vma_base != src) {
                dst = sdata2_vma_base;
                count = (xwsz_t)sdata2_vma_end - (xwsz_t)sdata2_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = sbss_vma_base;
        count = (xwsz_t)sbss_vma_end - (xwsz_t)sbss_vma_base;
        for (i = 0; i < count; i++) {
                *dst = 0;
                dst++;
        }

        dst = sbss2_vma_base;
        count = (xwsz_t)sbss2_vma_end - (xwsz_t)sbss2_vma_base;
        for (i = 0; i < count; i++) {
                *dst = 0;
                dst++;
        }

        dst = bss_vma_base;
        count = (xwsz_t)bss_vma_end - (xwsz_t)bss_vma_base;
        for (i = 0; i < count; i++) {
                *dst = 0;
                dst++;
        }
}
