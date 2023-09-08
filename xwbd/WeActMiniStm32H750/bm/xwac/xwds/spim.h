/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWOS设备栈：SPI主机控制器
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

#ifndef __bm_xwac_xwds_spim_h__
#define __bm_xwac_xwds_spim_h__

#include "board/std.h"

void stm32xwds_spi1m_cb_xfercplt(struct xwds_spim * spim, xwer_t xrc);
void stm32xwds_spi4m_cb_xfercplt(struct xwds_spim * spim, xwer_t xrc);

#endif /* bm/xwac/xwds/spim.h */
