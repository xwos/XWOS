/**
 * @file
 * @brief Intel8080 LCD Controller Driver
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

#include <xwos/standard.h>
#include <string.h>
#include <stdio.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/thd.h>
#include <xwcd/peripheral/i80/lcd/device.h>
#include <xwcd/peripheral/i80/lcd/driver.h>

#define I80LCD_DEBUG
#include <xwcd/peripheral/i80/lcd/log.h>

/******** ******** base driver ******** ********/
__xwbsp_code
xwer_t xwds_i80lcd_drv_start(struct xwds_device * dev)
{
        struct xwds_i80lcd * i80lcd;
        struct xwds_i80lcd_parameter * param;
        xwer_t rc;

        i80lcd = xwds_cast(struct xwds_i80lcd *, dev);
        param = &i80lcd->parameter;

        if (!((XWDS_I80LCD_ORIENTATION_PORTRAIT == param->orientation) ||
              (XWDS_I80LCD_ORIENTATION_PORTRAIT_ROT180 == param->orientation) ||
              (XWDS_I80LCD_ORIENTATION_LANDSCAPE == param->orientation) ||
              (XWDS_I80LCD_ORIENTATION_LANDSCAPE_ROT180 == param->orientation))) {
                rc = -EINVAL;
                goto err_param;
        }
        if (!(XWDS_I80LCD_FORMAT_RBG565 == param->pixelformat)) {
                rc = -EINVAL;
                goto err_param;
        }
        if (!((XWDS_I80LCD_INVERSION_OFF == param->inversion) ||
              (XWDS_I80LCD_INVERSION_ON == param->inversion))) {
                rc = -EINVAL;
                goto err_param;
        }
        if (!((XWDS_I80LCD_RGBSEQ_RGB == param->rgbseq) ||
              (XWDS_I80LCD_RGBSEQ_BGR == param->rgbseq))) {
                rc = -EINVAL;
                goto err_param;
        }
        if (param->brightness > XWDS_I80LCD_BRIGHTNESS_MAX) {
                param->brightness = XWDS_I80LCD_BRIGHTNESS_MAX;
        }
        return XWOK;

err_param:
        return rc;
}

__xwbsp_code
xwer_t xwds_i80lcd_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
__xwbsp_code
xwer_t xwds_i80lcd_drv_resume(struct xwds_device * dev)
{
        return xwds_i80lcd_drv_start(dev);
}

__xwbsp_code
xwer_t xwds_i80lcd_drv_suspend(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}
#endif

/******** APIs ********/
__xwbsp_api
xwer_t xwds_i80lcd_init(struct xwds_i80lcd * i80lcd)
{
        struct xwds_i80lcd_driver * drv;
        struct xwds_i80lcd_parameter * param;
        xwer_t rc;
        xwu16_t val;

        XWDS_VALIDATE(i80lcd, "nullptr", -EFAULT);

        param = &i80lcd->parameter;

        xwos_cthd_sleep(xwtm_ms(50));

        /* Read Chip ID */
        if (NULL != i80lcd->get_chipid) {
                rc = i80lcd->get_chipid(i80lcd);
                if (rc < 0) {
                        goto err_chipid;
                }
                i80lcdlogd("Chip ID: 0x%X\r\n", i80lcd->chipid);
        } else {
                rc = -ENODEV;
                goto err_chipid;
        }
        drv = xwds_cast(struct xwds_i80lcd_driver *, i80lcd->dev.drv);

        /* Device-spec init */
        if ((NULL != drv) && (NULL != drv->init)) {
                drv->init(i80lcd);
        }

        /* display orientation & RGB seq */
        val = param->orientation | param->rgbseq;
        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_MADCTL);
        xwds_i80lcd_write(i80lcd, val);

        /* color mode */
        val = param->pixelformat;
        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_COLMOD);
        xwds_i80lcd_write(i80lcd, val);

        /* inversion mode */
        if (XWDS_I80LCD_INVERSION_ON == param->inversion) {
                xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_INVON);
        } else {
                xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_INVOFF);
        }

        /* CLear display */
        xwds_i80lcd_clear(i80lcd, 0U, 0U, param->width, param->height,
                          XWDS_I80LCD_RGB_WHITE);
        return XWOK;

err_chipid:
        return rc;
}

__xwbsp_api
xwer_t xwds_i80lcd_fini(struct xwds_i80lcd * i80lcd)
{
        struct xwds_i80lcd_driver * drv;

        XWDS_VALIDATE(i80lcd, "nullptr", -EFAULT);

        drv = xwds_cast(struct xwds_i80lcd_driver *, i80lcd->dev.drv);
        xwds_i80lcd_switch_backlight(i80lcd, false);
        if (NULL != drv->fini) {
                drv->fini(i80lcd);
        } else {
                xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_DISPOFF);
                xwos_cthd_sleep(xwtm_ms(50));
                xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_SLPIN);
        }
        return XWOK;
}

__xwbsp_api
xwer_t xwds_i80lcd_switch_display(struct xwds_i80lcd * i80lcd, bool onoff)
{
        XWDS_VALIDATE(i80lcd, "nullptr", -EFAULT);

        if (onoff) {
                xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_DISPON);
        } else {
                xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_DISPOFF);
        }

        return XWOK;
}

__xwbsp_api
xwer_t xwds_i80lcd_switch_backlight(struct xwds_i80lcd * i80lcd, bool onoff)
{
        struct xwds_i80lcd_driver * drv;

        XWDS_VALIDATE(i80lcd, "nullptr");

        drv = xwds_cast(struct xwds_i80lcd_driver *, i80lcd->dev.drv);
        if (NULL != drv->set_backlight) {
                drv->set_backlight(i80lcd, onoff);
        }

        return XWOK;
}

__xwbsp_api
xwer_t xwds_i80lcd_set_brightness(struct xwds_i80lcd * i80lcd, xwu16_t brightness)
{
        struct xwds_i80lcd_driver * drv;

        XWDS_VALIDATE(i80lcd, "nullptr");

        if (brightness > XWDS_I80LCD_BRIGHTNESS_MAX) {
                brightness = XWDS_I80LCD_BRIGHTNESS_MAX;
        }
        drv = xwds_cast(struct xwds_i80lcd_driver *, i80lcd->dev.drv);
        if (NULL != drv->set_brightness) {
                i80lcd->parameter.brightness = brightness;
                drv->set_brightness(i80lcd, brightness);
        }
        return XWOK;
}

__xwbsp_api
xwer_t xwds_i80lcd_get_brightness(struct xwds_i80lcd * i80lcd, xwu16_t * brightness)
{
        XWDS_VALIDATE(i80lcd, "nullptr");
        XWDS_VALIDATE(brightness, "nullptr");

        if (brightness) {
                *brightness = i80lcd->parameter.brightness;
        }
        return XWOK;
}

__xwbsp_api
xwer_t xwds_i80lcd_set_window(struct xwds_i80lcd * i80lcd,
                              xwu16_t x, xwu16_t y, xwu16_t w, xwu16_t h)
{
        struct xwds_i80lcd_parameter * param;
        xwu16_t xs;
        xwu16_t xe;
        xwu16_t ys;
        xwu16_t ye;
        xwu8_t buffer[4];

        XWDS_VALIDATE(i80lcd, "nullptr");

        param = &i80lcd->parameter;
        /* Column Address Set */
        xs = x + param->hbp;
        xe = x + w + param->hbp - 1U;
        buffer[0] = (xs >> 8U) & 0xFFU;
        buffer[1] = xs & 0xFFU;
        buffer[2] = (xe >> 8U) & 0xFFU;
        buffer[3] = xe & 0xFFU;
        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_CASET);
        xwds_i80lcd_write(i80lcd, buffer[0]);
        xwds_i80lcd_write(i80lcd, buffer[1]);
        xwds_i80lcd_write(i80lcd, buffer[2]);
        xwds_i80lcd_write(i80lcd, buffer[3]);

        /* Row Address Set */
        ys = y + param->vbp;
        ye = y + h + param->vbp - 1U;
        buffer[0] = (ys >> 8U) & 0xFFU;
        buffer[1] = ys & 0xFFU;
        buffer[2] = (ye >> 8U) & 0xFFU;
        buffer[3] = ye & 0xFFU;
        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RASET);
        xwds_i80lcd_write(i80lcd, buffer[0]);
        xwds_i80lcd_write(i80lcd, buffer[1]);
        xwds_i80lcd_write(i80lcd, buffer[2]);
        xwds_i80lcd_write(i80lcd, buffer[3]);

        return XWOK;
}

__xwbsp_api
xwer_t xwds_i80lcd_draw_pixel(struct xwds_i80lcd * i80lcd,
                              xwu16_t x, xwu16_t y,
                              xwu16_t color)
{
        XWDS_VALIDATE(i80lcd, "nullptr");

        xwds_i80lcd_set_window(i80lcd, x, y, x + 1U, y + 1U);
        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RAMWR);
        xwds_i80lcd_write(i80lcd, color);

        return XWOK;
}

__xwbsp_api
xwer_t xwds_i80lcd_clear(struct xwds_i80lcd * i80lcd,
                         xwu16_t x, xwu16_t y, xwu16_t w, xwu16_t h,
                         xwu16_t color)
{
        struct xwds_i80lcd_driver * drv;
        struct xwds_i80lcd_parameter * param;
        xwsz_t total;
        xwsz_t i;

        XWDS_VALIDATE(i80lcd, "nullptr");

        drv = xwds_cast(struct xwds_i80lcd_driver *, i80lcd->dev.drv);
        param = &i80lcd->parameter;
        if (x + w > param->width) {
                w = param->width - x;
        }
        if (y + h > param->height) {
                h = param->height - y;
        }
        if (NULL != drv->clear) {
                drv->clear(i80lcd, x, y, w, h, color);
        } else {
                total = w * h;
                xwds_i80lcd_set_window(i80lcd, x, y, w, h);
                xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RAMWR);
                for (i = 0U; i < total; i++) {
                        xwds_i80lcd_write(i80lcd, color);
                }
        }

        return XWOK;
}

__xwbsp_api
xwer_t xwds_i80lcd_commit(struct xwds_i80lcd * i80lcd,
                          xwu16_t x, xwu16_t y, xwu16_t w, xwu16_t h,
                          xwu16_t * frame, xwtm_t to)
{
        xwer_t rc;
        struct xwds_i80lcd_driver * drv;
        struct xwds_i80lcd_parameter * param;
        xwsz_t total;
        xwsz_t i;

        XWDS_VALIDATE(i80lcd, "nullptr", -EFAULT);
        XWDS_VALIDATE(frame, "nullptr", -EFAULT);

        drv = xwds_cast(struct xwds_i80lcd_driver *, i80lcd->dev.drv);
        param = &i80lcd->parameter;
        if (x + w > param->width) {
                w = param->width - x;
        }
        if (y + h > param->height) {
                h = param->height - y;
        }
        rc = xwos_sem_wait_to(&i80lcd->frmsem, to);
        if (XWOK == rc) {
                if (NULL != drv->commit) {
                        drv->commit(i80lcd, x, y, w, h, frame);
                } else {
                        total = w * h;
                        xwds_i80lcd_set_window(i80lcd, x, y, w, h);
                        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RAMWR);
                        for (i = 0U; i < total; i++) {
                                xwds_i80lcd_write(i80lcd, frame[i]);
                        }
                }
        }
        return rc;
}


__xwbsp_api
void xwds_i80lcd_dump_data(struct xwds_i80lcd * i80lcd,
                           xwu16_t x, xwu16_t y, xwu16_t w, xwu16_t h)
{
        xwu32_t num = w * h;
        volatile xwu16_t data[num];
        xwsz_t i;
        xwsz_t j;

        XWDS_VALIDATE(i80lcd, "nullptr");

        xwds_i80lcd_set_window(i80lcd, x, y, w, h);
        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RAMRD);
        data[0] = xwds_i80lcd_read(i80lcd); /* dummy read */
        for (i = 0; i < num; i++) {
                data[i] = xwds_i80lcd_read(i80lcd);
        }
        i80lcdlogi("Dump GRAM, origin(%d, %d): pixelx num: %d\r\n", x, y, num);
        for (i = 0U; i < num; i += 8U) {
                printf("%04x: ", (unsigned int)i);
                for (j = 0U; j < 8U && (i + j) < num; j++) {
                        printf("%04x ", data[i + j]);
                }
                while (j < 8U) {
                        printf("     ");
                        j++;
                }
                printf("|\r\n");
        }
}

__xwbsp_api
void xwds_i80lcd_dump_status(struct xwds_i80lcd * i80lcd)
{
        volatile xwu16_t pwr;
        volatile xwu16_t mad;
        volatile xwu16_t col;
        volatile xwu16_t img;
        volatile xwu16_t sig;

        XWDS_VALIDATE(i80lcd, "nullptr");

        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RDDPM);
        pwr = xwds_i80lcd_read(i80lcd); /* dummy read */
        pwr = xwds_i80lcd_read(i80lcd);

        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RDDMADCTL);
        mad = xwds_i80lcd_read(i80lcd); /* dummy read */
        mad = xwds_i80lcd_read(i80lcd);

        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RDDCOLMOD);
        col = xwds_i80lcd_read(i80lcd); /* dummy read */
        col = xwds_i80lcd_read(i80lcd);

        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RDDIM);
        img = xwds_i80lcd_read(i80lcd); /* dummy read */
        img = xwds_i80lcd_read(i80lcd);

        xwds_i80lcd_write_cmd(i80lcd, XWDS_I80LCD_CMD_RDDSM);
        sig = xwds_i80lcd_read(i80lcd); /* dummy read */
        sig = xwds_i80lcd_read(i80lcd);

        i80lcdlogi("Status: Power=0x%X,MA=0x%X,Color=0x%X,Image=0x%X,Signal=0x%X\r\n",
                   pwr, mad, col, img, sig);
}
