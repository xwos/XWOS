/**
 * @file
 * @brief 板级描述层：XWOS适配层：内核：HOOK
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

#include "board/std.h"
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwos/ospl/skd.h>

__xwos_code
void board_xwskd_idle_hook(struct xwospl_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        armv8a_wfi();
}

__xwos_code
void board_xwskd_syshwt_hook(struct xwospl_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
}

__xwos_code
void board_thd_postinit_hook(struct xwospl_thd * thd)
{
        XWOS_UNUSED(thd);
}
