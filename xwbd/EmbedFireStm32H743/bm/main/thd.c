/**
 * @file
 * @brief 主模块：子线程
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
#include <xwos/osal/thd.h>
#include <xwcd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/thd.h>

#define LED_TASK_PRIORITY  (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

xwer_t led_task(void * arg);

const struct xwos_thd_desc child_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "task.led",
                        .stack = NULL,
                        .stack_size = 4096,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = LED_TASK_PRIORITY,
                        .detached = true,
                        .privileged = true,
                },
                .func = led_task,
                .arg = NULL,
        },
};
struct xwos_thd * child_thd[xw_array_size(child_thd_desc)];

xwer_t child_thd_start(void)
{
        xwer_t rc;
        xwsq_t i;

        for (i = 0; i < xw_array_size(child_thd_desc); i++) {
                rc = xwos_thd_create(&child_thd[i],
                                     &child_thd_desc[i].attr,
                                     child_thd_desc[i].func,
                                     child_thd_desc[i].arg);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }
        return XWOK;

err_thd_create:
        BDL_BUG();
        return rc;
}

#define LED_Blue_Pin XWDS_GPIO_PIN_1
#define LED_Blue_GPIO_Port XWDS_GPIO_PORT_B

xwer_t led_task(void * arg)
{
        XWOS_UNUSED(arg);

        xwds_gpio_req(&stm32cube_soc_cb, LED_Blue_GPIO_Port, LED_Blue_Pin);
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                }
                xwos_cthd_sleep(1 * XWTM_S);
                xwds_gpio_toggle(&stm32cube_soc_cb, LED_Blue_GPIO_Port, LED_Blue_Pin);
        }
        return XWOK;
}
