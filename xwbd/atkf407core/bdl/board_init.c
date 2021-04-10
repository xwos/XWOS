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
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#if defined(XuanWuOS_CFG_CORE__mp)
  #include <xwos/mp/thd.h>
  #include <xwos/mp/swt.h>
  #include <xwos/mp/sync/sem.h>
  #include <xwos/mp/sync/cond.h>
  #include <xwos/mp/sync/event.h>
  #include <xwos/mp/lock/mtx.h>
#endif /* XuanWuOS_CFG_CORE__mp */
#include <bdl/board_init.h>
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

static __xwbsp_init_code
xwer_t sys_mm_init(void);

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
        xwer_t rc;

        rc = xwmm_bma_init(ocheap_bma, "ocheap",
                           (xwptr_t)ocheap_mr_origin, (xwsz_t)ocheap_mr_size,
                           OCHEAP_BLKSZ, OCHEAP_BLKODR);
        if (rc < 0) {
                goto err_ocheap_bma_init;
        }

#if defined(XuanWuOS_CFG_CORE__mp)
        void * mem;
        xwsz_t size;

        size = sizeof(struct xwos_thd) * 16;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_thd_bma_alloc;
        }
        rc = xwos_thd_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
                goto err_thd_cache_init;
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

        size = sizeof(struct xwsync_sem) * 32;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_sem_bma_alloc;
        }
        rc = xwsync_sem_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
                goto err_sem_cache_init;
        }

        size = sizeof(struct xwsync_cond) * 32;
        rc = xwmm_kma_alloc(size, 8, &mem);
        if (rc < 0) {
                goto err_cond_bma_alloc;
        }
        rc = xwsync_cond_cache_init((xwptr_t)mem, size);
        if (rc < 0) {
                goto err_cond_cache_init;
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
err_ocheap_bma_init:
        BDL_BUG();
        return rc;
}
