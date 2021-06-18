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
#include <xwos/mm/mempool/allocator.h>
#if defined(XuanWuOS_CFG_CORE__mp)
  #include <xwos/mp/thd.h>
  #include <xwos/mp/swt.h>
  #include <xwos/mp/sync/sem.h>
  #include <xwos/mp/sync/cond.h>
  #include <xwos/mp/lock/mtx.h>
#endif /* XuanWuOS_CFG_CORE__mp */
#include <bm/stm32cube/mif.h>
#include <bdl/standard.h>
#include <bdl/board_init.h>

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
 *   - xwbd/WeActH750/bm/stm32cube/mif.h
 *   - xwos/mm/mempool/allocator.h
 * + API
 *   - 申请：xwmm_mempool_malloc(axisram_mempool, ...)
 *   - 释放：xwmm_mempool_free(axisram_mempool, ...)
 *   - 重新申请：xwmm_mempool_realloc(axisram_mempool, ...)
 */
__xwbsp_data struct xwmm_mempool * axisram_mempool = (void *)axisram_mr_origin;

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
        xwssq_t odr;

        odr = xwbop_fls(xwsz_t, (xwsz_t)axisram_mr_size / XWMM_MEMPOOL_PAGE_SIZE);
        rc = xwmm_mempool_init(axisram_mempool, "AXISRAM",
                               (xwptr_t)axisram_mr_origin,
                               (xwsz_t)axisram_mr_size,
                               (xwsz_t)odr);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_axisram_mempool_init;
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
#endif /* XuanWuOS_CFG_CORE__mp */

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
#endif /* XuanWuOS_CFG_CORE__mp */
err_dtcmheap_bma_init:
        BDL_BUG();
err_axisram_mempool_init:
        BDL_BUG();
        return rc;
}
