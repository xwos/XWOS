/**
 * @file
 * @brief STM32CUBE设备栈：子模块接口
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

#ifndef __bm_stm32cube_xwac_xwds_cmif_h__
#define __bm_stm32cube_xwac_xwds_cmif_h__

#include <bm/stm32cube/xwac/xwds/device.h>

xwer_t stm32cube_xwds_ll_start(void);
xwer_t stm32cube_xwds_ll_stop(void);
xwer_t stm32cube_xwds_start(void);
xwer_t stm32cube_xwds_stop(void);
xwer_t stm32cube_xwds_soc_start(void);
xwer_t stm32cube_xwds_soc_stop(void);
xwer_t stm32cube_xwds_uart_start(void);
xwer_t stm32cube_xwds_uart_stop(void);

#endif /* bm/stm32cube/xwac/xwds/cmif.h */
