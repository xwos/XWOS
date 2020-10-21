/**
 * @file
 * @brief STM32CUBE模块：应用程序入口
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
#include <xwos/osal/thread.h>
#include <xwos/osal/scheduler.h>
#include <bm/stm32cube/standard.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/cubemx/Core/Inc/gpio.h>
#include <bm/stm32cube/mif.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define MAIN_THRD_PRIORITY XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 0)

#define LED_GPIO_PORT GPIOC
#define LED_GPIO_PIN LL_GPIO_PIN_13

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t main_thrd(void * arg);

static
xwer_t led_task(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc main_thrd_td = {
        .name = "bdl.init.thrd",
        .prio = MAIN_THRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)main_thrd,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t main_thrd_id;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_main(void)
{
        xwer_t rc;

        rc = xwosal_thrd_create(&main_thrd_id,
                                main_thrd_td.name,
                                main_thrd_td.func,
                                main_thrd_td.arg,
                                main_thrd_td.stack_size,
                                main_thrd_td.prio,
                                main_thrd_td.attr);
        if (rc < 0) {
                goto err_init_thrd_create;
        }

        rc = xwosal_scheduler_start_lc();
        if (rc < 0) {
                goto err_scheduler_start_lc;
        }

        return XWOK;

err_init_thrd_create:
        BDL_BUG();
err_scheduler_start_lc:
        BDL_BUG();
        return rc;
}

static
xwer_t main_thrd(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = stm32cube_start();
        if (rc < 0) {
                goto err_stm32cube_start;
        }

        rc = led_task();
        return rc;

err_stm32cube_start:
        BDL_BUG();
        return rc;
}

xwer_t led_task(void)
{
        xwtm_t xwtm;

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
                LL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
        }
        return XWOK;
}
