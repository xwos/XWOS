/**
 * @file
 * @brief 主模块：子线程
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
#include <xwcd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/thd.h>

#define LED_TASK_PRIORITY (XWOS_SKD_PRIORITY_RAISE(XWOS_SKD_PRIORITY_RT_MIN, 1))

xwer_t led_task(void * arg);

const struct xwos_thd_desc child_thd_desc[] = {
        [0] = {
                .name = "task.led",
                .prio = LED_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 4096,
                .func = led_task,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED | XWOS_SKDATTR_DETACHED,
        },
};
struct xwos_thd * child_thd[xw_array_size(child_thd_desc)];

xwer_t child_thd_start(void)
{
        xwer_t rc;
        xwsq_t i;

        for (i = 0; i < xw_array_size(child_thd_desc); i++) {
                rc = xwos_thd_create(&child_thd[i],
                                     child_thd_desc[i].name,
                                     child_thd_desc[i].func,
                                     child_thd_desc[i].arg,
                                     child_thd_desc[i].stack_size,
                                     child_thd_desc[i].prio,
                                     child_thd_desc[i].attr);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }
        return XWOK;

err_thd_create:
        BDL_BUG();
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
