/**
 * @file
 * @brief 玄武OS移植实现层：SOC中断
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

#ifndef __xwosimpl_soc_irq_h__
#define __xwosimpl_soc_irq_h__

#ifndef __xwos_ospl_soc_irq_h__
  #error "This file should be included from <xwos/ospl/soc/irq.h>."
#endif

#include <armv6m_nvic.h>

struct soc_irq_cfg {
        struct cortexm_nvic_irq_cfg irqcfg;
};

struct soc_irq_data {
        void * data;
};

struct soc_ivt {
        xwisr_f exc[SOCCFG_EXC_NUM];
        xwisr_f irq[SOCCFG_IRQ_NUM];
};

struct soc_idvt {
        void * exc[SOCCFG_EXC_NUM];
        void * irq[SOCCFG_IRQ_NUM];
};

#endif /* xwosimpl_soc_irq.h */
