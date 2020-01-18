/**
 * @file
 * @brief IRQ Controller Description of cortex-m NVIC
 *        for Symmetrical Multi-Processing Cores
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#include <xwos/smp/irq.h>
#include <smp_nvic_drv.h>
#include <soc_irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __soc_isr_table_qualifier struct soc_isr_table soc_isr_table;
extern __soc_isr_table_qualifier struct soc_irq_data_table soc_irq_data_table;

__xwbsp_rodata const struct cortexm_nvic_cfg armv7_nvic_cfg = {
        .subprio = SOCCFG_NVIC_SUBPRIO_BITIDX,
        .basepri = 0,
};

__xwbsp_data struct xwos_irqc cortexm_nvic[CPUCFG_CPU_NUM] = {
        [0] = {
                .name = "cortex-m.nvic",
                .drv = &cortexm_nvic_drv,
                .irqs_num = (SOCCFG_IRQ_NUM + ARCHCFG_IRQ_NUM),
                .isr_table = &soc_isr_table,
                .irq_data_table = &soc_irq_data_table,
                .soc_cfg = &armv7_nvic_cfg,
                .data = NULL,
        }
};
