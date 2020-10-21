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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <bm/stm32cube/standard.h>
#include <xwos/mm/mempool/allocator.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/cubemx/Core/Inc/isr.h>
#include <bm/stm32cube/xwac/xwds/cmif.h>
#include <bm/stm32cube/mif.h>
#include <armv7m_core.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwu8_t sram_mr_origin[];
extern xwu8_t sram_mr_size[];

/**
 * @brief External SRAM
 */
struct xwmm_mempool * sram_mempool = (void *)sram_mr_origin;

/**
 * @brief 连接占位符
 * @note
 * + 确保链接时使用此符号的文件。
 */
void * const stm32cube_linkage_placeholder[] = {
        stm32cube_override_linkage_msp,
        stm32cube_override_linkage_it,
};

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
__xwbsp_init_code
void stm32cube_lowlevel_init(void)
{
        SystemInit();
        /* cm_scs.scnscb.actlr.bit.disdefwbuf = 1; */
}

__xwbsp_init_code
void stm32cube_init(void)
{
        xwer_t rc;

        HAL_Init();
        SystemClock_Config();

        rc = stm32cube_xwds_ll_start();
        BDL_BUG_ON(rc < 0);

#if defined(STM32CUBECFG_SRAM) && (1 == STM32CUBECFG_SRAM)
        rc = xwmm_mempool_init(sram_mempool, "SRAM",
                               (xwptr_t)sram_mr_origin,
                               (xwsz_t)sram_mr_size);
        BDL_BUG_ON(rc < 0);
#endif /* STM32CUBECFG_SRAM */
}

xwer_t stm32cube_start(void)
{
        xwer_t rc;

        /* xwds */
        rc = stm32cube_xwds_start();
        if (rc < 0) {
                goto err_xwds_start;
        }

        return XWOK;

err_xwds_start:
        return rc;
}

xwer_t stm32cube_stop(void)
{
        xwer_t rc;

        /* xwds */
        rc = stm32cube_xwds_stop();
        if (rc < 0) {
                goto err_xwds_stop;
        }
        return XWOK;

err_xwds_stop:
        return rc;
}