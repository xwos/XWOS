/**
 * @file
 * @brief STM32CUBE模块：接口
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

#ifndef __bm_stm32cube_mif_h__
#define __bm_stm32cube_mif_h__

#include <bm/stm32cube/standard.h>

/**
 * @brief STM32CUBE模块的低级初始化
 * @retrun 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：低级初始化流程
 * - 重入性：不可重入
 */
void stm32cube_lowlevel_init(void);

/**
 * @brief STM32CUBE模块的初始化
 * @retrun 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：初始化流程
 * - 重入性：不可重入
 */
void stm32cube_init(void);

/**
 * @brief 启动STM32CUBE模块
 * @retrun 错误码
 * @note
 * - 此函数会启动所有外设，有些外设启动流程需要延时，因此此函数只能运行在线程中。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
xwer_t stm32cube_start(void);

/**
 * @brief 停止STM32CUBE模块
 * @retrun 错误码
 * @note
 * - 此函数会停止所有外设，有些外设的停止流程需要延时，因此此函数只能运行在线程中。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
xwer_t stm32cube_stop(void);

#endif /* bm/stm32cube/mif.h */
