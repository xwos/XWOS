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

#include <bm/stm32cube/standard.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/cubemx/Core/Inc/gpio.h>
#include <bm/stm32cube/mif.h>

/**
 * @brief 连接占位符
 * @note
 * + 确保链接时使用此符号的文件。
 */
void * const stm32cube_linkage_table[] = {
        stm32cube_override_linkage_msp,
        stm32cube_override_linkage_it,
};

extern
void SystemInit(void);

extern
void SystemClock_Config(void);

/**
 * @brief Lowlevel-init stm32cube
 * @note
 * - Called by board_lowlevel_init()
 */
__xwbsp_init_code
void stm32cube_lowlevel_init(void)
{
        SystemInit();
}

/**
 * @brief Init stm32cube
 * @note
 * - Called by board_init()
 */
__xwbsp_init_code
void stm32cube_init(void)
{
        HAL_Init();
        SystemClock_Config();
        MX_GPIO_Init();
}

xwer_t stm32cube_start(void)
{
        return XWOK;
}

xwer_t stm32cube_stop(void)
{
        return XWOK;
}
