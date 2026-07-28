/**
 * @file
 * @brief 主线程
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#include <board/std.h>
#include <xwos/osal/thd.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/peripheral/spi/lcd/st7735/driver.h>
#include <xwmd/vm/lua/mi.h>
#include <board/xwac/fatfs/sdcard.h>
#include <board/xwac/xwssc/xwssc.h>
#include <board/xwac/libc/mi.h>
#include <bm/Hal/mi.h>

#include <xwos/lib/xwlog.h>
#define MTHD_LOGTAG "Mthd"

#define MTHD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

xwer_t mthd_mainfunc(void * arg);
xwos_thd_d mthd;

xwer_t xwos_main(void)
{
        xwer_t rc;
        struct xwos_thd_attr attr;

        xwos_thd_attr_init(&attr);
        attr.name = "main.thd";
        attr.stack = NULL;
        attr.stack_size = 8192;
        attr.priority = MTHD_PRIORITY;
        attr.detached = true;
        attr.privileged = true;
        rc = xwos_thd_create(&mthd,
                             &attr,
                             mthd_mainfunc,
                             NULL);
        if (rc < 0) {
                goto err_thd_create;
        }

        rc = xwos_skd_start_lc();
        if (rc < 0) {
                goto err_skd_start_lc;
        }
        return XWOK;

err_skd_start_lc:
        BOARD_BUG();
err_thd_create:
        BOARD_BUG();
        return rc;
}

extern const xwu8_t bootlogo[25600];
extern void xwrust_main(void);

xwer_t mthd_mainfunc(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        stm32hal_post_init();
        sdcard_fatfs_mount();
        xwds_st7735_draw(&stm32xwds_st7735, 0, 0, 160, 80, bootlogo, XWTM_MAX);

        xwlogf(I, MTHD_LOGTAG, "Init C++ Runtime ...\r\n");
        libc_init();
        xwlogf(I, MTHD_LOGTAG, "Init XWSSC.UART3 ...\r\n");
        xwssc3_init();
        xwlogf(I, MTHD_LOGTAG, "Start Rust Applications ...\r\n");
        xwrust_main();
        rc = xwlua_start();
        if (rc < 0) {
                xwlogf(ERR, MTHD_LOGTAG, "Start Lua VM ... <rc:%d>", rc);
        }

        xwlogf(I, MTHD_LOGTAG, "Start the Blinky LED ...\r\n");
        xwds_gpio_req(&stm32xwds_soc, XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwds_gpio_reset(&stm32xwds_soc,
                                        XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
                        xwds_gpio_rls(&stm32xwds_soc,
                                      XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
                        xwos_cthd_freeze();
                        xwds_gpio_req(&stm32xwds_soc,
                                      XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
                }
                xwos_cthd_sleep(XWTM_S(1));
                xwds_gpio_toggle(&stm32xwds_soc, XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
        }
        xwds_gpio_rls(&stm32xwds_soc, XWDS_GPIO_PORT_E, XWDS_GPIO_PIN_3);
        return XWOK;
}
