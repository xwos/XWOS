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
 * @note
 * - 所有API只可被单一线程访问。
 */

#ifndef __xwcd_peripheral_i80_lcd_driver_h__
#define __xwcd_peripheral_i80_lcd_driver_h__

#include <xwos/standard.h>
#include <xwcd/peripheral/i80/lcd/device.h>

/**
 * @ingroup xwcd_peripheral_i80_lcd
 * @{
 */

/**
 * @brief 驱动函数表
 */
struct xwds_i80lcd_driver {
        struct xwds_driver base; /**< C语言面向对象：继承 `struct xwds_driver` */
        void (* init)(struct xwds_i80lcd * /*i80lcd*/);
        void (* fini)(struct xwds_i80lcd * /*i80lcd*/);
        void (* set_backlight)(struct xwds_i80lcd * /*i80lcd*/, bool /*onoff*/);
        void (* set_brightness)(struct xwds_i80lcd * /*i80lcd*/, xwu16_t /*bv*/);
        void (* clear)(struct xwds_i80lcd * /*i80lcd*/,
                       xwu16_t /*x*/, xwu16_t /*y*/, xwu16_t /*w*/, xwu16_t /*h*/,
                       xwu16_t /*c*/);
        void (* commit)(struct xwds_i80lcd * /*i80lcd*/,
                        xwu16_t /*x*/, xwu16_t /*y*/, xwu16_t /*w*/, xwu16_t /*h*/,
                        xwu16_t */*frm*/);
};

/******** ******** base driver ******** ********/
/**
 * @brief 基本驱动：启动设备
 * @note
 * + 上下文：启动
 * @details
 * 基本驱动 `start()` 运行上下文是 **启动** ，限制了睡眠API的使用，
 * 因此不在其中初始化屏幕。
 */
xwer_t xwds_i80lcd_drv_start(struct xwds_device * dev);

/**
 * @brief 基本驱动：停止设备
 * @note
 * + 上下文：启动
 */
xwer_t xwds_i80lcd_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)

/**
 * @brief 基本驱动：暂停设备
 * @note
 * + 上下文：电源管理
 * @details
 * 基本驱动 `resume()` 运行上下文是 **电源管理** ，限制了睡眠API的使用，
 * 因此不在其中恢复屏幕。
 */
xwer_t xwds_i80lcd_drv_resume(struct xwds_device * dev);

/**
 * @brief 基本驱动：继续设备
 * @note
 * + 上下文：电源管理
 * @details
 * 基本驱动 `suspend()` 运行上下文是 **电源管理** ，限制了睡眠API的使用，
 * 因此不在其中挂起屏幕。
 */
xwer_t xwds_i80lcd_drv_suspend(struct xwds_device * dev);
#endif

/******** Base I/O Function ********/
/**
 * @brief 写命令
 * @param[in] i80lcd: 对象指针
 * @param[in] cmd: 命令
 * @note
 * + 上下文：任意
 */
static __xwcd_inline_api
void xwds_i80lcd_write_cmd(struct xwds_i80lcd * i80lcd, volatile xwu16_t cmd)
{
        *i80lcd->cmd = cmd;
}

/**
 * @brief 写数据
 * @param[in] i80lcd: 对象指针
 * @param[in] data: 数据
 * @note
 * + 上下文：任意
 */
static __xwcd_inline_api
void xwds_i80lcd_write(struct xwds_i80lcd * i80lcd, volatile xwu16_t data)
{
        *i80lcd->dat = data;
}

/**
 * @brief 读数据
 * @param[in] i80lcd: 对象指针
 * @return 数据
 * @note
 * + 上下文：任意
 */
static __xwcd_inline_api
xwu16_t xwds_i80lcd_read(struct xwds_i80lcd * i80lcd)
{
        return *i80lcd->dat;
}

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief 初始化屏幕
 * @param[in] i80lcd: 对象指针
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -ENODEV: 无法识别的屏幕
 * @note
 * + 上下文：线程
 */
xwer_t xwds_i80lcd_init(struct xwds_i80lcd * i80lcd);

/**
 * @brief 关闭屏幕
 * @param[in] i80lcd: 对象指针
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：线程
 */
xwer_t xwds_i80lcd_fini(struct xwds_i80lcd * i80lcd);

/**
 * @brief 开启/关闭显示
 * @param[in] i80lcd: 对象指针
 * @param[in] onoff: 开启/关闭
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwds_i80lcd_switch_display(struct xwds_i80lcd * i80lcd, bool onoff);

/**
 * @brief 开启/关闭背光
 * @param[in] i80lcd: 对象指针
 * @param[in] onoff: 开启/关闭
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwds_i80lcd_switch_backlight(struct xwds_i80lcd * i80lcd, bool onoff);

/**
 * @brief 设置亮度
 * @param[in] i80lcd: 对象指针
 * @param[in] brightness: 亮度 (最大值：1000)
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwds_i80lcd_set_brightness(struct xwds_i80lcd * i80lcd, xwu16_t brightness);

/**
 * @brief 读取亮度
 * @param[in] i80lcd: 对象指针
 * @param[out] brightness: 指向缓冲区的指针，通过此缓冲区返回亮度
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwds_i80lcd_get_brightness(struct xwds_i80lcd * i80lcd, xwu16_t * brightness);

/**
 * @brief 设置窗口
 * @param[in] i80lcd: 对象指针
 * @param[in] x: 起点水平坐标
 * @param[in] y: 起点垂直坐标
 * @param[in] w: 宽度
 * @param[in] h: 高度
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwds_i80lcd_set_window(struct xwds_i80lcd * i80lcd,
                              xwu16_t x, xwu16_t y, xwu16_t w, xwu16_t h);

/**
 * @brief 设置点的颜色
 * @param[in] i80lcd: 对象指针
 * @param[in] x: 起点水平坐标
 * @param[in] y: 起点垂直坐标
 * @param[in] color: 颜色的RGB565值
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwds_i80lcd_draw_pixel(struct xwds_i80lcd * i80lcd,
                              xwu16_t x, xwu16_t y,
                              xwu16_t color);

/**
 * @brief 清除屏幕
 * @param[in] i80lcd: 对象指针
 * @param[in] x: 起点水平坐标
 * @param[in] y: 起点垂直坐标
 * @param[in] w: 宽
 * @param[in] h: 高
 * @param[in] color: 清屏的颜色值
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwds_i80lcd_clear(struct xwds_i80lcd * i80lcd,
                         xwu16_t x, xwu16_t y, xwu16_t w, xwu16_t h,
                         xwu16_t color);

/**
 * @brief 向LCD提交一帧数据
 * @param[in] i80lcd: 对象指针
 * @param[in] x: 起点水平坐标
 * @param[in] y: 起点垂直坐标
 * @param[in] w: 宽
 * @param[in] h: 高
 * @param[in] frame: 帧缓存地址
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @note
 * + 上下文：线程
 * + 为提高效率，减少不必要的内存拷贝，驱动框架不额外创建数据缓冲区拷贝frame数据，
 *   这就要求 `frame` 指针任何时候都不为野指针，因此帧缓存应该为全局变量。
 */
xwer_t xwds_i80lcd_commit(struct xwds_i80lcd * i80lcd,
                          xwu16_t x, xwu16_t y, xwu16_t w, xwu16_t h,
                          xwu16_t * frame, xwtm_t to);

/**
 * @brief 通过终端打印屏幕数据
 * @param[in] i80lcd: 对象指针
 * @param[in] x: 起点水平坐标
 * @param[in] y: 起点垂直坐标
 * @param[in] w: 宽
 * @param[in] h: 高
 * @param[in] num: 读取的像素点
 * @note
 * + 上下文：线程
 */
void xwds_i80lcd_dump_data(struct xwds_i80lcd * i80lcd,
                           xwu16_t x, xwu16_t y, xwu16_t w, xwu16_t h);

/**
 * @brief 通过终端打印屏幕状态寄存器
 * @param[in] i80lcd: 对象指针
 * @param[in] x: 点X轴坐标
 * @param[in] y: 点Y轴坐标
 * @param[in] num: 读取的像素点
 * @note
 * + 上下文：线程
 */
void xwds_i80lcd_dump_status(struct xwds_i80lcd * i80lcd);

/**
 * @} xwcd_peripheral_i80_lcd
 */

#endif /* xwcd/peripheral/i80/lcd/driver.h */
