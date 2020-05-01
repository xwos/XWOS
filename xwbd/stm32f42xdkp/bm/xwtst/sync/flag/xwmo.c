/**
 * @file
 * @brief Board Module: xwos/osal/sync/flag test
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
#include <xwos/osal/sync/flag.h>
#include <bm/xwtst/sync/flag/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BM_XWTST_SYNC_FLAG_WTHRD_PRIORITY       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define BM_XWTST_SYNC_FLAG_ITHRD_PRIORITY       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_flag_wthrd_func(void * arg);
xwer_t bm_xwtst_sync_flag_ithrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_flg xwtst_sync_flag;

const struct xwosal_thrd_desc bm_xwtst_sync_flag_wthrd_td = {
        .name = "bm.xwtst.sync.flag.wthrd",
        .prio = BM_XWTST_SYNC_FLAG_WTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_xwtst_sync_flag_wthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwtst_sync_flag_wthrd;

const struct xwosal_thrd_desc bm_xwtst_sync_flag_ithrd_td = {
        .name = "bm.xwtst.sync.flag.ithrd",
        .prio = BM_XWTST_SYNC_FLAG_ITHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_xwtst_sync_flag_ithrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_xwtst_sync_flag_ithrd;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_xwtst_sync_flag_start(void)
{
        xwer_t rc;

        rc = xwosal_flg_init(&xwtst_sync_flag, NULL);
        if (rc < 0) {
                goto err_flg_init;
        }

        rc = xwosal_thrd_create(&bm_xwtst_sync_flag_wthrd,
                                bm_xwtst_sync_flag_wthrd_td.name,
                                bm_xwtst_sync_flag_wthrd_td.func,
                                bm_xwtst_sync_flag_wthrd_td.arg,
                                bm_xwtst_sync_flag_wthrd_td.stack_size,
                                bm_xwtst_sync_flag_wthrd_td.prio,
                                bm_xwtst_sync_flag_wthrd_td.attr);
        if (rc < 0) {
                goto err_wthrd_create;
        }

        rc = xwosal_thrd_create(&bm_xwtst_sync_flag_ithrd,
                                bm_xwtst_sync_flag_ithrd_td.name,
                                bm_xwtst_sync_flag_ithrd_td.func,
                                bm_xwtst_sync_flag_ithrd_td.arg,
                                bm_xwtst_sync_flag_ithrd_td.stack_size,
                                bm_xwtst_sync_flag_ithrd_td.prio,
                                bm_xwtst_sync_flag_ithrd_td.attr);
        if (rc < 0) {
                goto err_ithrd_create;
        }

        return OK;


err_ithrd_create:
        xwosal_thrd_terminate(bm_xwtst_sync_flag_wthrd, NULL);
        xwosal_thrd_delete(bm_xwtst_sync_flag_wthrd);
err_wthrd_create:
        xwosal_flg_destroy(&xwtst_sync_flag);
err_flg_init:
        return rc;
}

xwer_t bm_xwtst_sync_flag_wthrd_func(void * arg)
{
        XWOSAL_FLG_DECLARE_BITMAP(msk);
        XWOSAL_FLG_DECLARE_BITMAP(org);
        xwid_t flgid;
        xwtm_t sleep;
        xwer_t rc;

        XWOS_UNUSED(arg);

        flgid = xwosal_flg_get_id(&xwtst_sync_flag);
        msk[0] = 0xFF;
        rc = xwosal_flg_wait(flgid,
                             XWOSAL_FLG_TRIGGER_SET_ALL,
                             XWOSAL_FLG_ACTION_CONSUMPTION,
                             NULL, msk);
        if (rc < 0) {
                goto err_w_set_all;
        }

        sleep = 100 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_wait(flgid,
                             XWOSAL_FLG_TRIGGER_SET_ANY,
                             XWOSAL_FLG_ACTION_NONE,
                             NULL, msk);
        if (rc < 0) {
                goto err_w_set_any;
        }

        sleep = 100 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_wait(flgid,
                             XWOSAL_FLG_TRIGGER_CLR_ALL,
                             XWOSAL_FLG_ACTION_CONSUMPTION,
                             NULL, msk);
        if (rc < 0) {
                goto err_w_clr_all;
        }

        sleep = 100 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_wait(flgid,
                             XWOSAL_FLG_TRIGGER_CLR_ANY,
                             XWOSAL_FLG_ACTION_NONE,
                             NULL, msk);
        if (rc < 0) {
                goto err_w_clr_any;
        }

        sleep = 100 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        rc = xwosal_flg_read(flgid, org);
        msk[0] = 0xFF;
        rc = xwosal_flg_wait(flgid,
                             XWOSAL_FLG_TRIGGER_TGL_ALL,
                             XWOS_UNUSED_ARGUMENT,
                             org, msk);
        if (rc < 0) {
                goto err_w_tgl_all;
        }

        sleep = 100 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_wait(flgid,
                             XWOSAL_FLG_TRIGGER_TGL_ANY,
                             XWOS_UNUSED_ARGUMENT,
                             org, msk);
        if (rc < 0) {
                goto err_w_tgl_any;
        }

        sleep = 100 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

err_w_tgl_any:
err_w_tgl_all:
err_w_clr_any:
err_w_clr_all:
err_w_set_any:
err_w_set_all:
        return rc;
}

xwer_t bm_xwtst_sync_flag_ithrd_func(void * arg)
{
        XWOSAL_FLG_DECLARE_BITMAP(msk);
        xwid_t flgid;
        xwtm_t sleep;
        xwer_t rc;

        XWOS_UNUSED(arg);

        flgid = xwosal_flg_get_id(&xwtst_sync_flag);

        msk[0] = 0xFF;
        rc = xwosal_flg_s1m(flgid, msk);
        if (rc < 0) {
                goto err_s1m;
        }

        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_s1i(flgid, 3);
        if (rc < 0) {
                goto err_s1i;
        }

        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_c0m(flgid, msk);
        if (rc < 0) {
                goto err_c0m;
        }

        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_c0i(flgid, 7);
        if (rc < 0) {
                goto err_c0i;
        }

        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_x1m(flgid, msk);
        if (rc < 0) {
                goto err_x1m;
        }

        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        msk[0] = 0xFF;
        rc = xwosal_flg_x1i(flgid, 5);
        if (rc < 0) {
                goto err_x1i;
        }

        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

err_x1i:
err_x1m:
err_c0i:
err_c0m:
err_s1i:
err_s1m:
        return rc;
}
