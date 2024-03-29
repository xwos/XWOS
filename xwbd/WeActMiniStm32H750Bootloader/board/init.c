/**
 * @file
 * @brief 板级描述层：初始化
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
#include <xwcd/soc/arm/v7m/arch_init.h>
#include <xwcd/soc/arm/v7m/m7/stm32/soc_init.h>
#include "board/xwac/xwlib/crc.h"
#include "board/xwac/xwds/device.h"
#include "bm/stm32cube/mif.h"

/**
 * @brief XWOS预初始化
 */
__xwbsp_init_code
void xwos_preinit(void)
{
        arch_init();
        soc_init();
        stm32cube_preinit();
        soc_relocate_data();
        soc_relocate_ivt();
        stm32cube_init();
}

/**
 * @brief XWOS后初始化
 */
__xwbsp_init_code
void xwos_postinit(void)
{
        stm32xwds_init();
        stm32xwds_soc_init();
        xwlibac_crc_init();
}
