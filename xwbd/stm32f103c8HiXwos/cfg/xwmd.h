/**
 * @file
 * @brief 中间件/模块/驱动配置
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

#ifndef __cfg_xwmd_h__
#define __cfg_xwmd_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    base config    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_CHECK_PARAMETERS                                1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   device stack    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_ds                                              1
#define XWMDCFG_ds_PM                                           1

/******** ******** SOC ******** ********/
#define XWMDCFG_ds_SOC                                          1
#define XWMDCFG_ds_SOC_CLK                                      1
#define XWMDCFG_ds_SOC_PWR                                      1
#define XWMDCFG_ds_SOC_GPIO                                     1
#define XWMDCFG_ds_SOC_EIRQ                                     1
#define XWMDCFG_ds_SOC_EIRQ_ROISRT                              0
#define XWMDCFG_ds_SOC_DMA                                      1
#define XWMDCFG_ds_SOC_DMA_ROCBT                                1
#define XWMDCFG_ds_ERAM                                         0

/******** ******** UART ******** ********/
#define XWMDCFG_ds_UART                                         1
#define XWMDCFG_ds_UART_GNR                                     1
#define XWMDCFG_ds_UART_GNR_TXQ_SIZE                            (256U)
#define XWMDCFG_ds_UART_GNR_RXQ_SIZE                            (256U)
#define XWMDCFG_ds_UART_DMA                                     0
#define XWMDCFG_ds_UART_DMA_RXQ_SIZE                            (256U)

/******** ******** I2C device ******** ********/
#define XWMDCFG_ds_I2C_MASTER                                   1
#define XWMDCFG_ds_I2C_PERPHERAL                                1

/******** ******** SPI device ******** ********/
#define XWMDCFG_ds_SPI_MASTER                                   1
#define XWMDCFG_ds_SPI_PERPHERAL                                1

/******** ******** LIN Bus ******** ********/
#define XWMDCFG_ds_LIN                                          0
#define XWMDCFG_ds_LINID                                        1

/******** ******** CAN Bus ******** ********/
#define XWMDCFG_ds_CAN                                          1
#define XWMDCFG_ds_CAN_CONTROLLER                               1
#define XWMDCFG_ds_CAN_CONTROLLER_ROCBT                         0
#define XWMDCFG_ds_CAN_CONTROLLER_SDU_MAXSIZE                   8U
#define XWMDCFG_ds_CAN_CONTROLLER_RXQNUM                        8U
#define XWMDCFG_ds_CAN_CONTROLLER_I2CP                          0
#define XWMDCFG_ds_CAN_CONTROLLER_SPIP                          0
#define XWMDCFG_ds_CAN_TRANSCEIVER                              1
#define XWMDCFG_ds_CAN_TRANSCEIVER_I2CP                         0
#define XWMDCFG_ds_CAN_TRANSCEIVER_SPIP                         0

/******** ******** misc device ******** ********/
#define XWMDCFG_ds_MISC                                         0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     Inter-System Communication      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** point to point communication protocol ******** ********/
#define XWMDCFG_isc_xwpcp                                       1
#define XWMDCFG_isc_xwpcp_LOG                                   0
#define XWMDCFG_isc_xwpcp_PORT_NUM                              (12U)
#define XWMDCFG_isc_xwpcp_PRIORITY_NUM                          (4U)
#define XWMDCFG_isc_xwpcp_RETRY_PERIOD                          (1 * XWTM_S)
#define XWMDCFG_isc_xwpcp_RETRY_NUM                             (8U)
#define XWMDCFG_isc_xwpcp_MEMBLK_SIZE                           (64U)
#define XWMDCFG_isc_xwpcp_MEMBLK_NUM                            (64U)
#define XWMDCFG_isc_xwpcp_SYNC_KEY                              ('P')
#define XWMDCFG_isc_xwpcp_RXTHRD_PRIORITY                       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)
#define XWMDCFG_isc_xwpcp_TXTHRD_PRIORITY                       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

/******** ******** simple communication protocol ******** ********/
#define XWMDCFG_isc_xwscp                                       1
#define XWMDCFG_isc_xwscp_LOG                                   0
#define XWMDCFG_isc_xwscp_PERIOD                                (1 * XWTM_S)
#define XWMDCFG_isc_xwscp_RETRY_NUM                             (8U)
#define XWMDCFG_isc_xwscp_SDU_MAX_SIZE                          (40U)
#define XWMDCFG_isc_xwscp_FRMSLOT_NUM                           (8U)
#define XWMDCFG_isc_xwscp_SYNC_KEY                              ('S')
#define XWMDCFG_isc_xwscp_THRD_PRIORITY                         \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********            C/C++ Runtime            ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_libc_newlibac                                   0
#define XWMDCFG_cxx                                             0

#endif /* cfg/xwmd.h */
