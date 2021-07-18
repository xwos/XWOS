/**
 * @file
 * @brief STM32CUBE设备栈：ST7735
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
#include <xwcd/ds/spi/perpheral.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/perpheral/spi/lcd/st7735/device.h>
#include <xwcd/perpheral/spi/lcd/st7735/driver.h>
#include "bm/stm32cube/standard.h"
#include <bm/stm32cube/cubemx/Core/Inc/tim.h>
#include "bm/stm32cube/xwac/xwds/device.h"
#include "bm/stm32cube/xwac/xwds/st7735.h"

xwer_t stm32cube_st7735_drv_start(struct xwds_device * dev);
xwer_t stm32cube_st7735_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t stm32cube_st7735_drv_resume(struct xwds_device * dev);
xwer_t stm32cube_st7735_drv_suspend(struct xwds_device * dev);
#endif /* XWCDCFG_ds_PM */

xwer_t stm32cube_st7735_drv_write(struct xwds_st7735 * st7735, xwu8_t reg,
                                  const xwu8_t * data, xwsz_t * size,
                                  xwtm_t * xwtm);
xwer_t stm32cube_st7735_drv_read(struct xwds_st7735 * st7735, xwu8_t reg,
                                 xwu8_t * buf, xwsz_t * size,
                                 xwtm_t * xwtm);
xwer_t stm32cube_st7735_drv_set_brightness(struct xwds_st7735 * st7735,
                                           xwu32_t brightness);

const struct xwds_st7735_driver stm32cube_st7735_drv = {
        .spip = {
                .base = {
                        .name = "stm32cube.st7735",
                        .probe = NULL,
                        .remove = NULL,
                        .start = stm32cube_st7735_drv_start,
                        .stop = stm32cube_st7735_drv_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
                        .suspend = stm32cube_st7735_drv_suspend,
                        .resume =  stm32cube_st7735_drv_resume,
#endif /* XWMDCFG_ds_PM */
                },
        },
        .write = stm32cube_st7735_drv_write,
        .read = stm32cube_st7735_drv_read,
        .set_brightness = stm32cube_st7735_drv_set_brightness,
};

struct xwds_st7735 stm32cube_st7735_cb = {
        /* attributes */
        .spip = {
                .dev = {
                        .name = "stm32cube.st7735",
                        .id = 1,
                        .resources = NULL,
                        .drv = xwds_cast(struct xwds_driver *, &stm32cube_st7735_drv),
                        .data = NULL,
                },
                .bus = &stm32cube_spi4m_cb,
                .buscfgid = 0,
        },
        .parameter = {
                .width = 160,
                .width_offset = 1,
                .height = 80,
                .height_offset = 26,
                .orientation = XWDS_ST7735_ORIENTATION_LANDSCAPE,
                .pixelformat = XWDS_ST7735_FORMAT_RBG565,
                .inversion = XWDS_ST7735_INVERSION_ON,
                .rgbseq = XWDS_ST7735_RGBSEQ_BGR,
                .brightness = XWU32_MAX,
        },
};

#define STM32CUBE_ST7735_CS_PORT XWDS_GPIO_PORT_E
#define STM32CUBE_ST7735_CS_PIN XWDS_GPIO_PIN_11
#define STM32CUBE_ST7735_DC_PORT XWDS_GPIO_PORT_E
#define STM32CUBE_ST7735_DC_PIN XWDS_GPIO_PIN_13

#define STM32CUBE_ST7735_BRIGHTNESS_TIMER &htim1
#define STM32CUBE_ST7735_BRIGHTNESS_TIMER_CHANNEL TIM_CHANNEL_2

xwer_t stm32cube_st7735_drv_start(struct xwds_device * dev)
{
        xwer_t rc;

        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           STM32CUBE_ST7735_CS_PORT,
                           STM32CUBE_ST7735_CS_PIN);
        if (rc < 0) {
                goto err_gpio;
        }
        xwds_gpio_set(&stm32cube_soc_cb,
                           STM32CUBE_ST7735_CS_PORT,
                           STM32CUBE_ST7735_CS_PIN);

        rc = xwds_gpio_req(&stm32cube_soc_cb,
                           STM32CUBE_ST7735_DC_PORT,
                           STM32CUBE_ST7735_DC_PIN);
        if (rc < 0) {
                goto err_gpio;
        }
        MX_TIM1_Init();
        rc = xwds_st7735_drv_start(dev);
        if (rc < 0) {
                goto err_st7735_start;
        }
	HAL_TIMEx_PWMN_Start(STM32CUBE_ST7735_BRIGHTNESS_TIMER,
                             STM32CUBE_ST7735_BRIGHTNESS_TIMER_CHANNEL);
        return XWOK;

err_st7735_start:
err_gpio:
        return rc;
}

xwer_t stm32cube_st7735_drv_stop(struct xwds_device * dev)
{
        xwer_t rc;

	HAL_TIMEx_PWMN_Stop(STM32CUBE_ST7735_BRIGHTNESS_TIMER,
                            STM32CUBE_ST7735_BRIGHTNESS_TIMER_CHANNEL);
        rc = xwds_st7735_drv_stop(dev);
        if (rc < 0) {
                goto err_st7735_stop;
        }
        MX_TIM1_DeInit();
        xwds_gpio_rls(&stm32cube_soc_cb,
                      STM32CUBE_ST7735_DC_PORT,
                      STM32CUBE_ST7735_DC_PIN);
        xwds_gpio_rls(&stm32cube_soc_cb,
                      STM32CUBE_ST7735_CS_PORT,
                      STM32CUBE_ST7735_CS_PIN);
        return XWOK;

err_st7735_stop:
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t stm32cube_st7735_drv_resume(struct xwds_device * dev)
{
        return stm32cube_st7735_drv_start(dev);
}

xwer_t stm32cube_st7735_drv_suspend(struct xwds_device * dev)
{
        return stm32cube_st7735_drv_stop(dev);
}
#endif /* XWCDCFG_ds_PM */


xwer_t stm32cube_st7735_drv_write(struct xwds_st7735 * st7735, xwu8_t reg,
                                  const xwu8_t * data, xwsz_t * size,
                                  xwtm_t * xwtm)
{
        xwer_t rc;
        xwsz_t xfsz, rest, pos;

        xwds_gpio_reset(&stm32cube_soc_cb,
                        STM32CUBE_ST7735_CS_PORT,
                        STM32CUBE_ST7735_CS_PIN);
        xfsz = 1;
        xwds_gpio_reset(&stm32cube_soc_cb,
                        STM32CUBE_ST7735_DC_PORT,
                        STM32CUBE_ST7735_DC_PIN);
        rc = xwds_spim_xfer(st7735->spip.bus, &reg, NULL, &xfsz, xwtm);
        if (rc < 0) {
                goto err_write_cmd;
        }
        xwds_gpio_set(&stm32cube_soc_cb,
                      STM32CUBE_ST7735_DC_PORT,
                      STM32CUBE_ST7735_DC_PIN);
        xfsz = *size;
        pos = 0;
        while (pos < xfsz) {
                rest = xfsz - pos;
                rc = xwds_spim_xfer(st7735->spip.bus, &data[pos], NULL, &rest, xwtm);
                if (rc < 0) {
                        goto err_write_data;
                }
                pos += rest;
        }
        *size = pos;
        xwds_gpio_set(&stm32cube_soc_cb,
                      STM32CUBE_ST7735_CS_PORT,
                      STM32CUBE_ST7735_CS_PIN);
        return XWOK;

err_write_data:
err_write_cmd:
        return rc;
}

xwer_t stm32cube_st7735_drv_read(struct xwds_st7735 * st7735, xwu8_t reg,
                                 xwu8_t * buf, xwsz_t * size,
                                 xwtm_t * xwtm)
{
        xwer_t rc;
        xwsz_t xfsz, rest, pos;

        xwds_gpio_reset(&stm32cube_soc_cb,
                        STM32CUBE_ST7735_CS_PORT,
                        STM32CUBE_ST7735_CS_PIN);
        xfsz = 1;
        xwds_gpio_reset(&stm32cube_soc_cb,
                        STM32CUBE_ST7735_DC_PORT,
                        STM32CUBE_ST7735_DC_PIN);
        rc = xwds_spim_xfer(st7735->spip.bus, &reg, NULL, &xfsz, xwtm);
        if (rc < 0) {
                goto err_write_cmd;
        }
        xwds_gpio_set(&stm32cube_soc_cb,
                      STM32CUBE_ST7735_DC_PORT,
                      STM32CUBE_ST7735_DC_PIN);
        xfsz = *size;
        pos = 0;
        while (pos < xfsz) {
                rest = xfsz - pos;
                rc = xwds_spim_xfer(st7735->spip.bus, NULL, &buf[pos], &rest, xwtm);
                if (rc < 0) {
                        goto err_read_data;
                }
                pos += rest;
        }
        *size = pos;
        xwds_gpio_set(&stm32cube_soc_cb,
                      STM32CUBE_ST7735_CS_PORT,
                      STM32CUBE_ST7735_CS_PIN);
        return XWOK;

err_read_data:
err_write_cmd:
        return rc;
}

xwer_t stm32cube_st7735_drv_set_brightness(struct xwds_st7735 * st7735,
                                           xwu32_t brightness)
{

	__HAL_TIM_SET_COMPARE(STM32CUBE_ST7735_BRIGHTNESS_TIMER,
                              STM32CUBE_ST7735_BRIGHTNESS_TIMER_CHANNEL,
                              brightness);
        return XWOK;
}
