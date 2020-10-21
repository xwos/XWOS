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
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#if defined(XuanWuOS_CFG_CORE__smp)
  #include <xwos/smp/thread.h>
  #include <xwos/smp/swt.h>
  #include <xwos/smp/sync/semaphore.h>
  #include <xwos/smp/sync/condition.h>
  #include <xwos/smp/sync/event.h>
  #include <xwos/smp/lock/mutex.h>
#endif /* XuanWuOS_CFG_CORE__smp */
#include <bdl/board_init.h>
#include <bm/stm32cube/mif.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define OCHEAP_BLKSZ    BRDCFG_MM_OCHEAP_BLKSZ

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwsz_t ocheap_mr_origin[];
extern xwsz_t ocheap_mr_size[];

/**
 * @brief thread stack mempool zone
 */
__xwbsp_data struct xwmm_bma * ocheap_bma = NULL;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_init_code
xwer_t sys_mm_init(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       function implementations      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_init_code
void board_lowlevel_init(void)
{
        stm32cube_lowlevel_init();
}

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
        struct xwmm_bma * bma;
        xwer_t rc;

        rc = xwmm_bma_create(&bma, "ocheap",
                             (xwptr_t)ocheap_mr_origin,
                             (xwsz_t)ocheap_mr_size,
                             OCHEAP_BLKSZ);
        if (rc < 0) {
                goto err_ocheap_bma_create;
        }
        ocheap_bma = bma;

#if defined(XuanWuOS_CFG_CORE__smp)
        void * mem;
        xwsz_t size;

        size = sizeof(struct xwos_tcb) * 16;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_tcb_bma_alloc;
        }
        rc = xwos_tcb_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
                goto err_tcb_cache_init;
        }

        size = sizeof(struct xwsync_swt) * 32;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_swt_bma_alloc;
        }
        rc = xwsync_swt_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
                goto err_swt_cache_init;
        }

        size = sizeof(struct xwsync_smr) * 32;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_smr_bma_alloc;
        }
        rc = xwsync_smr_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
                goto err_smr_cache_init;
        }

        size = sizeof(struct xwsync_cdt) * 32;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_cdt_bma_alloc;
        }
        rc = xwsync_cdt_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
                goto err_cdt_cache_init;
        }

        size = sizeof(struct xwsync_evt) * 32;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_evt_bma_alloc;
        }
        rc = xwsync_evt_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
                goto err_evt_cache_init;
        }

        size = sizeof(struct xwsync_mtx) * 32;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_mtx_bma_alloc;
        }
        rc = xwsync_mtx_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
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
err_swt_cache_init:
        BDL_BUG();
err_swt_bma_alloc:
        BDL_BUG();
err_tcb_cache_init:
        BDL_BUG();
err_tcb_bma_alloc:
        BDL_BUG();
#endif /* XuanWuOS_CFG_CORE__smp */
err_ocheap_bma_create:
        BDL_BUG();
        return rc;
}
