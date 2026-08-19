/**
 * @file
 * @brief Intel8080 LCD Controller Device
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

#ifndef __xwcd_peripheral_i80_lcd_device_h__
#define __xwcd_peripheral_i80_lcd_device_h__

#include <xwos/standard.h>
#include <xwos/osal/sync/sem.h>
#include <xwcd/ds/device.h>

/**
 * @defgroup xwcd_peripheral_i80_lcd Intel8080 LCD Controller
 * @ingroup xwcd_peripheral_i80
 * @{
 */

/**
 * @brief 命令枚举
 */
enum xwds_i80lcd_cmd_em {
        XWDS_I80LCD_CMD_SWRESET = 0x01, /* Software Reset */
        XWDS_I80LCD_CMD_RDDID = 0x04, /* Read Display ID */
        XWDS_I80LCD_CMD_RDDPM = 0x0A, /* Read Display Power Mode */
        XWDS_I80LCD_CMD_RDDMADCTL = 0x0B, /* Read Display MADCTL */
        XWDS_I80LCD_CMD_RDDCOLMOD = 0x0C, /* Read Display Pixel Format */
        XWDS_I80LCD_CMD_RDDIM = 0x0D, /* Read Display Image Mode */
        XWDS_I80LCD_CMD_RDDSM = 0x0E, /* Read Display Signal Mode */
        XWDS_I80LCD_CMD_SLPIN = 0x10, /* Sleep in */
        XWDS_I80LCD_CMD_SLPOUT = 0x11, /* Sleep Out */
        XWDS_I80LCD_CMD_INVOFF = 0x20, /* Display Inversion Off */
        XWDS_I80LCD_CMD_INVON = 0x21, /* Display Inversion On */
        XWDS_I80LCD_CMD_DISPOFF = 0x28, /* Display Off */
        XWDS_I80LCD_CMD_DISPON = 0x29, /* Display On */
        XWDS_I80LCD_CMD_CASET = 0x2A, /* Column Address Set */
        XWDS_I80LCD_CMD_RASET = 0x2B, /* Row Address Set */
        XWDS_I80LCD_CMD_RAMWR = 0x2C, /* Memory Write */
        XWDS_I80LCD_CMD_RAMRD = 0x2E, /* Memory Read */
        XWDS_I80LCD_CMD_MADCTL = 0x36, /* Memory Data Access Control */
        XWDS_I80LCD_CMD_IDMOFF = 0x38, /* Idle Mode Off */
        XWDS_I80LCD_CMD_IDMON = 0x39, /* Idle mode on */
        XWDS_I80LCD_CMD_COLMOD = 0x3A, /* Pixel Color Format */
        XWDS_I80LCD_CMD_RDID4 = 0xD3, /* Read ID4 */
};

/**
 * @brief LCD定向枚举
 */
enum xwds_i80lcd_orientation_em {
        XWDS_I80LCD_ORIENTATION_PORTRAIT = 0x00U, /**< 纵向 */
        XWDS_I80LCD_ORIENTATION_PORTRAIT_ROT180 = 0xC0U, /**< 纵向并翻转180° */
        XWDS_I80LCD_ORIENTATION_LANDSCAPE = 0xA0U, /**< 横向 */
        XWDS_I80LCD_ORIENTATION_LANDSCAPE_ROT180 = 0x60U, /**< 横向并翻转180° */
};

/**
 * @brief 像素数据格式枚举
 */
enum xwds_i80lcd_format_em {
        XWDS_I80LCD_FORMAT_RBG565 = 0x55U, /**< RGB565, 16 bpp */
        XWDS_I80LCD_FORMAT_RBG666 = 0x66U, /**< RGB565, 18 bpp */
        XWDS_I80LCD_FORMAT_RBG888 = 0x77U, /**< RGB565, 24 bpp */
};

/**
 * @brief 反色枚举
 */
enum xwds_i80lcd_inversion_em {
        XWDS_I80LCD_INVERSION_OFF = 0U, /**< 反色关闭 */
        XWDS_I80LCD_INVERSION_ON,
};

/**
 * @brief RGB颜色顺序枚举
 */
enum xwds_i80lcd_rgbseq_em {
        XWDS_I80LCD_RGBSEQ_RGB = 0U, /**< RGB颜色顺序：RGB */
        XWDS_I80LCD_RGBSEQ_BGR = 0x8U, /**< RGB颜色顺序：BGR */
};

/**
 * @brief RGB常用颜色枚举
 */
enum xwds_i80lcd_rgb_em {
        XWDS_I80LCD_RGB_WHITE = 0xFFFFU,
        XWDS_I80LCD_RGB_BLACK = 0x0000U,
        XWDS_I80LCD_RGB_RED = 0xF800U,
        XWDS_I80LCD_RGB_GREEN = 0x07E0U,
        XWDS_I80LCD_RGB_BLUE = 0x001FU,
        XWDS_I80LCD_RGB_YELLOW = 0xFFE0U,
        XWDS_I80LCD_RGB_PURPLE = 0x8010U,
        XWDS_I80LCD_RGB_CYAN = 0x07FFU,
        XWDS_I80LCD_RGB_ORANGE = 0xFD00U,
        XWDS_I80LCD_RGB_SKYBLUE = 0x05FFU,
        XWDS_I80LCD_RGB_GRAY = 0X8430U,
};

#define XWDS_I80LCD_BRIGHTNESS_MAX      (1000U)

/**
 * @brief LCD参数
 *
 * ```
 * +-----+-------------------------------+-----+
 * |                    VBP                    |
 * +     +-------------------------------+     +
 * |     |             Width             |     |
 * |     |                             H |     |
 * |     |                             e |     |
 * | HBP |                             i | HFP |
 * |     |                             g |     |
 * |     |                             h |     |
 * |     |                             t |     |
 * |     |                               |     |
 * +     +-------------------------------+     +
 * |                    VFP                    |
 * +-----+-------------------------------+-----+
 * ```
 */
struct xwds_i80lcd_parameter {
        xwu16_t width; /**< 宽度 */
        xwu16_t hbp; /**< Horizontal Back Porch */
        xwu16_t hfp; /**< Horizontal Front Porch */
        xwu16_t height; /**< 高度 */
        xwu16_t vbp; /**< Vertical Back Porch */
        xwu16_t vfp; /**< Vertical Front Porch */
        xwu8_t orientation; /**< LCD定向，取值：@ref xwds_i80lcd_orientation_em */
        xwu8_t pixelformat; /**< 像素数据格式，取值：@ref xwds_i80lcd_format_em */
        xwu8_t inversion; /**< 是否反色 */
        xwu8_t rgbseq; /**< RGB顺序 */
        xwu16_t brightness; /**< 亮度 */
        xwu32_t frameperiod; /**< 刷新周期，单位: us */
};

/**
 * @brief I80LCD设备
 */
struct xwds_i80lcd {
        struct xwds_device dev; /**< C语言面向对象：继承 `struct xwds_device` */
        struct xwos_sem frmsem; /**< 刷新率信号量，由刷新率定时器周期触发 */

        /* attributes */
        xwer_t (* get_chipid)(struct xwds_i80lcd * /*i80lcd*/);
        xwu32_t chipid;
        volatile xwu16_t * cmd;
        volatile xwu16_t * dat;
        struct xwds_i80lcd_parameter parameter; /**< 参数 */
};

/**
 * @brief 对象的构造函数
 * @param[in] i80lcd: 对象指针
 */
void xwds_i80lcd_construct(struct xwds_i80lcd * i80lcd);

/**
 * @brief 对象的析构函数
 * @param[in] i80lcd: 对象指针
 */
void xwds_i80lcd_destruct(struct xwds_i80lcd * i80lcd);

/**
 * @brief 增加对象的引用计数
 * @param[in] i80lcd: 对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_i80lcd_grab(struct xwds_i80lcd * i80lcd)
{
        return xwds_device_grab(&i80lcd->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param[in] i80lcd: 对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_i80lcd_put(struct xwds_i80lcd * i80lcd)
{
        return xwds_device_put(&i80lcd->dev);
}

/**
 * @} xwcd_peripheral_i80_lcd
 */

#endif /* xwcd/peripheral/i80/lcd/device.h */
