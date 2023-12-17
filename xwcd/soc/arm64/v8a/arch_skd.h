/**
 * @file
 * @brief 架构描述层：调度器
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

#ifndef __xwcd_soc_arm_v8a_arch_skd_h__
#define __xwcd_soc_arm_v8a_arch_skd_h__

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm64/v8a/arch_exception.h>

void armv8a_skd_init_stack(struct xwospl_skdobj_stack * stk,
                           void (* exit)(xwer_t));
void armv8a_skd_start(__xwcc_unused struct xwospl_skd * xwskd, xwstk_t * sp_el2);
void armv8a_skd_swcx(void);
struct xwospl_skd * armv8a_skd_chk_swcx(xwreg_t psp);
struct xwospl_skd * armv8a_skd_chk_stk(xwreg_t csp);

#endif /* xwcd/soc/arm64/v8a/arch_skd.h */
