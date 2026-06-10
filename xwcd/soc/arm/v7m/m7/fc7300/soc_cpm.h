/**
 * @file
 * @brief SOC描述层：CPM
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwcd_soc_arm_v7m_m7_fc7300_soc_cpm_h__
#define __xwcd_soc_arm_v7m_m7_fc7300_soc_cpm_h__

#include <xwos/standard.h>

struct soc_cpm_regs {
        xwu32_t fiscr; /* offset: 0x00 */
        xwu32_t miscr; /* offset: 0x04 */
        xwu32_t reserved0[6]; /* offset: 0x08 */
        xwu32_t lock; /* offset: 0x20 */
};


#define SOC_CPM_MISCR_CPU_ID_MASK 0x000E0000UL
#define SOC_CPM_MISCR_CPU_ID_SHIFT 17U

#define SOC_CPM_BASE (0xE0080000UL)
#define soc_cpm (*((volatile struct soc_cpm_regs *)SOC_CPM_BASE))

#endif /* xwcd/soc/arm/v7m/m7/fc7300/soc_cpm.h */
