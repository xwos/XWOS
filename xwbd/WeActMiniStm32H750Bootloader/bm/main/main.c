/**
 * @file
 * @brief 主模块：应用程序入口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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
#include <arch_firmware.h>
#include <xwos/osal/thd.h>
#include <xwcd/ds/soc/gpio.h>
#include <bdl/standard.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/xwssc.h>
#include <xwam/application/ramcode/mif.h>

#define KEY_GPIO_PORT           XWDS_GPIO_PORT_C
#define KEY_GPIO_PIN            XWDS_GPIO_PIN_13

#define LED_GPIO_PORT           XWDS_GPIO_PORT_E
#define LED_GPIO_PIN            XWDS_GPIO_PIN_3

#define MAIN_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

static
xwer_t bm_ramcode_load(struct ramcode * ramcode,
                       xwu8_t * loadbuf, xwsz_t * bufsize,
                       xwtm_t to);

static
xwer_t main_task(void * arg);

const struct xwos_thd_desc main_thd_desc = {
        .attr = {
                .name = "main.thd",
                .stack = NULL,
                .stack_size = 4096,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = MAIN_THD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = main_task,
        .arg = NULL,
};
xwos_thd_d main_thd;

extern xwu8_t qspiflash_mr_origin[];
extern xwu8_t firmware_info_offset[];

struct ramcode bm_ramcode;
const struct ramcode_operation bm_ramcode_op = {
        .load = bm_ramcode_load,
};

xwer_t xwos_main(void)
{
        xwer_t rc;
        xwsq_t key;

        xwds_gpio_req(&stm32soc, KEY_GPIO_PORT, KEY_GPIO_PIN);
        xwds_gpio_input(&stm32soc, KEY_GPIO_PORT, KEY_GPIO_PIN, &key);
        if (0 == key) {
                arch_boot_firmware((void *)qspiflash_mr_origin,
                                   (xwsz_t)firmware_info_offset,
                                   BRDCFG_FIRMWARE_TAILFLAG);
        }

        xwds_gpio_req(&stm32soc, LED_GPIO_PORT, LED_GPIO_PIN);
        xwds_gpio_set(&stm32soc, LED_GPIO_PORT, LED_GPIO_PIN);

        rc = xwos_thd_create(&main_thd,
                             &main_thd_desc.attr,
                             main_thd_desc.func,
                             main_thd_desc.arg);
        if (rc < 0) {
                goto err_init_thd_create;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }
        return XWOK;

err_init_thd_create:
        BRD_BUG();
err_skd_start_lc:
        BRD_BUG();
        return rc;
}


static
xwer_t bm_ramcode_load(struct ramcode * ramcode,
                       xwu8_t * loadbuf, xwsz_t * bufsize,
                       xwtm_t to)
{
        XWOS_UNUSED(ramcode);
        return bm_xwssc_rx(loadbuf, bufsize, to);
}

static
xwer_t main_task(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        /* Init devices */
        rc = stm32cube_start();
        if (rc < 0) {
                goto err_stm32cube_start;
        }

        /* Init xwssc */
        rc =  bm_xwssc_start();
        if (rc < 0) {
                goto err_xwssc_start;
        }

        /* load ramcode */
        ramcode_init(&bm_ramcode,
                     &bm_ramcode_op, NULL,
                     (xwsz_t)firmware_info_offset,
                     BRDCFG_FIRMWARE_TAILFLAG);
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
        BRD_BUG();
err_ramcode_load:
        BRD_BUG();
err_xwssc_start:
        BRD_BUG();
err_stm32cube_start:
        BRD_BUG();
        return rc;
}
