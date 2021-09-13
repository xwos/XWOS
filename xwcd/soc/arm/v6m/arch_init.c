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
#include <armv6m_core.h>
#include <arch_irq.h>
#include <arch_image.h>
#include <arch_nvic.h>
#include <arch_init.h>

extern xwu8_t data_lma_base[];
extern xwu8_t data_vma_base[];
extern xwu8_t data_vma_end[];

extern xwu8_t bss_vma_base[];
extern xwu8_t bss_vma_end[];

extern const xwu8_t image_tail_lma_base[];
extern const xwu8_t image_tail_lma_end[];
extern const xwu8_t xwos_ivt_lma_base[];

__image_description
const struct arch_image_description arch_image_description = {
        .head = (void *)xwos_ivt_lma_base,
        .tail_flag_addr = (void *)image_tail_lma_base,
        .end_addr = (void *)image_tail_lma_end,
        .entry = arch_isr_reset,
};

__image_tail
const struct arch_image_tail arch_image_tail = {
        .flag = ARCHCFG_IMAGE_TAILFLAG,
};

/**
 * @brief Lowlevel-init architecture
 */
__xwbsp_init_code
void arch_lowlevel_init(void)
{
        cm_scs.scb.ccr.bit.stkalign = 1;
        cm_scs.scb.ccr.bit.unalign_trp = 1; /* enable unalign trap */
        arch_init_sysirqs();
}

/**
 * @brief Init architecture
 */
__xwbsp_init_code
void arch_init(void)
{
        arch_nvic_init();
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
        dst = data_vma_base;
        if (dst != src) {
                count = (xwsz_t)data_vma_end - (xwsz_t)data_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = bss_vma_base;
        count = (xwsz_t)bss_vma_end - (xwsz_t)bss_vma_base;
        for (i = 0; i < count; i++) {
                *dst = 0;
                dst++;
        }
}

#if defined(ARCHCFG_CXX) && (1 == ARCHCFG_CXX)
/**
 * @brief Init C++ runtime
 */
__xwbsp_init_code
void cxx_init(void)
{
        void (** f)();

        for (f = (void (**)())preinit_array_vma_base;
             f < (void (**)())preinit_array_vma_end;
             ++f) {
                (*f)();
        }

        for (f = (void (**)())init_array_vma_base;
             f < (void (**)())init_array_vma_end;
             ++f) {
                (*f)();
        }
}
#endif /* ARCHCFG_CXX */
