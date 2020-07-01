/**
 * @file
 * @brief 板级描述层：应用程序入口
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <bdl/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SYS_MGR_THRD_PRIORITY   XWOSAL_SD_PRIORITY_RT_MIN

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t sys_mgr_thrd(void *arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc sys_mgr_td = {
        .name = "sys.mgr",
        .prio = SYS_MGR_THRD_PRIORITY,
        .stack = NULL,
        .stack_size = 1024,
        .func = sys_mgr_thrd,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};

xwid_t sys_mgr_tid;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_main(void)
{
        xwer_t rc;

        rc = xwosal_thrd_create(&sys_mgr_tid,
                                sys_mgr_td.name,
                                sys_mgr_td.func,
                                sys_mgr_td.arg,
                                sys_mgr_td.stack_size,
                                sys_mgr_td.prio,
                                sys_mgr_td.attr);
        if (OK == rc) {
                rc = xwosal_scheduler_start_lc();
        }
        BDL_BUG_ON(rc < 0);
        return rc;
}

xwer_t sys_mgr_thrd(void *arg)
{
        xwtm_t time;
        xwer_t rc;

        XWOS_UNUSED(arg);
        while (true) {
                time = 300 * XWTM_MS;
                xwosal_cthrd_sleep(&time);
                time = 300 * XWTM_MS;
                xwosal_cthrd_sleep(&time);
        }
        return rc;
}
