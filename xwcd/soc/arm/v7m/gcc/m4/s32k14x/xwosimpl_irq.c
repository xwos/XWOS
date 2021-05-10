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

__xwbsp_code
xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf)
{
        return arch_irq_get_id(irqnbuf);
}

#if defined(XuanWuOS_CFG_CORE__up)
#include <up_nvic_drv.h>

__xwbsp_code
xwer_t xwospl_irqc_init(void)
{
        return cortexm_nvic_drv_init();
}

__xwbsp_code
xwer_t xwospl_irqc_request_irq(xwirq_t irqn, xwisr_f isrfunc, void * data,
                               const struct soc_irq_cfg * cfg)
{
        return cortexm_nvic_drv_request(irqn, isrfunc, data, cfg);
}

__xwbsp_code
xwer_t xwospl_irqc_release_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_release(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_enable_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_enable(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_disable_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_disable(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_save_irq(xwirq_t irqn, xwreg_t * flag)
{
        return cortexm_nvic_drv_save(irqn, flag);
}

__xwbsp_code
xwer_t xwospl_irqc_restore_irq(xwirq_t irqn, xwreg_t flag)
{
        return cortexm_nvic_drv_restore(irqn, flag);
}

__xwbsp_code
xwer_t xwospl_irqc_pend_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_pend(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_clear_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_clear(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_cfg_irq(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        return cortexm_nvic_drv_cfg(irqn, cfg);
}

__xwbsp_code
xwer_t xwospl_irqc_get_cfg_irq(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        return cortexm_nvic_drv_get_cfg(irqn, cfgbuf);
}

__xwbsp_code
xwer_t xwospl_irqc_get_data_irq(xwirq_t irqn, struct soc_irq_data * databuf)
{
        return cortexm_nvic_drv_get_data(irqn, databuf);
}
#endif /* XuanWuOS_CFG_CORE__up */
