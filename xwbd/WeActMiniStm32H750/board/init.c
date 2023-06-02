/**
 * @file
 * @brief 板级描述层：初始化
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

#include "board/std.h"
#include <xwcd/soc/arm/v7m/arch_init.h>
#include <xwcd/soc/arm/v7m/m7/stm32/soc_init.h>
#include <string.h>
#include <xwos/mm/bma.h>
#include <xwos/mm/mempool/allocator.h>
#include "bm/stm32cube/mif.h"

#define DTCMHEAP_BLKSZ          BRDCFG_MM_DTCMHEAP_BLKSZ
#define DTCMHEAP_BLKODR         BRDCFG_MM_DTCMHEAP_BLKODR

extern xwsz_t axisram_mr_origin[];
extern xwsz_t axisram_mr_size[];
extern xwsz_t dtcmheap_mr_origin[];
extern xwsz_t dtcmheap_mr_size[];

/**
 * @brief AXISRAM内存池
 * @note
 * + 头文件
 *   - board/axisram.h
 * + API
 *   - 申请：xwmm_mempool_malloc(axisram_mempool, ...)
 *   - 释放：xwmm_mempool_free(axisram_mempool, ...)
 *   - 重新申请：xwmm_mempool_realloc(axisram_mempool, ...)
 */
__xwbsp_data struct xwmm_mempool * axisram_mempool = (void *)axisram_mr_origin;

/**
 * @brief CCMRAM zone
 */
__xwbsp_data XWMM_BMA_DEF(dtcmheap_bma_objrawmem, DTCMHEAP_BLKODR);
__xwbsp_data struct xwmm_bma * dtcmheap_bma = (void *)&dtcmheap_bma_objrawmem;


/**
 * @brief 初始化内存管理
 */
static __xwbsp_init_code
void board_mm_init(void)
{
        xwer_t rc;
        xwssq_t odr;

        odr = xwbop_fls(xwsz_t, (xwsz_t)axisram_mr_size / XWMM_MEMPOOL_PAGE_SIZE);
        rc = xwmm_mempool_init(axisram_mempool, "AXISRAM",
                               (xwptr_t)axisram_mr_origin,
                               (xwsz_t)axisram_mr_size,
                               (xwsz_t)odr);
        BOARD_BUG_ON(rc < 0);

        rc = xwmm_bma_init(dtcmheap_bma, "dtcmheap",
                           (xwptr_t)dtcmheap_mr_origin, (xwsz_t)dtcmheap_mr_size,
                           DTCMHEAP_BLKSZ, DTCMHEAP_BLKODR);
        BOARD_BUG_ON(rc < 0);
}

/**
 * @brief XWOS预初始化
 */
__xwbsp_init_code
void xwos_preinit(void)
{
        arch_init();
        soc_init();
        stm32cube_lowlevel_init();
        soc_relocate_data();
        soc_relocate_ivt();
}

/**
 * @brief XWOS后初始化
 */
__xwbsp_init_code
void xwos_postinit(void)
{
        stm32cube_init();
        board_mm_init();
}
