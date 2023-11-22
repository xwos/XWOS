/**
 * @file
 * @brief 玄武设备栈：I2C IO扩展芯片：TCA9539：驱动
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
 * - 所有API只可被单一线程访问。
 */

#ifndef __xwcd_peripheral_i2c_iochip_tca9539_driver_h__
#define __xwcd_peripheral_i2c_iochip_tca9539_driver_h__

#include <xwos/standard.h>
#include <xwcd/ds/soc/eirq.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/peripheral/i2c/iochip/tca9539/device.h>

/**
 * @ingroup xwcd_peripheral_i2c_iochip_tca9539
 * @{
 */

/**
 * @brief TCA9539基本驱动：启动设备
 */
xwer_t xwds_tca9539_drv_start(struct xwds_device * dev);

/**
 * @brief TCA9539基本驱动：停止设备
 */
xwer_t xwds_tca9539_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/**
 * @brief TCA9539基本驱动：暂停设备
 */
xwer_t xwds_tca9539_drv_suspend(struct xwds_device * dev);

/**
 * @brief TCA9539基本驱动：继续设备
 */
xwer_t xwds_tca9539_drv_resume(struct xwds_device * dev);
#endif

/**
 * @brief TCA9539 GPIO驱动：申请IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EBUSY: GPIO PIN已被使用
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_drv_gpio_req(struct xwds_iochip * iochip,
                                 xwid_t port, xwsq_t pinmask,
                                 xwtm_t to);

/**
 * @brief TCA9539 GPIO驱动：释放IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_drv_gpio_rls(struct xwds_iochip * iochip,
                                 xwid_t port, xwsq_t pinmask,
                                 xwtm_t to);

/**
 * @brief TCA9539 GPIO驱动：配置IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码
 * @param[in] cfg: GPIO配置，取值依据不同IOCHIP
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_drv_gpio_cfg(struct xwds_iochip * iochip,
                                 xwid_t port, xwsq_t pinmsk, void * cfg,
                                 xwtm_t to);

/**
 * @brief TCA9539 GPIO驱动：将IO扩展芯片的GPIO设置为高电平
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_drv_gpio_set(struct xwds_iochip * iochip,
                                 xwid_t port, xwsq_t pinmsk,
                                 xwtm_t to);

/**
 * @brief TCA9539 GPIO驱动：将IO扩展芯片的GPIO设置为低电平
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_drv_gpio_reset(struct xwds_iochip * iochip,
                                   xwid_t port, xwsq_t pinmsk,
                                   xwtm_t to);

/**
 * @brief TCA9539 GPIO驱动：翻转IO扩展芯片的GPIO电平
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_drv_gpio_toggle(struct xwds_iochip * iochip,
                                    xwid_t port, xwsq_t pinmsk,
                                    xwtm_t to);

/**
 * @brief TCA9539 GPIO驱动：并行输出多个IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码
 * @param[in] out: 输出值
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_drv_gpio_output(struct xwds_iochip * iochip,
                                    xwid_t port, xwsq_t pinmsk, xwsq_t out,
                                    xwtm_t to);

/**
 * @brief TCA9539 GPIO驱动：并行读取多个IO扩展芯片的GPIO
 * @param[in] iochip: IOCHIP对象指针
 * @param[in] port: GPIO端口
 * @param[in] pinmask: 引脚的掩码
 * @param[out] inbuf: 输入缓冲区
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: GPIO PORT错误
 * @retval -EPERM: GPIO PIN未被申请
 * @retval -ENOSYS: 不支持的API
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_drv_gpio_input(struct xwds_iochip * iochip,
                                   xwid_t port, xwsq_t pinmsk, xwsq_t * in,
                                   xwtm_t to);

/**
 * @brief TCA9539的驱动
 */
extern const struct xwds_iochip_driver xwds_tca9539_drv;

/******** ******** TCA9539 APIs ******** ********/
/**
 * @brief TCA9539 API：复位TCA9539
 * @param[in] tca9539: TCA9539对象的指针
 * @param[in] active: 是否使能复位
 * @note
 * + 上下文：线程、中断底半部、线程
 */
void xwds_tca9539_reset(struct xwds_tca9539 * tca9539, bool active);

/**
 * @brief TCA9539 API：启动TCA9539
 * @param[in] tca9539: I2C TCA9539对象的指针
 * @param[in] eisr: 中断处理函数
 * @param[in] to: 期望唤醒的时间点
 * @retrun 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_tca9539_run(struct xwds_tca9539 * tca9539, xwds_eirq_f eisr, xwtm_t to);

/**
 * @brief TCA9539 API：停止运行TCA9539
 * @param[in] tca9539: I2C TCA9539对象的指针
 * @param[in] to: 期望唤醒的时间点
 * @note
 * + 上下文：中断、中断底半部、线程
 */
void xwds_tca9539_quit(struct xwds_tca9539 * tca9539);

/**
 * @brief TCA9539 API：输出TCA9539的寄存器信息
 * @param[in] tca9539: I2C TCA9539对象的指针
 * @param[in] to: 期望唤醒的时间点
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
void xwds_tca9539_dump(struct xwds_tca9539 * tca9539, xwtm_t to);

/**
 * @} xwcd_peripheral_i2c_iochip_tca9539
 */

#endif /* xwcd/peripheral/i2c/iochip/tca9539/driver.h */
