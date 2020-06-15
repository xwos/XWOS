/**
 * @file
 * @brief 玄武OS内核适配代码：系统硬件定时器
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

#ifndef __soc_syshwt_h__
#define __soc_syshwt_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <arch_systick.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_syshwt;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   XWOS HW Timer Adapter Functions   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_inline
xwer_t soc_syshwt_init(struct xwos_syshwt * hwt)
{
        return arch_systick_init(hwt);
}

static __xwbsp_inline
xwer_t soc_syshwt_start(struct xwos_syshwt * hwt)
{
        return arch_systick_start(hwt);
}

static __xwbsp_inline
xwer_t soc_syshwt_stop(struct xwos_syshwt * hwt)
{
        return arch_systick_stop(hwt);
}

static __xwbsp_inline
xwtm_t soc_syshwt_get_timeconfetti(struct xwos_syshwt * hwt)
{
        return arch_systick_get_timeconfetti(hwt);
}

#endif /* soc_syshwt.h */
