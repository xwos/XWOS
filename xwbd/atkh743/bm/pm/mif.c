/**
 * @file
 * @brief 电源管理模块：接口
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
#include <xwos/osal/skd.h>
#include <xwos/osal/pm.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>
#include <bm/stm32cube/mif.h>
#include <bm/pm/mif.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define PM_THRD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

#define PM_BTN_GPIO_PORT XWDS_GPIO_PORT_H
#define PM_BTN_GPIO_PIN XWDS_GPIO_PIN_3
#define PM_BTN_IRQLINE 3
#define PM_BTN_DEBOUNCING_DELAY (20 * XWTM_MS)
#define PM_BTN_LONGPRESS_CNT (100)

#define PM_LED_GPIO_PORT XWDS_GPIO_PORT_B
#define PM_LED_GPIO_PIN XWDS_GPIO_PIN_1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum brdpm_btn_event_em {
        PM_BTNEVT_CLICK,
        PM_BTNEVT_LONGPRESS,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/* static */
/* void xwospl_pm_cb_resume(void * arg); */

/* static */
/* void xwospl_pm_cb_suspend(void * arg); */

/* static */
/* void xwospl_pm_cb_wakeup(void * arg); */

/* static */
/* void xwospl_pm_cb_sleep(void * arg); */

static
void brdpm_suspend(void);

static
void brdpm_resume(void);

static
xwer_t brdpm_get_btn_evt(xwsq_t * evt);

static
xwer_t brdpm_thrd_init(void);

static
void brdpm_thrd_deinit(void);

static
void brdpm_req_btn_irq(void);

static
void brdpm_rls_btn_irq(void);

static
void brdpm_handle_evt(xwsq_t evt);

static
void brdpm_led_blink(void);

static
void brdpm_eirq_btn_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg);

static
xwer_t brdpm_thrd(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwos_thrd_desc brdpm_thrd_td = {
        .name = "bm.pm.thrd",
        .prio = PM_THRD_PRIORITY,
        .stack = XWOS_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwos_thrd_f)brdpm_thrd,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
xwid_t brdpm_thrd_id;

struct xwos_sem brdpm_sem;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t brdpm_start(void)
{
        xwer_t rc;

        /* xwos_pm_set_cb(xwospl_pm_cb_resume, */
        /*                xwospl_pm_cb_suspend, */
        /*                xwospl_pm_cb_wakeup, */
        /*                xwospl_pm_cb_sleep, */
        /*                NULL); */

        rc = xwos_sem_init(&brdpm_sem, 0, 1);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_init;
        }

        rc = xwos_thrd_create(&brdpm_thrd_id,
                              brdpm_thrd_td.name,
                              brdpm_thrd_td.func,
                              brdpm_thrd_td.arg,
                              brdpm_thrd_td.stack_size,
                              brdpm_thrd_td.prio,
                              brdpm_thrd_td.attr);
        if (rc < 0) {
                goto err_thrd_create;
        }
        return XWOK;

err_thrd_create:
        xwos_sem_destroy(&brdpm_sem);
err_sem_init:
        return rc;
}

xwer_t brdpm_stop(void)
{
        xwer_t rc, trc;

        rc = xwos_thrd_terminate(brdpm_thrd_id, &trc);
        if (XWOK == rc) {
                rc = xwos_thrd_delete(brdpm_thrd_id);
                if (XWOK == rc) {
                        brdpm_thrd_id = 0;
                }
        }
        xwos_sem_destroy(&brdpm_sem);
        return rc;
}

static
void brdpm_suspend(void)
{
        xwds_gpio_rls(&stm32cube_soc_cb,
                      PM_LED_GPIO_PORT, PM_LED_GPIO_PIN);
}

static
void brdpm_resume(void)
{
        xwds_gpio_req(&stm32cube_soc_cb,
                      PM_LED_GPIO_PORT, PM_LED_GPIO_PIN);
}

/* static */
/* void xwospl_pm_cb_resume(void * arg) */
/* { */
/*         XWOS_UNUSED(arg); */
/*         stm32cube_pm_resume(); */
/* } */

/* static */
/* void xwospl_pm_cb_suspend(void * arg) */
/* { */
/*         XWOS_UNUSED(arg); */
/*         stm32cube_pm_suspend(); */
/* } */
/* static */
/* void xwospl_pm_cb_wakeup(void * arg) */
/* { */
/*         XWOS_UNUSED(arg); */
/*         stm32cube_pm_wakeup(); */
/* } */

/* static */
/* void xwospl_pm_cb_sleep(void * arg) */
/* { */
/*         XWOS_UNUSED(arg); */
/*         stm32cube_pm_sleep(); */
/* } */

static
xwer_t brdpm_get_btn_evt(xwsq_t * evt)
{
        xwer_t rc;
        xwsq_t in;
        xwsq_t cnt;
        xwtm_t time;

        rc = xwos_sem_wait(&brdpm_sem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_wait;
        }

        time = PM_BTN_DEBOUNCING_DELAY;
        rc = xwos_cthrd_sleep(&time);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sleep;
        }

        cnt = 0;
        do {
                rc = xwds_gpio_input(&stm32cube_soc_cb,
                                     PM_BTN_GPIO_PORT,
                                     PM_BTN_GPIO_PIN,
                                     &in);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_gpio_input;
                }
                cnt++;
                time = PM_BTN_DEBOUNCING_DELAY;
                rc = xwos_cthrd_sleep(&time);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sleep;
                }
        } while (0 == in);

        if (cnt < PM_BTN_LONGPRESS_CNT) {
                *evt = PM_BTNEVT_CLICK;
        } else {
                *evt = PM_BTNEVT_LONGPRESS;
        }
        return XWOK;

err_gpio_input:
err_sleep:
err_sem_wait:
        return rc;
}

static
xwer_t brdpm_thrd_init(void)
{
        xwer_t rc;

        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           PM_LED_GPIO_PORT, PM_LED_GPIO_PIN);
        if (rc < 0) {
                goto err_led_gpio_req;
        }
        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           PM_BTN_GPIO_PORT, PM_BTN_GPIO_PIN);
        if (rc < 0) {
                goto err_btn_gpio_req;
        }
        rc = xwds_eirq_req(&stm32cube_soc_cb,
                           PM_BTN_GPIO_PORT, PM_BTN_GPIO_PIN,
                           PM_BTN_IRQLINE,
                           XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                           brdpm_eirq_btn_isr, &brdpm_sem);
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
void brdpm_thrd_deinit(void)
{
        xwds_eirq_rls(&stm32cube_soc_cb,
                      PM_BTN_GPIO_PORT, PM_BTN_GPIO_PIN,
                      PM_BTN_IRQLINE);
        xwds_gpio_rls(&stm32cube_soc_cb, PM_BTN_GPIO_PORT, PM_BTN_GPIO_PIN);
        xwds_gpio_rls(&stm32cube_soc_cb,
                      PM_LED_GPIO_PORT, PM_LED_GPIO_PIN);
}

static
void brdpm_req_btn_irq(void)
{
        xwds_eirq_req(&stm32cube_soc_cb,
                      PM_BTN_GPIO_PORT, PM_BTN_GPIO_PIN,
                      PM_BTN_IRQLINE, XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                      brdpm_eirq_btn_isr, &brdpm_sem);
}

static
void brdpm_rls_btn_irq(void)
{
        xwds_eirq_rls(&stm32cube_soc_cb,
                      PM_BTN_GPIO_PORT, PM_BTN_GPIO_PIN,
                      PM_BTN_IRQLINE);
}

static
void brdpm_led_blink(void)
{
        xwtm_t time;

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         PM_LED_GPIO_PORT,
                         PM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         PM_LED_GPIO_PORT,
                         PM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         PM_LED_GPIO_PORT,
                         PM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         PM_LED_GPIO_PORT,
                         PM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthrd_sleep(&time);
}

static
void brdpm_handle_evt(xwsq_t evt)
{
        switch (evt) {
        case PM_BTNEVT_CLICK:
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 PM_LED_GPIO_PORT,
                                 PM_LED_GPIO_PIN);
                break;
        case PM_BTNEVT_LONGPRESS:
                brdpm_led_blink();
                /* xwos_pm_suspend(); */
                break;
        default:
                break;
        }
}

static
xwer_t brdpm_thrd(void * arg)
{
        xwer_t rc;
        xwsq_t evt;

        XWOS_UNUSED(arg);

        rc = brdpm_thrd_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_init;
        }

        while (!xwos_cthrd_shld_stop()) {
                if (xwos_cthrd_shld_frz()) {
                        brdpm_suspend();
                        xwos_cthrd_freeze();
                        brdpm_resume();
                } else {
                        rc = brdpm_get_btn_evt(&evt);
                        if (XWOK == rc) {
                                brdpm_handle_evt(evt);
                                brdpm_req_btn_irq();
                        } else if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                brdpm_req_btn_irq();
                                /* continue to do freeze */
                        } else if (-ETIMEDOUT == rc) {
                                brdpm_req_btn_irq();
                        } else {
                                brdpm_req_btn_irq();
                                goto err_get_btn_evt;
                        }
                }
        }

err_get_btn_evt:
        brdpm_thrd_deinit();
err_init:
        return rc;
}

static
void brdpm_eirq_btn_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(id);

        /* if (xwos_pm_get_stage() < XWOS_PM_STAGE_RUNNING) { */
        /*         xwos_pm_resume(); */
        /* } */
        brdpm_rls_btn_irq();
        xwos_sem_post((struct xwos_sem *)arg);
}
