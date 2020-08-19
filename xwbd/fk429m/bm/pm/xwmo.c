/**
 * @file
 * @brief Board Module: Power Management
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <soc_xwpmdm.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/eirq.h>
#include <bm/stm32cube/xwac/xwds/stm32cube.h>
#include <bm/stm32cube/xwac/xwds/pm.h>
#include <bm/pm/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BM_PM_THRD_PRIORITY XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define BM_PM_BTN_GPIO_PORT XWDS_GPIO_PORT_I
#define BM_PM_BTN_GPIO_PIN XWDS_GPIO_PIN_8
#define BM_PM_BTN_DEBOUNCING_DELAY (20 * XWTM_MS)
#define BM_PM_BTN_LONGPRESS_CNT (100)

#define BM_PM_LED_GPIO_PORT XWDS_GPIO_PORT_G
#define BM_PM_LED_GPIO_PIN XWDS_GPIO_PIN_7

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum bm_pm_btn_event_em {
        BM_PM_BTNEVT_CLICK,
        BM_PM_BTNEVT_LONGPRESS,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
void bm_pmdm_resume(struct xwos_pmdm * pmdm, void * arg);

static
void bm_pmdm_suspend(struct xwos_pmdm * pmdm, void * arg);

static
void bm_pmdm_wakeup(struct xwos_pmdm * pmdm, void * arg);

static
void bm_pmdm_sleep(struct xwos_pmdm * pmdm, void * arg);

static
void bm_pm_suspend(void);

static
void bm_pm_resume(void);

static
xwer_t bm_pm_get_btn_evt(xwsq_t * evt);

static
xwer_t bm_pm_thrd_init(void);

static
void bm_pm_thrd_deinit(void);

static
void bm_pm_req_btn_irq(void);

static
void bm_pm_rls_btn_irq(void);

static
void bm_pm_handle_evt(xwsq_t evt);

static
void bm_pm_led_blink(void);

static
void bm_pm_eirq_btn_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg);

static
xwer_t bm_pm_thrd(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc bm_pm_thrd_td = {
        .name = "bm.pm.thrd",
        .prio = BM_PM_THRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)bm_pm_thrd,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t bm_pm_thrd_id;

struct xwosal_smr bm_pm_smr;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t bm_pm_start(void)
{
        xwer_t rc;

        xwos_pmdm_set_cb(&soc_xwpm_domain,
                         bm_pmdm_resume,
                         bm_pmdm_suspend,
                         bm_pmdm_wakeup,
                         bm_pmdm_sleep,
                         NULL);

        rc = xwosal_smr_init(&bm_pm_smr, 0, 1);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_smr_init;
        }

        rc = xwosal_thrd_create(&bm_pm_thrd_id,
                                bm_pm_thrd_td.name,
                                bm_pm_thrd_td.func,
                                bm_pm_thrd_td.arg,
                                bm_pm_thrd_td.stack_size,
                                bm_pm_thrd_td.prio,
                                bm_pm_thrd_td.attr);
        if (rc < 0) {
                goto err_thrd_create;
        }
        return XWOK;

err_thrd_create:
        xwosal_smr_destroy(&bm_pm_smr);
err_smr_init:
        return rc;
}

xwer_t bm_pm_stop(void)
{
        xwer_t rc, trc;

        rc = xwosal_thrd_terminate(bm_pm_thrd_id, &trc);
        if (XWOK == rc) {
                rc = xwosal_thrd_delete(bm_pm_thrd_id);
                if (XWOK == rc) {
                        bm_pm_thrd_id = 0;
                }
        }
        xwosal_smr_destroy(&bm_pm_smr);
        return rc;
}

static
void bm_pm_suspend(void)
{
        xwds_gpio_rls(&stm32cube_soc_cb,
                      BM_PM_LED_GPIO_PORT, BM_PM_LED_GPIO_PIN);
}

static
void bm_pm_resume(void)
{
        xwds_gpio_req(&stm32cube_soc_cb,
                      BM_PM_LED_GPIO_PORT, BM_PM_LED_GPIO_PIN);
}

static
void bm_pmdm_resume(struct xwos_pmdm * pmdm, void * arg)
{
        XWOS_UNUSED(pmdm);
        XWOS_UNUSED(arg);
        stm32cube_pm_resume();
}

static
void bm_pmdm_suspend(struct xwos_pmdm * pmdm, void * arg)
{
        XWOS_UNUSED(pmdm);
        XWOS_UNUSED(arg);
        stm32cube_pm_suspend();
}
static
void bm_pmdm_wakeup(struct xwos_pmdm * pmdm, void * arg)
{
        XWOS_UNUSED(pmdm);
        XWOS_UNUSED(arg);
        stm32cube_pm_wakeup();
}

static
void bm_pmdm_sleep(struct xwos_pmdm * pmdm, void * arg)
{
        XWOS_UNUSED(pmdm);
        XWOS_UNUSED(arg);
        stm32cube_pm_sleep();
}

static
xwer_t bm_pm_get_btn_evt(xwsq_t * evt)
{
        xwer_t rc;
        xwid_t smrid;
        xwsq_t in;
        xwsq_t cnt;
        xwtm_t time;

        smrid = xwosal_smr_get_id(&bm_pm_smr);
        rc = xwosal_smr_wait(smrid);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_smr_wait;
        }

        time = BM_PM_BTN_DEBOUNCING_DELAY;
        rc = xwosal_cthrd_sleep(&time);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sleep;
        }

        cnt = 0;
        do {
                rc = xwds_gpio_input(&stm32cube_soc_cb,
                                     BM_PM_BTN_GPIO_PORT,
                                     BM_PM_BTN_GPIO_PIN,
                                     &in);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_gpio_input;
                }
                cnt++;
                time = BM_PM_BTN_DEBOUNCING_DELAY;
                rc = xwosal_cthrd_sleep(&time);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sleep;
                }
        } while (0 == in);

        if (cnt < BM_PM_BTN_LONGPRESS_CNT) {
                *evt = BM_PM_BTNEVT_CLICK;
        } else {
                *evt = BM_PM_BTNEVT_LONGPRESS;
        }
        return XWOK;

err_gpio_input:
err_sleep:
err_smr_wait:
        return rc;
}

static
xwer_t bm_pm_thrd_init(void)
{
        xwer_t rc;

        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BM_PM_LED_GPIO_PORT, BM_PM_LED_GPIO_PIN);
        if (rc < 0) {
                goto err_led_gpio_req;
        }
        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           BM_PM_BTN_GPIO_PORT, BM_PM_BTN_GPIO_PIN);
        if (rc < 0) {
                goto err_btn_gpio_req;
        }
        rc = xwds_eirq_req(&stm32cube_soc_cb,
                           BM_PM_BTN_GPIO_PORT, BM_PM_BTN_GPIO_PIN,
                           8, XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                           bm_pm_eirq_btn_isr, &bm_pm_smr);
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
void bm_pm_thrd_deinit(void)
{
        xwds_eirq_rls(&stm32cube_soc_cb, BM_PM_BTN_GPIO_PORT, BM_PM_BTN_GPIO_PIN, 8);
        xwds_gpio_rls(&stm32cube_soc_cb, BM_PM_BTN_GPIO_PORT, BM_PM_BTN_GPIO_PIN);
        xwds_gpio_rls(&stm32cube_soc_cb,
                      BM_PM_LED_GPIO_PORT, BM_PM_LED_GPIO_PIN);
}

static
void bm_pm_req_btn_irq(void)
{
        xwds_eirq_req(&stm32cube_soc_cb,
                      BM_PM_BTN_GPIO_PORT, BM_PM_BTN_GPIO_PIN,
                      8, XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                      bm_pm_eirq_btn_isr, &bm_pm_smr);
}

static
void bm_pm_rls_btn_irq(void)
{
        xwds_eirq_rls(&stm32cube_soc_cb, BM_PM_BTN_GPIO_PORT, BM_PM_BTN_GPIO_PIN, 8);
}

static
void bm_pm_led_blink(void)
{
        xwtm_t time;

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BM_PM_LED_GPIO_PORT,
                         BM_PM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwosal_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BM_PM_LED_GPIO_PORT,
                         BM_PM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwosal_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BM_PM_LED_GPIO_PORT,
                         BM_PM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwosal_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BM_PM_LED_GPIO_PORT,
                         BM_PM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwosal_cthrd_sleep(&time);
}

static
void bm_pm_handle_evt(xwsq_t evt)
{
        switch (evt) {
        case BM_PM_BTNEVT_CLICK:
                xwds_gpio_toggle(&stm32cube_soc_cb,
                                 BM_PM_LED_GPIO_PORT,
                                 BM_PM_LED_GPIO_PIN);
                break;
        case BM_PM_BTNEVT_LONGPRESS:
                bm_pm_led_blink();
                xwos_pmdm_suspend(xwos_pmdm_get_lc());
                break;
        default:
                break;
        }
}

static
xwer_t bm_pm_thrd(void * arg)
{
        xwer_t rc;
        xwsq_t evt;

        XWOS_UNUSED(arg);

        rc = bm_pm_thrd_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_init;
        }

        while (!xwosal_cthrd_shld_stop()) {
                if (xwosal_cthrd_shld_frz()) {
                        bm_pm_suspend();
                        xwosal_cthrd_freeze();
                        bm_pm_resume();
                } else {
                        rc = bm_pm_get_btn_evt(&evt);
                        if (XWOK == rc) {
                                bm_pm_handle_evt(evt);
                                bm_pm_req_btn_irq();
                        } else if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                bm_pm_req_btn_irq();
                                /* continue to do freeze */
                        } else if (-ETIMEDOUT == rc) {
                                bm_pm_req_btn_irq();
                        } else {
                                bm_pm_req_btn_irq();
                                goto err_get_btn_evt;
                        }
                }
        }

err_get_btn_evt:
        bm_pm_thrd_deinit();
err_init:
        return rc;
}

static
void bm_pm_eirq_btn_isr(struct xwds_soc * soc, xwid_t id, xwds_eirq_arg_t arg)
{
        xwid_t smrid;
        struct xwos_pmdm * pmdm;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(id);

        pmdm = xwos_pmdm_get_lc();
        if (xwos_pmdm_get_stage(pmdm) < XWOS_PMDM_STAGE_RUNNING) {
                xwos_pmdm_resume(pmdm);
        }
        bm_pm_rls_btn_irq();
        smrid = xwosal_smr_get_id((struct xwosal_smr *)arg);
        xwosal_smr_post(smrid);
}
