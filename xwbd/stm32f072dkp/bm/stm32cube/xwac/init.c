/**
 * @file
 * @brief STM32CUBEMX：初始化
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <bm/stm32cube/standard.h>
#include <xwos/osal/scheduler.h>
#include <arch_irq.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/xwac/override.h>
#include <bm/stm32cube/xwac/init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwsz_t eram_mr_origin[];
extern xwsz_t eram_mr_size[];

void (*stm32cube_override_placeholder)(void) = stm32cube_override_placeholder_stub;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern
void SystemInit(void);

extern
void SystemClock_Config(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
}
