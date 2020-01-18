/**
 * @file
 * @brief 板级描述层：初始化
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
#include <xwos/lib/xwbop.h>
#include <soc_gpio.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwmd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bdl/ds/description/s32k14xdkp.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BDL_LED_RED_PORT        SOC_GPIO_PORT_B
#define BDL_LED_RED_PIN         SOC_GPIO_PIN_4
#define BDL_LED_GREEN_PORT      SOC_GPIO_PORT_B
#define BDL_LED_GREEN_PIN       SOC_GPIO_PIN_5
#define BDL_LED_BLUE_PORT       SOC_GPIO_PORT_E
#define BDL_LED_BLUE_PIN        SOC_GPIO_PIN_8
#define TASK_LED_PRIORITY       XWOSAL_SD_PRIORITY_RT_MIN

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
void task_led_init(void);

static
xwer_t task_led(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc board_td_table[] = {
        [0] = {
                .name = "led",
                .prio = TASK_LED_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = task_led,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwid_t bdl_tid[xw_array_size(board_td_table)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_main(void)
{
        xwer_t rc;
        xwsz_t i;


        for (i = 0; i < xw_array_size(board_td_table); i ++) {
                rc = xwosal_thrd_create(&bdl_tid[i], board_td_table[i].name,
                                        board_td_table[i].func,
                                        board_td_table[i].arg,
                                        board_td_table[i].stack_size,
                                        board_td_table[i].prio,
                                        board_td_table[i].attr);
                BDL_BUG_ON(rc);
        }
        rc = xwosal_scheduler_start_lc();
        return rc;
}

static
void task_led_init(void)
{
        xwsq_t pinmask;
        xwer_t rc;

        pinmask = BIT(BDL_LED_RED_PIN);
        rc = xwds_gpio_req(&s32k14x_soc_cb, BDL_LED_RED_PORT, pinmask);
        if (rc < 0) {
        }
        pinmask = BIT(BDL_LED_GREEN_PIN);
        rc = xwds_gpio_req(&s32k14x_soc_cb, BDL_LED_GREEN_PORT, pinmask);
        if (rc < 0) {
        }
        pinmask = BIT(BDL_LED_BLUE_PIN);
        rc = xwds_gpio_req(&s32k14x_soc_cb, BDL_LED_BLUE_PORT, pinmask);
        if (rc < 0) {
        }
}

xwer_t task_led(void * arg)
{
        xwsq_t pinmask;
        xwtm_t time;
        xwer_t rc = OK;

        XWOS_UNUSED(arg);
        task_led_init();
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                pinmask = BIT(BDL_LED_RED_PIN);
                xwds_gpio_reset(&s32k14x_soc_cb, BDL_LED_RED_PORT, pinmask);
                pinmask = BIT(BDL_LED_GREEN_PIN);
                xwds_gpio_reset(&s32k14x_soc_cb, BDL_LED_GREEN_PORT, pinmask);
                pinmask = BIT(BDL_LED_BLUE_PIN);
                xwds_gpio_set(&s32k14x_soc_cb, BDL_LED_BLUE_PORT, pinmask);
                time = 500 * XWTM_MS;
                rc = xwosal_cthrd_sleep(&time);
                if (__unlikely(rc < 0)) {
                        break;
                }

                pinmask = BIT(BDL_LED_BLUE_PIN);
                xwds_gpio_reset(&s32k14x_soc_cb, BDL_LED_BLUE_PORT, pinmask);
                pinmask = BIT(BDL_LED_RED_PIN);
                xwds_gpio_reset(&s32k14x_soc_cb, BDL_LED_RED_PORT, pinmask);
                pinmask = BIT(BDL_LED_GREEN_PIN);
                xwds_gpio_set(&s32k14x_soc_cb, BDL_LED_GREEN_PORT, pinmask);
                time = 500 * XWTM_MS;
                rc = xwosal_cthrd_sleep(&time);
                if (__unlikely(rc < 0)) {
                        break;
                }

                pinmask = BIT(BDL_LED_GREEN_PIN);
                xwds_gpio_reset(&s32k14x_soc_cb, BDL_LED_GREEN_PORT, pinmask);
                pinmask = BIT(BDL_LED_BLUE_PIN);
                xwds_gpio_reset(&s32k14x_soc_cb, BDL_LED_BLUE_PORT, pinmask);
                pinmask = BIT(BDL_LED_RED_PIN);
                xwds_gpio_set(&s32k14x_soc_cb, BDL_LED_RED_PORT, pinmask);
                time = 500 * XWTM_MS;
                rc = xwosal_cthrd_sleep(&time);
                if (__unlikely(rc < 0)) {
                        break;
                }
        }
        return rc;
}
