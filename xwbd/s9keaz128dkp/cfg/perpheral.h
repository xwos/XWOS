/**
 * @file
 * @brief 外设驱动配置
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

#ifndef __cfg_perpheral_h__
#define __cfg_perpheral_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   I2C perpheral   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define PPCFG_i2c_eeprom                                0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   SPI perpheral   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define PPCFG_spi_flash_w25qxx                          0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  CAN transceiver  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define PPCFG_can_transceiver_tja1042                   0

#endif /* cfg/perpheral.h */
