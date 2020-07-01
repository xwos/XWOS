/**
 * @file
 * @brief Cortex-M Systick Timer
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

#ifndef __arch_systick_h__
#define __arch_systick_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <arch_irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define ARCH_IRQ_TICK_PRIO      (ARCH_IRQ_PRIO_0 | ARCH_IRQ_SUBPRIO_HIGH)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_syshwt;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code
xwer_t arch_systick_init(struct xwos_syshwt * hwt);

__xwbsp_isr
void arch_systick_isr(void);

__xwbsp_code
xwer_t arch_systick_start(struct xwos_syshwt * hwt);

__xwbsp_code
xwer_t arch_systick_stop(struct xwos_syshwt * hwt);

__xwbsp_code
xwtm_t arch_systick_get_timeconfetti(struct xwos_syshwt * hwt);

#endif /* arch_systick.h */
