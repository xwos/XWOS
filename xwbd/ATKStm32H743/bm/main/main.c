/**
 * @file
 * @brief 主模块：应用程序入口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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
#include <xwos/osal/thd.h>
#include <xwmd/libc/newlibac/mif.h>
#include <xwem/vm/lua/mif.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>
#include <bm/button/mif.h>
#include <bm/main/xwpcp.h>
#include <bm/main/thd.h>

#define MAIN_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

xwer_t main_task(void * arg);

const struct xwos_thd_desc main_thd_desc = {
        .attr = {
                .name = "main.thd",
                .stack = NULL,
                .stack_size = 4096,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = MAIN_THD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = main_task,
        .arg = NULL,
};
xwos_thd_d main_thd;

xwer_t xwos_main(void)
{
        xwer_t rc;

        rc = xwos_thd_create(&main_thd,
                             &main_thd_desc.attr,
                             main_thd_desc.func,
                             main_thd_desc.arg);
        if (rc < 0) {
                goto err_init_thd_create;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }
        return XWOK;

err_init_thd_create:
        BDL_BUG();
err_skd_start_lc:
        BDL_BUG();
        return rc;
}

extern void xwrust_main(void);

xwer_t main_task(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = stm32cube_start();
        if (rc < 0) {
                goto err_stm32cube_start;
        }

        rc = newlibac_init();
        if (rc < 0) {
                goto err_newlibac_init;
        }

        rc = child_thd_start();
        if (rc < 0) {
                goto err_child_thd_start;
        }

        rc = bmbtn_start();
        if (rc < 0) {
                goto err_bmbtn_start;
        }

        rc = bm_xwpcp_start();
        if (rc < 0) {
                goto err_xwpcp_start;
        }

#if defined(XWEMCFG_vm_lua) && (1 == XWEMCFG_vm_lua)
        rc = xwlua_start();
        if (rc < 0) {
                goto err_xwlua_start;
        }
#endif
        xwrust_main();

        return XWOK;

#if defined(XWEMCFG_vm_lua) && (1 == XWEMCFG_vm_lua)
err_xwlua_start:
        BDL_BUG();
#endif
err_xwpcp_start:
        BDL_BUG();
err_bmbtn_start:
        BDL_BUG();
err_child_thd_start:
        BDL_BUG();
err_newlibac_init:
        BDL_BUG();
err_stm32cube_start:
        BDL_BUG();
        return rc;
}
