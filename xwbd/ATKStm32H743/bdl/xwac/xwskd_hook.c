/**
 * @file
 * @brief 玄武OS内核适配代码：内核HOOK
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

#include <xwos/standard.h>
#include <armv7m_isa.h>
#include <xwos/ospl/skd.h>

extern
void stm32cube_systick_hook(void);

__xwos_code
void board_xwskd_idle_hook(struct xwospl_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        cm_wfi();
}

__xwos_code
void board_xwskd_syshwt_hook(struct xwospl_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        stm32cube_systick_hook();
}

__xwos_code
void board_thd_postinit_hook(struct xwospl_thd * thd)
{
        XWOS_UNUSED(thd);
        /* Add MPU code here. */
}
