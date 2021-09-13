/**
 * @file
 * @brief 设备驱动配置
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

#ifndef __cfg_xwcd_h__
#define __cfg_xwcd_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    Base Config    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWCDCFG_CHECK_PARAMETERS                                0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   Device Stack    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWCDCFG_ds                                              1
#define XWCDCFG_ds_LITE                                         1
#define XWCDCFG_ds_PM                                           1

/******** ******** SOC ******** ********/
#define XWCDCFG_ds_SOC                                          1
#define XWCDCFG_ds_SOC_CLK                                      0
#define XWCDCFG_ds_SOC_PWR                                      0
#define XWCDCFG_ds_SOC_GPIO                                     1
#define XWCDCFG_ds_SOC_EIRQ                                     1
#define XWCDCFG_ds_SOC_EIRQ_ROISRT                              0
#define XWCDCFG_ds_SOC_DMA                                      0
#define XWCDCFG_ds_SOC_DMA_ROCBT                                0
#define XWCDCFG_ds_SOC_ERAM                                     1

/******** ******** UART ******** ********/
#define XWCDCFG_ds_UART                                         1
#define XWCDCFG_ds_UART_GNR                                     1
#define XWCDCFG_ds_UART_GNR_TXQ_SIZE                            (128U)
#define XWCDCFG_ds_UART_GNR_RXQ_SIZE                            (128U)
#define XWCDCFG_ds_UART_DMA                                     1
#define XWCDCFG_ds_UART_DMA_RXQ_SIZE                            (256U)

/******** ******** I2C device ******** ********/
#define XWCDCFG_ds_I2C_MASTER                                   1
#define XWCDCFG_ds_I2C_PERPHERAL                                1

/******** ******** SPI device ******** ********/
#define XWCDCFG_ds_SPI_MASTER                                   1
#define XWCDCFG_ds_SPI_PERPHERAL                                1

/******** ******** LIN Bus ******** ********/
#define XWCDCFG_ds_LIN                                          1
#define XWCDCFG_ds_LINID                                        1

/******** ******** CAN Bus ******** ********/
#define XWCDCFG_ds_CAN                                          1
#define XWCDCFG_ds_CAN_CONTROLLER                               1
#define XWCDCFG_ds_CAN_CONTROLLER_ROCBT                         0
#define XWCDCFG_ds_CAN_CONTROLLER_SDU_MAXSIZE                   8U
#define XWCDCFG_ds_CAN_CONTROLLER_RXQNUM                        8U
#define XWCDCFG_ds_CAN_CONTROLLER_I2CP                          0
#define XWCDCFG_ds_CAN_CONTROLLER_SPIP                          0
#define XWCDCFG_ds_CAN_TRANSCEIVER                              1
#define XWCDCFG_ds_CAN_TRANSCEIVER_I2CP                         0
#define XWCDCFG_ds_CAN_TRANSCEIVER_SPIP                         0

/******** ******** misc device ******** ********/
#define XWCDCFG_ds_MISC                                         1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   I2C perpheral   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWCDCFG_perpheral_i2c_eeprom                            1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   SPI perpheral   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWCDCFG_perpheral_spi_flash_w25qxx                      1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  CAN transceiver  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWCDCFG_perpheral_can_transceiver_tja1042               1

#endif /* cfg/xwcd.h */
