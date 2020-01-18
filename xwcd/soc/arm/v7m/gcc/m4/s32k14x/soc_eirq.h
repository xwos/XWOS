/**
 * @file
 * @brief S32K14x External IRQs
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

#ifndef __soc_eirq_h__
#define __soc_eirq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SOC_EIRQ_NUM            (5 * 32U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SOC external IRQ configuration enumerations
 */
enum soc_eirq_cfg_em {
        SOC_EIRQC_DISABLE = 0,
        SOC_EIRQC_DMA_RISING,
        SOC_EIRQC_DMA_FALLING,
        SOC_EIRQC_DMA_EITHER,
        SOC_RESERVED4,
        SOC_RESERVED5,
        SOC_RESERVED6,
        SOC_RESERVED7,
        SOC_EIRQC_IRQ_LOW,
        SOC_EIRQC_IRQ_RISING,
        SOC_EIRQC_IRQ_FALLING,
        SOC_EIRQC_IRQ_EITHER,
        SOC_EIRQC_IRQ_HIGH,
        SOC_RESERVED13,
        SOC_RESERVED14,
        SOC_RESERVED15,
};

#endif /* soc_eirq.h */
