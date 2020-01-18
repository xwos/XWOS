/**
 * @file
 * @brief 应用程序：ARMP演示
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
#include <xwos/init.h>
#include <xwos/lib/xwlog.h>
#include <xwos/core/pm.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/sync/semaphore.h>
#include <soc_gpio.h>
#include <soc_me.h>
#include <soc_eirq.h>
#include <soc_wdg.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/eirq.h>
#include <xwmd/sysm/pm/core.h>
#include <xwmd/isc/armp/protocol.h>
#include <xwmd/isc/armp/api.h>
#include <bdl/standard.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <bdl/eirq.h>
#include <bdl/xwmdpm.h>
#include <bdl/armpif.h>
#include <bdl/bkup.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define TASK_LED_PRIORITY       XWOSAL_SD_PRIORITY_RT_MIN
#define TASK_KEY_PRIORITY       XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1)
#define TASK_TEST_PRIORITY      XWOSAL_SD_PRIORITY_RAISE(XWOSAL_SD_PRIORITY_RT_MIN, 1)
#define TASK_ARMP_PRIORITY      XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)
#define LED_PORT                SOC_GPIO_PORT_E

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t task_led1(void * arg);
xwer_t task_led2(void * arg);
xwer_t task_key(void * arg);
xwer_t task_armp_test(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc board_td_tbl[] = {
        [0] = {
                .name = "running_led1",
                .prio = TASK_LED_PRIORITY,
                .stack = NULL,
                .stack_size = 1024,
                .func = task_led1,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [1] = {
                .name = "running_led2",
                .prio = TASK_LED_PRIORITY,
                .stack = NULL,
                .stack_size = 1024,
                .func = task_led2,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [2] = {
                .name = "task_key",
                .prio = TASK_KEY_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = task_key,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [3] = {
                .name = "xwmd_isc_armp_txthread",
                .prio = TASK_ARMP_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)armp_txthrd,
                .arg = (void *)&bdl_armp,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [4] = {
                .name = "xwmd_isc_armp_rxthread",
                .prio = TASK_ARMP_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)armp_rxthrd,
                .arg = (void *)&bdl_armp,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [5] = {
                .name = "armp_test",
                .prio = TASK_TEST_PRIORITY,
                .stack = NULL,
                .stack_size = 2048,
                .func = task_armp_test,
                .arg = &bdl_armp,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};

xwosal_tcb_d board_tcb[xw_array_size(board_td_tbl)];
struct xwosal_smr key_smr;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t task_led1(void * arg)
{
        xwtm_t time;
        xwsq_t pinmask;
        xwer_t rc;

        XWOS_UNUSED(arg);
        pinmask = BIT(SOC_GPIO_PIN_12);
        rc = OK;
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                rc = xwds_gpio_req(&mpc560xb_soc_cb, LED_PORT, pinmask);
                if (OK == rc) {
                        while (1) {
                                xwds_gpio_set(&mpc560xb_soc_cb, LED_PORT,
                                              pinmask);
                                time = 500*XWTM_MS;
                                rc = xwosal_cthrd_sleep(&time);
                                if (__unlikely(rc < 0))
                                        break;
                                xwds_gpio_reset(&mpc560xb_soc_cb, LED_PORT,
                                                pinmask);
                                time = 500*XWTM_MS;
                                rc = xwosal_cthrd_sleep(&time);
                                if (__unlikely(rc < 0))
                                        break;
                        }
                        xwds_gpio_rls(&mpc560xb_soc_cb, LED_PORT, pinmask);
                }
        }
        return rc;
}

xwer_t task_led2(void * arg)
{
        xwtm_t time;
        xwsq_t pinmask;
        xwer_t rc;

        XWOS_UNUSED(arg);
        pinmask = BIT(SOC_GPIO_PIN_13);
        rc = OK;
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                rc = xwds_gpio_req(&mpc560xb_soc_cb, LED_PORT, pinmask);
                if (OK == rc) {
                        while (1) {
                                xwds_gpio_set(&mpc560xb_soc_cb, LED_PORT,
                                              pinmask);
                                time = 1000*XWTM_MS;
                                rc = xwosal_cthrd_sleep(&time);
                                if (__unlikely(rc < 0))
                                        break;
                                xwds_gpio_reset(&mpc560xb_soc_cb, LED_PORT,
                                                pinmask);
                                time = 1000*XWTM_MS;
                                rc = xwosal_cthrd_sleep(&time);
                                if (__unlikely(rc < 0))
                                        break;
                        }
                        xwds_gpio_rls(&mpc560xb_soc_cb, LED_PORT, pinmask);
                }
        }
        return rc;
}

xwer_t task_key(void * arg)
{
        xwosal_smr_d smrd;
        xwsq_t keypin, ledpin;
        xwsq_t in;
        xwer_t rc;
        xwtm_t stime;
        xwsq_t cnt;

        XWOS_UNUSED(arg);
        cnt = 0;
        rc = xwosal_smr_init(&key_smr, 0, 1);
        if (__unlikely(rc < 0)) {
                goto err_keysmr_init;
        }

        /* setup led GPIO */
        ledpin = BIT(SOC_GPIO_PIN_14);
        rc = xwds_gpio_req(&mpc560xb_soc_cb, LED_PORT, ledpin);
        if (__unlikely(rc < 0)) {
                goto err_req_ledgpio;
        }

        /* setup WKUP */
        keypin = BIT(SOC_GPIO_PIN_1);
        rc = xwds_gpio_req(&mpc560xb_soc_cb, SOC_GPIO_PORT_B, keypin);
        if (__unlikely(rc < 0)) {
                goto err_req_wkup4gpio;
        }
        rc = xwds_eirq_req(&mpc560xb_soc_cb, SOC_GPIO_PORT_B, keypin,
                           SOC_WKUP_4,
                           SODS_SOC_EIF_TM_FALLING | SODS_SOC_EIF_WKUP,
                           bdl_wkup4_isr);
        if (__unlikely(rc < 0)) {
                goto err_reqei_wkup4;
        }

        smrd = xwosal_smr_getd(&key_smr);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                rc = xwosal_smr_wait(smrd);
                if (OK == rc) {
                        stime = 1 * XWTM_S;
                        rc = xwosal_cthrd_sleep(&stime);
                        if (__unlikely(rc < 0))
                                continue;

                        keypin = BIT(SOC_GPIO_PIN_1);
                        rc = xwds_gpio_input(&mpc560xb_soc_cb, SOC_GPIO_PORT_B,
                                             keypin, &in);
                        if (OK == rc) {
                                if (0 == in) {
                                        cnt ++;
                                        if (cnt & 1UL) {
                                                xwds_gpio_reset(&mpc560xb_soc_cb,
                                                                LED_PORT,
                                                                ledpin);
                                                stime = 1 * XWTM_S;
                                                rc = xwosal_cthrd_sleep(&stime);
                                                xwmdpm_req_lpm(&bdl_xwmdpm);
                                        } else {
                                                xwds_gpio_set(&mpc560xb_soc_cb,
                                                              LED_PORT,
                                                              ledpin);
                                        }
                                }
                        }
                }
        }

        keypin = BIT(SOC_GPIO_PIN_1);
        xwds_eirq_rls(&mpc560xb_soc_cb, SOC_GPIO_PORT_B, keypin, SOC_WKUP_4);
err_reqei_wkup4:
        xwds_gpio_rls(&mpc560xb_soc_cb, SOC_GPIO_PORT_B, keypin);
err_req_wkup4gpio:
        xwds_gpio_rls(&mpc560xb_soc_cb, LED_PORT, ledpin);
err_req_ledgpio:
        xwosal_smr_destroy(&key_smr);
err_keysmr_init:
        return rc;
}

xwer_t task_armp_test(__maybe_unused void * arg)
{
        xwer_t rc;
        xwtm_t xwtm;
        struct armp_msg msg;
        xwu8_t buffer[XWMDCFG_isc_armp_MAX_SDU_SIZE];

        XWOS_UNUSED(arg);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                msg.port = BDL_ARMP_PORT_LOG;
                msg.size = sizeof(buffer);
                msg.text = buffer;
                xwtm = XWTM_MAX;
                rc = armp_rx(&bdl_armp, &msg, &xwtm);
                if (OK == rc) {
                        xwtm = XWTM_MAX;
                        rc = armp_tx(&bdl_armp, &msg, 1, &xwtm);
                }
        }
        return rc;
}

xwer_t xwos_main(void)
{
        xwer_t rc;
        xwsz_t i;

        /* watchdog */
        soc_wdg_init(10000);
#if (defined(BRDCFG_WDG) && (1 == BRDCFG_WDG))
        soc_wdg_start();
        soc_wdg_feed();
#endif

        for (i = 0; i < xw_array_size(board_td_tbl); i ++) {
                rc = xwosal_thrd_create(&board_tcb[i], board_td_tbl[i].name,
                                        board_td_tbl[i].func,
                                        board_td_tbl[i].arg,
                                        board_td_tbl[i].stack_size,
                                        board_td_tbl[i].prio,
                                        board_td_tbl[i].attr);
                BUG_ON(rc);
        }
        rc = xwosal_scheduler_start_lc();
        return rc;
}
