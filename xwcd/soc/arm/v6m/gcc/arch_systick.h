/**
 * @file
 * @brief 架构描述层：Cortex-M Systick Timer
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

#include <xwos/standard.h>
#include <xwos/ospl/syshwt.h>
#include <arch_irq.h>

#define SOC_EXC_TICK_PRIO       (SOC_IRQ_PRIO_LOWEST)

xwer_t arch_systick_init(struct xwospl_syshwt * hwt);
xwer_t arch_systick_start(struct xwospl_syshwt * hwt);
xwer_t arch_systick_stop(struct xwospl_syshwt * hwt);
xwtm_t arch_systick_get_timeconfetti(struct xwospl_syshwt * hwt);

#endif /* arch_systick.h */
