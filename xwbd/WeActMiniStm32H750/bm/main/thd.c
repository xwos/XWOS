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
#include <xwos/osal/skd.h>
#include <xwcd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/thd.h>

#define LED_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

xwer_t led_task(void * arg);

struct xwos_thd * led_thd;

xwer_t child_thd_start(void)
{
        xwer_t rc;
        struct xwos_thd_attr attr;

        xwos_thd_attr_init(&attr);
        attr.name = "led.thd";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = LED_TASK_PRIORITY;
        attr.detached = true;
        attr.privileged = true;
        rc = xwos_thd_create(&led_thd, &attr, led_task, NULL);
        if (rc < 0) {
                goto err_thd_create;
        }
        return XWOK;

err_thd_create:
        return rc;
}

xwer_t led_task(void * arg)
{
        xwtm_t xwtm;

        XWOS_UNUSED(arg);
        xwds_gpio_req(&stm32cube_soc_cb, XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwds_gpio_reset(&stm32cube_soc_cb,
                                        XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
                        xwds_gpio_rls(&stm32cube_soc_cb,
                                      XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
                        xwos_cthd_freeze();
                        xwds_gpio_req(&stm32cube_soc_cb,
                                      XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
                }
                xwtm = 1 * XWTM_S;
                xwos_cthd_sleep(&xwtm);
                xwds_gpio_toggle(&stm32cube_soc_cb, XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
        }
        xwds_gpio_rls(&stm32cube_soc_cb, XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
        return XWOK;
}
