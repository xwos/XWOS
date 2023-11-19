/**
 * @file
 * @brief SPI LCD Controller ST7735 Device
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
 * @note
 * + SPI总线配置：
 *   + `spip->bus` 指明是SOC中的哪条SPI总线；
 *   + 若需要支持API `xwds_w25qxx_cfgbus()` ，需要在配置 `spip->buscfgid` 指明
 *     SPI总线配置的序号，SPI主机配置列表（spip->bus->buscfg数组）
 *     中必须要有此配置；
 */

#ifndef __xwcd_peripheral_spi_lcd_st7735_device_h__
#define __xwcd_peripheral_spi_lcd_st7735_device_h__

#include <xwos/standard.h>
#include <xwcd/ds/spi/peripheral.h>

/**
 * @brief ST7735 ID
 */
#define XWDS_ST7735_ID  0x5CU

/**
  * @brief ST7735 Size
  */
#define XWDS_ST7735_WIDTH       132U
#define XWDS_ST7735_HEIGHT      162U

/**
 * @brief ST7735 寄存器枚举
 */
enum xwds_st7735_reg_em {
        XWDS_ST7735_NOP         = 0x00U, /* No Operation: NOP */
        XWDS_ST7735_SWRESET     = 0x01U, /* Software reset: SWRESET */
        XWDS_ST7735_RDDID       = 0x04U, /* Read Display ID: RDDID */
        XWDS_ST7735_RDDST       = 0x09U, /* Read Display Statu: RDDST */
        XWDS_ST7735_RDDPM       = 0x0AU, /* Read Display Power: RDDPM */
        XWDS_ST7735_RDDMADCTL   = 0x0BU, /* Read Display: RDDMADCTL */
        XWDS_ST7735_RDDCOLMOD   = 0x0CU, /* Read Display Pixel: RDDCOLMOD */
        XWDS_ST7735_RDDIM       = 0x0DU, /* Read Display Image: RDDIM */
        XWDS_ST7735_RDDSM       = 0x0EU, /* Read Display Signal: RDDSM */
        XWDS_ST7735_SLPIN       = 0x10U, /* Sleep in & booster off: SLPIN */
        XWDS_ST7735_SLPOUT      = 0x11U, /* Sleep out & booster on: SLPOUT */
        XWDS_ST7735_PTLON       = 0x12U, /* Partial mode on: PTLON */
        XWDS_ST7735_NORON       = 0x13U, /* Partial off (Normal): NORON */
        XWDS_ST7735_INVOFF      = 0x20U, /* Display inversion off: INVOFF */
        XWDS_ST7735_INVON       = 0x21U, /* Display inversion on: INVON */
        XWDS_ST7735_GAMSET      = 0x26U, /* Gamma curve select: GAMSET */
        XWDS_ST7735_DISPOFF     = 0x28U, /* Display off: DISPOFF */
        XWDS_ST7735_DISPON      = 0x29U, /* Display on: DISPON */
        XWDS_ST7735_CASET       = 0x2AU, /* Column address set: CASET */
        XWDS_ST7735_RASET       = 0x2BU, /* Row address set: RASET */
        XWDS_ST7735_RAMWR       = 0x2CU, /* Memory write: RAMWR */
        XWDS_ST7735_RGBSET      = 0x2DU, /* LUT for 4k,65k,262k color: RGBSET */
        XWDS_ST7735_RAMRD       = 0x2EU, /* Memory read: RAMRD */
        XWDS_ST7735_PTLAR       = 0x30U, /* Partial start/end address set: PTLAR */
        XWDS_ST7735_TEOFF       = 0x34U, /* Tearing effect line off: TEOFF */
        XWDS_ST7735_TEON        = 0x35U, /* Tearing effect mode set & on: TEON */
        XWDS_ST7735_MADCTL      = 0x36U, /* Memory data access control: MADCTL */
        XWDS_ST7735_IDMOFF      = 0x38U, /* Idle mode off: IDMOFF */
        XWDS_ST7735_IDMON       = 0x39U, /* Idle mode on: IDMON */
        XWDS_ST7735_COLMOD      = 0x3AU, /* Interface pixel format: COLMOD */
        XWDS_ST7735_FRMCTR1     = 0xB1U, /* In normal mode (Full colors): FRMCTR1 */
        XWDS_ST7735_FRMCTR2     = 0xB2U, /* In Idle mode (8-colors): FRMCTR2 */
        XWDS_ST7735_FRMCTR3     = 0xB3U, /* In partial mode + Full colors: FRMCTR3 */
        XWDS_ST7735_INVCTR      = 0xB4U, /* Display inversion control: INVCTR */
        XWDS_ST7735_PWCTR1      = 0xC0U, /* Power control setting: PWCTR1 */
        XWDS_ST7735_PWCTR2      = 0xC1U, /* Power control setting: PWCTR2 */
        XWDS_ST7735_PWCTR3      = 0xC2U, /* In normal mode (Full colors): PWCTR3 */
        XWDS_ST7735_PWCTR4      = 0xC3U, /* In Idle mode (8-colors): PWCTR4 */
        XWDS_ST7735_PWCTR5      = 0xC4U, /* In partial mode + Full colors: PWCTR5 */
        XWDS_ST7735_VMCTR1      = 0xC5U, /* VCOM control 1: VMCTR1 */
        XWDS_ST7735_VMOFCTR     = 0xC7U, /* Set VCOM offset control: VMOFCTR */
        XWDS_ST7735_WRID2       = 0xD1U, /* Set LCM version code: WRID2 */
        XWDS_ST7735_WRID3       = 0xD2U, /* Customer Project code: WRID3 */
        XWDS_ST7735_NVCTR1      = 0xD9U, /* NVM control status: NVCTR1 */
        XWDS_ST7735_RDID1       = 0xDAU, /* Read ID1: RDID1 */
        XWDS_ST7735_RDID2       = 0xDBU, /* Read ID2: RDID2 */
        XWDS_ST7735_RDID3       = 0xDCU, /* Read ID3: RDID3 */
        XWDS_ST7735_NVCTR2      = 0xDEU, /* NVM Read Command: NVCTR2 */
        XWDS_ST7735_NVCTR3      = 0xDFU, /* NVM Write Command: NVCTR3 */
        XWDS_ST7735_GMCTRP1    = 0xE0U, /* Set Gamma adjustment (+ polarity): GMCTRP1 */
        XWDS_ST7735_GMCTRN1    = 0xE1U, /* Set Gamma adjustment (- polarity): GMCTRN1 */
        XWDS_ST7735_PWCTR6      = 0xFCU, /* In partial mode + Idle mode: PWCTR6 */
};

/**
 *  @brief ST7735 数据类型枚举
 */
enum xwds_st7735_datatype_em {
        XWDS_ST7735_DTYPE_CMD = 0, /**< 命令 */
        XWDS_ST7735_DTYPE_DATA = 1, /**< 数据 */
};

/**
 *  @brief ST7735 LCD定向枚举
 */
enum xwds_st7735_orientation_em {
        XWDS_ST7735_ORIENTATION_PORTRAIT = 0x00U, /**< 纵向 */
        XWDS_ST7735_ORIENTATION_PORTRAIT_ROT180 = 0xC0U, /**< 纵向并翻转180° */
        XWDS_ST7735_ORIENTATION_LANDSCAPE = 0xA0U, /**< 横向 */
        XWDS_ST7735_ORIENTATION_LANDSCAPE_ROT180 = 0x60U, /**< 横向并翻转180° */
};

/**
 *  @brief ST7735 像素数据格式枚举
 */
enum xwds_st7735_format_em {
        XWDS_ST7735_FORMAT_RBG565 = 0x05U, /**< RGB565, 16 bpp */
};

/**
 *  @brief ST7735 反色枚举
 */
enum xwds_st7735_inversion_em {
        XWDS_ST7735_INVERSION_OFF = 0U, /**< 反色关闭 */
        XWDS_ST7735_INVERSION_ON,
};

/**
 *  @brief ST7735 RGB颜色顺序枚举
 */
enum xwds_st7735_bgr_em {
        XWDS_ST7735_RGBSEQ_RGB = 0U, /**< RGB颜色顺序：RGB */
        XWDS_ST7735_RGBSEQ_BGR = 0x8U, /**< RGB颜色顺序：BGR */
};

/**
 * @brief ST7735 参数
 */
struct xwds_st7735_parameter {
        xwu8_t width; /**< 宽度 */
        xwu8_t width_offset; /**< 宽度起始点的偏移 */
        xwu8_t height; /**< 高度 */
        xwu8_t height_offset; /**< 高度起始点的偏移 */
        xwu8_t orientation; /**< LCD定向，取值：@ref xwds_st7735_orientation_em */
        xwu8_t pixelformat; /**< 像素数据格式，取值：@ref xwds_st7735_format_em */
        xwu8_t inversion; /**< 是否反色 */
        xwu8_t rgbseq; /**< RGB顺序 */
        xwu32_t brightness; /**< 亮度 */
};

/**
 * @brief ST7735 设备
 */
struct xwds_st7735 {
        struct xwds_spip spip; /**< C语言面向对象：继承 `struct xwds_spip` */

        /* attributes */
        struct xwds_st7735_parameter parameter; /**< 参数 */
};

void xwds_st7735_construct(struct xwds_st7735 * st7735);
void xwds_st7735_destruct(struct xwds_st7735 * st7735);

/**
 * @brief ST7735 API：增加对象的引用计数
 * @param[in] st7735: ST7735 设备指针
 * @return 错误码
 * @retval @ref xwds_spip_grab()
 */
static __xwds_inline
xwer_t xwds_st7735_grab(struct xwds_st7735 * st7735)
{
        return xwds_spip_grab(&st7735->spip);
}

/**
 * @brief ST7735 API：减少对象的引用计数
 * @param[in] st7735: ST7735 设备指针
 * @return 错误码
 * @retval @ref xwds_spip_put()
 */
static __xwds_inline
xwer_t xwds_st7735_put(struct xwds_st7735 * st7735)
{
        return xwds_spip_put(&st7735->spip);
}

#endif /* xwcd/peripheral/spi/lcd/st7735/device.h */
