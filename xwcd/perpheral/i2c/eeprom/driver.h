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
 * @note
 * - 所有API只可被单一线程访问。
 */

#ifndef __xwcd_perpheral_i2c_eeprom_driver_h__
#define __xwcd_perpheral_i2c_eeprom_driver_h__

#include <xwos/standard.h>
#include <xwcd/ds/i2c/perpheral.h>
#include <xwcd/perpheral/i2c/eeprom/device.h>

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
xwer_t xwds_eeprom_power_on(struct xwds_eeprom * eeprom);
xwer_t xwds_eeprom_power_off(struct xwds_eeprom * eeprom);
xwer_t xwds_eeprom_wp_enable(struct xwds_eeprom * eeprom);
xwer_t xwds_eeprom_wp_disable(struct xwds_eeprom * eeprom);
xwer_t xwds_eeprom_putc(struct xwds_eeprom * eeprom,
                        xwu8_t data, xwptr_t addr,
                        xwtm_t * xwtm);
xwer_t xwds_eeprom_getc(struct xwds_eeprom * eeprom,
                        xwu8_t * buf, xwptr_t addr,
                        xwtm_t * xwtm);
xwer_t xwds_eeprom_pgwrite(struct xwds_eeprom * eeprom,
                           xwu8_t * data, xwsz_t * size, xwsq_t pgidx,
                           xwtm_t * xwtm);
xwer_t xwds_eeprom_pgread(struct xwds_eeprom * eeprom,
                          xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                          xwtm_t * xwtm);
xwer_t xwds_eeprom_reset(struct xwds_eeprom * eeprom, xwtm_t * xwtm);

#endif /* xwcd/perpheral/i2c/eeprom/driver.h */
