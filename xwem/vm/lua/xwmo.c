/**
 * @file
 * @brief 外部模块：Lua
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
#include <xwos/mm/mempool/allocator.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwem/vm/lua/xwlua/port.h>
#include <xwem/vm/lua/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWLUA_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
char * xwlua_argv[] = {
        "xwlua",
};

struct xwlua_arg xwlua_arg = {
        .argc = xw_array_size(xwlua_argv),
        .argv = xwlua_argv,
};

const struct xwosal_thrd_desc xwlua_tbd[] = {
        [0] = {
                .name = "task.lua",
                .prio = XWLUA_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 8192,
                .func = xwlua_thrd,
                .arg = &xwlua_arg,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwid_t xwlua_tid[xw_array_size(xwlua_tbd)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwlua_start(void)
{
        xwer_t rc;
        xwsq_t i;

        for (i = 0; i < xw_array_size(xwlua_tbd); i++) {
                rc = xwosal_thrd_create(&xwlua_tid[i],
                                        xwlua_tbd[i].name,
                                        xwlua_tbd[i].func,
                                        xwlua_tbd[i].arg,
                                        xwlua_tbd[i].stack_size,
                                        xwlua_tbd[i].prio,
                                        xwlua_tbd[i].attr);
                if (rc < 0) {
                        break;
                }
        }
        return rc;
}
