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

#ifndef __xwcd_soc_arm_v7m_arch_skd_h__
#define __xwcd_soc_arm_v7m_arch_skd_h__

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>

void arch_skd_init_pendsv(void);
void arch_skd_init_stack(struct xwospl_skdobj_stack * stk, void (* exit)(xwer_t));
void arch_svc_skd_start(struct xwospl_skd * xwskd);
struct xwospl_skd * arch_skd_chk_stk(void);
void xwospl_skd_isr_swcx(void);

#endif /* xwcd/soc/arm/v7m/arch_skd.h */
