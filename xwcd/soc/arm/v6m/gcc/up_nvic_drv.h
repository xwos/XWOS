/**
 * @file
 * @brief ARMv7-m NVIC Driver for Unique-Processing Core
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

#ifndef __up_nvic_drv_h__
#define __up_nvic_drv_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc_irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code
xwer_t cortexm_nvic_drv_init(void);

__xwbsp_code
xwer_t cortexm_nvic_drv_request(xwirq_t irqn, xwisr_f isrfunc, xwsq_t flag,
                                void * data);

__xwbsp_code
xwer_t cortexm_nvic_drv_release(xwirq_t irqn);

__xwbsp_code
xwer_t cortexm_nvic_drv_enable(xwirq_t irqn);

__xwbsp_code
xwer_t cortexm_nvic_drv_disable(xwirq_t irqn);

__xwbsp_code
xwer_t cortexm_nvic_drv_save(xwirq_t irqn, xwreg_t * flag);

__xwbsp_code
xwer_t cortexm_nvic_drv_restore(xwirq_t irqn, xwreg_t flag);

__xwbsp_code
xwer_t cortexm_nvic_drv_pend(xwirq_t irqn);

__xwbsp_code
xwer_t cortexm_nvic_drv_clear(xwirq_t irqn);

__xwbsp_code
xwer_t cortexm_nvic_drv_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);

__xwbsp_code
xwer_t cortexm_nvic_drv_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);

__xwbsp_code
xwer_t cortexm_nvic_drv_get_data(xwirq_t irqn, struct soc_irq_data * databuf);

#endif /* up_nvic_drv.h */
