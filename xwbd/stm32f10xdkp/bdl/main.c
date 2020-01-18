/**
 * @file
 * @brief 应用程序入口
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <bdl/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define TASK_LED_RUNNING_PRIORITY       \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t task_state(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc bdl_tbd[] = {
        [0] = {
                .name = "state",
                .prio = TASK_LED_RUNNING_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = task_state,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwid_t bdl_tid[xw_array_size(bdl_tbd)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t task_state(__maybe_unused void * arg)
{
        xwtm_t xwtm;
        xwer_t rc;

        rc = OK;
        while (!xwosal_cthrd_shld_stop()) {
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
        }
        return rc;
}

xwer_t xwos_main(void)
{
        xwer_t rc;
        xwsz_t i;

        /* Create threads */
        rc = OK;
        for (i = 0; i < xw_array_size(bdl_tbd); i ++) {
                rc = xwosal_thrd_create(&bdl_tid[i], bdl_tbd[i].name,
                                        bdl_tbd[i].func,
                                        bdl_tbd[i].arg,
                                        bdl_tbd[i].stack_size,
                                        bdl_tbd[i].prio,
                                        bdl_tbd[i].attr);
                BDL_BUG_ON(rc);
        }
        xwosal_scheduler_start_lc();
        return rc;
}
