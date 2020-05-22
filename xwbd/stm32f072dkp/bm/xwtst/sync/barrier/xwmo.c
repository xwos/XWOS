/**
 * @file
 * @brief Board Module: xwos/osal/sync/barrier test
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/sync/barrier.h>
#include <bm/xwtst/sync/barrier/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BM_XWTST_SYNC_BARRIER_THRD_PRIORITY       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_barrier_thrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_barrier xwtst_sync_barrier;

const struct xwosal_thrd_desc bm_xwtst_sync_barrier_thrd_td[] = {
        [0] = {
                .name = "bm.xwtst.sync.barrier.thrd",
                .prio = BM_XWTST_SYNC_BARRIER_THRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)bm_xwtst_sync_barrier_thrd_func,
                .arg = (void *)0,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [1] = {
                .name = "bm.xwtst.sync.barrier.thrd",
                .prio = BM_XWTST_SYNC_BARRIER_THRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)bm_xwtst_sync_barrier_thrd_func,
                .arg = (void *)1,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [2] = {
                .name = "bm.xwtst.sync.barrier.thrd",
                .prio = BM_XWTST_SYNC_BARRIER_THRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)bm_xwtst_sync_barrier_thrd_func,
                .arg = (void *)2,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};
xwid_t bm_xwtst_sync_barrier_thrd[xw_array_size(bm_xwtst_sync_barrier_thrd_td)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_barrier_start(void)
{
        xwsq_t i;
        xwer_t rc;

        rc = xwosal_barrier_init(&xwtst_sync_barrier);
        if (rc < 0) {
                goto err_barrier_init;
        }

        for (i = 0; i < xw_array_size(bm_xwtst_sync_barrier_thrd_td); i++) {
                rc = xwosal_thrd_create(&bm_xwtst_sync_barrier_thrd[i],
                                        bm_xwtst_sync_barrier_thrd_td[i].name,
                                        bm_xwtst_sync_barrier_thrd_td[i].func,
                                        bm_xwtst_sync_barrier_thrd_td[i].arg,
                                        bm_xwtst_sync_barrier_thrd_td[i].stack_size,
                                        bm_xwtst_sync_barrier_thrd_td[i].prio,
                                        bm_xwtst_sync_barrier_thrd_td[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return OK;

err_thrd_create:
        xwosal_barrier_destroy(&xwtst_sync_barrier);
err_barrier_init:
        return rc;
}

xwer_t bm_xwtst_sync_barrier_thrd_func(void * arg)
{
        xwosal_barrier_declare_bitmap(msk);
        xwid_t barid;
        xwer_t rc;

        xwsq_t pos = (xwsq_t)arg;
        xwbmpop_c0all(msk, XWOSAL_BARRIER_MAXNUM);
        for (xwsq_t i = 0; i < xw_array_size(bm_xwtst_sync_barrier_thrd_td); i++) {
                xwbmpop_s1i(msk, i);
        }
        barid = xwosal_barrier_get_id(&xwtst_sync_barrier);
        rc = xwosal_barrier_sync(barid, pos, msk);
        return rc;
}
