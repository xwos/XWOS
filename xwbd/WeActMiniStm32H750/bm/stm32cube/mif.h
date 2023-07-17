/**
 * @file
 * @brief 板级描述层：STM32CUBE模块：模块接口
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

#ifndef __bm_stm32cube_mif_h__
#define __bm_stm32cube_mif_h__

#include "board/std.h"

void stm32cube_lowlevel_init(void);
void stm32cube_init(void);
xwer_t stm32cube_start(void);
xwer_t stm32cube_stop(void);

#endif /* bm/stm32cube/mif.h */
