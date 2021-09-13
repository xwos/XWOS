/**
 * @file
 * @brief SPI LCD Controller ST7735 Driver
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
 * @note
 * - 所有API只可被单一线程访问。
 */

#ifndef __xwcd_perpheral_spi_lcd_st7735_driver_h__
#define __xwcd_perpheral_spi_lcd_st7735_driver_h__

#include <xwos/standard.h>
#include <xwcd/ds/spi/perpheral.h>
#include <xwcd/perpheral/spi/lcd/st7735/device.h>

/**
 * @brief ST7735驱动函数表
 */
struct xwds_st7735_driver {
        struct xwds_spip_driver spip; /**< C语言面向对象：继承struct xwds_spip_driver */
        xwer_t (* write)(struct xwds_st7735 * /*st7735*/, xwu8_t /*reg*/,
                         const xwu8_t * /*data*/, xwsz_t * /*size*/,
                         xwtm_t * /*xwtm*/);
        xwer_t (* read)(struct xwds_st7735 * /*st7735*/, xwu8_t /*reg*/,
                        xwu8_t * /*buf*/, xwsz_t * /*size*/,
                        xwtm_t * /*xwtm*/);
        xwer_t (* set_brightness)(struct xwds_st7735 * /*st7735*/,
                                  xwu32_t /*brightness*/);
};

/******** ******** base driver ******** ********/
xwer_t xwds_st7735_drv_start(struct xwds_device * dev);
xwer_t xwds_st7735_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_st7735_drv_resume(struct xwds_device * dev);
xwer_t xwds_st7735_drv_suspend(struct xwds_device * dev);
#endif

/******** ******** ******** APIs ******** ******** ********/
xwer_t xwds_st7735_read_id(struct xwds_st7735 * st7735,
                           xwu32_t * id,
                           xwtm_t * xwtm);
xwer_t xwds_st7735_read_status(struct xwds_st7735 * st7735,
                               xwu8_t st[5],
                               xwtm_t * xwtm);
xwer_t xwds_st7735_read_madctl(struct xwds_st7735 * st7735,
                               xwu8_t madctl[2],
                               xwtm_t * xwtm);
xwer_t xwds_st7735_read_colmod(struct xwds_st7735 * st7735,
                               xwu8_t colmod[2],
                               xwtm_t * xwtm);
xwer_t xwds_st7735_display_on(struct xwds_st7735 * st7735, xwtm_t * xwtm);
xwer_t xwds_st7735_display_off(struct xwds_st7735 * st7735, xwtm_t * xwtm);
xwer_t xwds_st7735_set_brightness(struct xwds_st7735 * st7735, xwu32_t brightness);
void xwds_st7735_get_brightness(struct xwds_st7735 * st7735, xwu32_t * brightness);
xwer_t xwds_st7735_set_orientation(struct xwds_st7735 * st7735,
                                   xwu8_t orientation, xwtm_t * xwtm);
void xwds_st7735_get_orientation(struct xwds_st7735 * st7735, xwu8_t * orientation);
xwer_t xwds_st7735_invert_color(struct xwds_st7735 * st7735, bool inv, xwtm_t * xwtm);
xwer_t xwds_st7735_set_pixel(struct xwds_st7735 * st7735,
                             xwu8_t x, xwu8_t y,
                             xwu16_t color, xwtm_t * xwtm);
xwer_t xwds_st7735_fill_rect(struct xwds_st7735 * st7735,
                             xwu8_t x, xwu8_t y, xwu8_t width, xwu8_t height,
                             xwu16_t color, xwtm_t * xwtm);
xwer_t xwds_st7735_draw(struct xwds_st7735 * st7735,
                        xwu8_t x, xwu8_t y, xwu8_t width, xwu8_t height,
                        const xwu8_t img[], xwtm_t * xwtm);

#endif /* xwcd/perpheral/spi/lcd/st7735/driver.h */
