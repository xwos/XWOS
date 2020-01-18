/**
 * @file
 * @brief xwos/osal/sync/selector test
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
#include <xwos/osal/sync/semaphore.h>
#include <xwos/osal/sync/selector.h>
#include <bm/xwtst/sync/selector/demo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BM_XWTST_SYNC_SELECTOR_WTHRD_PRIORITY       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define BM_XWTST_SYNC_SELECTOR_ITHRD_PRIORITY       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_selector_wthrd_func(void * arg);
xwer_t bm_xwtst_sync_selector_ithrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_selector xwtst_sync_selector;
struct xwosal_smr xwtst_sync_smr0;
struct xwosal_smr xwtst_sync_smr1;
struct xwosal_smr xwtst_sync_smr2;

const struct xwosal_thrd_desc bm_xwtst_sync_selector_wthrd_td = {
        .name = "bm.xwtst.sync.selector.wthrd",
        .prio = BM_XWTST_SYNC_SELECTOR_WTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_xwtst_sync_selector_wthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwtst_sync_selector_wthrd;

const struct xwosal_thrd_desc bm_xwtst_sync_selector_ithrd_td = {
        .name = "bm.xwtst.sync.selector.ithrd",
        .prio = BM_XWTST_SYNC_SELECTOR_ITHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_xwtst_sync_selector_ithrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwtst_sync_selector_ithrd;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_selector_start(void)
{
        xwid_t sltid;
        xwid_t smrid0, smrid1, smrid2;
        xwer_t rc;

        rc = xwosal_selector_init(&xwtst_sync_selector);
        if (rc < 0) {
                goto err_selector_init;
        }
        rc = xwosal_smr_init(&xwtst_sync_smr0, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr0_init;
        }
        rc = xwosal_smr_init(&xwtst_sync_smr1, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr1_init;
        }
        rc = xwosal_smr_init(&xwtst_sync_smr2, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr2_init;
        }

        sltid = xwosal_selector_get_id(&xwtst_sync_selector);
        smrid0 = xwosal_smr_get_id(&xwtst_sync_smr0);
        smrid1 = xwosal_smr_get_id(&xwtst_sync_smr1);
        smrid2 = xwosal_smr_get_id(&xwtst_sync_smr2);

        rc = xwosal_selector_bind(sltid, smrid0, 0);
        if (rc < 0) {
                goto err_smr0_bind;
        }
        rc = xwosal_selector_bind(sltid, smrid1, 1);
        if (rc < 0) {
                goto err_smr1_bind;
        }
        rc = xwosal_selector_bind(sltid, smrid2, 2);
        if (rc < 0) {
                goto err_smr2_bind;
        }

        rc = xwosal_thrd_create(&bm_xwtst_sync_selector_wthrd,
                                bm_xwtst_sync_selector_wthrd_td.name,
                                bm_xwtst_sync_selector_wthrd_td.func,
                                bm_xwtst_sync_selector_wthrd_td.arg,
                                bm_xwtst_sync_selector_wthrd_td.stack_size,
                                bm_xwtst_sync_selector_wthrd_td.prio,
                                bm_xwtst_sync_selector_wthrd_td.attr);
        if (rc < 0) {
                goto err_wthrd_create;
        }

        rc = xwosal_thrd_create(&bm_xwtst_sync_selector_ithrd,
                                bm_xwtst_sync_selector_ithrd_td.name,
                                bm_xwtst_sync_selector_ithrd_td.func,
                                bm_xwtst_sync_selector_ithrd_td.arg,
                                bm_xwtst_sync_selector_ithrd_td.stack_size,
                                bm_xwtst_sync_selector_ithrd_td.prio,
                                bm_xwtst_sync_selector_ithrd_td.attr);
        if (rc < 0) {
                goto err_ithrd_create;
        }

        return OK;


err_ithrd_create:
        xwosal_thrd_terminate(bm_xwtst_sync_selector_wthrd, NULL);
        xwosal_thrd_delete(bm_xwtst_sync_selector_wthrd);
err_wthrd_create:
        xwosal_selector_unbind(sltid, smrid2);
err_smr2_bind:
        xwosal_selector_unbind(sltid, smrid1);
err_smr1_bind:
        xwosal_selector_unbind(sltid, smrid0);
err_smr0_bind:
        xwosal_smr_destroy(&xwtst_sync_smr2);
err_smr2_init:
        xwosal_smr_destroy(&xwtst_sync_smr1);
err_smr1_init:
        xwosal_smr_destroy(&xwtst_sync_smr0);
err_smr0_init:
        xwosal_selector_destroy(&xwtst_sync_selector);
err_selector_init:
        return rc;
}

xwer_t bm_xwtst_sync_selector_wthrd_func(void * arg)
{
        DECLARE_BITMAP(msk, XWOSAL_SELECTOR_MAXNUM);
        DECLARE_BITMAP(trg, XWOSAL_SELECTOR_MAXNUM);
        xwid_t sltid;
        xwid_t smrid0, smrid1, smrid2;
        xwtm_t sleep;
        xwer_t rc;

        XWOS_UNUSED(arg);

        smrid0 = xwosal_smr_get_id(&xwtst_sync_smr0);
        smrid1 = xwosal_smr_get_id(&xwtst_sync_smr1);
        smrid2 = xwosal_smr_get_id(&xwtst_sync_smr2);

        sltid = xwosal_selector_get_id(&xwtst_sync_selector);
        while (!xwosal_cthrd_shld_stop()) {
                msk[0] = BIT(0) | BIT(1) | BIT(2);
                xwbmpop_c0all(trg, XWOSAL_SELECTOR_MAXNUM);
                rc = xwosal_selector_select(sltid, msk, trg);
                if (OK == rc) {
                        if (xwbmpop_t1i(trg, 0)) {
                                rc = xwosal_smr_trywait(smrid0);
                        }
                        if (xwbmpop_t1i(trg, 1)) {
                                rc = xwosal_smr_trywait(smrid1);
                        }
                        if (xwbmpop_t1i(trg, 2)) {
                                rc = xwosal_smr_trywait(smrid2);
                        }
                }
                sleep = 100 * XWTM_MS;
                xwosal_cthrd_sleep(&sleep);
        }
        return rc;
}

xwer_t bm_xwtst_sync_selector_ithrd_func(void * arg)
{
        xwid_t smrid0, smrid1, smrid2;
        xwtm_t sleep;
        xwer_t rc;

        XWOS_UNUSED(arg);

        smrid0 = xwosal_smr_get_id(&xwtst_sync_smr0);
        smrid1 = xwosal_smr_get_id(&xwtst_sync_smr1);
        smrid2 = xwosal_smr_get_id(&xwtst_sync_smr2);

        rc = xwosal_smr_post(smrid0);
        if (rc < 0) {
                goto err_post_smr0;
        }
        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        rc = xwosal_smr_post(smrid1);
        if (rc < 0) {
                goto err_post_smr1;
        }
        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        rc = xwosal_smr_post(smrid2);
        if (rc < 0) {
                goto err_post_smr2;
        }
        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

err_post_smr2:
err_post_smr1:
err_post_smr0:
        return rc;
}
