/**
 * @file
 * @brief 示例：sync/condition
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/sync/condition.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwam/example/sync/condition/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define EXAMPLE_CONDITION_WTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define EXAMPLE_CONDITION_NTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t example_condition_wthrd_func(void * arg);
xwer_t example_condition_nthrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_cdt example_cdt;
struct xwosal_splk example_lock;
bool example_flag;

const struct xwosal_thrd_desc example_condition_wthrd_td = {
        .name = "example.condition.wthrd",
        .prio = EXAMPLE_CONDITION_WTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)example_condition_wthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t example_condition_wthrd;

const struct xwosal_thrd_desc example_condition_nthrd_td = {
        .name = "example.condition.nthrd",
        .prio = EXAMPLE_CONDITION_NTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)example_condition_nthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t example_condition_nthrd;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t example_condition_start(void)
{
        xwer_t rc;

        example_flag = false;
        xwosal_splk_init(&example_lock);
        rc = xwosal_cdt_init(&example_cdt);
        if (rc < 0) {
                goto err_cdt_init;
        }

        rc = xwosal_thrd_create(&example_condition_wthrd,
                                example_condition_wthrd_td.name,
                                example_condition_wthrd_td.func,
                                example_condition_wthrd_td.arg,
                                example_condition_wthrd_td.stack_size,
                                example_condition_wthrd_td.prio,
                                example_condition_wthrd_td.attr);
        if (rc < 0) {
                goto err_wthrd_create;
        }

        rc = xwosal_thrd_create(&example_condition_nthrd,
                                example_condition_nthrd_td.name,
                                example_condition_nthrd_td.func,
                                example_condition_nthrd_td.arg,
                                example_condition_nthrd_td.stack_size,
                                example_condition_nthrd_td.prio,
                                example_condition_nthrd_td.attr);
        if (rc < 0) {
                goto err_nthrd_create;
        }

        return XWOK;


err_nthrd_create:
        xwosal_thrd_terminate(example_condition_wthrd, NULL);
        xwosal_thrd_delete(example_condition_wthrd);
err_wthrd_create:
        xwosal_cdt_destroy(&example_cdt);
err_cdt_init:
        return rc;
}

xwer_t example_condition_wthrd_func(void * arg)
{
        xwid_t cdtid;
        xwtm_t time;
        union xwlk_ulock ulk;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        cdtid = xwosal_cdt_get_id(&example_cdt);
        ulk.osal.splk = &example_lock;
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwosal_splk_lock_cpuirqsv(&example_lock, &cpuirq);
                if (example_flag) {
                        xwlogf(INFO, "cdttst", "Acquired!");
                        example_flag = false;
                } else {
                        time = 10 * XWTM_S;
                        rc = xwosal_cdt_timedwait(cdtid,
                                                  ulk, XWLK_TYPE_SPLK, NULL,
                                                  &time, &lkst);
                        if (XWOK == rc) {
                                xwlogf(INFO, "cdttst", "Acquired!");
                                example_flag = false;
                        } else {
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwosal_splk_lock(&example_lock);
                                }
                                if (example_flag) {
                                        xwlogf(INFO, "cdttst", "Acquired!");
                                        example_flag = false;
                                } else {
                                        xwlogf(INFO, "cdttst", "Error:%d\n", rc);
                                }
                        }
                }
                xwosal_splk_unlock_cpuirqrs(&example_lock, cpuirq);
        }
        return rc;
}

xwer_t example_condition_nthrd_func(void * arg)
{
        xwid_t cdtid;
        xwtm_t time;
        xwreg_t cpuirq;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        cdtid = xwosal_cdt_get_id(&example_cdt);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwosal_splk_lock_cpuirqsv(&example_lock, &cpuirq);
                example_flag = true;
                xwosal_splk_unlock_cpuirqrs(&example_lock, cpuirq);
                xwosal_cdt_broadcast(cdtid);
                time = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&time);
        }
        return rc;
}
