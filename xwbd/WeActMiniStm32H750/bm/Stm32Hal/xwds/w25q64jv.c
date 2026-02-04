/**
 * @file
 * @brief Stm32Hal::xwds::w25q64jv
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
#include <xwcd/ds/spi/peripheral.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/peripheral/spi/flash/w25qxx/device.h>
#include <xwcd/peripheral/spi/flash/w25qxx/driver.h>
#include "bm/Stm32Hal/xwds/device.h"

xwer_t stm32xwds_w25q64jv_drv_start(struct xwds_device * dev);
xwer_t stm32xwds_w25q64jv_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t stm32xwds_w25q64jv_drv_resume(struct xwds_device * dev);
xwer_t stm32xwds_w25q64jv_drv_suspend(struct xwds_device * dev);
#endif

xwer_t stm32xwds_w25q64jv_drv_io(struct xwds_w25qxx * w25qxx,
                                 xwu8_t * txq, xwu8_t * rxq,
                                 xwsz_t * size, xwtm_t to);

const struct xwds_w25qxx_driver stm32xwds_w25q64jv_drv = {
        .spip = {
                .base = {
                        .name = "stm32xwds.w25q64jv",
                        .probe = NULL,
                        .remove = NULL,
                        .start = stm32xwds_w25q64jv_drv_start,
                        .stop = stm32xwds_w25q64jv_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                        .suspend = stm32xwds_w25q64jv_drv_suspend,
                        .resume =  stm32xwds_w25q64jv_drv_resume,
#endif
                },
        },
        .io = stm32xwds_w25q64jv_drv_io,
};

struct xwds_w25qxx stm32xwds_w25q64jv = {
        /* attributes */
        .spip = {
                .dev = {
                        .name = "stm32xwds.w25q64jv",
                        .id = 1,
                        .resources = NULL,
                        .drv = xwds_cast(struct xwds_driver *, &stm32xwds_w25q64jv_drv),
                        .data = NULL,
                },
                .bus = &stm32xwds_spi1m,
                .buscfgid = 0,
        },
        .parameter = {
                .size = 8388608,
                .uid = 0,
                .mid = XWDS_W25Q64_MID,
                .jid = 0,
        },
        .cmdtbl = xwds_w25q64jv_cmd,
};

#define STM32XWDS_W25Q64JV_CS_PORT XWDS_GPIO_PORT_D
#define STM32XWDS_W25Q64JV_CS_PIN XWDS_GPIO_PIN_6

xwer_t stm32xwds_w25q64jv_drv_start(struct xwds_device * dev)
{
        xwer_t rc;

        rc = xwds_gpio_req(&stm32xwds_soc,
                           STM32XWDS_W25Q64JV_CS_PORT,
                           STM32XWDS_W25Q64JV_CS_PIN);
        if (rc < 0) {
                goto err_gpio;
        }
        xwds_gpio_set(&stm32xwds_soc,
                      STM32XWDS_W25Q64JV_CS_PORT,
                      STM32XWDS_W25Q64JV_CS_PIN);

        rc = xwds_w25qxx_drv_start(dev);
        if (rc < 0) {
                goto err_w25q64jv_start;
        }
        return XWOK;

err_w25q64jv_start:
err_gpio:
        return rc;
}

xwer_t stm32xwds_w25q64jv_drv_stop(struct xwds_device * dev)
{
        xwer_t rc;

        rc = xwds_w25qxx_drv_stop(dev);
        if (rc < 0) {
                goto err_w25q64jv_stop;
        }
        xwds_gpio_rls(&stm32xwds_soc,
                      STM32XWDS_W25Q64JV_CS_PORT,
                      STM32XWDS_W25Q64JV_CS_PIN);
        return XWOK;

err_w25q64jv_stop:
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t stm32xwds_w25q64jv_drv_resume(struct xwds_device * dev)
{
        return stm32xwds_w25q64jv_drv_stop(dev);
}

xwer_t stm32xwds_w25q64jv_drv_suspend(struct xwds_device * dev)
{
        return stm32xwds_w25q64jv_drv_start(dev);
}
#endif

xwer_t stm32xwds_w25q64jv_drv_io(struct xwds_w25qxx * w25qxx,
                                 xwu8_t * txq, xwu8_t * rxq,
                                 xwsz_t * size, xwtm_t to)
{
        xwer_t rc;
        xwsz_t xfsz, rest, pos;

        xwds_gpio_reset(&stm32xwds_soc,
                        STM32XWDS_W25Q64JV_CS_PORT, STM32XWDS_W25Q64JV_CS_PIN);
        rc = XWOK;
        xfsz = *size;
        pos = 0;
        while (pos < xfsz) {
                rest = xfsz - pos;
                rc = xwds_spim_xfer(w25qxx->spip.bus, w25qxx->spip.buscfgid,
                                    &txq[pos], &rxq[pos], &rest,
                                    to);
                if (rc < 0) {
                        goto err_write_data;
                }
                pos += rest;
        }
        xwds_gpio_set(&stm32xwds_soc,
                      STM32XWDS_W25Q64JV_CS_PORT, STM32XWDS_W25Q64JV_CS_PIN);
err_write_data:
        *size = pos;
        return rc;
}
