/**
 * @file
 * @brief I2C EEPROM 驱动
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

#ifndef __xwcd_peripheral_i2c_eeprom_driver_h__
#define __xwcd_peripheral_i2c_eeprom_driver_h__

#include <xwos/standard.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/peripheral/i2c/eeprom/device.h>

/**
 * @brief EEPROM驱动函数表
 */
struct xwds_eeprom_driver {
        struct xwds_i2cp_driver i2cp; /**< C语言面向对象：继承 `struct xwds_i2cp_driver` */
        xwer_t (* putc)(struct xwds_eeprom * /*eeprom*/,
                        xwu8_t /*data*/, xwptr_t /*addr*/,
                        xwtm_t /*to*/);
        xwer_t (* getc)(struct xwds_eeprom * /*eeprom*/,
                        xwu8_t * /*buf*/, xwptr_t /*addr*/,
                        xwtm_t /*to*/);
        xwer_t (* pgwrite)(struct xwds_eeprom * /*eeprom*/,
                           xwu8_t * /*data*/, xwsz_t * /*size*/, xwsq_t /*pgidx*/,
                           xwtm_t /*to*/);
        xwer_t (* pgread)(struct xwds_eeprom * /*eeprom*/,
                          xwu8_t * /*buf*/, xwsz_t * /*size*/, xwsq_t /*pgidx*/,
                          xwtm_t /*to*/);
};

extern const struct xwds_eeprom_driver at24sd_drv;
extern const struct xwds_eeprom_driver at24md_drv;

/******** ******** base driver ******** ********/
xwer_t xwds_eeprom_drv_start(struct xwds_device * dev);
xwer_t xwds_eeprom_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_eeprom_drv_resume(struct xwds_device * dev);
xwer_t xwds_eeprom_drv_suspend(struct xwds_device * dev);
#endif

/******** ******** I2C EEPROM APIs ******** ********/
/**
 * @brief EEPROM API：开启EEPROM的电源
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @retrun 错误码
 * @note
 * + 上下文：线程、中断底半部、线程
 */
xwer_t xwds_eeprom_power_on(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：关闭EEPROM的电源
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @retrun 错误码
 * @note
 * + 上下文：线程、中断底半部、线程
 */
xwer_t xwds_eeprom_power_off(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：开启EEPROM的写保护
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @retrun 错误码
 * @note
 * + 上下文：线程、中断底半部、线程
 */
xwer_t xwds_eeprom_wp_enable(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：关闭EEPROM的写保护
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @retrun 错误码
 * @note
 * + 上下文：线程、中断底半部、线程
 */
xwer_t xwds_eeprom_wp_disable(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：写一个字节到EEPROM
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[in] data: 数据
 * @param[in] addr: 地址
 * @param[in] to: 期望唤醒的时间点
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_eeprom_putc(struct xwds_eeprom * eeprom,
                        xwu8_t data, xwsq_t addr,
                        xwtm_t to);

/**
 * @brief EEPROM API：从EEPROM中读取一个字节
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in] addr: 地址
 * @param[in] to: 期望唤醒的时间点
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_eeprom_getc(struct xwds_eeprom * eeprom,
                        xwu8_t * buf, xwsq_t addr,
                        xwtm_t to);

/**
 * @brief EEPROM API：写一页数据到EEPROM
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[in] data: 待写入的数据的缓冲区
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据的大小
 * + (O) 作为输出时，返回实际写入的数据大小
 * @param[in] pgidx: 页的序号
 * @param[in] to: 期望唤醒的时间点
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_eeprom_pgwrite(struct xwds_eeprom * eeprom,
                           xwu8_t * data, xwsz_t * size, xwsq_t pgidx,
                           xwtm_t to);

/**
 * @brief EEPROM API：从EEPROM读一页数据
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据的大小
 * + (O) 作为输出时，返回实际写入的数据大小
 * @param[in] pgidx: 页的序号
 * @param[in] to: 期望唤醒的时间点
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_eeprom_pgread(struct xwds_eeprom * eeprom,
                          xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                          xwtm_t to);

/**
 * @brief EEPROM API：复位EEPROM的I2C总线
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[in] to: 期望唤醒的时间点
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_eeprom_reset(struct xwds_eeprom * eeprom, xwtm_t to);

#endif /* xwcd/peripheral/i2c/eeprom/driver.h */
