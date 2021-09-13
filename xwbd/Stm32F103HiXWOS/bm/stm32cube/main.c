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

#include <xwos/osal/skd.h>
#include <bm/stm32cube/standard.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/cubemx/Core/Inc/gpio.h>
#include <bm/stm32cube/mif.h>

#define MAIN_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

#define LED_GPIO_PORT GPIOC
#define LED_GPIO_PIN LL_GPIO_PIN_13

static
xwer_t main_task(void * arg);

static
xwer_t led_task(void);

const struct xwos_thd_desc main_thd_desc = {
        .name = "main.thd",
        .prio = MAIN_THD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = main_task,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED | XWOS_SKDATTR_DETACHED,
};
struct xwos_thd * main_thd;

xwer_t xwos_main(void)
{
        xwer_t rc;

        rc = xwos_thd_create(&main_thd,
                             main_thd_desc.name,
                             main_thd_desc.func,
                             main_thd_desc.arg,
                             main_thd_desc.stack_size,
                             main_thd_desc.prio,
                             main_thd_desc.attr);
        if (rc < 0) {
                goto err_init_thd_create;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }

        return XWOK;

err_init_thd_create:
        BDL_BUG();
err_skd_start_lc:
        BDL_BUG();
        return rc;
}

static
xwer_t main_task(void * arg)
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

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                xwtm = 1 * XWTM_S;
                xwos_cthd_sleep(&xwtm);
                LL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
        }
        return XWOK;
}