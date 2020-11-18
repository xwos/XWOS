/**
 * @file
 * @brief 主模块：应用程序入口
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
#include <xwam/example/sync/flg/mif.h>
#include <xwcd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>

#define MAIN_THRD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

static
xwer_t main_thrd(void * arg);

static
xwer_t led_task(void);

const struct xwos_thrd_desc main_thrd_td = {
        .name = "main.thrd",
        .prio = MAIN_THRD_PRIORITY,
        .stack = XWOS_THRD_STACK_DYNAMIC,
        .stack_size = 4096,
        .func = (xwos_thrd_f)main_thrd,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
xwid_t main_thrd_id;

xwer_t xwos_main(void)
{
        xwer_t rc;

        rc = xwos_thrd_create(&main_thrd_id,
                              main_thrd_td.name,
                              main_thrd_td.func,
                              main_thrd_td.arg,
                              main_thrd_td.stack_size,
                              main_thrd_td.prio,
                              main_thrd_td.attr);
        if (rc < 0) {
                goto err_init_thrd_create;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }

        return XWOK;

err_init_thrd_create:
        BDL_BUG();
err_skd_start_lc:
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

        rc = example_flg_start();
        if (rc < 0) {
                goto err_example_flg_start;
        }

        rc = led_task();
        return rc;

err_example_flg_start:
        BDL_BUG();
err_stm32cube_start:
        BDL_BUG();
        return rc;
}

static
xwer_t led_task(void)
{
        xwtm_t xwtm;

        xwds_gpio_req(&stm32cube_soc_cb,
                      XWDS_GPIO_PORT_B,
                      XWDS_GPIO_PIN_5);
        xwds_gpio_req(&stm32cube_soc_cb,
                      XWDS_GPIO_PORT_E,
                      XWDS_GPIO_PIN_5);
        while (!xwos_cthrd_shld_stop()) {
                if (xwos_cthrd_shld_frz()) {
                        xwos_cthrd_freeze();
                }
                xwtm = 1 * XWTM_S;
                xwos_cthrd_sleep(&xwtm);
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 XWDS_GPIO_PORT_B,
                                 XWDS_GPIO_PIN_5);
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 XWDS_GPIO_PORT_E,
                                 XWDS_GPIO_PIN_5);
        }
        return XWOK;
}
