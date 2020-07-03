/**
 * @file
 * @brief 应用程序入口
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwem/libc/newlibac/xwmo.h>
#include <xwem/vm/lua/xwmo.h>
#include <bdl/standard.h>
#include <bm/stm32cube/xwmo.h>
#include <bm/pm/xwmo.h>
#include <bm/cxx/xwmo.h>
#include <bm/xwtst/sync/selector/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BDL_INIT_THRD_PRIORITY XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 0)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bdl_init_thrd(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc bdl_init_thrd_td = {
        .name = "bdl.init.thrd",
        .prio = BDL_INIT_THRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bdl_init_thrd,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bdl_init_thrd_id;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_main(void)
{
        xwer_t rc;

        rc = xwosal_thrd_create(&bdl_init_thrd_id,
                                bdl_init_thrd_td.name,
                                bdl_init_thrd_td.func,
                                bdl_init_thrd_td.arg,
                                bdl_init_thrd_td.stack_size,
                                bdl_init_thrd_td.prio,
                                bdl_init_thrd_td.attr);
        if (rc < 0) {
                goto err_init_thrd_create;
        }

        rc = xwosal_scheduler_start_lc();
        if (rc < 0) {
                goto err_scheduler_start_lc;
        }

        return OK;

err_init_thrd_create:
        BDL_BUG();
err_scheduler_start_lc:
        BDL_BUG();
        return rc;
}

xwer_t bdl_init_thrd(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = bm_stm32cube_start();
        if (rc < 0) {
                goto bm_stm32cube_start;
        }

        rc = newlibac_start();
        if (rc < 0) {
                goto newlibac_start;
        }

        rc = bm_pm_start();
        if (rc < 0) {
                goto bm_pm_start;
        }

#if defined(XWEMCFG_vm_lua) && (1 == XWEMCFG_vm_lua)
        rc = xwlua_start();
        if (rc < 0) {
                goto err_xwlua_start;
        }
#endif /* XWEMCFG_vm_lua */

        rc = bm_cxx_start();
        if (rc < 0) {
                goto err_bm_cxx_start;
        }

        rc = bm_xwtst_sync_selector_start();
        if (rc < 0) {
                goto err_bm_xwtst_sync_selector_start;
        }

        xwosal_thrd_delete(bdl_init_thrd_id);
        return OK;

err_bm_xwtst_sync_selector_start:
        BDL_BUG();
err_bm_cxx_start:
#if defined(XWEMCFG_vm_lua) && (1 == XWEMCFG_vm_lua)
        BDL_BUG();
err_xwlua_start:
#endif /* XWEMCFG_vm_lua */
        BDL_BUG();
bm_pm_start:
        BDL_BUG();
newlibac_start:
        BDL_BUG();
bm_stm32cube_start:
        BDL_BUG();
        return rc;
}
