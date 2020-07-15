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
#include <bm/stm32cube/standard.h>
#include <xwos/mm/mempool/allocator.h>
#include <bm/stm32cube/xwac/xwds/init.h>
#include <bm/stm32cube/xwac/xwds/stm32cube.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwsz_t itcm_mr_origin[];
extern xwsz_t itcm_mr_size[];

extern xwsz_t dtcm_mr_origin[];
extern xwsz_t dtcm_mr_size[];

extern xwsz_t axisram_mr_origin[];
extern xwsz_t axisram_mr_size[];

extern xwsz_t sdram_mr_origin[];
extern xwsz_t sdram_mr_size[];

extern xwu8_t data4_lma_base[];
extern xwu8_t data4_vma_base[];
extern xwu8_t data4_vma_end[];

/**
 * @brief External SDRAM zone
 */
struct xwmm_mempool * sdram_mempool = (void *)sdram_mr_origin;

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
        SCB_EnableICache();
        SCB_EnableDCache();
        SystemInit();
        __HAL_RCC_D2SRAM1_CLK_ENABLE();
        __HAL_RCC_D2SRAM2_CLK_ENABLE();
        __HAL_RCC_D2SRAM3_CLK_ENABLE();
        __HAL_RCC_D3SRAM1_CLKAM_ENABLE();
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
        axisram_init();
        sram4_init();

        rc = stm32cube_xwds_ll_start();
        BDL_BUG_ON(rc < 0);

        rc = xwmm_mempool_init(sdram_mempool, "SDRAM",
                               (xwptr_t)sdram_mr_origin,
                               (xwsz_t)sdram_mr_size);
        BDL_BUG_ON(rc < 0);
}

void axisram_init(void)
{
        xwsq_t i;
        xwsz_t * origin = (xwsz_t *)axisram_mr_origin;
        xwsz_t n = (xwsz_t)axisram_mr_size / sizeof(xwsz_t);

        for (i = 0; i < n; i++) {
                origin[i] = 0;
        }
}

void sram4_init(void)
{
        xwsz_t count, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = data4_lma_base;
        dst = data4_vma_base;
        if (dst != src) {
                count = (xwsz_t)data4_vma_end - (xwsz_t)data4_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }
}
