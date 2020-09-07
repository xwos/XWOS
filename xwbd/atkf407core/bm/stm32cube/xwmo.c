/**
 * @file
 * @brief Board Module: STM32CUBE
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
#include <bm/stm32cube/standard.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwmd/ds/soc/gpio.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/cubemx/Core/Inc/isr.h>
#include <bm/stm32cube/xwac/xwds/stm32cube.h>
#include <bm/stm32cube/xwmo.h>

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
/**
 * @brief 连接占位符
 * @note
 * + 确保链接时使用此符号的文件。
 */
void * const stm32cube_linkage_placeholder[] = {
        stm32cube_override_linkage_msp,
        stm32cube_override_linkage_it,
};

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
        xwsq_t i;

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
        return XWOK;

err_thrd_create:
        BDL_BUG();
        return rc;
}

xwer_t led_task(void * arg)
{
        xwtm_t xwtm;

        XWOS_UNUSED(arg);
        xwds_gpio_req(&stm32cube_soc_cb,
                      XWDS_GPIO_PORT_F,
                      XWDS_GPIO_PIN_9);
        xwds_gpio_req(&stm32cube_soc_cb,
                      XWDS_GPIO_PORT_F,
                      XWDS_GPIO_PIN_10);
        while (!xwosal_cthrd_shld_stop()) {
                if (xwosal_cthrd_shld_frz()) {
                        xwosal_cthrd_freeze();
                }
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 XWDS_GPIO_PORT_F,
                                 XWDS_GPIO_PIN_9);
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 XWDS_GPIO_PORT_F,
                                 XWDS_GPIO_PIN_10);
        }
        return XWOK;
}