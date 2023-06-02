/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWOS设备栈
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

#ifndef __bm_xwac_xwds_core_h__
#define __bm_xwac_xwds_core_h__

#include "board/std.h"

xwer_t stm32cube_xwds_probe(void);
xwer_t stm32cube_xwds_remove(void);
xwer_t stm32cube_xwds_start(void);
xwer_t stm32cube_xwds_stop(void);

xwer_t stm32cube_xwds_soc_start(void);
xwer_t stm32cube_xwds_soc_stop(void);
xwer_t stm32cube_xwds_uart_start(void);
xwer_t stm32cube_xwds_uart_stop(void);
xwer_t stm32cube_xwds_spi_start(void);
xwer_t stm32cube_xwds_spi_stop(void);
xwer_t stm32cube_xwds_st7735_start(void);
xwer_t stm32cube_xwds_st7735_stop(void);
xwer_t stm32cube_xwds_w25q64jv_start(void);
xwer_t stm32cube_xwds_w25q64jv_stop(void);

#endif /* bm/xwac/xwds/core.h */
