/**
 * @file
 * @brief STM32CUBE：重写某些override属性的函数
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <bm/stm32cube/standard.h>
#include <arch_systick.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern uint32_t uwTickPrio;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void stm32cube_override_linkage_placeholder(void)
{
        /* 链接时，优先使用此文件中的符号 */
}

void HAL_MspInit(void)
{
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_1);

        HAL_EnableDBGSleepMode();
        HAL_EnableDBGStopMode();
        HAL_EnableDBGStandbyMode();
}

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
        (void)TickPriority;
        uwTickPrio = ARCH_IRQ_TICK_PRIO;
        return HAL_OK;
}
