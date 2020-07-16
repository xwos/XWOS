/**
 * @file
 * @brief Board Module: S32KSDK
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
#include <oem/s32ksdk/standard.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <oem/s32ksdk/xwmo.h>
#include "Cpu.h"

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define LED_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t led_task(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc s32ksdk_tbd[] = {
        [0] = {
                .name = "task.led",
                .prio = LED_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = led_task,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwid_t s32ksdk_tid[xw_array_size(s32ksdk_tbd)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t s32ksdk_start(void)
{
        xwer_t rc;
        xwsz_t i;

        /* start devices */

        /* start thread */
        for (i = 0; i < xw_array_size(s32ksdk_tbd); i++) {
                rc = xwosal_thrd_create(&s32ksdk_tid[i], s32ksdk_tbd[i].name,
                                        s32ksdk_tbd[i].func,
                                        s32ksdk_tbd[i].arg,
                                        s32ksdk_tbd[i].stack_size,
                                        s32ksdk_tbd[i].prio,
                                        s32ksdk_tbd[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return XWOK;

err_thrd_create:
        return rc;
}

xwer_t led_task(void * arg)
{
        xwtm_t time;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                PINS_DRV_WritePin(PTE, 8, 1);
                PINS_DRV_WritePin(PTB, 5, 0);
                PINS_DRV_WritePin(PTB, 4, 0);
                time = 1000 * XWTM_MS;
                rc = xwosal_cthrd_sleep(&time);
                if (__unlikely(rc < 0)) {
                        break;
                }

                PINS_DRV_WritePin(PTE, 8, 0);
                PINS_DRV_WritePin(PTB, 5, 1);
                PINS_DRV_WritePin(PTB, 4, 0);
                time = 1000 * XWTM_MS;
                rc = xwosal_cthrd_sleep(&time);
                if (__unlikely(rc < 0)) {
                        break;
                }

                PINS_DRV_WritePin(PTE, 8, 0);
                PINS_DRV_WritePin(PTB, 5, 0);
                PINS_DRV_WritePin(PTB, 4, 1);
                time = 1000 * XWTM_MS;
                rc = xwosal_cthrd_sleep(&time);
                if (__unlikely(rc < 0)) {
                        break;
                }
        }
        return rc;
}
