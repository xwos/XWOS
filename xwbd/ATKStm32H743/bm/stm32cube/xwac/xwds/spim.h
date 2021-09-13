/**
 * @file
 * @brief STM32CUBE设备栈：SPI主机
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

#ifndef __bm_stm32cube_xwac_xwds_spim_h__
#define __bm_stm32cube_xwac_xwds_spim_h__

#include <bm/stm32cube/standard.h>

void stm32cube_spi1m_cb_xfercplt(struct xwds_spim * spim, xwer_t xrc);
void stm32cube_spi2m_cb_xfercplt(struct xwds_spim * spim, xwer_t xrc);

#endif /* bm/stm32cube/xwac/xwds/spim.h */
