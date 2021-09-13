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

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include <bdl/standard.h>

#define SYSMGR_THD_PRIORITY   XWOS_SKD_PRIORITY_RT_MIN

static
xwer_t sysmgr_thd_main(void *arg);

const struct xwos_thd_desc sysmgr_thd_desc = {
        .name = "sys.mgr",
        .prio = SYSMGR_THD_PRIORITY,
        .stack = NULL,
        .stack_size = 1024,
        .func = sysmgr_thd_main,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED | XWOS_SKDATTR_DETACHED,
};
struct xwos_thd * sysmgr_thd;

xwer_t xwos_main(void)
{
        xwer_t rc;

        rc = xwos_thd_create(&sysmgr_thd,
                             sysmgr_thd_desc.name,
                             sysmgr_thd_desc.func,
                             sysmgr_thd_desc.arg,
                             sysmgr_thd_desc.stack_size,
                             sysmgr_thd_desc.prio,
                             sysmgr_thd_desc.attr);
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
