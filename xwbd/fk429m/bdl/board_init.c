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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <string.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#if defined(XuanWuOS_CFG_CORE__smp)
  #include <xwos/smp/thread.h>
  #include <xwos/smp/sync/semaphore.h>
  #include <xwos/smp/sync/condition.h>
  #include <xwos/smp/sync/event.h>
  #include <xwos/smp/lock/mutex.h>
#endif /* XuanWuOS_CFG_CORE__smp */
#include <bm/stm32cube/init.h>
#include <bdl/standard.h>
#include <bdl/board_init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define STKMEMPOOL_BLKSZ        BRDCFG_MM_STKMEMPOOL_BLKSZ
#define CCMHEAP_BLKSZ           BRDCFG_MM_CCMHEAP_BLKSZ

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwsz_t stkmempool_mr_origin[];
extern xwsz_t stkmempool_mr_size[];

extern xwsz_t ccmheap_mr_origin[];
extern xwsz_t ccmheap_mr_size[];

/**
 * @brief thread stack mempool zone
 */
__xwbsp_data struct xwmm_bma * stkmempool_bma = NULL;

/**
 * @brief CCMRAM zone
 */
__xwbsp_data struct xwmm_bma * ccmheap_bma = NULL;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_init_code
xwer_t sys_mm_init(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_init_code
void board_lowlevel_init(void)
{
        memset((void *)stkmempool_mr_origin, 0, (xwsz_t)stkmempool_mr_size);
        stm32cube_lowlevel_init();
}

/**
 * @brief init board
 */
__xwbsp_init_code
void board_init(void)
{
        xwer_t rc;

        /* memory management */
        rc = sys_mm_init();
        BDL_BUG_ON(rc < 0);

        /* cubemx init */
        stm32cube_init();
}

/**
 * @brief 初始化内存管理
 */
static __xwbsp_init_code
xwer_t sys_mm_init(void)
{
        struct xwmm_bma * bma;
        xwer_t rc;

        rc = xwmm_bma_create(&bma, "stkmempool",
                             (xwptr_t)stkmempool_mr_origin,
                             (xwsz_t)stkmempool_mr_size,
                             STKMEMPOOL_BLKSZ);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_stkmempool_bma_create;
        }
        stkmempool_bma = bma;

        rc = xwmm_bma_create(&bma, "ccmheap",
                             (xwptr_t)ccmheap_mr_origin,
                             (xwsz_t)ccmheap_mr_size,
                             CCMHEAP_BLKSZ);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_ccmheap_bma_create;
        }
        ccmheap_bma = bma;

#if defined(XuanWuOS_CFG_CORE__smp)
        void * mem;

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_THRD_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_tcb_bma_alloc;
        }
        rc = xwos_tcb_cache_init((xwptr_t)mem,
                                 (CCMHEAP_BLKSZ << BRDCFG_XWOS_THRD_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_tcb_cache_init;
        }

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_SMR_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_smr_bma_alloc;
        }
        rc = xwsync_smr_cache_init((xwptr_t)mem,
                                   (CCMHEAP_BLKSZ << BRDCFG_XWOS_SMR_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_smr_cache_init;
        }

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_CDT_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_cdt_bma_alloc;
        }
        rc = xwsync_cdt_cache_init((xwptr_t)mem,
                                   (CCMHEAP_BLKSZ << BRDCFG_XWOS_CDT_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_cdt_cache_init;
        }

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_EVT_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_evt_bma_alloc;
        }
        rc = xwsync_evt_cache_init((xwptr_t)mem,
                                   (CCMHEAP_BLKSZ << BRDCFG_XWOS_EVT_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_evt_cache_init;
        }

        rc = xwmm_bma_alloc(ccmheap_bma, BRDCFG_XWOS_MTX_CACHE_ODR, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_bma_alloc;
        }
        rc = xwlk_mtx_cache_init((xwptr_t)mem,
                                 (CCMHEAP_BLKSZ << BRDCFG_XWOS_MTX_CACHE_ODR));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_cache_init;
        }
#endif /* XuanWuOS_CFG_CORE__smp */

        return XWOK;

#if defined(XuanWuOS_CFG_CORE__smp)
err_mtx_cache_init:
        BDL_BUG();
err_mtx_bma_alloc:
        BDL_BUG();
err_evt_cache_init:
        BDL_BUG();
err_evt_bma_alloc:
        BDL_BUG();
err_cdt_cache_init:
        BDL_BUG();
err_cdt_bma_alloc:
        BDL_BUG();
err_smr_cache_init:
        BDL_BUG();
err_smr_bma_alloc:
        BDL_BUG();
err_tcb_cache_init:
        BDL_BUG();
err_tcb_bma_alloc:
        BDL_BUG();
#endif /* XuanWuOS_CFG_CORE__smp */
err_ccmheap_bma_create:
        BDL_BUG();
err_stkmempool_bma_create:
        BDL_BUG();
        return rc;
}
