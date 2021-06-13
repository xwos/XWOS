/**
 * @file
 * @brief 外部模块：Lua
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#include <xwos/standard.h>
#include <xwos/mm/mempool/allocator.h>
#include <xwos/osal/skd.h>
#include <xwem/vm/lua/xwlua/port.h>
#include <xwem/vm/lua/mif.h>

#define XWLUA_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

char * xwlua_argv[] = {
        "xwlua",
        NULL,
};

struct xwlua_arg xwlua_arg = {
        .argc = xw_array_size(xwlua_argv) - 1,
        .argv = xwlua_argv,
};

const struct xwos_thd_desc xwlua_thd_desc[] = {
        [0] = {
                .name = "task.lua",
                .prio = XWLUA_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = XWLUA_THD_STACK,
                .func = xwlua_task,
                .arg = &xwlua_arg,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};

struct xwos_thd * xwlua_thd[xw_array_size(xwlua_thd_desc)];

xwer_t xwlua_start(void)
{
        xwer_t rc;
        xwsq_t i;

        for (i = 0; i < xw_array_size(xwlua_thd_desc); i++) {
                rc = xwos_thd_create(&xwlua_thd[i],
                                     xwlua_thd_desc[i].name,
                                     xwlua_thd_desc[i].func,
                                     xwlua_thd_desc[i].arg,
                                     xwlua_thd_desc[i].stack_size,
                                     xwlua_thd_desc[i].prio,
                                     xwlua_thd_desc[i].attr);
                if (rc < 0) {
                        break;
                }
        }
        return rc;
}
