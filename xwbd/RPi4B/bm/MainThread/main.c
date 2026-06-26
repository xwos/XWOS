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
#include <xwos/osal/skd.h>
#include <xwos/osal/thd.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_mp.h>
#include "board/xwac/libc/mi.h"
#include "board/xwac/xwds/device.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "MThd"
#define MTHD_DBG
#if defined(MTHD_DBG)
#  define mthdlogd(fmt, ...) xwlogf(D, LOGTAG, fmt, ##__VA_ARGS__)
#else
#  define mthdlogd(fmt, ...)
#endif
#define mthdlogi(fmt, ...) xwlogf(I, LOGTAG, fmt, ##__VA_ARGS__)
#define mthdloge(fmt, ...) xwlogf(E, LOGTAG, fmt, ##__VA_ARGS__)

#define CPU0_MTHD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
xwer_t cpu0_mthd_mainfunc(void * arg);
__xwos_thd_stack xwstk_t cpu0_mthd_stack[1024U] = {0};
const struct xwos_thd_desc cpu0_mthd_desc = {
        .attr = {
                .name = "mthd@0",
                .stack = (xwstk_t *)cpu0_mthd_stack,
                .stack_size = sizeof(cpu0_mthd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = CPU0_MTHD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = cpu0_mthd_mainfunc,
        .arg = NULL,
};
struct xwos_thd cpu0_mthd;
xwos_thd_d cpu0_mthdd;

#define CPU1_MTHD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
xwer_t cpu1_mthd_mainfunc(void * arg);
__xwos_thd_stack xwstk_t cpu1_mthd_stack[1024U] = {0};
const struct xwos_thd_desc cpu1_mthd_desc = {
        .attr = {
                .name = "mthd@1",
                .stack = (xwstk_t *)cpu1_mthd_stack,
                .stack_size = sizeof(cpu1_mthd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = CPU1_MTHD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = cpu1_mthd_mainfunc,
        .arg = NULL,
};
struct xwos_thd cpu1_mthd;
xwos_thd_d cpu1_mthdd;

#define CPU2_MTHD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
xwer_t cpu2_mthd_mainfunc(void * arg);
__xwos_thd_stack xwstk_t cpu2_mthd_stack[1024U] = {0};
const struct xwos_thd_desc cpu2_mthd_desc = {
        .attr = {
                .name = "mthd@2",
                .stack = (xwstk_t *)cpu2_mthd_stack,
                .stack_size = sizeof(cpu2_mthd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = CPU2_MTHD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = cpu2_mthd_mainfunc,
        .arg = NULL,
};
struct xwos_thd cpu2_mthd;
xwos_thd_d cpu2_mthdd;

#define CPU3_MTHD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
xwer_t cpu3_mthd_mainfunc(void * arg);
__xwos_thd_stack xwstk_t cpu3_mthd_stack[1024U] = {0};
const struct xwos_thd_desc cpu3_mthd_desc = {
        .attr = {
                .name = "mthd@3",
                .stack = (xwstk_t *)cpu3_mthd_stack,
                .stack_size = sizeof(cpu3_mthd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = CPU3_MTHD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = cpu3_mthd_mainfunc,
        .arg = NULL,
};
struct xwos_thd cpu3_mthd;
xwos_thd_d cpu3_mthdd;

xwer_t xwos_main(void)
{
        xwer_t rc;
        xwid_t cpu;

        cpu = xwos_skd_get_cpuid_lc();
        switch (cpu) {
        case 0U:
                rc = xwos_thd_init(&cpu0_mthd, &cpu0_mthdd,
                                   &cpu0_mthd_desc.attr,
                                   cpu0_mthd_desc.func,
                                   cpu0_mthd_desc.arg);
                break;
        case 1U:
                rc = xwos_thd_init(&cpu1_mthd, &cpu1_mthdd,
                                   &cpu1_mthd_desc.attr,
                                   cpu1_mthd_desc.func,
                                   cpu1_mthd_desc.arg);
                break;
        case 2U:
                rc = xwos_thd_init(&cpu2_mthd, &cpu2_mthdd,
                                   &cpu2_mthd_desc.attr,
                                   cpu2_mthd_desc.func,
                                   cpu2_mthd_desc.arg);
                break;
        case 3U:
                rc = xwos_thd_init(&cpu3_mthd, &cpu3_mthdd,
                                   &cpu3_mthd_desc.attr,
                                   cpu3_mthd_desc.func,
                                   cpu3_mthd_desc.arg);
                break;
        default:
                rc = -ENOSYS;
                break;
        }
        if (rc < 0) {
                goto err_init_mthd;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }

        return XWOK;

err_init_mthd:
        XWOS_BUG();
err_skd_start_lc:
        XWOS_BUG();
        return rc;
}

xwer_t cpu0_mthd_mainfunc(void * arg)
{
        xwtm_t origin;
        xwu64_t ms;

        XWOS_UNUSED(arg);
        ms = 0;
        mthdlogi("Init C/C++ Runtime ...\r\n");
        libc_init();
        soc_boot_mp(XWBOP_BIT(1) | XWBOP_BIT(2) | XWBOP_BIT(3));
        origin = xwtm_now();
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                } else {
                        xwos_cthd_sleep_from(&origin, xwtm_ms(10));
                        ms += 10;
                        if (0 == (ms % 1000)) {
                                mthdlogi("Loop ... %ld ms\r\n", ms);
                        }
                }
        }
        return XWOK;
}

xwer_t cpu1_mthd_mainfunc(void * arg)
{
        xwtm_t origin;
        xwu64_t ms;

        XWOS_UNUSED(arg);
        ms = 0;
        origin = xwtm_now();
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                } else {
                        xwos_cthd_sleep_from(&origin, xwtm_ms(10));
                        ms += 10;
                        if (0 == (ms % 1000)) {
                                mthdlogi("Loop ... %ld ms\r\n", ms);
                        }
                }
        }
        return XWOK;
}

xwer_t cpu2_mthd_mainfunc(void * arg)
{
        xwtm_t origin;
        xwu64_t ms;

        XWOS_UNUSED(arg);
        ms = 0;
        origin = xwtm_now();
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                } else {
                        xwos_cthd_sleep_from(&origin, xwtm_ms(10));
                        ms += 10;
                        if (0 == (ms % 1000)) {
                                mthdlogi("Loop ... %ld ms\r\n", ms);
                        }
                }
        }
        return XWOK;
}

xwer_t cpu3_mthd_mainfunc(void * arg)
{
        xwtm_t origin;
        xwu64_t ms;

        XWOS_UNUSED(arg);
        ms = 0;
        origin = xwtm_now();
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                } else {
                        xwos_cthd_sleep_from(&origin, xwtm_ms(10));
                        ms += 10;
                        if (0 == (ms % 1000)) {
                                mthdlogi("Loop ... %ld ms\r\n", ms);
                        }
                }
        }
        return XWOK;
}
