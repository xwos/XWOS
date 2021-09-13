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
#include <xwos/mm/mempool/allocator.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/cubemx/Core/Inc/sdio.h>
#include <bm/stm32cube/xwac/xwds/cmif.h>
#include <bm/stm32cube/mif.h>

extern xwu8_t sram_mr_origin[];
extern xwu8_t sram_mr_size[];

/**
 * @brief SRAM内存池
 * @note
 * + 头文件
 *   - xwbd/atkf103core/bm/stm32cube/mif.h
 *   - xwos/mm/mempool/allocator.h
 * + API
 *   - 申请：xwmm_mempool_malloc(sram_mempool, ...)
 *   - 释放：xwmm_mempool_free(sram_mempool, ...)
 *   - 重新申请：xwmm_mempool_realloc(sram_mempool, ...)
 */
struct xwmm_mempool * sram_mempool = (void *)sram_mr_origin;

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
 * @brief STM32CUBE模块的低级初始化
 * @retrun 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：低级初始化流程
 * - 重入性：不可重入
 */
__xwbsp_init_code
void stm32cube_lowlevel_init(void)
{
        SystemInit();
}

/**
 * @brief STM32CUBE模块的初始化
 * @retrun 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：初始化流程
 * - 重入性：不可重入
 */
__xwbsp_init_code
void stm32cube_init(void)
{
        xwer_t rc;

        HAL_Init();
        SystemClock_Config();

        rc = stm32cube_xwds_ll_start();
        BDL_BUG_ON(rc < 0);

#if defined(STM32CUBECFG_SRAM) && (1 == STM32CUBECFG_SRAM)
        xwssq_t odr;

        odr = xwbop_fls(xwsz_t, (xwsz_t)sram_mr_size / XWMM_MEMPOOL_PAGE_SIZE);
        rc = xwmm_mempool_init(sram_mempool, "SRAM",
                               (xwptr_t)sram_mr_origin,
                               (xwsz_t)sram_mr_size,
                               (xwsz_t)odr);
        BDL_BUG_ON(rc < 0);
#endif
}

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
xwer_t stm32cube_start(void)
{
        xwer_t rc;

        /* xwds */
        rc = stm32cube_xwds_start();
        if (rc < 0) {
                goto err_xwds_start;
        }

        /* SD */
        MX_SDIO_SD_Init();
        rc = MX_SDIO_SD_TrimClk(4);
        if (rc < 0) {
                goto err_sd_init;
        }
        return XWOK;


err_sd_init:
        stm32cube_xwds_stop();
err_xwds_start:
        return rc;
}

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
xwer_t stm32cube_stop(void)
{
        xwer_t rc;

        /* SD */
        MX_SDIO_SD_DeInit();

        /* xwds */
        rc = stm32cube_xwds_stop();
        if (rc < 0) {
                goto err_xwds_stop;
        }
        return XWOK;

err_xwds_stop:
        return rc;
}
