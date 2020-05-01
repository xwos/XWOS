/**
 * @file
 * @brief Board Module: C++
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
#include <bm/cxx/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define CXX_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwer_t cxx_thrd(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc cxx_tbd[] = {
        [0] = {
                .name = "task.lua",
                .prio = CXX_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 4096,
                .func = cxx_thrd,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwid_t cxx_tid[xw_array_size(cxx_tbd)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_cxx_start(void)
{
        xwer_t rc;
        xwsq_t i;

        for (i = 0; i < xw_array_size(cxx_tbd); i++) {
                rc = xwosal_thrd_create(&cxx_tid[i],
                                        cxx_tbd[i].name,
                                        cxx_tbd[i].func,
                                        cxx_tbd[i].arg,
                                        cxx_tbd[i].stack_size,
                                        cxx_tbd[i].prio,
                                        cxx_tbd[i].attr);
                if (rc < 0) {
                        break;
                }
        }
        return rc;
}
