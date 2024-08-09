/**
 * @file
 * @brief SOC描述层：SOC
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

#ifndef __xwcd_soc_arm_v6m_m0p_kea_soc_h__
#define __xwcd_soc_arm_v6m_m0p_kea_soc_h__

#include <xwos/standard.h>

#if (defined(SOCCFG_CHIP_KEA128) && (1 == SOCCFG_CHIP_KEA128))
#  define KEA128
#elif (defined(SOCCFG_CHIP_KEA64) && (1 == SOCCFG_CHIP_KEA64))
#  define KEA64
#elif (defined(SOCCFG_CHIP_KEA8) && (1 == SOCCFG_CHIP_KEA8))
#  define KEA8
#else
#  error "unsupport SOC!"
#endif

#define __flscfg __xwcc_section(".flash.cfg")

#endif /* xwcd/soc/arm/v6m/m0p/kea/soc.h */
