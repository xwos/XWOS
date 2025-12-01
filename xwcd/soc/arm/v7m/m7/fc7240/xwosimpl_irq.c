/**
 * @file
 * @brief XWOS移植实现层：中断
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
#include <xwcd/soc/arm/v7m/arch_irq.h>

__xwbsp_code
xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf)
{
        return arch_nvic_irq_get_id(irqnbuf);
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
