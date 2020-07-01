/**
 * @file
 * @brief STM32CUBE：设备栈
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

#ifndef __bm_stm32cube_xwac_xwds_stm32cube_h__
#define __bm_stm32cube_xwac_xwds_stm32cube_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <bm/stm32cube/standard.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/chip.h>
#include <xwmd/ds/uart/dma.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ds ******** ********/
extern struct xwds stm32cube_ds;

/******** ******** soc ******** ********/
extern struct xwds_soc stm32cube_soc_cb;

/******** ******** uart ******** ********/
extern struct xwds_dmauartc stm32cube_dmauartc1_cb;

#endif /* bm/stm32cube/xwac/xwds/stm32cube.h */
