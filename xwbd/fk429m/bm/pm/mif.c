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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <soc_xwpmdm.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/eirq.h>
#include <bm/stm32cube/mif.h>
#include <bm/stm32cube/xwac/xwds/pm.h>
#include <bm/pm/mif.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BRDPM_THRD_PRIORITY XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define BRDPM_BTN_GPIO_PORT XWDS_GPIO_PORT_I
#define BRDPM_BTN_GPIO_PIN XWDS_GPIO_PIN_8
#define BRDPM_BTN_DEBOUNCING_DELAY (20 * XWTM_MS)
#define BRDPM_BTN_LONGPRESS_CNT (100)

#define BRDPM_LED_GPIO_PORT XWDS_GPIO_PORT_G
#define BRDPM_LED_GPIO_PIN XWDS_GPIO_PIN_7

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum brdpm_btn_event_em {
        BRDPM_BTNEVT_CLICK,
        BRDPM_BTNEVT_LONGPRESS,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
void brdpmdm_resume(struct xwos_pmdm * pmdm, void * arg);

static
void brdpmdm_suspend(struct xwos_pmdm * pmdm, void * arg);

static
void brdpmdm_wakeup(struct xwos_pmdm * pmdm, void * arg);

static
void brdpmdm_sleep(struct xwos_pmdm * pmdm, void * arg);

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
const struct xwosal_thrd_desc brdpm_thrd_td = {
        .name = "bm.pm.thrd",
        .prio = BRDPM_THRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)brdpm_thrd,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t brdpm_thrd_id;

struct xwosal_smr brdpm_smr;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t brdpm_start(void)
{
        xwer_t rc;

        xwos_pmdm_set_cb(&soc_xwpm_domain,
                         brdpmdm_resume,
                         brdpmdm_suspend,
                         brdpmdm_wakeup,
                         brdpmdm_sleep,
                         NULL);

        rc = xwosal_smr_init(&brdpm_smr, 0, 1);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_smr_init;
        }

        rc = xwosal_thrd_create(&brdpm_thrd_id,
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
        xwosal_smr_destroy(&brdpm_smr);
err_smr_init:
        return rc;
}

xwer_t brdpm_stop(void)
{
        xwer_t rc, trc;

        rc = xwosal_thrd_terminate(brdpm_thrd_id, &trc);
        if (XWOK == rc) {
                rc = xwosal_thrd_delete(brdpm_thrd_id);
                if (XWOK == rc) {
                        brdpm_thrd_id = 0;
                }
        }
        xwosal_smr_destroy(&brdpm_smr);
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
void brdpmdm_resume(struct xwos_pmdm * pmdm, void * arg)
{
        XWOS_UNUSED(pmdm);
        XWOS_UNUSED(arg);
        stm32cube_pm_resume();
}

static
void brdpmdm_suspend(struct xwos_pmdm * pmdm, void * arg)
{
        XWOS_UNUSED(pmdm);
        XWOS_UNUSED(arg);
        stm32cube_pm_suspend();
}
static
void brdpmdm_wakeup(struct xwos_pmdm * pmdm, void * arg)
{
        XWOS_UNUSED(pmdm);
        XWOS_UNUSED(arg);
        stm32cube_pm_wakeup();
}

static
void brdpmdm_sleep(struct xwos_pmdm * pmdm, void * arg)
{
        XWOS_UNUSED(pmdm);
        XWOS_UNUSED(arg);
        stm32cube_pm_sleep();
}

static
xwer_t brdpm_get_btn_evt(xwsq_t * evt)
{
        xwer_t rc;
        xwid_t smrid;
        xwsq_t in;
        xwsq_t cnt;
        xwtm_t time;

        smrid = xwosal_smr_get_id(&brdpm_smr);
        rc = xwosal_smr_wait(smrid);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_smr_wait;
        }

        time = BRDPM_BTN_DEBOUNCING_DELAY;
        rc = xwosal_cthrd_sleep(&time);
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
                rc = xwosal_cthrd_sleep(&time);
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
err_smr_wait:
        return rc;
}

static
xwer_t brdpm_thrd_init(void)
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
                           brdpm_eirq_btn_isr, &brdpm_smr);
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
                      brdpm_eirq_btn_isr, &brdpm_smr);
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
        xwosal_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BRDPM_LED_GPIO_PORT,
                         BRDPM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwosal_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BRDPM_LED_GPIO_PORT,
                         BRDPM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwosal_cthrd_sleep(&time);

        xwds_gpio_toggle(&stm32cube_soc_cb,
                         BRDPM_LED_GPIO_PORT,
                         BRDPM_LED_GPIO_PIN);
        time = 500 * XWTM_MS;
        xwosal_cthrd_sleep(&time);
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
                xwos_pmdm_suspend(xwos_pmdm_get_lc());
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

        while (!xwosal_cthrd_shld_stop()) {
                if (xwosal_cthrd_shld_frz()) {
                        brdpm_suspend();
                        xwosal_cthrd_freeze();
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
        xwid_t smrid;
        struct xwos_pmdm * pmdm;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(id);

        pmdm = xwos_pmdm_get_lc();
        if (xwos_pmdm_get_stage(pmdm) < XWOS_PMDM_STAGE_RUNNING) {
                xwos_pmdm_resume(pmdm);
        }
        brdpm_rls_btn_irq();
        smrid = xwosal_smr_get_id((struct xwosal_smr *)arg);
        xwosal_smr_post(smrid);
}
