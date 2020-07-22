/**
 * @file
 * @brief 板级描述层：中断向量表
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc_sched.h>
#include <soc_irq.h>
#include <soc_syshwt.h>
#include <xwos/irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240)
  #error "SOCCFG_IRQ_NUM must be between 8 and 240 inclusive"
#endif /* #if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240) */

extern xwu8_t xwos_stk_top[];

/**
 * @brief ISR table
 */
__soc_isr_table_qualifier struct soc_isr_table soc_isr_table __xwos_vctbl = {
        .arch = {
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
                arch_scheduler_isr_swcx,
                arch_systick_isr,
        },
        .soc = {
                [0 ... (SOCCFG_IRQ_NUM - 1)] = arch_isr_noop,
        },
};

__soc_isr_table_qualifier struct soc_irq_data_table soc_irq_data_table = {
        .arch = {
                [0 ... (ARCHCFG_IRQ_NUM - 1)] = NULL,
        },
        .soc = {
                [0 ... (SOCCFG_IRQ_NUM - 1)] = arch_isr_noop,
        },
};
