/**
 * @file
 * @brief 应用程序入口
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
#include <xwos/mm/mempool/allocator.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/eirq.h>
#include <bm/stm32cube/xwds/stm32cube.h>
#include <bm/stm32cube/xwds/init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define LED_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1))
#define BUTTON_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 2))

#define MEMTST_TASK_PRIORITY \
        (XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 3))

#define BUTTON_GPIO_PORT SODS_GPIO_PORT_I
#define BUTTON_GPIO_PIN SODS_GPIO_PIN_8
#define BUTTON_DEBOUNCING_DELAY (20 * XWTM_MS)

#define BLUE_LED_GPIO_PORT SODS_GPIO_PORT_D
#define BLUE_LED_GPIO_PIN SODS_GPIO_PIN_12

#define GREEN_LED_GPIO_PORT SODS_GPIO_PORT_G
#define GREEN_LED_GPIO_PIN SODS_GPIO_PIN_7

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t led_task(void * arg);

xwer_t button_task(void * arg);

xwer_t memtst_task(void * arg);

void eirq_button_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
        [1] = {
                .name = "task.button",
                .prio = BUTTON_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = button_task,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [2] = {
                .name = "task.memtst",
                .prio = MEMTST_TASK_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = memtst_task,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwid_t stm32cube_tid[xw_array_size(stm32cube_tbd)];

struct xwosal_smr button_smr;

extern struct xwmm_mempool * eram_mempool;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_stm32cube_start(void)
{
        xwer_t rc;
        xwsz_t i;

        /* start devices */
        rc = stm32cube_xwds_uart_start();
        if (rc < 0) {
                goto err_xwds_uart_start;
        }

        rc = xwosal_smr_init(&button_smr, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_button_smr_init;
        }

        /* start thread */
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

        return OK;

err_thrd_create:
err_button_smr_init:
        stm32cube_xwds_uart_stop();
err_xwds_uart_start:
        return rc;
}

xwer_t led_task(void * arg)
{
        xwtm_t xwtm;
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = xwds_gpio_req(&stm32cube_soc_cb, BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
        if (rc < 0) {
                goto err_gpio_req;
        }

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwds_gpio_set(&stm32cube_soc_cb,
                              BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
                xwds_gpio_reset(&stm32cube_soc_cb,
                                BLUE_LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
                xwtm = 1 * XWTM_S;
                xwosal_cthrd_sleep(&xwtm);
        }
        return OK;

err_gpio_req:
        return rc;
}

static
xwer_t button_task_fsm(void)
{
        xwer_t rc;
        xwsq_t in;
        xwtm_t sleep;
        xwsq_t cnt;

        sleep = BUTTON_DEBOUNCING_DELAY;
        rc = xwosal_cthrd_sleep(&sleep);
        if (__unlikely(rc < 0)) {
                goto err_sleep;
        }

        xwds_gpio_input(&stm32cube_soc_cb,
                        BUTTON_GPIO_PORT,
                        BUTTON_GPIO_PIN,
                        &in);

        if (0 == in) {
                cnt = 0;
                while (true) {
                        sleep = BUTTON_DEBOUNCING_DELAY;
                        rc = xwosal_cthrd_sleep(&sleep);
                        if (__unlikely(rc < 0)) {
                                goto err_sleep;
                        }

                        xwds_gpio_input(&stm32cube_soc_cb,
                                        BUTTON_GPIO_PORT,
                                        BUTTON_GPIO_PIN,
                                        &in);

                        if (in) {
                                break;
                        }
                        cnt++;
                }
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 GREEN_LED_GPIO_PORT, GREEN_LED_GPIO_PIN);
        } else {
        }

err_sleep:
        return rc;
}

xwer_t button_task(void * arg)
{
        xwer_t rc;
        xwid_t smrid;

        XWOS_UNUSED(arg);
        smrid = xwosal_smr_get_id(&button_smr);

        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           GREEN_LED_GPIO_PORT, GREEN_LED_GPIO_PIN);
        if (rc < 0) {
                goto err_led_gpio_req;
        }
        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BUTTON_GPIO_PORT, BUTTON_GPIO_PIN);
        if (rc < 0) {
                goto err_button_gpio_req;
        }

        rc = xwds_eirq_req(&stm32cube_soc_cb, BUTTON_GPIO_PORT, BUTTON_GPIO_PIN,
                           8, SODS_SOC_EIF_TM_FALLING,
                           eirq_button_isr, &button_smr);
        if (__unlikely(rc < 0)) {
                goto err_eirq_req;
        }

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                rc = xwosal_smr_wait(smrid);
                if (OK == rc) {
                        button_task_fsm();
                }
        }
        return OK;

err_eirq_req:
err_button_gpio_req:
err_led_gpio_req:
        return rc;
}

void eirq_button_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg)
{
        xwid_t smrid;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(id);

        smrid = xwosal_smr_get_id(&button_smr);
        xwosal_smr_post(smrid);
}

xwer_t memtst_task(void * arg)
{
        xwer_t rc = OK;
        void * mem;
        xwtm_t xwtm;
        __xw_io bool running = true;

        XWOS_UNUSED(arg);

        while ((!xwosal_cthrd_frz_shld_stop(NULL)) && (running)) {
                xwtm = 2 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 8, &mem);
                xwtm = 10 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 16, &mem);
                xwtm = 6 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 32, &mem);
                xwtm = 12 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 4 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 64, &mem);
                xwtm = 9 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 128, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 192, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 18 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 256, &mem);
                xwtm = 6 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 384, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 1 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 512, &mem);
                xwtm = 1 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 15 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 768, &mem);
                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 1024, &mem);
                xwtm = 11 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 8 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 2048, &mem);
                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 7 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 4096, &mem);
                xwtm = 5 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);

                xwtm = 3 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_malloc(eram_mempool, 8192, &mem);
                xwtm = 2 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
                rc = xwmm_mempool_free(eram_mempool, mem);
        }

        return rc;
}
