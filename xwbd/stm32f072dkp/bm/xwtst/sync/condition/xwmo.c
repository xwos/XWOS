/**
 * @file
 * @brief Board Module: xwos/osal/sync/condition test
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
#include <xwos/osal/sync/condition.h>
#include <xwos/osal/lock/spinlock.h>
#include <bm/xwtst/sync/condition/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BM_XWTST_SYNC_CONDITION_WTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define BM_XWTST_SYNC_CONDITION_NTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_condition_wthrd_func(void * arg);
xwer_t bm_xwtst_sync_condition_nthrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_cdt bm_xwtst_sync_cdt;
struct xwosal_splk bm_xwtst_sync_lock;
bool bm_xwtst_sync_flag;

const struct xwosal_thrd_desc bm_xwtst_sync_condition_wthrd_td = {
        .name = "bm.xwtst.sync.condition.wthrd",
        .prio = BM_XWTST_SYNC_CONDITION_WTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_xwtst_sync_condition_wthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwtst_sync_condition_wthrd;

const struct xwosal_thrd_desc bm_xwtst_sync_condition_nthrd_td = {
        .name = "bm.xwtst.sync.condition.nthrd",
        .prio = BM_XWTST_SYNC_CONDITION_NTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_xwtst_sync_condition_nthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwtst_sync_condition_nthrd;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_condition_start(void)
{
        xwer_t rc;

        bm_xwtst_sync_flag = false;
        xwosal_splk_init(&bm_xwtst_sync_lock);
        rc = xwosal_cdt_init(&bm_xwtst_sync_cdt);
        if (rc < 0) {
                goto err_cdt_init;
        }

        rc = xwosal_thrd_create(&bm_xwtst_sync_condition_wthrd,
                                bm_xwtst_sync_condition_wthrd_td.name,
                                bm_xwtst_sync_condition_wthrd_td.func,
                                bm_xwtst_sync_condition_wthrd_td.arg,
                                bm_xwtst_sync_condition_wthrd_td.stack_size,
                                bm_xwtst_sync_condition_wthrd_td.prio,
                                bm_xwtst_sync_condition_wthrd_td.attr);
        if (rc < 0) {
                goto err_wthrd_create;
        }

        rc = xwosal_thrd_create(&bm_xwtst_sync_condition_nthrd,
                                bm_xwtst_sync_condition_nthrd_td.name,
                                bm_xwtst_sync_condition_nthrd_td.func,
                                bm_xwtst_sync_condition_nthrd_td.arg,
                                bm_xwtst_sync_condition_nthrd_td.stack_size,
                                bm_xwtst_sync_condition_nthrd_td.prio,
                                bm_xwtst_sync_condition_nthrd_td.attr);
        if (rc < 0) {
                goto err_nthrd_create;
        }

        return OK;


err_nthrd_create:
        xwosal_thrd_terminate(bm_xwtst_sync_condition_wthrd, NULL);
        xwosal_thrd_delete(bm_xwtst_sync_condition_wthrd);
err_wthrd_create:
        xwosal_cdt_destroy(&bm_xwtst_sync_cdt);
err_cdt_init:
        return rc;
}

xwer_t bm_xwtst_sync_condition_wthrd_func(void * arg)
{
        xwid_t cdtid;
        xwtm_t time;
        union xwlk_ulock ulk;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc = OK;

        XWOS_UNUSED(arg);

        cdtid = xwosal_cdt_get_id(&bm_xwtst_sync_cdt);
        ulk.osal.splk = &bm_xwtst_sync_lock;
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwosal_splk_lock_cpuirqsv(&bm_xwtst_sync_lock, &cpuirq);
                if (bm_xwtst_sync_flag) {
                        xwlogf(INFO, "Acquired!");
                        bm_xwtst_sync_flag = false;
                } else {
                        time = 10 * XWTM_S;
                        rc = xwosal_cdt_timedwait(cdtid, ulk,
                                                  XWLK_TYPE_SPLK, NULL, 0,
                                                  &time, &lkst);
                        if (OK == rc) {
                                xwlogf(INFO, "Acquired!");
                                bm_xwtst_sync_flag = false;
                        } else {
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwosal_splk_lock_cpuirq(&bm_xwtst_sync_lock);
                                }
                                if (bm_xwtst_sync_flag) {
                                        xwlogf(INFO, "Acquired!");
                                        bm_xwtst_sync_flag = false;
                                } else {
                                        xwlogf(INFO, "Error:%d\n", rc);
                                }
                        }
                }
                xwosal_splk_unlock_cpuirqrs(&bm_xwtst_sync_lock, cpuirq);
        }
        return rc;
}

xwer_t bm_xwtst_sync_condition_nthrd_func(void * arg)
{
        xwid_t cdtid;
        xwtm_t time;
        xwreg_t cpuirq;
        xwer_t rc = OK;

        XWOS_UNUSED(arg);

        cdtid = xwosal_cdt_get_id(&bm_xwtst_sync_cdt);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwosal_splk_lock_cpuirqsv(&bm_xwtst_sync_lock, &cpuirq);
                bm_xwtst_sync_flag = true;
                xwosal_splk_unlock_cpuirqrs(&bm_xwtst_sync_lock, cpuirq);
                xwosal_cdt_broadcast(cdtid);
                time = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&time);
        }
        return rc;
}