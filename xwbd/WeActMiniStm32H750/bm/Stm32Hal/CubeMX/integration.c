/**
 * @file
 * @brief stm32hal::CubeMX::integration
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

#include "board/std.h"
#include "bm/Stm32Hal/CubeMX/Core/Inc/main.h"

/**
 * @brief 链接选择表
 * @details
 * 用于强制链接表中符号所在的.c/.cxx文件中的其他符号。
 */
void * const stm32cubemx_linkage_table[] = {
        stm32cubemx_override_linkage_msp,
        stm32cubemx_override_linkage_it,
};

extern
void SystemInit(void);

extern
void SystemClock_Config(void);

extern
void stm32cubemx_init_vtor(void);

/**
 * @brief STM32CUBE模块：预初始化
 * @details
 * + 此函数在 `xwos_preinit()` 中被调用。
 * + 此函数被设计为，在重定向数据 `soc_relocate_data()` 函数之 **前** 调用。
 *   只能配置部分寄存器，不可访问全局变量。
 */
__xwbsp_init_code
void stm32cubemx_preinit(void)
{
        SystemInit();
        __HAL_RCC_D2SRAM1_CLK_ENABLE();
        __HAL_RCC_D2SRAM2_CLK_ENABLE();
        __HAL_RCC_D2SRAM3_CLK_ENABLE();
        __HAL_RCC_D3SRAM1_CLKAM_ENABLE();
        stm32cubemx_init_vtor(); /* 覆盖HAL库中的VTOR的值 */
}

/**
 * @brief STM32CUBEMX模块：初始化
 * @details
 * + 此函数可在 `xwos_preinit()` 或 `xwos_postinit()` 中被调用。
 * + 此函数被设计为，在重定向数据 `soc_relocate_data()` 函数之 **后** 调用。
 *   可访问全局变量。
 */
__xwbsp_init_code
void stm32cubemx_init(void)
{
        HAL_Init();
        SystemClock_Config();
}
