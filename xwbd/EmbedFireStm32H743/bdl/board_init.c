/**
 * @file
 * @brief 板级描述层：初始化
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

#include <xwos/standard.h>
#include <string.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#if defined(XuanWuOS_CFG_CORE__mp)
#  include <xwos/mp/thd.h>
#  include <xwos/mp/swt.h>
#  include <xwos/mp/sync/sem.h>
#  include <xwos/mp/sync/cond.h>
#  include <xwos/mp/lock/mtx.h>
#endif
#include <bm/stm32cube/mif.h>
#include <bdl/standard.h>
#include <bdl/board_init.h>

#define AXISRAM_BLKSZ           BRDCFG_MM_AXISRAM_BLKSZ
#define AXISRAM_BLKODR          BRDCFG_MM_AXISRAM_BLKODR

#define DTCMHEAP_BLKSZ          BRDCFG_MM_DTCMHEAP_BLKSZ
#define DTCMHEAP_BLKODR         BRDCFG_MM_DTCMHEAP_BLKODR

extern xwsz_t axisram_mr_origin[];
extern xwsz_t axisram_mr_size[];

extern xwsz_t dtcmheap_mr_origin[];
extern xwsz_t dtcmheap_mr_size[];

/**
 * @brief thread stack mempool zone
 */
__xwbsp_data XWMM_BMA_DEF(axisram_bma_raw, AXISRAM_BLKODR);
__xwbsp_data struct xwmm_bma * axisram_bma = (void *)&axisram_bma_raw;

/**
 * @brief CCMRAM zone
 */
__xwbsp_data XWMM_BMA_DEF(dtcmheap_bma_raw, DTCMHEAP_BLKODR);
__xwbsp_data struct xwmm_bma * dtcmheap_bma = (void *)&dtcmheap_bma_raw;

static __xwbsp_init_code
xwer_t sys_mm_init(void);

__xwbsp_init_code
void board_lowlevel_init(void)
{
        stm32cube_lowlevel_init();
}

/**
 * @brief init board
 */
__xwbsp_init_code
void board_init(void)
{
        xwer_t rc;

        stm32cube_init();

        rc = sys_mm_init();
        BDL_BUG_ON(rc < 0);
}

/**
 * @brief 初始化内存管理
 */
static __xwbsp_init_code
xwer_t sys_mm_init(void)
{
        xwer_t rc;

        rc = xwmm_bma_init(axisram_bma, "axisram",
                           (xwptr_t)axisram_mr_origin, (xwsz_t)axisram_mr_size,
                           AXISRAM_BLKSZ, AXISRAM_BLKODR);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_axisram_bma_init;
        }

        rc = xwmm_bma_init(dtcmheap_bma, "dtcmheap",
                           (xwptr_t)dtcmheap_mr_origin, (xwsz_t)dtcmheap_mr_size,
                           DTCMHEAP_BLKSZ, DTCMHEAP_BLKODR);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dtcmheap_bma_init;
        }

#if defined(XuanWuOS_CFG_CORE__mp)
        void * mem;

        rc = xwmm_bma_alloc(dtcmheap_bma, BRDCFG_XWOS_THD_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_thd_bma_alloc;
        }
        rc = xwmp_thd_cache_init((xwptr_t)mem,
                                 (DTCMHEAP_BLKSZ << BRDCFG_XWOS_THD_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_thd_cache_init;
        }

        rc = xwmm_bma_alloc(dtcmheap_bma, BRDCFG_XWOS_SWT_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_swt_bma_alloc;
        }
        rc = xwmp_swt_cache_init((xwptr_t)mem,
                                 (DTCMHEAP_BLKSZ << BRDCFG_XWOS_SWT_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_swt_cache_init;
        }

        rc = xwmm_bma_alloc(dtcmheap_bma, BRDCFG_XWOS_SEM_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_bma_alloc;
        }
        rc = xwmp_sem_cache_init((xwptr_t)mem,
                                 (DTCMHEAP_BLKSZ << BRDCFG_XWOS_SEM_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_cache_init;
        }

        rc = xwmm_bma_alloc(dtcmheap_bma, BRDCFG_XWOS_COND_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_cond_bma_alloc;
        }
        rc = xwmp_cond_cache_init((xwptr_t)mem,
                                  (DTCMHEAP_BLKSZ << BRDCFG_XWOS_COND_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_cond_cache_init;
        }

        rc = xwmm_bma_alloc(dtcmheap_bma, BRDCFG_XWOS_MTX_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_bma_alloc;
        }
        rc = xwmp_mtx_cache_init((xwptr_t)mem,
                                 (DTCMHEAP_BLKSZ << BRDCFG_XWOS_MTX_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_cache_init;
        }
#endif

        return XWOK;

#if defined(XuanWuOS_CFG_CORE__mp)
err_mtx_cache_init:
        BDL_BUG();
err_mtx_bma_alloc:
        BDL_BUG();
err_cond_cache_init:
        BDL_BUG();
err_cond_bma_alloc:
        BDL_BUG();
err_sem_cache_init:
        BDL_BUG();
err_sem_bma_alloc:
        BDL_BUG();
err_swt_cache_init:
        BDL_BUG();
err_swt_bma_alloc:
        BDL_BUG();
err_thd_cache_init:
        BDL_BUG();
err_thd_bma_alloc:
        BDL_BUG();
#endif
err_dtcmheap_bma_init:
        BDL_BUG();
err_axisram_bma_init:
        BDL_BUG();
        return rc;
}
