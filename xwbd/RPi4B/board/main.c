/**
 * @file
 * @brief 板级描述层：用户程序：应用程序入口
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

#include "board/std.h"
#include <xwos/lib/xwlog.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/thd.h>
#include "board/xwac/xwds/device.h"
#include "bm/ISocC/api.h"

#define SOC_DBGF
#include <soc_debug.h>

#define MAIN_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

static
xwer_t main_task(void * arg);

__xwcc_aligned(16) xwu8_t main_thd_stack[8192] = {0};
const struct xwos_thd_desc main_thd_desc = {
        .attr = {
                .name = "main.thd",
                .stack = (xwstk_t *)main_thd_stack,
                .stack_size = sizeof(main_thd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = MAIN_THD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = main_task,
        .arg = NULL,
};
struct xwos_thd main_thd;
xwos_thd_d main_thdd;


#define CHILD_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

static
xwer_t child_task(void * arg);

__xwcc_aligned(16) xwu8_t child_thd_stack[8192] = {0};
const struct xwos_thd_desc child_thd_desc = {
        .attr = {
                .name = "child.thd",
                .stack = (xwstk_t *)child_thd_stack,
                .stack_size = sizeof(child_thd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = CHILD_THD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = child_task,
        .arg = NULL,
};
struct xwos_thd child_thd;
xwos_thd_d child_thdd;

xwer_t xwos_main(void)
{
        xwer_t rc;

        soc_dbgf("Main", "create task ...\r\n");
        rc = xwos_thd_init(&main_thd, &main_thdd,
                           &main_thd_desc.attr,
                           main_thd_desc.func,
                           main_thd_desc.arg);
        if (rc < 0) {
                soc_dbgf("Main", "Failed to init task ... [%ld]\r\n", rc);
                goto err_init_thd_create;
        }

        rc = xwos_thd_init(&child_thd, &child_thdd,
                           &child_thd_desc.attr,
                           child_thd_desc.func,
                           child_thd_desc.arg);
        if (rc < 0) {
                soc_dbgf("Child", "Failed to init task ... [%ld]\r\n", rc);
                goto err_init_thd_create;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }

        return XWOK;

err_init_thd_create:
        BOARD_BUG();
err_skd_start_lc:
        BOARD_BUG();
        return rc;
}

static
xwer_t main_task(void * arg)
{
        xwu64_t ms = 0;
        xwtm_t origin;

        XWOS_UNUSED(arg);
        isocc_init();
        origin = xwtm_now();
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                } else {
                        xwos_cthd_sleep_from(&origin, xwtm_ms(10));
                        ms += 10;
                        if (0 == (ms % 1000)) {
                                xwlogf(I, "XWOS.Main", "Main Task ... %ld ms\r\n", ms);
                        }
                }
        }
        return XWOK;
}

static
xwer_t child_task(void * arg)
{
        xwer_t rc;
        xwu8_t rxbuf[64];
        xwsz_t rxsz = sizeof(rxbuf);

        XWOS_UNUSED(arg);
        while (true) {
                soc_dbgf("Child", "Child Task ...\r\n");
                rxsz = sizeof(rxbuf);
                isocc_rx(rxbuf, &rxsz, 1, XWTM_MAX);
                if (rxsz > 0) {
                        rxbuf[rxsz] = '\0';
                        soc_dbgf("Child", "%s\r\n", rxbuf);
                }
        }
        return rc;
}
