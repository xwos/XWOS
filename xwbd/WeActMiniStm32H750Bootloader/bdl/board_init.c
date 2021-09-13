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
#include <string.h>
#include <armv7m_core.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#include <bm/stm32cube/mif.h>
#include <bdl/board_init.h>

__xwbsp_init_code
void board_lowlevel_init(void)
{
        stm32cube_lowlevel_init();
}

/**
 * @brief init board
 */
__xwbsp_init_code
void board_init(void)
{
        stm32cube_init();
}

void board_reset(void)
{
        cm_nvic_disable_faults();
        cm_reset_system();
}
