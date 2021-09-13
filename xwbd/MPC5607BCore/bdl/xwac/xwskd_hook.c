/**
 * @file
 * @brief 玄武OS内核适配代码：内核HOOK
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

#include <xwos/standard.h>
#include <soc_me.h>
#include <soc_wdg.h>
#include <bdl/xwac/xwskd_hook.h>

__xwos_code
void board_xwskd_idle_hook(struct xwos_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        /* soc_me_enter_mode(XPC5_ME_MODE_HALT); */
        /* soc_wdg_feed(); */
}
