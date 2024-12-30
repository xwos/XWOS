/**
 * @file
 * @brief SOC描述层：初始化
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

#ifndef __xwcd_soc_arm_v7m_m4_fc4150_soc_init_h__
#define __xwcd_soc_arm_v7m_m4_fc4150_soc_init_h__

#include <xwos/standard.h>

void soc_isr_reset(void);
void soc_relocate_ivt(void);
void soc_relocate_data(void);
void soc_disable_wdg(void);

#endif /* xwcd/soc/arm/v7m/m4/fc4150/soc_init.h */
