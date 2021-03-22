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
#include <arch_skd.h>
#include <arch_systick.h>
#include "bm/s32kbsp/../../../SDK/platform/devices/device_registers.h"
#include "bm/s32kbsp/xwac/ivt/isr.h"

extern xwu8_t xwos_stk_top[];

__soc_isr_table_qualifier struct soc_isr_table soc_isr_table __xwos_vctbl = {
        .arch = {
                [ARCH_SP_TOP + ARCHCFG_IRQ_NUM] = (xwisr_f)xwos_stk_top,
                [ARCH_IRQ_RESET + ARCHCFG_IRQ_NUM] = (xwisr_f)arch_isr_reset,
                [ARCH_IRQ_NMI + ARCHCFG_IRQ_NUM] = arch_isr_nmi,
                [ARCH_IRQ_HARDFAULT + ARCHCFG_IRQ_NUM] = arch_isr_hardfault,
                [ARCH_IRQ_MMFAULT + ARCHCFG_IRQ_NUM] = arch_isr_mm,
                [ARCH_IRQ_BUSFAULT + ARCHCFG_IRQ_NUM] = arch_isr_busfault,
                [ARCH_IRQ_USGFAULT + ARCHCFG_IRQ_NUM] = arch_isr_usagefault,
                [ARCH_IRQ_RSVN9 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_RSVN8 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_RSVN7 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_RSVN6 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_SVCALL + ARCHCFG_IRQ_NUM] = (xwisr_f)arch_isr_svc,
                [ARCH_IRQ_DBGMON + ARCHCFG_IRQ_NUM] = arch_isr_dbgmon,
                [ARCH_IRQ_RSVN3 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_PENDSV + ARCHCFG_IRQ_NUM] = arch_isr_pendsv,
                [ARCH_IRQ_SYSTICK + ARCHCFG_IRQ_NUM] = arch_isr_systick,
        },
        .soc = {
[T:VECTOR]
        },
};

__soc_isr_table_qualifier struct soc_isr_data_table soc_isr_data_table = {
        .arch = {
                [ARCH_SP_TOP + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RESET + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_NMI + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_HARDFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_MMFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_BUSFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_USGFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN9 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN8 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN7 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN6 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_SVCALL + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_DBGMON + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN3 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_PENDSV + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_SYSTICK + ARCHCFG_IRQ_NUM] = NULL,
        },
        .soc = {
                [0 ... (SOCCFG_IRQ_NUM - 1)] = NULL,
        },
};
