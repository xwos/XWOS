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

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/pm.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>
#include <bm/stm32cube/mif.h>
#include <bm/stm32cube/xwac/xwds/pm.h>
#include <bm/pm/mif.h>

#define BRDPM_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

#define BRDPM_BTN_GPIO_PORT XWDS_GPIO_PORT_I
#define BRDPM_BTN_GPIO_PIN XWDS_GPIO_PIN_8
#define BRDPM_BTN_DEBOUNCING_DELAY (20 * XWTM_MS)
#define BRDPM_BTN_LONGPRESS_CNT (100)

#define BRDPM_LED_GPIO_PORT XWDS_GPIO_PORT_G
#define BRDPM_LED_GPIO_PIN XWDS_GPIO_PIN_7

enum brdpm_btn_event_em {
        BRDPM_BTNEVT_CLICK,
        BRDPM_BTNEVT_LONGPRESS,
};

static
void xwospl_pm_cb_resume(void * arg);

static
void xwospl_pm_cb_suspend(void * arg);

static
void xwospl_pm_cb_wakeup(void * arg);

static
void xwospl_pm_cb_sleep(void * arg);

static
void brdpm_suspend(void);

static
void brdpm_resume(void);

static
xwer_t brdpm_get_btn_evt(xwsq_t * evt);

static
xwer_t brdpm_thd_init(void);

static
void brdpm_thd_deinit(void);

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
xwer_t brdpm_thd(void * arg);

const struct xwos_thd_desc brdpm_thd_td = {
        .name = "bm.pm.thd",
        .prio = BRDPM_THD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwos_thd_f)brdpm_thd,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
xwos_thd_d brdpm_thdd;

struct xwos_sem brdpm_sem;

xwer_t brdpm_start(void)
{
        xwer_t rc;

        xwos_pm_set_cb(xwospl_pm_cb_resume,
                       xwospl_pm_cb_suspend,
                       xwospl_pm_cb_wakeup,
                       xwospl_pm_cb_sleep,
                       NULL);

        rc = xwos_sem_init(&brdpm_sem, 0, 1);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_init;
        }

        rc = xwos_thd_create(&brdpm_thdd,
                             brdpm_thd_td.name,
                             brdpm_thd_td.func,
                             brdpm_thd_td.arg,
                             brdpm_thd_td.stack_size,
                             brdpm_thd_td.prio,
                             brdpm_thd_td.attr);
        if (rc < 0) {
                goto err_thd_create;
        }
        return XWOK;

err_thd_create:
        xwos_sem_destroy(&brdpm_sem);
err_sem_init:
        return rc;
}

xwer_t brdpm_stop(void)
{
        xwer_t rc, trc;

        rc = xwos_thd_stop(brdpm_thdd, &trc);
        xwos_sem_destroy(&brdpm_sem);
        return rc;
}

static
void brdpm_suspend(void)
{
        xwds_gpio_rls(&stm32cube_soc_cb,
                      BRDPM_LED_GPIO_PORT, BRDPM_LED_GPIO_PIN);
}

static
void brdpm_resume(void)
{
        xwds_gpio_req(&stm32cube_soc_cb,
                      BRDPM_LED_GPIO_PORT, BRDPM_LED_GPIO_PIN);
}

static
void xwospl_pm_cb_resume(void * arg)
{
        XWOS_UNUSED(arg);
        stm32cube_pm_resume();
}

static
void xwospl_pm_cb_suspend(void * arg)
{
        XWOS_UNUSED(arg);
        stm32cube_pm_suspend();
}
static
void xwospl_pm_cb_wakeup(void * arg)
{
        XWOS_UNUSED(arg);
        stm32cube_pm_wakeup();
}

static
void xwospl_pm_cb_sleep(void * arg)
{
        XWOS_UNUSED(arg);
        stm32cube_pm_sleep();
}

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

        time = BRDPM_BTN_DEBOUNCING_DELAY;
        rc = xwos_cthd_sleep(&time);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sleep;
        }

        cnt = 0;
        do {
                rc = xwds_gpio_input(&stm32cube_soc_cb,
                                     BRDPM_BTN_GPIO_PORT,
                                     BRDPM_BTN_GPIO_PIN,
                                     &in);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_gpio_input;
                }
                cnt++;
                time = BRDPM_BTN_DEBOUNCING_DELAY;
                rc = xwos_cthd_sleep(&time);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sleep;
                }
        } while (0 == in);

        if (cnt < BRDPM_BTN_LONGPRESS_CNT) {
                *evt = BRDPM_BTNEVT_CLICK;
        } else {
                *evt = BRDPM_BTNEVT_LONGPRESS;
        }
        return XWOK;

err_gpio_input:
err_sleep:
err_sem_wait:
        return rc;
}

static
xwer_t brdpm_thd_init(void)
{
        xwer_t rc;

        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BRDPM_LED_GPIO_PORT, BRDPM_LED_GPIO_PIN);
        if (rc < 0) {
                goto err_led_gpio_req;
        }
        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BRDPM_BTN_GPIO_PORT, BRDPM_BTN_GPIO_PIN);
        if (rc < 0) {
                goto err_btn_gpio_req;
        }
        rc = xwds_eirq_req(&stm32cube_soc_cb,
                           BRDPM_BTN_GPIO_PORT, BRDPM_BTN_GPIO_PIN,
                           8, XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
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
void brdpm_thd_deinit(void)
{
        xwds_eirq_rls(&stm32cube_soc_cb, BRDPM_BTN_GPIO_PORT, BRDPM_BTN_GPIO_PIN, 8);
        xwds_gpio_rls(&stm32cube_soc_cb, BRDPM_BTN_GPIO_PORT, BRDPM_BTN_GPIO_PIN);
        xwds_gpio_rls(&stm32cube_soc_cb,
                      BRDPM_LED_GPIO_PORT, BRDPM_LED_GPIO_PIN);
}

static
void brdpm_req_btn_irq(void)
{
        xwds_eirq_req(&stm32cube_soc_cb,
                      BRDPM_BTN_GPIO_PORT, BRDPM_BTN_GPIO_PIN,
                      8, XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                      brdpm_eirq_btn_isr, &brdpm_sem);
}

static
void brdpm_rls_btn_irq(void)
{
        xwds_eirq_rls(&stm32cube_soc_cb, BRDPM_BTN_GPIO_PORT, BRDPM_BTN_GPIO_PIN, 8);
}

static
void brdpm_led_blink(void)
{
        xwtm_t time;

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BRDPM_LED_GPIO_PORT,
                         BRDPM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BRDPM_LED_GPIO_PORT,
                         BRDPM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BRDPM_LED_GPIO_PORT,
                         BRDPM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BRDPM_LED_GPIO_PORT,
                         BRDPM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwos_cthd_sleep(&time);
}

static
void brdpm_handle_evt(xwsq_t evt)
{
        switch (evt) {
        case BRDPM_BTNEVT_CLICK:
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 BRDPM_LED_GPIO_PORT,
                                 BRDPM_LED_GPIO_PIN);
                break;
        case BRDPM_BTNEVT_LONGPRESS:
                brdpm_led_blink();
                xwos_pm_suspend();
                break;
        default:
                break;
        }
}

static
xwer_t brdpm_thd(void * arg)
{
        xwer_t rc;
        xwsq_t evt;

        XWOS_UNUSED(arg);

        rc = brdpm_thd_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_init;
        }

        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        brdpm_suspend();
                        xwos_cthd_freeze();
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
        brdpm_thd_deinit();
err_init:
        return rc;
}

static
void brdpm_eirq_btn_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(id);

        if (xwos_pm_get_stage() < XWOS_PM_STAGE_RUNNING) {
                xwos_pm_resume();
        }
        brdpm_rls_btn_irq();
        xwos_sem_post((struct xwos_sem *)arg);
}
