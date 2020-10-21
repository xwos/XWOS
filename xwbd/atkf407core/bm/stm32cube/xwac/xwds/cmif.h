/**
 * @file
 * @brief STM32CUBE：子模块接口
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <bm/stm32cube/xwac/xwds/device.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 准备启动设备栈
 * @retrun 错误码
 * @note
 * - 在初始化过程中调用此函数，此函数中只能初始化SOC的基本资源，
 *   例如：GPIO、时钟等。
 * @note
 * - 同步/异步：同步
 * - 上下文：初始化流程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_ll_start(void);

/**
 * @brief 准备停止设备栈
 * @retrun 错误码
 * @note
 * - 在反初始化过程中调用此函数，此函数中只能停止SOC的基本资源，
 *   例如：GPIO、时钟等。
 * @note
 * - 同步/异步：同步
 * - 上下文：反初始化流程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_ll_stop(void);

/**
 * @brief 启动设备栈
 * @retrun 错误码
 * @note
 * - 此函数会启动所有外设，有些外设启动流程需要延时，
 *   因此此函数只能运行在线程中。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_start(void);

/**
 * @brief 停止设备栈
 * @retrun 错误码
 * @note
 * - 此函数会停止所有外设，有些外设的停止流程需要延时，
 *   因此此函数只能运行在线程中。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_stop(void);

/**
 * @brief 启动SOC
 * @retrun 错误码
 * @note
 * - 已经由@ref stm32cube_xwds_ll_start()调用。
 * @note
 * - 同步/异步：同步
 * - 上下文：初始化流程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_soc_start(void);

/**
 * @brief 停止SOC
 * @retrun 错误码
 * @note
 * - 已经由@ref stm32cube_xwds_ll_stop()调用。
 * @note
 * - 同步/异步：同步
 * - 上下文：反初始化流程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_soc_stop(void);

/**
 * @brief 启动UART
 * @retrun 错误码
 * @note
 * - 已经由@ref stm32cube_xwds_start()调用。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_uart_start(void);

/**
 * @brief 停止UART
 * @retrun 错误码
 * @note
 * - 已经由@ref stm32cube_xwds_stop()调用。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_uart_stop(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* bm/stm32cube/xwac/xwds/cmif.h */
