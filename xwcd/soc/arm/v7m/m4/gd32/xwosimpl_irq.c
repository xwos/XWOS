/**
 * @file
 * @brief 玄武OS移植实现层：中断
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
#include <arch_irq.h>
#include <arch_nvic.h>

__xwbsp_code
xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf)
{
        return arch_nvic_irq_get_id(irqnbuf);
}

__xwbsp_code
xwer_t xwospl_irq_request(xwirq_t irqn, xwisr_f isrfunc, void * data,
                          const struct soc_irq_cfg * cfg)
{
        return arch_nvic_irq_request(irqn, isrfunc, data, cfg);
}

__xwbsp_code
xwer_t xwospl_irq_release(xwirq_t irqn)
{
        return arch_nvic_irq_release(irqn);
}

__xwbsp_code
xwer_t xwospl_irq_enable(xwirq_t irqn)
{
        return arch_nvic_irq_enable(irqn);
}

__xwbsp_code
xwer_t xwospl_irq_disable(xwirq_t irqn)
{
        return arch_nvic_irq_disable(irqn);
}

__xwbsp_code
xwer_t xwospl_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        return arch_nvic_irq_save(irqn, flag);
}

__xwbsp_code
xwer_t xwospl_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        return arch_nvic_irq_restore(irqn, flag);
}

__xwbsp_code
xwer_t xwospl_irq_pend(xwirq_t irqn)
{
        return arch_nvic_irq_pend(irqn);
}

__xwbsp_code
xwer_t xwospl_irq_clear(xwirq_t irqn)
{
        return arch_nvic_irq_clear(irqn);
}

__xwbsp_code
xwer_t xwospl_irq_tst(xwirq_t irqn, bool * pending)
{
        return arch_nvic_irq_tst(irqn, pending);
}

__xwbsp_code
xwer_t xwospl_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        return arch_nvic_irq_cfg(irqn, cfg);
}

__xwbsp_code
xwer_t xwospl_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        return arch_nvic_irq_get_cfg(irqn, cfgbuf);
}

__xwbsp_code
xwer_t xwospl_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        return arch_nvic_irq_get_data(irqn, databuf);
}
