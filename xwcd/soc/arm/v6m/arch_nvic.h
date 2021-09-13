/**
 * @file
 * @brief 架构描述层：NVIC中断控制器
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

#ifndef __arch_nvic_h__
#define __arch_nvic_h__

#include <xwos/standard.h>

struct soc_irq_cfg;
struct soc_irq_data;

xwer_t arch_nvic_init(void);
xwer_t arch_nvic_irq_get_id(xwirq_t * irqnbuf);
xwer_t arch_nvic_irq_request(xwirq_t irqn, xwisr_f isrfunc, void * data,
                             const struct soc_irq_cfg * cfg);
xwer_t arch_nvic_irq_release(xwirq_t irqn);
xwer_t arch_nvic_irq_enable(xwirq_t irqn);
xwer_t arch_nvic_irq_disable(xwirq_t irqn);
xwer_t arch_nvic_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t arch_nvic_irq_restore(xwirq_t irqn, xwreg_t flag);
xwer_t arch_nvic_irq_pend(xwirq_t irqn);
xwer_t arch_nvic_irq_clear(xwirq_t irqn);
xwer_t arch_nvic_irq_tst(xwirq_t irqn, bool * pending);
xwer_t arch_nvic_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);
xwer_t arch_nvic_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);
xwer_t arch_nvic_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf);

#endif /* arch_nvic.h */
