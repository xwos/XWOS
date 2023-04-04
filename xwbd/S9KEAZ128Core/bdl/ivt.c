/**
 * @file
 * @brief XWOS Kernel Adaptation Code in BDL：中断向量表
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
#include <xwos/osal/irq.h>
#include <xwos/ospl/syshwt.h>
#include <xwos/ospl/skd.h>

#if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240)
#  error "SOCCFG_IRQ_NUM must be between 8 and 240 inclusive"
#endif

extern xwu8_t xwos_stk_top[];

struct soc_ivt {
        xwisr_f exc[SOCCFG_EXC_NUM];
        xwisr_f irq[SOCCFG_IRQ_NUM];
};

/**
 * @brief 中断向量表
 */
__xwcc_section(".armv6m.ivt") struct soc_ivt kea_ivt = {
        .exc = {
                (xwisr_f)xwos_stk_top,
                (xwisr_f)arch_isr_reset,
                arch_isr_nmi,
                arch_isr_hardfault,
                arch_isr_mm,
                arch_isr_busfault,
                arch_isr_usagefault,
                arch_isr_noop,
                arch_isr_noop,
                arch_isr_noop,
                arch_isr_noop,
                (xwisr_f)arch_isr_svc,
                arch_isr_dbgmon,
                arch_isr_noop,
                xwospl_skd_isr_swcx,
                xwospl_syshwt_isr,
        },
        .irq = {
                arch_isr_noop,
        },
};
