/**
 * @file
 * @brief 应用程序：DMA方式的UART测试
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
#include <xwos/lib/xwlog.h>
#include <string.h>
#include <xwos/osal/thd.h>
#include <soc_gpio.h>
#include <xwcd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bdl/ds/description/mpc560xbdkp.h>

#define LED_TASK_PRIORITY       XWOS_SKD_PRIORITY_RT_MIN
#define LED_PORT                SOC_GPIO_PORT_E
#define UART_TASK_PRIORITY       XWOS_SKD_PRIORITY_RT_MIN

xwer_t led_task(void * arg);
xwer_t uart_task(void * arg);

const struct xwos_thd_desc board_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "led",
                        .stack = NULL,
                        .stack_size = 1536,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = LED_TASK_PRIORITY,
                        .detached = true,
                        .privileged = true,
                },
                .func = led_task,
                .arg = NULL,
        },
        [1] = {
                .attr = {
                        .name = "uart",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = UART_TASK_PRIORITY,
                        .detached = true,
                        .privileged = true,
                },
                .func = uart_task,
                .arg = NULL,
        },
};

struct xwos_thd * board_thd[xw_array_size(board_thd_desc)];
xwu8_t rxbuffer[4096];

static __xwos_code
xwer_t bdl_iftx(const xwu8_t * str, xwsz_t size)
{
        xwer_t rc;

        rc = xwds_dmauartc_tx(&mpc560xb_uart0_cb, str, &size, XWTM_MAX);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmatx;
        }
        return XWOK;

err_dmatx:
        return rc;
}

xwer_t uart_task(void * arg)
{
        xwsz_t size;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                size = sizeof(rxbuffer);
                rc = xwds_dmauartc_rx(&mpc560xb_uart0_cb, rxbuffer, &size, XWTM_MAX);
                if ((XWOK == rc) && (size > 0)) {
                        xwos_cthd_sleep(5 * XWTM_S);
                        rc = bdl_iftx(rxbuffer, size >> 1);
                        if (XWOK == rc) {
                                rc = bdl_iftx(&rxbuffer[size >> 1], size >> 1);
                        }
                }
        }
        return rc;
}

xwer_t led_task(void * arg)
{
        xwsq_t pinmask;
        xwer_t rc;

        XWOS_UNUSED(arg);
        pinmask = (XWBOP_BIT(SOC_GPIO_PIN_12) | XWBOP_BIT(SOC_GPIO_PIN_13));
        rc = XWOK;
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rc = xwds_gpio_req(&mpc560xb_soc_cb, LED_PORT, pinmask);
                if (XWOK == rc) {
                        while (1) {
                                pinmask = XWBOP_BIT(SOC_GPIO_PIN_12);
                                xwds_gpio_set(&mpc560xb_soc_cb, LED_PORT,
                                              pinmask);
                                pinmask = XWBOP_BIT(SOC_GPIO_PIN_13);
                                xwds_gpio_reset(&mpc560xb_soc_cb, LED_PORT,
                                                pinmask);
                                rc = xwos_cthd_sleep(500 * XWTM_MS);
                                if (__xwcc_unlikely(rc < 0)) {
                                        break;
                                }
                                pinmask = XWBOP_BIT(SOC_GPIO_PIN_12);
                                xwds_gpio_reset(&mpc560xb_soc_cb, LED_PORT,
                                                pinmask);
                                pinmask = XWBOP_BIT(SOC_GPIO_PIN_13);
                                xwds_gpio_set(&mpc560xb_soc_cb, LED_PORT,
                                              pinmask);
                                rc = xwos_cthd_sleep(500 * XWTM_MS);
                                if (__xwcc_unlikely(rc < 0)) {
                                        break;
                                }
                        }
                        xwds_gpio_rls(&mpc560xb_soc_cb, LED_PORT, pinmask);
                }
        }
        return rc;
}

xwer_t xwos_main(void)
{
        xwer_t rc = XWOK;
        xwsz_t i;

        for (i = 0; i < xw_array_size(board_thd_desc); i++) {
                rc = xwos_thd_create(&board_thd[i],
                                     &board_thd_desc[i].attr,
                                     board_thd_desc[i].func,
                                     board_thd_desc[i].arg);
                BDL_BUG_ON(rc);
        }
        rc = xwos_skd_start_lc();
        return rc;
}
