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
#include <xwos/mp/irq.h>
#include <mp_nvic.h>
#include <bdl/standard.h>
#include <bdl/board_init.h>

extern void s32kbsp_lowlevel_init(void);
extern void s32kbsp_init(void);

__xwos_init_code
void board_lowlevel_init(void)
{
        s32kbsp_lowlevel_init();
}

__xwos_init_code
void board_init(void)
{
        s32kbsp_init();
}
