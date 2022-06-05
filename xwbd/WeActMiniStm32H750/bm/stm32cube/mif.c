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

#include <xwos/lib/xwbop.h>
#include <bm/stm32cube/standard.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/xwac/xwlib/crc.h>
#include <bm/stm32cube/xwac/xwds/cmif.h>
#include <bm/stm32cube/xwac/fatfs/cmif.h>
#include <bm/stm32cube/mif.h>

extern xwsz_t itcm_mr_origin[];
extern xwsz_t itcm_mr_size[];

extern xwsz_t dtcm_mr_origin[];
extern xwsz_t dtcm_mr_size[];

extern xwsz_t axisram_mr_origin[];
extern xwsz_t axisram_mr_size[];

extern xwu8_t data4_lma_base[];
extern xwu8_t data4_vma_base[];
extern xwu8_t data4_vma_end[];

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

static
void axisram_init(void);

static
void sram4_init(void);

/**
 * @brief STM32CUBE模块的低级初始化
 * @details
 * 此函数在 `board_lowlevel_init()` 中被调用。只能配置部分寄存器，不可访问全局变量。
 */
__xwbsp_init_code
void stm32cube_lowlevel_init(void)
{
        SystemInit();
        __HAL_RCC_D2SRAM1_CLK_ENABLE();
        __HAL_RCC_D2SRAM2_CLK_ENABLE();
        __HAL_RCC_D2SRAM3_CLK_ENABLE();
        __HAL_RCC_D3SRAM1_CLKAM_ENABLE();
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
        axisram_init();
        sram4_init();

        rc = stm32cube_xwds_probe();
        BDL_BUG_ON(rc < 0);

        /*
           若SDRAM、QSPI Flash等可映射到内存地址上的器件未初始化完成，
           开启Cache可能会因为Cache的预取操作导致宕机。
         */
#if defined(STM32CUBECFG_ICACHE) && (1 == STM32CUBECFG_ICACHE)
        SCB_EnableICache();
#endif
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
        SCB_EnableDCache();
        SCB_CleanInvalidateDCache();
#endif

        stm32cube_crc_init();
}

/**
 * @brief 启动STM32CUBE模块
 * + 上下文：线程
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
        stm32cube_xwds_stop();
err_xwds_start:
        return rc;
}

/**
 * @brief 停止STM32CUBE模块
 * + 上下文：线程
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

static
void axisram_init(void)
{
        xwsq_t i;
        xwsz_t * origin = (xwsz_t *)axisram_mr_origin;
        xwsz_t n = (xwsz_t)axisram_mr_size / sizeof(xwsz_t);

        for (i = 0; i < n; i++) {
                origin[i] = 0;
        }
}

static
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
