/**
 * @file
 * @brief SOC描述层：MP IRQ controller
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
#include <xwos/mp/irq.h>
#include <soc_mp_irqc.h>
#include <soc_mp_irqc_drv.h>

extern __soc_isr_table_qualifier struct soc_isr_table soc_isr_table;
extern __soc_isr_table_qualifier struct soc_isr_data_table soc_isr_data_table;

__xwbsp_data struct xwmp_irqc soc_irqc[CPUCFG_CPU_NUM] = {
        [0] = {
                .name = "e200z0h.irqc",
                .drv = &soc_irqc_drv,
                .irqs_num = (SOCCFG_IRQ_NUM + ARCHCFG_IRQ_NUM),
                .isr_table = &soc_isr_table,
                .irq_data_table = &soc_isr_data_table,
                .soc_cfg = NULL,
                .data = NULL,
        },
};
