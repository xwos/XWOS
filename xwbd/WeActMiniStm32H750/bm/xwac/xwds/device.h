/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWOS设备栈：设备
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

#ifndef __bm_xwac_xwds_device_h__
#define __bm_xwac_xwds_device_h__

#include "board/std.h"
#include <xwcd/ds/xwds.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/uart/controller.h>
#include <xwcd/ds/spi/master.h>
#include <xwcd/perpheral/spi/flash/w25qxx/device.h>
#include <xwcd/perpheral/spi/lcd/st7735/device.h>
#include <xwcd/ds/i2c/master.h>
#include <xwcd/perpheral/i2c/eeprom/device.h>

/******** ******** ds ******** ********/
extern struct xwds stm32xwds;
void stm32xwds_init(void);
void stm32xwds_fini(void);

/******** ******** soc ******** ********/
extern struct xwds_soc stm32xwds_soc;
xwer_t stm32xwds_soc_init(void);
xwer_t stm32xwds_soc_fini(void);

/******** ******** uart ******** ********/
extern struct xwds_uartc stm32xwds_usart1;
extern struct xwds_uartc stm32xwds_usart3;
xwer_t stm32xwds_uart_init(void);
xwer_t stm32xwds_uart_fini(void);

/******** ******** spi ******** ********/
extern struct xwds_spim stm32xwds_spi1m;
extern struct xwds_spim stm32xwds_spi4m;
xwer_t stm32xwds_spi_init(void);
xwer_t stm32xwds_spi_fini(void);

extern struct xwds_st7735 stm32xwds_st7735;
xwer_t stm32xwds_st7735_init(void);
xwer_t stm32xwds_st7735_fini(void);

extern struct xwds_w25qxx stm32xwds_w25q64jv;
xwer_t stm32xwds_w25q64jv_init(void);
xwer_t stm32xwds_w25q64jv_fini(void);

/******** ******** i2c ******** ********/
extern struct xwds_i2cm stm32xwds_i2c1m;
xwer_t stm32xwds_i2c_init(void);
xwer_t stm32xwds_i2c_fini(void);

extern struct xwds_eeprom stm32xwds_eeprom256k;
xwer_t stm32xwds_eeprom_init(void);
xwer_t stm32xwds_eeprom_fini(void);

#endif /* bm/xwac/xwds/device.h */
