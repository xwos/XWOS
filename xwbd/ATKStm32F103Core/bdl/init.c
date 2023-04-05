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

#include <xwos/standard.h>
#include <bdl/standard.h>
#include <xwcd/soc/arm/v7m/arch_init.h>
#include <xwcd/soc/arm/v7m/m3/stm32/soc_init.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#include <bm/stm32cube/mif.h>

#define OCHEAP_BLKSZ    BRDCFG_MM_OCHEAP_BLKSZ
#define OCHEAP_BLKODR   BRDCFG_MM_OCHEAP_BLKODR

extern xwsz_t ocheap_mr_origin[];
extern xwsz_t ocheap_mr_size[];

/**
 * @brief thread stack mempool zone
 */
__xwbsp_data XWMM_BMA_DEF(ocheap_bma_raw, OCHEAP_BLKODR);
__xwbsp_data struct xwmm_bma * ocheap_bma = (void *)ocheap_bma_raw;

/**
 * @brief 初始化内存管理
 */
static __xwbsp_init_code
void board_mm_init(void)
{
        xwer_t rc;

        rc = xwmm_bma_init(ocheap_bma, "ocheap",
                           (xwptr_t)ocheap_mr_origin, (xwsz_t)ocheap_mr_size,
                           OCHEAP_BLKSZ, OCHEAP_BLKODR);
        BDL_BUG_ON(rc < 0);
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
