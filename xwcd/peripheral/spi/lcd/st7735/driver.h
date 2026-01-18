/**
 * @file
 * @brief SPI LCD Controller ST7735 Driver
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
 * @note
 * - 所有API只可被单一线程访问。
 */

#ifndef __xwcd_peripheral_spi_lcd_st7735_driver_h__
#define __xwcd_peripheral_spi_lcd_st7735_driver_h__

#include <xwos/standard.h>
#include <xwcd/ds/spi/peripheral.h>
#include <xwcd/peripheral/spi/lcd/st7735/device.h>

/**
 * @ingroup xwcd_peripheral_spi_lcd_st7735
 * @{
 */

/**
 * @brief ST7735驱动函数表
 */
struct xwds_st7735_driver {
        struct xwds_spip_driver spip; /**< C语言面向对象：继承 `struct xwds_spip_driver` */
        xwer_t (* write)(struct xwds_st7735 * /*st7735*/, xwu8_t /*reg*/,
                         const xwu8_t * /*data*/, xwsz_t * /*size*/,
                         xwtm_t /*to*/);
        xwer_t (* read)(struct xwds_st7735 * /*st7735*/, xwu8_t /*reg*/,
                        xwu8_t * /*buf*/, xwsz_t * /*size*/,
                        xwtm_t /*to*/);
        xwer_t (* set_brightness)(struct xwds_st7735 * /*st7735*/,
                                  xwu32_t /*brightness*/);
};

/******** ******** base driver ******** ********/
/**
 * @brief ST7735基本驱动：启动设备
 */
xwer_t xwds_st7735_drv_start(struct xwds_device * dev);

/**
 * @brief ST7735基本驱动：停止设备
 */
xwer_t xwds_st7735_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)

/**
 * @brief ST7735基本驱动：暂停设备
 */
xwer_t xwds_st7735_drv_resume(struct xwds_device * dev);

/**
 * @brief ST7735基本驱动：继续设备
 */
xwer_t xwds_st7735_drv_suspend(struct xwds_device * dev);
#endif

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief ST7735 API：读取ID
 * @param[in] st7735: ST7735对象指针
 * @param[out] id: 指向缓冲区的指针，通过此缓冲区返回ID
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_read_id(struct xwds_st7735 * st7735,
                           xwu32_t * id,
                           xwtm_t to);

/**
 * @brief ST7735 API：读取状态
 * @param[in] st7735: ST7735对象指针
 * @param[out] st: 指向缓冲区的指针，通过此缓冲区返回状态
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_read_status(struct xwds_st7735 * st7735,
                               xwu8_t st[5],
                               xwtm_t to);

/**
 * @brief ST7735 API：读取RDDMADCTL寄存器的值
 * @param[in] st7735: ST7735对象指针
 * @param[out] madctl: 指向缓冲区的指针，通过此缓冲区返回寄存器的值
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_read_madctl(struct xwds_st7735 * st7735,
                               xwu8_t madctl[2],
                               xwtm_t to);

/**
 * @brief ST7735 API：读取RDDCOLMOD寄存器的值
 * @param[in] st7735: ST7735对象指针
 * @param[out] colmod: 指向缓冲区的指针，通过此缓冲区返回寄存器的值
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_read_colmod(struct xwds_st7735 * st7735,
                               xwu8_t colmod[2],
                               xwtm_t to);

/**
 * @brief ST7735 API：开启显示
 * @param[in] st7735: ST7735对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_display_on(struct xwds_st7735 * st7735, xwtm_t to);

/**
 * @brief ST7735 API：关闭显示
 * @param[in] st7735: ST7735对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_display_off(struct xwds_st7735 * st7735, xwtm_t to);

/**
 * @brief ST7735 API：设置亮度
 * @param[in] st7735: ST7735对象指针
 * @param[in] brightness: 亮度
 * @return 错误码
 * @note
 * + 上下文：线程
 */
xwer_t xwds_st7735_set_brightness(struct xwds_st7735 * st7735, xwu32_t brightness);

/**
 * @brief ST7735 API：读取亮度
 * @param[in] st7735: ST7735对象指针
 * @param[out] brightness: 指向缓冲区的指针，通过此缓冲区返回亮度
 * @return 错误码
 * @note
 * + 上下文：线程
 */
void xwds_st7735_get_brightness(struct xwds_st7735 * st7735, xwu32_t * brightness);

/**
 * @brief ST7735 API：设置显示方向
 * @param[in] st7735: ST7735对象指针
 * @param[in] orientation: 方向
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_set_orientation(struct xwds_st7735 * st7735,
                                   xwu8_t orientation, xwtm_t to);

/**
 * @brief ST7735 API：读取显示方向
 * @param[in] st7735: ST7735对象指针
 * @param[out] orientation: 指向缓冲区的指针，通过此缓冲区返回显示方向
 * @return 错误码
 * @note
 * + 上下文：线程
 */
void xwds_st7735_get_orientation(struct xwds_st7735 * st7735, xwu8_t * orientation);

/**
 * @brief ST7735 API：设置光标坐标
 * @param[in] st7735: ST7735对象指针
 * @param[in] x: X轴坐标
 * @param[in] y: Y轴坐标
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_set_cursor(struct xwds_st7735 * st7735, xwu8_t x, xwu8_t y,
                              xwtm_t to);

/**
 * @brief ST7735 API：设置窗口
 * @param[in] st7735: ST7735对象指针
 * @param[in] x: 窗口起始点X轴坐标
 * @param[in] y: 窗口起始点Y轴坐标
 * @param[in] width: 窗口的宽
 * @param[in] height: 窗口的高
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_set_window(struct xwds_st7735 * st7735,
                              xwu8_t x, xwu8_t y, xwu8_t width, xwu8_t height,
                              xwtm_t to);

/**
 * @brief ST7735 API：设置是否翻转颜色
 * @param[in] st7735: ST7735对象指针
 * @param[in] inv: 是否翻转颜色
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_invert_color(struct xwds_st7735 * st7735, bool inv, xwtm_t to);

/**
 * @brief ST7735 API：设置点的颜色
 * @param[in] st7735: ST7735对象指针
 * @param[in] x: 点X轴坐标
 * @param[in] y: 点Y轴坐标
 * @param[in] color: 颜色的RGB565值
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_set_pixel(struct xwds_st7735 * st7735,
                             xwu8_t x, xwu8_t y,
                             xwu16_t color, xwtm_t to);

/**
 * @brief ST7735 API：绘制矩形
 * @param[in] st7735: ST7735对象指针
 * @param[in] x: 矩形起始点X轴坐标
 * @param[in] y: 矩形起始点Y轴坐标
 * @param[in] width: 矩形的宽
 * @param[in] height: 矩形的高
 * @param[in] color: 填充颜色的RGB565值
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_fill_rect(struct xwds_st7735 * st7735,
                             xwu8_t x, xwu8_t y, xwu8_t width, xwu8_t height,
                             xwu16_t color, xwtm_t to);

/**
 * @brief ST7735 API：绘制位图
 * @param[in] st7735: ST7735对象指针
 * @param[in] x: 位图起始点X轴坐标
 * @param[in] y: 位图起始点Y轴坐标
 * @param[in] width: 位图的宽
 * @param[in] height: 位图的高
 * @param[in] img: 位图数组的指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_st7735_draw(struct xwds_st7735 * st7735,
                        xwu8_t x, xwu8_t y, xwu8_t width, xwu8_t height,
                        const xwu8_t img[], xwtm_t to);

/**
 * @} xwcd_peripheral_spi_lcd_st7735
 */

#endif /* xwcd/peripheral/spi/lcd/st7735/driver.h */
