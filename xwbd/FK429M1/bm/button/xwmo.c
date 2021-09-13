/**
 * @file
 * @brief 按键模块
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
#include <xwos/osal/pm.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>
#include <bm/stm32cube/mif.h>
#include <bm/button/mif.h>

#define BMBTN_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

#define BMBTN_GPIO_PORT XWDS_GPIO_PORT_I
#define BMBTN_GPIO_PIN XWDS_GPIO_PIN_8
#define BMBTN_DEBOUNCING_DELAY (20 * XWTM_MS)
#define BMBTN_LONGPRESS_CNT (100)

#define BMBTN_LED_GPIO_PORT XWDS_GPIO_PORT_G
#define BMBTN_LED_GPIO_PIN XWDS_GPIO_PIN_7

enum bmbtn_event_em {
        BMBTN_EVT_CLICK,
        BMBTN_EVT_LONGPRESS,
};

static
void bmbtn_suspend(void);

static
void bmbtn_resume(void);

static
xwer_t bmbtn_get_btn_evt(xwsq_t * evt);

static
xwer_t bmbtn_thd_init(void);

static
void bmbtn_thd_deinit(void);

static
void bmbtn_req_btn_irq(void);

static
void bmbtn_rls_btn_irq(void);

static
void bmbtn_handle_evt(xwsq_t evt);

static
void bmbtn_led_blink(void);

static
void bmbtn_eirq_btn_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg);

static
xwer_t bmbtn_task(void * arg);

const struct xwos_thd_desc bmbtn_thd_desc = {
        .name = "bm.btn.thd",
        .prio = BMBTN_THD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = bmbtn_task,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED | XWOS_SKDATTR_DETACHED,
};
struct xwos_thd * bmbtn_thd;

struct xwos_sem bmbtn_sem;

xwer_t bmbtn_start(void)
{
        xwer_t rc;

        rc = xwos_sem_init(&bmbtn_sem, 0, 1);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_init;
        }

        rc = xwos_thd_create(&bmbtn_thd,
                             bmbtn_thd_desc.name,
                             bmbtn_thd_desc.func,
                             bmbtn_thd_desc.arg,
                             bmbtn_thd_desc.stack_size,
                             bmbtn_thd_desc.prio,
                             bmbtn_thd_desc.attr);
        if (rc < 0) {
                goto err_thd_create;
        }
        return XWOK;

err_thd_create:
        xwos_sem_fini(&bmbtn_sem);
err_sem_init:
        return rc;
}

xwer_t bmbtn_stop(void)
{
        xwer_t rc;

        rc = xwos_thd_cancel(bmbtn_thd);
        xwos_sem_fini(&bmbtn_sem);
        return rc;
}

static
void bmbtn_suspend(void)
{
        xwds_gpio_rls(&stm32cube_soc_cb,
                      BMBTN_LED_GPIO_PORT, BMBTN_LED_GPIO_PIN);
}

static
void bmbtn_resume(void)
{
        xwds_gpio_req(&stm32cube_soc_cb,
                      BMBTN_LED_GPIO_PORT, BMBTN_LED_GPIO_PIN);
}

static
xwer_t bmbtn_get_btn_evt(xwsq_t * evt)
{
        xwer_t rc;
        xwsq_t in;
        xwsq_t cnt;
        xwtm_t time;

        rc = xwos_sem_wait(&bmbtn_sem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_wait;
        }

        time = BMBTN_DEBOUNCING_DELAY;
        rc = xwos_cthd_sleep(&time);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sleep;
        }

        cnt = 0;
        do {
                rc = xwds_gpio_input(&stm32cube_soc_cb,
                                     BMBTN_GPIO_PORT,
                                     BMBTN_GPIO_PIN,
                                     &in);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_gpio_input;
                }
                cnt++;
                time = BMBTN_DEBOUNCING_DELAY;
                rc = xwos_cthd_sleep(&time);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sleep;
                }
        } while (0 == in);

        if (cnt < BMBTN_LONGPRESS_CNT) {
                *evt = BMBTN_EVT_CLICK;
        } else {
                *evt = BMBTN_EVT_LONGPRESS;
        }
        return XWOK;

err_gpio_input:
err_sleep:
err_sem_wait:
        return rc;
}

static
xwer_t bmbtn_thd_init(void)
{
        xwer_t rc;

        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BMBTN_LED_GPIO_PORT, BMBTN_LED_GPIO_PIN);
        if (rc < 0) {
                goto err_led_gpio_req;
        }
        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BMBTN_GPIO_PORT, BMBTN_GPIO_PIN);
        if (rc < 0) {
                goto err_btn_gpio_req;
        }
        rc = xwds_eirq_req(&stm32cube_soc_cb,
                           BMBTN_GPIO_PORT, BMBTN_GPIO_PIN,
                           8, XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                           bmbtn_eirq_btn_isr, &bmbtn_sem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_eirq_req;
        }
        return XWOK;

err_eirq_req:
err_btn_gpio_req:
err_led_gpio_req:
        return rc;
}

static
void bmbtn_thd_deinit(void)
{
        xwds_eirq_rls(&stm32cube_soc_cb, BMBTN_GPIO_PORT, BMBTN_GPIO_PIN, 8);
        xwds_gpio_rls(&stm32cube_soc_cb, BMBTN_GPIO_PORT, BMBTN_GPIO_PIN);
        xwds_gpio_rls(&stm32cube_soc_cb,
                      BMBTN_LED_GPIO_PORT, BMBTN_LED_GPIO_PIN);
}

static
void bmbtn_req_btn_irq(void)
{
        xwds_eirq_req(&stm32cube_soc_cb,
                      BMBTN_GPIO_PORT, BMBTN_GPIO_PIN,
                      8, XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                      bmbtn_eirq_btn_isr, &bmbtn_sem);
}

static
void bmbtn_rls_btn_irq(void)
{
        xwds_eirq_rls(&stm32cube_soc_cb, BMBTN_GPIO_PORT, BMBTN_GPIO_PIN, 8);
}

static
void bmbtn_led_blink(void)
{
        xwtm_t time;

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BMBTN_LED_GPIO_PORT,
                         BMBTN_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BMBTN_LED_GPIO_PORT,
                         BMBTN_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BMBTN_LED_GPIO_PORT,
                         BMBTN_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BMBTN_LED_GPIO_PORT,
                         BMBTN_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthd_sleep(&time);
}

static
void bmbtn_handle_evt(xwsq_t evt)
{
        switch (evt) {
        case BMBTN_EVT_CLICK:
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 BMBTN_LED_GPIO_PORT,
                                 BMBTN_LED_GPIO_PIN);
                break;
        case BMBTN_EVT_LONGPRESS:
                bmbtn_led_blink();
                xwos_pm_suspend();
                break;
        default:
                break;
        }
}

static
xwer_t bmbtn_task(void * arg)
{
        xwer_t rc;
        xwsq_t evt;

        XWOS_UNUSED(arg);

        rc = bmbtn_thd_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_init;
        }

        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        bmbtn_suspend();
                        xwos_cthd_freeze();
                        bmbtn_resume();
                } else {
                        rc = bmbtn_get_btn_evt(&evt);
                        if (XWOK == rc) {
                                bmbtn_handle_evt(evt);
                                bmbtn_req_btn_irq();
                        } else if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                bmbtn_req_btn_irq();
                                /* continue to do freeze */
                        } else if (-ETIMEDOUT == rc) {
                                bmbtn_req_btn_irq();
                        } else {
                                bmbtn_req_btn_irq();
                                goto err_get_btn_evt;
                        }
                }
        }

err_get_btn_evt:
        bmbtn_thd_deinit();
err_init:
        return rc;
}

static
void bmbtn_eirq_btn_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(id);

        if (xwos_pm_get_stage() < XWOS_PM_STAGE_RUNNING) {
                xwos_pm_resume();
        }
        bmbtn_rls_btn_irq();
        xwos_sem_post((struct xwos_sem *)arg);
}
