/**
 * @file
 * @brief S32KBSP模块：中断向量表
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
#include <xwos/ospl/irq.h>
#include <xwos/ospl/syshwt.h>
#include <xwos/ospl/skd.h>
#include "bm/s32kbsp/../../../SDK/platform/devices/device_registers.h"
#include "bm/s32kbsp/xwac/ivt/isr.h"

extern xwu8_t xwos_stk_top[];

__xwos_ivt __xwos_ivt_qualifier struct soc_ivt xwospl_ivt = {
        .exc = {
                [SOC_SP_TOP + SOCCFG_EXC_NUM] = (xwisr_f)xwos_stk_top,
                [SOC_EXC_RESET + SOCCFG_EXC_NUM] = (xwisr_f)arch_isr_reset,
                [SOC_EXC_NMI + SOCCFG_EXC_NUM] = arch_isr_nmi,
                [SOC_EXC_HARDFAULT + SOCCFG_EXC_NUM] = arch_isr_hardfault,
                [SOC_EXC_MMFAULT + SOCCFG_EXC_NUM] = arch_isr_mm,
                [SOC_EXC_BUSFAULT + SOCCFG_EXC_NUM] = arch_isr_busfault,
                [SOC_EXC_USGFAULT + SOCCFG_EXC_NUM] = arch_isr_usagefault,
                [SOC_EXC_RSVN9 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_RSVN8 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_RSVN7 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_RSVN6 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_SVCALL + SOCCFG_EXC_NUM] = (xwisr_f)arch_isr_svc,
                [SOC_EXC_DBGMON + SOCCFG_EXC_NUM] = arch_isr_dbgmon,
                [SOC_EXC_RSVN3 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_PENDSV + SOCCFG_EXC_NUM] = xwospl_skd_isr_swcx,
                [SOC_EXC_SYSTICK + SOCCFG_EXC_NUM] = xwospl_syshwt_isr,
        },
        .irq = {
[T:VECTOR]
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
                [0 ... (SOCCFG_IRQ_NUM - 1)] = NULL,
        },
};
