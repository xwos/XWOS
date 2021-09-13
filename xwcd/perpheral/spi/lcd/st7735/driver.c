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
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <string.h>
#include <xwcd/ds/spi/perpheral.h>
#include <xwcd/perpheral/spi/lcd/st7735/device.h>
#include <xwcd/perpheral/spi/lcd/st7735/driver.h>

static
xwer_t xwds_st7735_write(struct xwds_st7735 * st7735, xwu8_t reg,
                         const xwu8_t * data, xwsz_t size,
                         xwtm_t * xwtm)
{
        struct xwds_st7735_driver * drv;

        drv = xwds_cast(struct xwds_st7735_driver *, st7735->spip.dev.drv);
        return drv->write(st7735, reg, data, &size, xwtm);
}

static
xwer_t xwds_st7735_read(struct xwds_st7735 * st7735, xwu8_t reg,
                        xwu8_t * buf, xwsz_t size,
                        xwtm_t * xwtm)
{
        struct xwds_st7735_driver * drv;

        drv = xwds_cast(struct xwds_st7735_driver *, st7735->spip.dev.drv);
        return drv->read(st7735, reg, buf, &size, xwtm);
}

/******** ******** base driver ******** ********/
struct xwds_st7735_cmd_table {
        xwu8_t reg;
        xwu8_t size;
        xwu8_t data[16];
};

const struct xwds_st7735_cmd_table xwds_st7735_init_table[] = {
        /* software reset */
        {XWDS_ST7735_SWRESET, 0, {0},},
        /* Out of sleep mode */
        {XWDS_ST7735_SLPOUT, 0, {0},},
        /* Framerate ctrl - normal mode. Rate = fosc/(1x2+40) * (LINE+2C+2D) */
        {XWDS_ST7735_FRMCTR1, 3, {0x01, 0x2C, 0x2D},},
        /* Framerate ctrl - idle mode.  Rate = fosc/(1x2+40) * (LINE+2C+2D) */
        {XWDS_ST7735_FRMCTR2, 3, {0x01, 0x2C, 0x2D},},
        /* Framerate - partial mode. Dot/Line inversion mode */
        {XWDS_ST7735_FRMCTR3, 6, {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D},},
        /* Display inversion ctrl: No inversion */
        {XWDS_ST7735_INVCTR, 1, {0x07},},
        /* Power control1 set GVDD: -4.6V, AUTO mode. */
        {XWDS_ST7735_PWCTR1, 3, {0xA2, 0x02, 0x84},},
        /* Power control2 set VGH/VGL: VGH25=2.4C VGSEL=-10 VGH=3 * AVDD */
        {XWDS_ST7735_PWCTR2, 1, {0xC5},},
        /* Power control3 normal mode(Full color): Op-amp current small, booster voltage */
        {XWDS_ST7735_PWCTR3, 2, {0x0A, 0x00},},
        /* Power control4 idle mode(8-colors): Op-amp current small & medium low */
        {XWDS_ST7735_PWCTR4, 2, {0x8A, 0x2A},},
        /* Power control5 partial mode + full colors */
        {XWDS_ST7735_PWCTR5, 2, {0x8A, 0xEE},},
        /* VCOMH VoltageVCOM control 1: VCOMH=0x0E=2.850 */
        {XWDS_ST7735_VMCTR1, 1, {0x0E},},
        /* Gamma Adjustments (pos. polarity).
           Not entirely necessary, but provides accurate colors. */
        {XWDS_ST7735_GMCTRP1, 16,
         {0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D,
          0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10},
         /* {0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22,
          0x1F, 0x1B, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10}, */
        },
        /* Gamma Adjustments (neg. polarity).
           Not entirely necessary, but provides accurate colors. */
        {XWDS_ST7735_GMCTRN1, 16,
         {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
          0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10},
         /* {0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E,
          0x30, 0x30, 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10}, */
        },
        /* Normal Display Mode On */
        {XWDS_ST7735_NORON, 0, {0},},
};

xwer_t xwds_st7735_drv_start(struct xwds_device * dev)
{
        struct xwds_st7735 * st7735;
        struct xwds_st7735_parameter * param;
        struct xwds_st7735_driver * drv;
        const struct xwds_st7735_cmd_table * init;
        xwu8_t buffer[4];
        xwsq_t i;
        xwtm_t xwtm;
        xwer_t rc;

        st7735 = xwds_cast(struct xwds_st7735 *, dev);
        param = &st7735->parameter;
        drv = xwds_cast(struct xwds_st7735_driver *, st7735->spip.dev.drv);
        init = xwds_st7735_init_table;

        if (!((drv) && (drv->write) && (drv->read))) {
                rc = -EINVAL;
                goto err_drv;
        }
        if (!((XWDS_ST7735_ORIENTATION_PORTRAIT == param->orientation) ||
              (XWDS_ST7735_ORIENTATION_PORTRAIT_ROT180 == param->orientation) ||
              (XWDS_ST7735_ORIENTATION_LANDSCAPE == param->orientation) ||
              (XWDS_ST7735_ORIENTATION_LANDSCAPE_ROT180 == param->orientation))) {
                rc = -EINVAL;
                goto err_param;
        }
        if (!(XWDS_ST7735_FORMAT_RBG565 == param->pixelformat)) {
                rc = -EINVAL;
                goto err_param;
        }
        if (!((XWDS_ST7735_INVERSION_OFF == param->inversion) ||
              (XWDS_ST7735_INVERSION_ON == param->inversion))) {
                rc = -EINVAL;
                goto err_param;
        }
        if (!((XWDS_ST7735_RGBSEQ_RGB == param->rgbseq) ||
              (XWDS_ST7735_RGBSEQ_BGR == param->rgbseq))) {
                rc = -EINVAL;
                goto err_param;
        }

        /* Init process */
        xwtm = XWTM_MAX;
        for (i = 0; i < xw_array_size(xwds_st7735_init_table); i++) {
                rc = xwds_st7735_write(st7735,
                                       init[i].reg, init[i].data, init[i].size,
                                       &xwtm);
                if (rc < 0) {
                        goto err_write;
                }
        }

        /* Set color mode */
        buffer[0] = param->pixelformat;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_COLMOD, buffer, 1,
                               &xwtm);
        if (rc < 0) {
                goto err_write;
        }

        /* Set inversion mode */
        if (XWDS_ST7735_INVERSION_ON == param->inversion) {
                rc = xwds_st7735_write(st7735,
                                       XWDS_ST7735_INVON, NULL, 0,
                                       &xwtm);
        } else {
                rc = xwds_st7735_write(st7735,
                                       XWDS_ST7735_INVOFF, NULL, 0,
                                       &xwtm);
        }
        if (rc < 0) {
                goto err_write;
        }

        /* Set the display orientation & RGB seq */
        buffer[0] = param->orientation | param->rgbseq;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_MADCTL, buffer, 1,
                               &xwtm);
        if (rc < 0) {
                goto err_write;
        }

        /* Column Address Set */
        buffer[0] = 0;
        buffer[1] = param->width_offset;
        buffer[2] = 0;
        buffer[3] = param->width + param->width_offset - 1;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_CASET, buffer, 4,
                               &xwtm);
        if (rc < 0) {
                goto err_write;
        }

        /* Row Address Set */
        buffer[0] = 0;
        buffer[1] = param->height_offset;
        buffer[2] = 0;
        buffer[3] = param->height + param->height_offset - 1;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_RASET, buffer, 4,
                               &xwtm);
        if (rc < 0) {
                goto err_write;
        }

        /* Display On */
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_DISPON, NULL, 0,
                               &xwtm);
        if (rc < 0) {
                goto err_write;
        }

        /* Set brightness */
        rc = xwds_st7735_set_brightness(st7735, param->brightness);
        if (rc < 0) {
                goto err_set_brightness;
        }
        return XWOK;

err_set_brightness:
err_write:
err_param:
err_drv:
        return rc;
}

const struct xwds_st7735_cmd_table xwds_st7735_deinit_table[] = {
        /* enter sleep mode */
        {XWDS_ST7735_SLPIN, 0, {0},},
};

xwer_t xwds_st7735_drv_stop(struct xwds_device * dev)
{
        struct xwds_st7735 * st7735;
        const struct xwds_st7735_cmd_table * deinit;
        xwsq_t i;
        xwtm_t xwtm;
        xwer_t rc;

        st7735 = xwds_cast(struct xwds_st7735 *, dev);
        deinit = xwds_st7735_deinit_table;
        xwtm = XWTM_MAX;
        for (i = 0; i < xw_array_size(xwds_st7735_deinit_table); i++) {
                rc = xwds_st7735_write(st7735,
                                       deinit[i].reg, deinit[i].data, deinit[i].size,
                                       &xwtm);
                if (rc < 0) {
                        goto err_write;
                }
        }
        return XWOK;

err_write:
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_st7735_drv_resume(struct xwds_device * dev)
{
        return xwds_st7735_drv_start(dev);
}

xwer_t xwds_st7735_drv_suspend(struct xwds_device * dev)
{
        return xwds_st7735_drv_stop(dev);
}
#endif

/******** ******** ******** APIs ******** ******** ********/
xwer_t xwds_st7735_cfgbus(struct xwds_st7735 * st7735, xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_st7735_grab(st7735);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_grab;
        }
        rc = xwds_spim_buscfg(st7735->spip.bus, st7735->spip.buscfgid, xwtm);
        if (rc < 0) {
                goto err_spim_buscfg;
        }
        xwds_st7735_put(st7735);
        return XWOK;

err_spim_buscfg:
        xwds_st7735_put(st7735);
err_st7735_grab:
        return rc;
}

xwer_t xwds_st7735_read_id(struct xwds_st7735 * st7735, xwu32_t * id, xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(id, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_st7735_grab(st7735);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_grab;
        }
        rc = xwds_st7735_read(st7735, XWDS_ST7735_RDDID, (xwu8_t *)id, 4, xwtm);
        xwds_st7735_put(st7735);
        return XWOK;

err_st7735_grab:
        return rc;
}

xwer_t xwds_st7735_read_status(struct xwds_st7735 * st7735,
                               xwu8_t st[5],
                               xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(id, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_st7735_grab(st7735);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_grab;
        }
        rc = xwds_st7735_read(st7735, XWDS_ST7735_RDDST, st, 5, xwtm);
        xwds_st7735_put(st7735);
        return XWOK;

err_st7735_grab:
        return rc;
}

xwer_t xwds_st7735_read_madctl(struct xwds_st7735 * st7735,
                               xwu8_t madctl[2],
                               xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(id, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_st7735_grab(st7735);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_grab;
        }
        rc = xwds_st7735_read(st7735, XWDS_ST7735_RDDMADCTL, madctl, 2, xwtm);
        xwds_st7735_put(st7735);
        return XWOK;

err_st7735_grab:
        return rc;
}

xwer_t xwds_st7735_read_colmod(struct xwds_st7735 * st7735,
                               xwu8_t colmod[2],
                               xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(id, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_st7735_grab(st7735);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_grab;
        }
        rc = xwds_st7735_read(st7735, XWDS_ST7735_RDDCOLMOD, colmod, 2, xwtm);
        xwds_st7735_put(st7735);
        return XWOK;

err_st7735_grab:
        return rc;
}

xwer_t xwds_st7735_display_on(struct xwds_st7735 * st7735, xwtm_t * xwtm)
{
        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        return xwds_st7735_write(st7735,
                                 XWDS_ST7735_DISPON, NULL, 0,
                                 xwtm);
}

xwer_t xwds_st7735_display_off(struct xwds_st7735 * st7735, xwtm_t * xwtm)
{
        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        return xwds_st7735_write(st7735,
                                 XWDS_ST7735_DISPOFF, NULL, 0,
                                 xwtm);
}

xwer_t xwds_st7735_set_brightness(struct xwds_st7735 * st7735, xwu32_t brightness)
{
        struct xwds_st7735_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);

        drv = xwds_cast(struct xwds_st7735_driver *, st7735->spip.dev.drv);
        if ((drv) && (drv->set_brightness)) {
                rc = drv->set_brightness(st7735, brightness);
                if (XWOK == rc) {
                        st7735->parameter.brightness = brightness;
                }
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

void xwds_st7735_get_brightness(struct xwds_st7735 * st7735, xwu32_t * brightness)
{
        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(brightness, "nullptr", -EFAULT);

        *brightness = st7735->parameter.brightness;
}

xwer_t xwds_st7735_set_orientation(struct xwds_st7735 * st7735,
                                   xwu8_t orientation, xwtm_t * xwtm)
{
        struct xwds_st7735_parameter * param;
        xwu8_t tmp;
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        XWDS_VALIDATE(((XWDS_ST7735_ORIENTATION_PORTRAIT == orientation) ||
                       (XWDS_ST7735_ORIENTATION_LANDSCAPE_ROT180 == orientation) ||
                       (XWDS_ST7735_ORIENTATION_PORTRAIT == orientation) ||
                       (XWDS_ST7735_ORIENTATION_LANDSCAPE_ROT180 == orientation)),
                      "nullptr", -EINVAL);

        param = &st7735->parameter;
        tmp = orientation;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_MADCTL, &tmp, 1,
                               xwtm);
        if (rc < 0) {
                goto err_write;
        }

        if ((XWDS_ST7735_ORIENTATION_PORTRAIT == param->orientation) ||
            (XWDS_ST7735_ORIENTATION_PORTRAIT_ROT180 == param->orientation)) {
                if ((XWDS_ST7735_ORIENTATION_LANDSCAPE == orientation) ||
                    (XWDS_ST7735_ORIENTATION_LANDSCAPE_ROT180 == orientation)) {
                        tmp = param->width;
                        param->width = param->height;
                        param->height = tmp;
                        tmp = param->width_offset;
                        param->width_offset = param->height_offset;
                        param->height_offset = tmp;
                }
        } else {
                if ((XWDS_ST7735_ORIENTATION_PORTRAIT == orientation) ||
                    (XWDS_ST7735_ORIENTATION_PORTRAIT_ROT180 == orientation)) {
                        tmp = param->width;
                        param->width = param->height;
                        param->height = tmp;
                        tmp = param->width_offset;
                        param->width_offset = param->height_offset;
                        param->height_offset = tmp;
                }
        }
        param->orientation = orientation;
        return XWOK;

err_write:
        return rc;
}

void xwds_st7735_get_orientation(struct xwds_st7735 * st7735, xwu8_t * orientation)
{
        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(orientation, "nullptr", -EFAULT);

        *orientation = st7735->parameter.orientation;
}

static
xwer_t xwds_st7735_set_cursor(struct xwds_st7735 * st7735, xwu8_t x, xwu8_t y,
                              xwtm_t * xwtm)
{
        struct xwds_st7735_parameter * param;
        xwu8_t buffer[4];
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        param = &st7735->parameter;

        XWDS_VALIDATE((x < param->width), "nullptr", -ERANGE);
        XWDS_VALIDATE((y < param->height), "nullptr", -ERANGE);

        /* Column Address Set */
        buffer[0] = 0;
        buffer[1] = x + param->width_offset;
        buffer[2] = 0;
        buffer[3] = param->width + param->width_offset - 1;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_CASET, buffer, 4,
                               xwtm);
        if (rc < 0) {
                goto err_write;
        }

        /* Row Address Set */
        buffer[0] = 0;
        buffer[1] = y + param->height_offset;
        buffer[2] = 0;
        buffer[3] = param->height + param->height_offset - 1;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_RASET, buffer, 4,
                               xwtm);
        if (rc < 0) {
                goto err_write;
        }
        return XWOK;

err_write:
        return rc;
}

static
xwer_t xwds_st7735_set_window(struct xwds_st7735 * st7735,
                              xwu8_t x, xwu8_t y, xwu8_t width, xwu8_t height,
                              xwtm_t * xwtm)
{
        struct xwds_st7735_parameter * param;
        xwu8_t buffer[4];
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);
        XWDS_VALIDATE((x < width), "nullptr", -ERANGE);
        XWDS_VALIDATE((y < height), "nullptr", -ERANGE);

        param = &st7735->parameter;

        XWDS_VALIDATE((width < param->width), "nullptr", -ERANGE);
        XWDS_VALIDATE((height < param->height), "nullptr", -ERANGE);

        /* Column Address Set */
        buffer[0] = 0;
        buffer[1] = x + param->width_offset;
        buffer[2] = 0;
        buffer[3] = width + param->width_offset - 1;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_CASET, buffer, 4,
                               xwtm);
        if (rc < 0) {
                goto err_write;
        }

        /* Row Address Set */
        buffer[0] = 0;
        buffer[1] = y + param->height_offset;
        buffer[2] = 0;
        buffer[3] = height + param->height_offset - 1;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_RASET, buffer, 4,
                               xwtm);
        if (rc < 0) {
                goto err_write;
        }
        return XWOK;

err_write:
        return rc;
}

xwer_t xwds_st7735_invert_color(struct xwds_st7735 * st7735, bool inv, xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        if (inv) {
                rc = xwds_st7735_write(st7735,
                                       XWDS_ST7735_INVON, NULL, 0,
                                       xwtm);
        } else {
                rc = xwds_st7735_write(st7735,
                                       XWDS_ST7735_INVOFF, NULL, 0,
                                       xwtm);
        }
        return rc;
}

xwer_t xwds_st7735_set_pixel(struct xwds_st7735 * st7735,
                             xwu8_t x, xwu8_t y,
                             xwu16_t color, xwtm_t * xwtm)
{
        xwu8_t buffer[2];
        xwer_t rc;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_st7735_set_cursor(st7735, x, y, xwtm);
        if (rc < 0) {
                goto err_set_cursor;
        }
        buffer[0] = color >> 8;
        buffer[1] = color & 0xFF;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_RAMWR, buffer, 2,
                               xwtm);

err_set_cursor:
        return rc;
}

xwer_t xwds_st7735_fill_rect(struct xwds_st7735 * st7735,
                             xwu8_t x, xwu8_t y, xwu8_t width, xwu8_t height,
                             xwu16_t color, xwtm_t * xwtm)
{
        xwer_t rc;
        xwsz_t i, total, wrsz;
        xwu8_t fb[64];
        xwu8_t colorh, colorl;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        colorh = color >> 8;
        colorl = color & 0xFF;
        for (i = 0; i < sizeof(fb); i = i + 2) {
                fb[i] = colorh;
                fb[i + 1] = colorl;
        }
        rc = xwds_st7735_set_window(st7735, x, y, width, height, xwtm);
        if (rc < 0) {
                goto err_set_window;
        }
        total = (xwsz_t)width * (xwsz_t)height * (xwsz_t)2;
        wrsz = 0;
        while (wrsz < total) {
                if (wrsz + sizeof(fb) < total) {
                        i = sizeof(fb);
                } else {
                        i = total - wrsz;
                }
                rc = xwds_st7735_write(st7735,
                                       XWDS_ST7735_RAMWR, fb, i,
                                       xwtm);
                if (rc < 0) {
                        goto err_write;
                }
                wrsz += i;
        }
        return XWOK;

err_write:
err_set_window:
        return rc;
}

xwer_t xwds_st7735_draw(struct xwds_st7735 * st7735,
                        xwu8_t x, xwu8_t y, xwu8_t width, xwu8_t height,
                        const xwu8_t img[], xwtm_t * xwtm)
{
        xwer_t rc;
        xwsz_t total;

        XWDS_VALIDATE(st7735, "nullptr", -EFAULT);
        XWDS_VALIDATE(image, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_st7735_set_window(st7735, x, y, width, height, xwtm);
        if (rc < 0) {
                goto err_set_window;
        }

        total = (xwsz_t)width * (xwsz_t)height * (xwsz_t)2;
        rc = xwds_st7735_write(st7735,
                               XWDS_ST7735_RAMWR, img, total,
                               xwtm);

err_set_window:
        return rc;
}
