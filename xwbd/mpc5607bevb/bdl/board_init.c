/**
 * @file
 * @brief 板级描述层：初始化
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
#include <bdl/standard.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <bdl/bkup.h>
#include <bdl/ds/device.h>
#include <bdl/board_init.h>

__xwos_init_code
void board_lowlevel_init(void)
{
}

__xwos_init_code
void board_init(void)
{
        /* save hw reset flags */
        if (soc_reset_flags.fes || soc_reset_flags.des) {
                bdl_bkup_data.soc_reset_flags.reg.fes = soc_reset_flags.fes;
                bdl_bkup_data.soc_reset_flags.reg.des = soc_reset_flags.des;
        } else if (soc_wkup_flags.nsr || soc_wkup_flags.wisr) {
                bdl_bkup_data.soc_wkup_flags.nsr = soc_wkup_flags.nsr;
                bdl_bkup_data.soc_wkup_flags.wisr = soc_wkup_flags.wisr;
        }

        /* set IRQ Controller data */
        bdl_xwds_start();
}
