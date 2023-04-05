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
#include <xwcd/soc/arm/v7m/m4/stm32/soc_init.h>
#include <string.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#if defined(XWOS_CFG_CORE__mp)
#  include <xwos/mp/thd.h>
#  include <xwos/mp/swt.h>
#  include <xwos/mp/sync/sem.h>
#  include <xwos/mp/sync/cond.h>
#  include <xwos/mp/lock/mtx.h>
#endif
#include <bm/stm32cube/mif.h>

#define STKMEMPOOL_BLKSZ        BRDCFG_MM_STKMEMPOOL_BLKSZ
#define STKMEMPOOL_BLKODR       BRDCFG_MM_STKMEMPOOL_BLKODR

#define CCMHEAP_BLKSZ           BRDCFG_MM_CCMHEAP_BLKSZ
#define CCMHEAP_BLKODR          BRDCFG_MM_CCMHEAP_BLKODR

extern xwsz_t stkmempool_mr_origin[];
extern xwsz_t stkmempool_mr_size[];

extern xwsz_t ccmheap_mr_origin[];
extern xwsz_t ccmheap_mr_size[];

/**
 * @brief thread stack mempool zone
 */
__xwbsp_data XWMM_BMA_DEF(stkmempool_bma_raw, STKMEMPOOL_BLKODR);
__xwbsp_data struct xwmm_bma * stkmempool_bma = (void *)&stkmempool_bma_raw;

/**
 * @brief CCMRAM zone
 */
__xwbsp_data XWMM_BMA_DEF(ccmheap_bma_raw, CCMHEAP_BLKODR);
__xwbsp_data struct xwmm_bma * ccmheap_bma = (void *)&ccmheap_bma_raw;

/**
 * @brief 初始化内存管理
 */
static __xwbsp_init_code
void board_mm_init(void)
{
        xwer_t rc;

        rc = xwmm_bma_init(stkmempool_bma, "stkmempool",
                           (xwptr_t)stkmempool_mr_origin,
                           (xwsz_t)stkmempool_mr_size,
                           STKMEMPOOL_BLKSZ,
                           STKMEMPOOL_BLKODR);
        BDL_BUG_ON(rc < 0);

        rc = xwmm_bma_init(ccmheap_bma, "ccmheap",
                           (xwptr_t)ccmheap_mr_origin,
                           (xwsz_t)ccmheap_mr_size,
                           CCMHEAP_BLKSZ,
                           CCMHEAP_BLKODR);
        BDL_BUG_ON(rc < 0);

#if defined(XWOS_CFG_CORE__mp)
        void * mem;

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_THD_CACHE_ODR, &mem);
        BDL_BUG_ON(rc < 0);
        rc = xwmp_thd_cache_init((xwptr_t)mem,
                                 (CCMHEAP_BLKSZ << BRDCFG_XWOS_THD_CACHE_ODR));
        BDL_BUG_ON(rc < 0);

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_SWT_CACHE_ODR, &mem);
        BDL_BUG_ON(rc < 0);
        rc = xwmp_swt_cache_init((xwptr_t)mem,
                                 (CCMHEAP_BLKSZ << BRDCFG_XWOS_SWT_CACHE_ODR));
        BDL_BUG_ON(rc < 0);

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_SEM_CACHE_ODR, &mem);
        BDL_BUG_ON(rc < 0);
        rc = xwmp_sem_cache_init((xwptr_t)mem,
                                 (CCMHEAP_BLKSZ << BRDCFG_XWOS_SEM_CACHE_ODR));
        BDL_BUG_ON(rc < 0);

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_COND_CACHE_ODR, &mem);
        BDL_BUG_ON(rc < 0);
        rc = xwmp_cond_cache_init((xwptr_t)mem,
                                  (CCMHEAP_BLKSZ << BRDCFG_XWOS_COND_CACHE_ODR));
        BDL_BUG_ON(rc < 0);

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_MTX_CACHE_ODR, &mem);
        BDL_BUG_ON(rc < 0);
        rc = xwmp_mtx_cache_init((xwptr_t)mem,
                                 (CCMHEAP_BLKSZ << BRDCFG_XWOS_MTX_CACHE_ODR));
        BDL_BUG_ON(rc < 0);
#endif
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
