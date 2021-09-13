/**
 * @file
 * @brief XWOS Kernel Adaptation Code in BDL：中断向量表
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
#include <xwos/osal/irq.h>
#include <xwos/ospl/syshwt.h>
#include <xwos/ospl/skd.h>

#if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240)
  #error "SOCCFG_IRQ_NUM must be between 8 and 240 inclusive"
#endif /* #if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240) */

extern xwu8_t xwos_stk_top[];

/**
 * @brief 中断向量表
 */
__xwos_ivt __xwos_ivt_qualifier struct soc_ivt xwospl_ivt = {
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

__xwos_ivt_qualifier struct soc_idvt xwospl_idvt = {
        .exc = {
                [SOC_SP_TOP + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RESET + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_NMI + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_HARDFAULT + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_MMFAULT + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_BUSFAULT + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_USGFAULT + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN9 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN8 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN7 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN6 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_SVCALL + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_DBGMON + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN3 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_PENDSV + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_SYSTICK + SOCCFG_EXC_NUM] = NULL,
        },
        .irq = {
                NULL,
        },
};
