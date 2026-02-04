/**
 * @file
 * @brief Stm32Hal::mi 模块集成接口
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#ifndef bm_Stm32Hal_mi_h__
#define bm_Stm32Hal_mi_h__

#include <board/std.h>
#include "bm/Stm32Hal/xwds/device.h"

/**
 * @brief Stm32Hal模块：预初始化
 * @details
 * + 此函数在 `xwos_preinit()` 中被调用。
 */
void stm32hal_preinit(void);

/**
 * @brief Stm32Hal模块：初始化
 * @details
 * + 此函数在 `xwos_postinit()` 中被调用。
 */
void stm32hal_init(void);

/**
 * @brief Stm32Hal模块：后初始化
 * @details
 * + 此函数在主线程中被调用。
 */
void stm32hal_postinit(void);

#endif /* bm/Stm32Hal/mi.h */
