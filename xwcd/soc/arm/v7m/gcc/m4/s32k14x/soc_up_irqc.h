/**
 * @file
 * @brief SOC IRQC Driver for Unique-Processing Core
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

#ifndef __soc_up_irqc_h__
#define __soc_up_irqc_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <up_nvic_drv.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_inline
xwer_t soc_irqc_init(void)
{
        return cortexm_nvic_drv_init();
}

static __xwbsp_inline
xwer_t soc_irqc_request_irq(xwirq_t irqn, xwisr_f isrfunc, xwsq_t flag, void * data)
{
        return cortexm_nvic_drv_request(irqn, isrfunc, flag, data);
}

static __xwbsp_inline
xwer_t soc_irqc_release_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_release(irqn);
}

static __xwbsp_inline
xwer_t soc_irqc_enable_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_enable(irqn);
}

static __xwbsp_inline
xwer_t soc_irqc_disable_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_disable(irqn);
}

static __xwbsp_inline
xwer_t soc_irqc_save_irq(xwirq_t irqn, xwreg_t * flag)
{
        return cortexm_nvic_drv_save(irqn, flag);
}

static __xwbsp_inline
xwer_t soc_irqc_restore_irq(xwirq_t irqn, xwreg_t flag)
{
        return cortexm_nvic_drv_restore(irqn, flag);
}

static __xwbsp_inline
xwer_t soc_irqc_pend_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_pend(irqn);
}

static __xwbsp_inline
xwer_t soc_irqc_clear_irq(xwirq_t irqn)
{
        return cortexm_nvic_drv_clear(irqn);
}

static __xwbsp_inline
xwer_t soc_irqc_cfg_irq(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        return cortexm_nvic_drv_cfg(irqn, cfg);
}

static __xwbsp_inline
xwer_t soc_irqc_get_cfg_irq(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        return cortexm_nvic_drv_get_cfg(irqn, cfgbuf);
}

static __xwbsp_inline
xwer_t soc_irqc_get_data_irq(xwirq_t irqn, struct soc_irq_data * databuf)
{
        return cortexm_nvic_drv_get_data(irqn, databuf);
}

#endif /* soc_up_irqc.h */
