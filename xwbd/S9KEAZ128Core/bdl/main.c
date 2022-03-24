/**
 * @file
 * @brief 板级描述层：应用程序入口
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

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include <bdl/standard.h>

#define SYSMGR_THD_PRIORITY   XWOS_SKD_PRIORITY_RT_MIN

static
xwer_t sysmgr_thd_main(void *arg);

const struct xwos_thd_desc sysmgr_thd_desc = {
        .attr = {
                .name = "sysmgr.thd",
                .stack = NULL,
                .stack_size = 1024,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = SYSMGR_THD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = sysmgr_thd_main,
        .arg = NULL,
};
struct xwos_thd * sysmgr_thd;

xwer_t xwos_main(void)
{
        xwer_t rc;

        rc = xwos_thd_create(&sysmgr_thd,
                             &sysmgr_thd_desc.attr,
                             sysmgr_thd_desc.func,
                             sysmgr_thd_desc.arg);
        if (XWOK == rc) {
                rc = xwos_skd_start_lc();
        }
        BDL_BUG_ON(rc < 0);
        return rc;
}

xwer_t sysmgr_thd_main(void *arg)
{
        xwtm_t time;
        xwer_t rc;

        XWOS_UNUSED(arg);
        while (true) {
                time = 300 * XWTM_MS;
                xwos_cthd_sleep(&time);
                time = 300 * XWTM_MS;
                xwos_cthd_sleep(&time);
        }
        return rc;
}
