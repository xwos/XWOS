/**
 * @file
 * @brief STM32CUBE设备栈：设备
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

#ifndef __bm_stm32cube_xwac_xwds_device_h__
#define __bm_stm32cube_xwac_xwds_device_h__

#include <bm/stm32cube/standard.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/uart/dma.h>
#include <xwcd/ds/i2c/master.h>
#include <xwcd/ds/spi/master.h>
#include <xwcd/perpheral/i2c/eeprom/device.h>
#include <xwcd/perpheral/spi/flash/w25qxx/device.h>

/******** ******** ds ******** ********/
extern struct xwds stm32cube_ds;

/******** ******** soc ******** ********/
extern struct xwds_soc stm32cube_soc_cb;

/******** ******** uart ******** ********/
extern struct xwds_dmauartc stm32cube_usart1_cb;
extern struct xwds_dmauartc stm32cube_usart2_cb;

/******** ******** i2c ******** ********/
extern struct xwds_i2cm stm32cube_i2c2m_cb;
extern struct xwds_eeprom stm32cube_at24c02_cb;

/******** ******** spi ******** ********/
extern struct xwds_spim stm32cube_spi1m_cb;
extern struct xwds_spim stm32cube_spi2m_cb;

#endif /* bm/stm32cube/xwac/xwds/device.h */
