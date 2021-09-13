/**
 * @file
 * @brief STM32CUBE设备栈：SPI Flash W25Q64JV
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

#include <xwcd/ds/spi/perpheral.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/perpheral/spi/flash/w25qxx/device.h>
#include <xwcd/perpheral/spi/flash/w25qxx/driver.h>
#include "bm/stm32cube/standard.h"
#include "bm/stm32cube/xwac/xwds/device.h"
#include "bm/stm32cube/xwac/xwds/w25q64jv.h"

xwer_t stm32cube_w25q64jv_drv_start(struct xwds_device * dev);
xwer_t stm32cube_w25q64jv_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t stm32cube_w25q64jv_drv_resume(struct xwds_device * dev);
xwer_t stm32cube_w25q64jv_drv_suspend(struct xwds_device * dev);
#endif

xwer_t stm32cube_w25q64jv_drv_io(struct xwds_w25qxx * w25qxx,
                                 xwu8_t * txq, xwu8_t * rxq,
                                 xwsz_t * size, xwtm_t * xwtm);

const struct xwds_w25qxx_driver stm32cube_w25q64jv_drv = {
        .spip = {
                .base = {
                        .name = "stm32cube.w25q64jv.1",
                        .probe = NULL,
                        .remove = NULL,
                        .start = stm32cube_w25q64jv_drv_start,
                        .stop = stm32cube_w25q64jv_drv_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
                        .suspend = stm32cube_w25q64jv_drv_suspend,
                        .resume =  stm32cube_w25q64jv_drv_resume,
#endif
                },
        },
        .io = stm32cube_w25q64jv_drv_io,
};

struct xwds_w25qxx stm32cube_w25q64jv_cb = {
        /* attributes */
        .spip = {
                .dev = {
                        .name = "stm32cube.w25q64jv.1",
                        .id = 1,
                        .resources = NULL,
                        .drv = xwds_cast(struct xwds_driver *, &stm32cube_w25q64jv_drv),
                        .data = NULL,
                },
                .bus = &stm32cube_spi1m_cb,
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

#define STM32CUBE_W25Q64JV_CS_PORT XWDS_GPIO_PORT_D
#define STM32CUBE_W25Q64JV_CS_PIN XWDS_GPIO_PIN_6

xwer_t stm32cube_w25q64jv_drv_start(struct xwds_device * dev)
{
        xwer_t rc;

        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           STM32CUBE_W25Q64JV_CS_PORT,
                           STM32CUBE_W25Q64JV_CS_PIN);
        if (rc < 0) {
                goto err_gpio;
        }
        xwds_gpio_set(&stm32cube_soc_cb,
                      STM32CUBE_W25Q64JV_CS_PORT,
                      STM32CUBE_W25Q64JV_CS_PIN);

        rc = xwds_w25qxx_drv_start(dev);
        if (rc < 0) {
                goto err_w25q64jv_start;
        }
        return XWOK;

err_w25q64jv_start:
err_gpio:
        return rc;
}

xwer_t stm32cube_w25q64jv_drv_stop(struct xwds_device * dev)
{
        xwer_t rc;

        rc = xwds_w25qxx_drv_stop(dev);
        if (rc < 0) {
                goto err_w25q64jv_stop;
        }
        xwds_gpio_rls(&stm32cube_soc_cb,
                      STM32CUBE_W25Q64JV_CS_PORT,
                      STM32CUBE_W25Q64JV_CS_PIN);
        return XWOK;

err_w25q64jv_stop:
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t stm32cube_w25q64jv_drv_resume(struct xwds_device * dev)
{
        return stm32cube_w25q64jv_drv_stop(dev);
}

xwer_t stm32cube_w25q64jv_drv_suspend(struct xwds_device * dev)
{
        return stm32cube_w25q64jv_drv_start(dev);
}
#endif

xwer_t stm32cube_w25q64jv_drv_io(struct xwds_w25qxx * w25qxx,
                                 xwu8_t * txq, xwu8_t * rxq,
                                 xwsz_t * size, xwtm_t * xwtm)
{
        xwer_t rc;
        xwsz_t xfsz, rest, pos;

        xwds_gpio_reset(&stm32cube_soc_cb,
                        STM32CUBE_W25Q64JV_CS_PORT, STM32CUBE_W25Q64JV_CS_PIN);
        rc = XWOK;
        xfsz = *size;
        pos = 0;
        while (pos < xfsz) {
                rest = xfsz - pos;
                rc = xwds_spim_xfer(w25qxx->spip.bus,
                                    &txq[pos], &rxq[pos], &rest,
                                    xwtm);
                if (rc < 0) {
                        goto err_write_data;
                }
                pos += rest;
        }
        xwds_gpio_set(&stm32cube_soc_cb,
                      STM32CUBE_W25Q64JV_CS_PORT, STM32CUBE_W25Q64JV_CS_PIN);
err_write_data:
        *size = pos;
        return rc;
}
