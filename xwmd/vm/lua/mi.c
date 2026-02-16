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
#include <xwos/mm/mempool/allocator.h>
#include <xwos/osal/thd.h>
#include <xwmd/vm/lua/xwlua/prefix.h>
#include <xwmd/vm/lua/xwlua/port.h>
#include <xwmd/vm/lua/mi.h>

#define XWLUA_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

char * xwlua_argv[] = {
        "xwlua",
        NULL,
};

struct xwlua_arg xwlua_arg = {
        .argc = xw_array_size(xwlua_argv) - 1,
        .argv = xwlua_argv,
};

xwos_thd_d xwlua_thd;

xwer_t xwlua_start(void)
{
        xwer_t rc;
        struct xwos_thd_attr attr;

        xwos_thd_attr_init(&attr);
        attr.name = "xwlua.thd";
        attr.stack = NULL;
        attr.stack_size = XWLUA_THD_STACK_SIZE;
        attr.priority = XWLUA_TASK_PRIORITY;
        attr.detached = true;
        attr.privileged = true;
        rc = xwos_thd_create(&xwlua_thd, &attr, xwlua_task, &xwlua_arg);
        return rc;
}
