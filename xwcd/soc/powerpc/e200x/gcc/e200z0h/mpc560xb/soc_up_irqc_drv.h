/**
 * @file
 * @brief SOC IRQ controller driver
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

#ifndef __soc_up_irqc_drv_h__
#define __soc_up_irqc_drv_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc_irq.h>
#include <soc_irqc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code
xwer_t soc_irqc_drv_init(void);

__xwbsp_code
xwer_t soc_irqc_drv_request(xwirq_t irqn, xwisr_f isrfunc, xwsq_t flag, void * data);

__xwbsp_code
xwer_t soc_irqc_drv_release(xwirq_t irqn);

__xwbsp_code
xwer_t soc_irqc_drv_enable(xwirq_t irqn);

__xwbsp_code
xwer_t soc_irqc_drv_disable(xwirq_t irqn);

__xwbsp_code
xwer_t soc_irqc_drv_save(xwirq_t irqn, xwreg_t * flag);

__xwbsp_code
xwer_t soc_irqc_drv_restore(xwirq_t irqn, xwreg_t flag);

__xwbsp_code
xwer_t soc_irqc_drv_pend(xwirq_t irqn);

__xwbsp_code
xwer_t soc_irqc_drv_clear(xwirq_t irqn);

__xwbsp_code
xwer_t soc_irqc_drv_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);

__xwbsp_code
xwer_t soc_irqc_drv_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);

__xwbsp_code
xwer_t soc_irqc_drv_get_data(xwirq_t irqn, struct soc_irq_data * databuf);

#endif /* soc_irqc_drv.h */
