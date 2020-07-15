/**
 * @file
 * @brief STM32CUBE：初始化
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
#include <xwos/mm/mempool/allocator.h>
#include <bm/stm32cube/standard.h>
#include <bm/stm32cube/init.h>
#include "cubemx/Core/Inc/gpio.h"
#include "cubemx/Core/Inc/fsmc.h"
#include "cubemx/Core/Inc/main.h"

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SRAM_ORIGIN     0x68000000UL
#define SRAM_SIZE       0x100000UL

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/**
 * @brief External SRAM
 */
struct xwmm_mempool * sram_mempool = (void *)SRAM_ORIGIN;

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
        xwer_t rc;

        HAL_Init();
        SystemClock_Config();

        MX_GPIO_Init();
        MX_FSMC_Init();

        rc = xwmm_mempool_init(sram_mempool, "SRAM",
                               (xwptr_t)SRAM_ORIGIN,
                               (xwsz_t)SRAM_SIZE);
        BDL_BUG_ON(rc < 0);
}
