/**
 * @file
 * @brief 架构描述层：调度器
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

#ifndef __arch_skd_h__
#define __arch_skd_h__

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>

xwer_t arch_skd_init_pendsv(struct xwospl_skd * xwskd);
void arch_skd_init_stack(struct xwospl_skd_stack_info * stk,
                         void (* exit)(xwer_t),
                         xwsq_t attr);
void arch_skd_svcsr_start(struct xwospl_skd * xwskd);
struct xwospl_skd * arch_skd_chk_stk(void);

#endif /* arch_skd.h */
