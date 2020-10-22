/**
 * @file
 * @brief I2C EEPROM 驱动
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

#ifndef __xwcd_perpheral_i2c_eeprom_driver_h__
#define __xwcd_perpheral_i2c_eeprom_driver_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/ds/i2c/perpheral.h>
#include <xwcd/perpheral/i2c/eeprom/device.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief EEPROM驱动函数表
 */
struct xwds_eeprom_driver {
        struct xwds_i2cp_driver i2cp; /**< C语言面向对象：继承struct xwds_i2cp_driver */
        xwer_t (* putc)(struct xwds_eeprom * /*eeprom*/,
                        xwu8_t /*data*/, xwptr_t /*addr*/,
                        xwtm_t * /*xwtm*/);
        xwer_t (* getc)(struct xwds_eeprom * /*eeprom*/,
                        xwu8_t * /*buf*/, xwptr_t /*addr*/,
                        xwtm_t * /*xwtm*/);
        xwer_t (* pgwrite)(struct xwds_eeprom * /*eeprom*/,
                           xwu8_t * /*data*/, xwsz_t * /*size*/, xwsq_t /*pgidx*/,
                           xwtm_t * /*xwtm*/);
        xwer_t (* pgread)(struct xwds_eeprom * /*eeprom*/,
                          xwu8_t * /*buf*/, xwsz_t * /*size*/, xwsq_t /*pgidx*/,
                          xwtm_t * /*xwtm*/);
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwds_eeprom_driver at24sd_drv;
const struct xwds_eeprom_driver at24md_drv;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** base driver ******** ********/
xwer_t xwds_eeprom_drv_start(struct xwds_device * dev);
xwer_t xwds_eeprom_drv_stop(struct xwds_device * dev);
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
xwer_t xwds_eeprom_drv_resume(struct xwds_device * dev);
xwer_t xwds_eeprom_drv_suspend(struct xwds_device * dev);
#endif /* XWMDCFG_ds_PM */

/******** ******** I2C EEPROM APIs ******** ********/
/**
 * @brief EEPROM API：开启EEPROM的电源
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @retrun 错误码
 */
xwer_t xwds_eeprom_power_on(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：关闭EEPROM的电源
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @retrun 错误码
 */
xwer_t xwds_eeprom_power_off(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：开启EEPROM的写保护
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @retrun 错误码
 */
xwer_t xwds_eeprom_wp_enable(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：关闭EEPROM的写保护
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @retrun 错误码
 */
xwer_t xwds_eeprom_wp_disable(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：写一个字节到EEPROM
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param data: (I) 数据
 * @param addr: (I) 地址
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_putc(struct xwds_eeprom * eeprom,
                        xwu8_t data, xwptr_t addr,
                        xwtm_t * xwtm);

/**
 * @brief EEPROM API：从EEPROM中读取一个字节
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param buf: (O) 指向缓冲区的指针，通过此缓冲区返回数据
 * @param addr: (I) 地址
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_getc(struct xwds_eeprom * eeprom,
                        xwu8_t * buf, xwptr_t addr,
                        xwtm_t * xwtm);

/**
 * @brief EEPROM API：写一页数据到EEPROM
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param data: (I) 待写入的数据的缓冲区
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，数据的大小
 *              (O) 作为输出时，实际写入的数据大小
 * @param pgidx: (I) 页的序号
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_pgwrite(struct xwds_eeprom * eeprom,
                           xwu8_t * data, xwsz_t * size, xwsq_t pgidx,
                           xwtm_t * xwtm);

/**
 * @brief EEPROM API：从EEPROM读一页数据
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param buf: (O) 指向缓冲区的指针，通过此缓冲区返回数据
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，缓冲区的大小
 *              (O) 作为输出时，实际读取的数据大小
 * @param pgidx: (I) 页的序号
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_pgread(struct xwds_eeprom * eeprom,
                          xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                          xwtm_t * xwtm);

/**
 * @brief EEPROM API：复位I2C EEPROM
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param xwtm: (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_reset(struct xwds_eeprom * eeprom, xwtm_t * xwtm);

#endif /* xwcd/perpheral/i2c/eeprom/driver.h */
