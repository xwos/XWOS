/**
 * @file
 * @brief SOC描述层：Inter-Cpu Communication
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

#ifndef __xwcd_soc_arm_v7m_m7_fc7300_soc_icc_h__
#define __xwcd_soc_arm_v7m_m7_fc7300_soc_icc_h__

#include <xwos/standard.h>

typedef void (* soc_icc_func_f)(void *);

xwer_t soc_icc_call(xwid_t dstcpu, soc_icc_func_f func, void * arg);
void soc_icc_finish(void);

#endif /* xwcd/soc/arm/v7m/m7/fc7300/soc_icc.h */
