/**
 * @file
 * @brief Board Module: STM32CUBE
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
#include <xwos/osal/sync/semaphore.h>
#include <bm/stm32cube/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define LED_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))

#define LED_GPIO_PORT SODS_GPIO_PORT_C
#define LED_GPIO_PIN SODS_GPIO_PIN_12

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t led_task(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc stm32cube_tbd[] = {
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

xwid_t stm32cube_tid[xw_array_size(stm32cube_tbd)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_stm32cube_start(void)
{
        xwer_t rc;
        xwsz_t i;

        for (i = 0; i < xw_array_size(stm32cube_tbd); i++) {
                rc = xwosal_thrd_create(&stm32cube_tid[i], stm32cube_tbd[i].name,
                                        stm32cube_tbd[i].func,
                                        stm32cube_tbd[i].arg,
                                        stm32cube_tbd[i].stack_size,
                                        stm32cube_tbd[i].prio,
                                        stm32cube_tbd[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return OK;

err_thrd_create:
        return rc;
}

xwer_t led_task(void * arg)
{
        xwtm_t xwtm;

        XWOS_UNUSED(arg);

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);

                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
        }
        return OK;
}