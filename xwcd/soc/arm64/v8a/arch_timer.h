/**
 * @file
 * @brief 架构描述层：ARMv8A Generic Timer
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

#ifndef __xwcd_soc_arm_v8a_arch_timer_h__
#define __xwcd_soc_arm_v8a_arch_timer_h__

#include <xwos/standard.h>

void armv8a_timer_init(void);
void armv8a_cnthp_init(xwu64_t hz, void (* handler)(xwid_t));
void armv8a_cnthp_fini(void);
void armv8a_cnthp_reload(xwu64_t hz);
xwtm_t armv8a_cnthp_get_timeconfetti(xwu64_t hz);
void armv8a_cnthp_isr(void);
void armv8a_cntp_init(xwu64_t hz, void (* handler)(xwid_t));
void armv8a_cntp_fini(void);
void armv8a_cntp_reload(xwu64_t hz);
xwtm_t armv8a_cntp_get_timeconfetti(xwu64_t hz);
void armv8a_cntp_isr(void);

#endif /* xwcd/soc/arm64/v8a/arch_timer.h */
