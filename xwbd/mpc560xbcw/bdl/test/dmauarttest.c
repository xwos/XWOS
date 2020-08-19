/**
 * @file
 * @brief 应用程序：DMA方式的UART测试
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
#include <xwos/standard.h>
#include <xwos/init.h>
#include <xwos/lib/xwlog.h>
#include <string.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <soc_gpio.h>
#include <xwmd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bdl/ds/description/mpc560xbdkp.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define TASK_LED_PRIORITY       XWOSAL_SD_PRIORITY_RT_MIN
#define LED_PORT                SOC_GPIO_PORT_E

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t task_led(void * arg);
xwer_t task_dmauart(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc board_td_tbl[] = {
        [0] = {
                .name = "led",
                .prio = TASK_LED_PRIORITY,
                .stack = NULL,
                .stack_size = 1024,
                .func = task_led,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [1] = {
                .name = "dmauart",
                .prio = TASK_LED_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = task_dmauart,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwid_t board_tcb[xw_array_size(board_td_tbl)];

xwu8_t rxbuffer[4096];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
xwer_t bdl_iftx(const xwu8_t * str, xwsz_t size)
{
        xwer_t rc;
        xwtm_t time;

        time = XWTM_MAX;
        rc = xwds_dmauartc_tx(&mpc560xb_uart0_cb, str, size, &time);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dmatx;
        }
        return XWOK;

err_dmatx:
        return rc;
}

xwer_t task_dmauart(void * arg)
{
        xwsz_t size;
        xwtm_t time;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                time = XWTM_MAX;
                size = sizeof(rxbuffer);
                rc = xwds_dmauartc_rx(&mpc560xb_uart0_cb, rxbuffer, &size, &time);
                if ((XWOK == rc) && (size > 0)) {
                        time = 5 * XWTM_S;
                        xwosal_cthrd_sleep(&time);
                        rc = bdl_iftx(rxbuffer, size >> 1);
                        if (XWOK == rc) {
                                rc = bdl_iftx(&rxbuffer[size >> 1], size >> 1);
                        }
                }
        }
        return rc;
}

xwer_t task_led(void * arg)
{
        xwtm_t time;
        xwsq_t pinmask;
        xwer_t rc;

        XWOS_UNUSED(arg);
        pinmask = (BIT(SOC_GPIO_PIN_12) | BIT(SOC_GPIO_PIN_13));
        rc = XWOK;
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                rc = xwds_gpio_req(&mpc560xb_soc_cb, LED_PORT, pinmask);
                if (XWOK == rc) {
                        while (1) {
                                pinmask = BIT(SOC_GPIO_PIN_12);
                                xwds_gpio_set(&mpc560xb_soc_cb, LED_PORT,
                                              pinmask);
                                pinmask = BIT(SOC_GPIO_PIN_13);
                                xwds_gpio_reset(&mpc560xb_soc_cb, LED_PORT,
                                                pinmask);
                                time = 500*XWTM_MS;
                                rc = xwosal_cthrd_sleep(&time);
                                if (__xwcc_unlikely(rc < 0)) {
                                        break;
                                }
                                pinmask = BIT(SOC_GPIO_PIN_12);
                                xwds_gpio_reset(&mpc560xb_soc_cb, LED_PORT,
                                                pinmask);
                                pinmask = BIT(SOC_GPIO_PIN_13);
                                xwds_gpio_set(&mpc560xb_soc_cb, LED_PORT,
                                              pinmask);
                                time = 500*XWTM_MS;
                                rc = xwosal_cthrd_sleep(&time);
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

        for (i = 0; i < xw_array_size(board_td_tbl); i ++) {
                rc = xwosal_thrd_create(&board_tcb[i], board_td_tbl[i].name,
                                        board_td_tbl[i].func,
                                        board_td_tbl[i].arg,
                                        board_td_tbl[i].stack_size,
                                        board_td_tbl[i].prio,
                                        board_td_tbl[i].attr);
                BDL_BUG_ON(rc);
        }
        rc = xwosal_scheduler_start_lc();
        return rc;
}
