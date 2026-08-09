/**
 * @file
 * @brief XWLUA：模块集成接口
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/thd.h>
#include <xwmd/vm/lua/xwlua/prefix.h>
#include <xwmd/vm/lua/xwlua/port.h>
#include <xwmd/vm/lua/mi.h>

char * xwlua_replthd_argv[] = {
        "xwlua",
        NULL,
};

struct xwlua_arg xwlua_replthd_arg = {
        .argc = xw_array_size(xwlua_replthd_argv) - 1,
        .argv = xwlua_replthd_argv,
};

#define XWLUA_REPLTHD_PRIORITY          (XWOS_SKD_PRIORITY_RT_MAX)
#define XWLUA_REPLTHD_STACK_SIZE        (16384U)
__xwcc_alignl1cache xwu8_t xwlua_replthd_stack[XWLUA_REPLTHD_STACK_SIZE] = {0};
const struct xwos_thd_desc xwlua_replthd_desc = {
        .attr = {
                .name = "xwlua.repl.thd",
                .stack = (xwstk_t *)xwlua_replthd_stack,
                .stack_size = sizeof(xwlua_replthd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = XWLUA_REPLTHD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = xwlua_replthd_mainfunc,
        .arg = &xwlua_replthd_arg,
};
struct xwos_thd xwlua_replthd;
xwos_thd_d xwlua_replthdd;

xwer_t xwlua_init(void)
{
        return xwos_thd_init(&xwlua_replthd, &xwlua_replthdd,
                             &xwlua_replthd_desc.attr,
                             xwlua_replthd_desc.func,
                             xwlua_replthd_desc.arg);
}
