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
#include <armv7m_isa.h>
#include <arch_irq.h>
#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
#  include <arch_fpu.h>
#endif
#include <arch_image.h>
#include <arch_nvic.h>
#include <arch_init.h>

extern const xwu8_t image_tail_lma_base[];
extern const xwu8_t image_tail_lma_end[];
extern const xwu8_t image_head_lma[];

__image_description
const struct arch_image_description arch_image_description = {
        .head = (void *)image_head_lma,
        .tail_flag_addr = (void *)image_tail_lma_base,
        .end_addr = (void *)image_tail_lma_end,
        .entry = arch_isr_reset,
};

__image_tail
const struct arch_image_tail arch_image_tail = {
        .flag = ARCHCFG_IMAGE_TAILFLAG,
};

/**
 * @brief Init Architecture
 */
__xwbsp_init_code
void arch_init(void)
{
        cm_scs.scb.ccr.bit.stkalign = 1; /* stack aligned to 8-byte */
        cm_scs.scb.ccr.bit.bp = 1; /* enable branch prediction */
        cm_scs.scb.ccr.bit.div_0_trp = 1; /* enable divide by 0 trap */
        arch_nvic_init();
#if (ARCHCFG_FPU == 1)
        arch_fpu_init();
#endif
        arch_init_sysirqs();
}
