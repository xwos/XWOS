/**
 * @file
 * @brief 主模块：应用程序入口
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
#include <arch_image.h>
#include <armv7m_core.h>
#include <xwos/osal/skd.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwmd/ramcode/mif.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/xwscp.h>

#define KEY_GPIO_PORT           XWDS_GPIO_PORT_C
#define KEY_GPIO_PIN            XWDS_GPIO_PIN_13

#define LED_GPIO_PORT           XWDS_GPIO_PORT_E
#define LED_GPIO_PIN            XWDS_GPIO_PIN_3

#define MAIN_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

static
xwer_t bm_ramcode_load(struct ramcode * ramcode,
                       xwu8_t * loadbuf, xwsz_t * bufsize,
                       xwtm_t * xwtm);

static
xwer_t main_task(void * arg);

const struct xwos_thd_desc main_thd_desc = {
        .name = "main.thd",
        .prio = MAIN_THD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 4096,
        .func = main_task,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED | XWOS_SKDATTR_DETACHED,
};
struct xwos_thd * main_thd;

extern uint8_t qspiflash_mr_origin[];

struct ramcode bm_ramcode;
const struct ramcode_operation bm_ramcode_op = {
        .load = bm_ramcode_load,
};

xwer_t xwos_main(void)
{
        xwer_t rc;
        xwsq_t key;

        xwds_gpio_req(&stm32cube_soc_cb, KEY_GPIO_PORT, KEY_GPIO_PIN);
        xwds_gpio_input(&stm32cube_soc_cb, KEY_GPIO_PORT, KEY_GPIO_PIN, &key);
        if (0 == key) {
                arch_boot_image((void *)qspiflash_mr_origin);
        }

        xwds_gpio_req(&stm32cube_soc_cb, LED_GPIO_PORT, LED_GPIO_PIN);
        xwds_gpio_set(&stm32cube_soc_cb, LED_GPIO_PORT, LED_GPIO_PIN);

        rc = xwos_thd_create(&main_thd,
                             main_thd_desc.name,
                             main_thd_desc.func,
                             main_thd_desc.arg,
                             main_thd_desc.stack_size,
                             main_thd_desc.prio,
                             main_thd_desc.attr);
        if (rc < 0) {
                goto err_init_thd_create;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }
        return XWOK;

err_init_thd_create:
        BDL_BUG();
err_skd_start_lc:
        BDL_BUG();
        return rc;
}

static
xwer_t bm_ramcode_load(struct ramcode * ramcode,
                       xwu8_t * loadbuf, xwsz_t * bufsize,
                       xwtm_t * xwtm)
{
        return xwscp_rx(ramcode->opcb, loadbuf, bufsize, xwtm);
}

void bm_reset(void)
{
        cm_nvic_disable_faults();
        cm_reset_system();
}

static
xwer_t main_task(void * arg)
{
        xwer_t rc;
        xwtm_t xwtm;

        XWOS_UNUSED(arg);

        /* Init devices */
        rc = stm32cube_start();
        if (rc < 0) {
                goto err_stm32cube_start;
        }

        /* Init xwscp */
        rc =  bm_xwscp_start();
        if (rc < 0) {
                goto err_xwscp_start;
        }
        xwtm = XWTM_MAX;
        rc = xwscp_connect(&bm_xwscp, &xwtm);
        if (rc < 0) {
                goto err_xwscp_connect;
        }

        /* load ramcode */
        ramcode_init(&bm_ramcode, &bm_ramcode_op, &bm_xwscp);
        rc = ramcode_load(&bm_ramcode);
        if (rc < 0) {
                goto err_ramcode_load;
        }
        rc = ramcode_boot(&bm_ramcode);
        if (rc < 0) {
                goto err_ramcode_boot;
        }
        return XWOK;

err_ramcode_boot:
        BDL_BUG();
err_ramcode_load:
        BDL_BUG();
err_xwscp_connect:
        BDL_BUG();
err_xwscp_start:
        BDL_BUG();
err_stm32cube_start:
        BDL_BUG();
        return rc;
}
