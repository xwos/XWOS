/**
 * @file
 * @brief xwos/osal/sync/semaphore test
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/sync/semaphore.h>
#include <bm/xwtst/sync/semaphore/demo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BM_XWTST_SYNC_SEMAPHORE_WTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define BM_XWTST_SYNC_SEMAPHORE_NTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_semaphore_wthrd_func(void * arg);
xwer_t bm_xwtst_sync_semaphore_nthrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_smr bm_xwtst_sync_smr;

const struct xwosal_thrd_desc bm_xwtst_sync_semaphore_wthrd_td = {
        .name = "bm.xwtst.sync.semaphore.wthrd",
        .prio = BM_XWTST_SYNC_SEMAPHORE_WTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_xwtst_sync_semaphore_wthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwtst_sync_semaphore_wthrd;

const struct xwosal_thrd_desc bm_xwtst_sync_semaphore_nthrd_td = {
        .name = "bm.xwtst.sync.semaphore.nthrd",
        .prio = BM_XWTST_SYNC_SEMAPHORE_NTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_xwtst_sync_semaphore_nthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwtst_sync_semaphore_nthrd;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_semaphore_start(void)
{
        xwer_t rc;

        rc = xwosal_smr_init(&bm_xwtst_sync_smr, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr_init;
        }

        rc = xwosal_thrd_create(&bm_xwtst_sync_semaphore_wthrd,
                                bm_xwtst_sync_semaphore_wthrd_td.name,
                                bm_xwtst_sync_semaphore_wthrd_td.func,
                                bm_xwtst_sync_semaphore_wthrd_td.arg,
                                bm_xwtst_sync_semaphore_wthrd_td.stack_size,
                                bm_xwtst_sync_semaphore_wthrd_td.prio,
                                bm_xwtst_sync_semaphore_wthrd_td.attr);
        if (rc < 0) {
                goto err_wthrd_create;
        }

        rc = xwosal_thrd_create(&bm_xwtst_sync_semaphore_nthrd,
                                bm_xwtst_sync_semaphore_nthrd_td.name,
                                bm_xwtst_sync_semaphore_nthrd_td.func,
                                bm_xwtst_sync_semaphore_nthrd_td.arg,
                                bm_xwtst_sync_semaphore_nthrd_td.stack_size,
                                bm_xwtst_sync_semaphore_nthrd_td.prio,
                                bm_xwtst_sync_semaphore_nthrd_td.attr);
        if (rc < 0) {
                goto err_nthrd_create;
        }

        return OK;


err_nthrd_create:
        xwosal_thrd_terminate(bm_xwtst_sync_semaphore_wthrd, NULL);
        xwosal_thrd_delete(bm_xwtst_sync_semaphore_wthrd);
err_wthrd_create:
        xwosal_smr_destroy(&bm_xwtst_sync_smr);
err_smr_init:
        return rc;
}

xwer_t bm_xwtst_sync_semaphore_wthrd_func(void * arg)
{
        xwid_t smrid;
        xwtm_t time;
        xwer_t rc = OK;

        XWOS_UNUSED(arg);

        smrid = xwosal_smr_get_id(&bm_xwtst_sync_smr);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                time = 10 * XWTM_S;
                rc = xwosal_smr_timedwait(smrid, &time);
                if (OK == rc) {
                        xwlogf(INFO, "Acquired!\n");
                } else {
                        xwlogf(INFO, "Error:%d\n", rc);
                }
        }
        return rc;
}

xwer_t bm_xwtst_sync_semaphore_nthrd_func(void * arg)
{
        xwid_t smrid;
        xwtm_t time;
        xwer_t rc = OK;

        XWOS_UNUSED(arg);

        smrid = xwosal_smr_get_id(&bm_xwtst_sync_smr);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwosal_smr_post(smrid);
                time = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&time);
        }
        return rc;
}
