/**
 * @file
 * @brief 板级描述层：RAMCODE下载器：应用程序入口
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

#include "board/std.h"
#include <xwcd/soc/arm/v7m/arch_firmware.h>
#include <xwos/osal/thd.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwam/application/ramcode/mif.h>
#include "bm/xwac/xwds/device.h"
#include "bm/stm32cube/mif.h"
#include "bm/rcd/xwssc.h"

#define KEY_GPIO_PORT           XWDS_GPIO_PORT_C
#define KEY_GPIO_PIN            XWDS_GPIO_PIN_13

#define LED_GPIO_PORT           XWDS_GPIO_PORT_E
#define LED_GPIO_PIN            XWDS_GPIO_PIN_3

#define MAIN_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

static
xwer_t board_ramcode_load(struct ramcode * ramcode,
                          xwu8_t * loadbuf, xwsz_t * bufsize,
                          xwtm_t to);

static
xwer_t main_task(void * arg);

__xwcc_alignl1cache xwu8_t main_thd_stack[4096] = {0};
const struct xwos_thd_desc main_thd_desc = {
        .attr = {
                .name = "main.thd",
                .stack = (xwstk_t *)main_thd_stack,
                .stack_size = sizeof(main_thd_stack),
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = MAIN_THD_PRIORITY,
                .detached = true,
                .privileged = true,
        },
        .func = main_task,
        .arg = NULL,
};
struct xwos_thd main_thd;
xwos_thd_d main_thdd;

extern xwu8_t qspiflash_mr_origin[];
extern xwu8_t firmware_info_offset[];

struct ramcode board_ramcode;
const struct ramcode_operation board_ramcode_op = {
        .load = board_ramcode_load,
};

xwer_t xwos_main(void)
{
        xwer_t rc;
        xwsq_t key;

        xwds_gpio_req(&stm32xwds_soc, KEY_GPIO_PORT, KEY_GPIO_PIN);
        xwds_gpio_input(&stm32xwds_soc, KEY_GPIO_PORT, KEY_GPIO_PIN, &key);
        if (0 == key) {
                arch_boot_firmware((void *)qspiflash_mr_origin,
                                   (xwsz_t)firmware_info_offset,
                                   BRDCFG_FIRMWARE_TAILFLAG);
        }

        xwds_gpio_req(&stm32xwds_soc, LED_GPIO_PORT, LED_GPIO_PIN);
        xwds_gpio_set(&stm32xwds_soc, LED_GPIO_PORT, LED_GPIO_PIN);

        rc = xwos_thd_init(&main_thd, &main_thdd,
                           &main_thd_desc.attr,
                           main_thd_desc.func,
                           main_thd_desc.arg);
        if (rc < 0) {
                goto err_thd_init;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }
        return XWOK;

err_thd_init:
        BOARD_BUG();
err_skd_start_lc:
        BOARD_BUG();
        return rc;
}


static
xwer_t board_ramcode_load(struct ramcode * ramcode,
                       xwu8_t * loadbuf, xwsz_t * bufsize,
                       xwtm_t to)
{
        XWOS_UNUSED(ramcode);
        return board_xwssc_rx(loadbuf, bufsize, to);
}

xwer_t board_init_devices(void)
{
        xwer_t rc;

        rc = stm32xwds_uart_init();
        if (rc < 0) {
                goto err_uart_init;
        }
        return XWOK;

err_uart_init:
        return rc;
}

static
xwer_t main_task(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        /* Init devices */
        rc = board_init_devices();
        if (rc < 0) {
                goto err_init_devices;
        }

        /* Init xwssc */
        rc =  board_xwssc_start();
        if (rc < 0) {
                goto err_xwssc_start;
        }

        /* load ramcode */
        ramcode_init(&board_ramcode,
                     &board_ramcode_op, NULL,
                     (xwsz_t)firmware_info_offset,
                     BRDCFG_FIRMWARE_TAILFLAG);
        rc = ramcode_load(&board_ramcode);
        if (rc < 0) {
                goto err_ramcode_load;
        }
        rc = ramcode_boot(&board_ramcode);
        if (rc < 0) {
                goto err_ramcode_boot;
        }
        return XWOK;

err_ramcode_boot:
        BOARD_BUG();
err_ramcode_load:
        BOARD_BUG();
err_xwssc_start:
        BOARD_BUG();
err_init_devices:
        BOARD_BUG();
        return rc;
}
