/**
 * @file
 * @brief STM32CUBE模块：接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include <bm/stm32cube/standard.h>
#include <armv7m_isa.h>
#include <xwos/mm/mempool/allocator.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/xwac/xwds/cmif.h>
#include <bm/stm32cube/xwac/fatfs/cmif.h>
#include <bm/stm32cube/mif.h>

extern xwu8_t sram_mr_origin[];
extern xwu8_t sram_mr_size[];

/**
 * @brief SRAM内存池
 * @note
 * + 头文件
 *   + `xwbd/atkf103core/bm/stm32cube/mif.h`
 *   + `xwos/mm/mempool/allocator.h`
 * + API
 *   + 申请： `xwmm_mempool_malloc(sram_mempool, ...)`
 *   + 释放： `xwmm_mempool_free(sram_mempool, ...)`
 *   + 重新申请： `xwmm_mempool_realloc(sram_mempool, ...)`
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
 * @details
 * 此函数在 `board_lowlevel_init()` 中被调用。只能配置部分寄存器，不可访问全局变量。
 */
__xwbsp_init_code
void stm32cube_lowlevel_init(void)
{
        SystemInit();
#if defined(STM32CUBECFG_DISDEFWBUF) && (1 == STM32CUBECFG_DISDEFWBUF)
        cm_scs.scnscb.actlr.bit.disdefwbuf = 1;
#endif
}

/**
 * @brief STM32CUBE模块的初始化
 * @details
 * 此函数在 `board_init()` 中被调用。
 */
__xwbsp_init_code
void stm32cube_init(void)
{
        xwer_t rc;

        HAL_Init();
        SystemClock_Config();

        rc = stm32cube_xwds_probe();
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
 * @note
 * - 上下文：线程
 */
xwer_t stm32cube_start(void)
{
        xwer_t rc;

        /* xwds */
        rc = stm32cube_xwds_start();
        if (rc < 0) {
                goto err_xwds_start;
        }

        /* fatfs */
        rc = sdcard_fatfs_mount();
        if (rc < 0) {
                goto err_fatfs_mount;
        }
        return XWOK;


err_fatfs_mount:
err_xwds_start:
        return rc;
}

/**
 * @brief 停止STM32CUBE模块
 * @note
 * - 上下文：线程
 */
xwer_t stm32cube_stop(void)
{
        xwer_t rc;

        /* fatfs */
        sdcard_fatfs_unmount();

        /* xwds */
        rc = stm32cube_xwds_stop();
        if (rc < 0) {
                goto err_xwds_stop;
        }
        return XWOK;

err_xwds_stop:
        return rc;
}
