/**
 * @file
 * @brief 板级描述层：用户程序：应用程序入口
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#ifdef XWCFG_LIBC__newlib
#  include <xwmd/libc/newlibac/mi.h>
#endif
#ifdef XWCFG_LIBC__picolibc
#  include <xwmd/libc/picolibcac/mi.h>
#endif

#define SOC_DBGF
#include <soc_debug.h>
#define LOGTAG "MainThd"

#define MAINTHD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
xwer_t mainthd_mainfunc(void * arg);
__xwcc_aligned(16) xwu8_t mainthd_stack[8192] = {0};
const struct xwos_thd_desc mainthd_desc = {
        .attr = {
                .name = "main.thd",
                .stack = (xwstk_t *)mainthd_stack,
                .stack_size = sizeof(mainthd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = MAINTHD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = mainthd_mainfunc,
        .arg = NULL,
};
struct xwos_thd mainthd;
xwos_thd_d mainthdd;

xwer_t xwos_main(void)
{
        xwer_t rc;

        soc_dbgf("Main", "create task ...\r\n");
        rc = xwos_thd_init(&mainthd, &mainthdd,
                           &mainthd_desc.attr,
                           mainthd_desc.func,
                           mainthd_desc.arg);
        if (rc < 0) {
                soc_dbgf("Main", "Failed to init task ... [%ld]\r\n", rc);
                goto err_init_mainthd;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }

        return XWOK;

err_init_mainthd:
        BOARD_BUG();
err_skd_start_lc:
        BOARD_BUG();
        return rc;
}

xwer_t mainthd_mainfunc(void * arg)
{
        xwu64_t ms = 0;
        xwtm_t origin;

        XWOS_UNUSED(arg);
        xwlogf(I, LOGTAG, "Init C++ Runtime ...\r\n");
#ifdef XWCFG_LIBC__newlib
        newlibac_init();
#endif
#ifdef XWCFG_LIBC__picolibc
        picolibcac_init();
#endif
        origin = xwtm_now();
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                } else {
                        xwos_cthd_sleep_from(&origin, xwtm_ms(10));
                        ms += 10;
                        if (0 == (ms % 1000)) {
                                xwlogf(I, LOGTAG, "Loop ... %ld ms\r\n", ms);
                        }
                }
        }
        return XWOK;
}
