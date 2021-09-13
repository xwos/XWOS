/**
 * @file
 * @brief SOC描述层：MPC560xB External IRQs
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

#ifndef __soc_eirq_h__
#define __soc_eirq_h__

#include <xwos/standard.h>
#include <soc.h>

/**
 * @brief soc external IRQ enumerations
 */
enum soc_eirq_em {
        SOC_WKUP_MIN = 0,
        SOC_WKUP_0 = SOC_WKUP_MIN,
        SOC_WKUP_1,
        SOC_WKUP_2,
        SOC_WKUP_3,
        SOC_WKUP_4,
        SOC_WKUP_5,
        SOC_WKUP_6,
        SOC_WKUP_7,
        SOC_WKUP_8,
        SOC_WKUP_9,
        SOC_WKUP_10,
        SOC_WKUP_11,
        SOC_WKUP_12,
        SOC_WKUP_13,
        SOC_WKUP_14,
        SOC_WKUP_15,
        SOC_WKUP_16,
        SOC_WKUP_17,
        SOC_WKUP_18,
        SOC_WKUP_19,
#if defined(MPC5604B)
        SOC_WKUP_MAX = SOC_WKUP_19,
#endif
#if defined(MPC5607B)
        SOC_WKUP_20,
        SOC_WKUP_21,
        SOC_WKUP_22,
        SOC_WKUP_23,
        SOC_WKUP_24,
        SOC_WKUP_25,
        SOC_WKUP_26,
        SOC_WKUP_27,
        SOC_WKUP_28,
        SOC_WKUP_MAX = SOC_WKUP_28,
#endif
        SOC_WKUP_NUM,
        SOC_EIRQ_MIN = SOC_WKUP_MAX + 1,
        SOC_EIRQ_0 = SOC_EIRQ_MIN,
        SOC_EIRQ_1,
        SOC_EIRQ_2,
        SOC_EIRQ_3,
        SOC_EIRQ_4,
        SOC_EIRQ_5,
        SOC_EIRQ_6,
        SOC_EIRQ_7,
        SOC_EIRQ_8,
        SOC_EIRQ_9,
        SOC_EIRQ_10,
        SOC_EIRQ_11,
        SOC_EIRQ_12,
        SOC_EIRQ_13,
        SOC_EIRQ_14,
        SOC_EIRQ_15,
        SOC_EIRQ_MAX = SOC_EIRQ_15,
        SOC_EIRQ_NUM = SOC_EIRQ_MAX - SOC_EIRQ_MIN + 1,
};

#endif /* soc_eirq.h */
