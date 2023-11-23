/**
 * @file
 * @brief I2C EEPROM 设备
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
 */

#ifndef __xwcd_peripheral_i2c_eeprom_device_h__
#define __xwcd_peripheral_i2c_eeprom_device_h__

#include <xwos/standard.h>
#include <xwcd/ds/i2c/peripheral.h>

/**
 * @defgroup xwcd_peripheral_i2c_eeprom I2C EEPROM
 * @ingroup xwcd_peripheral_i2c
 * @{
 */

struct xwds_eeprom_parameter {
        xwsz_t page_size;
        xwsz_t total;
};

struct xwds_eeprom {
        struct xwds_i2cp i2cp; /**< C语言面向对象：继承 `struct xwds_i2cp` */
        const struct xwds_eeprom_parameter parameter; /**< 参数 */
        const struct xwds_resource_gpio * pwr_gpiorsc; /**< 电源开关GPIO */
        const struct xwds_resource_gpio * wp_gpiorsc; /**< 写保护GPIO */
};

void xwds_eeprom_construct(struct xwds_eeprom * eeprom);
void xwds_eeprom_destruct(struct xwds_eeprom * eeprom);

/**
 * @brief EEPROM API：增加对象的引用计数
 * @param[in] eeprom: I2C EEPROM对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_grab()
 */
static __xwds_inline
xwer_t xwds_eeprom_grab(struct xwds_eeprom * eeprom)
{
        return xwds_i2cp_grab(&eeprom->i2cp);
}

/**
 * @brief EEPROM API：减少对象的引用计数
 * @param[in] eeprom: I2C EEPROM对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_put()
 */
static __xwds_inline
xwer_t xwds_eeprom_put(struct xwds_eeprom * eeprom)
{
        return xwds_i2cp_put(&eeprom->i2cp);
}

/**
 * @} xwcd_peripheral_i2c_eeprom
 */

#endif /* xwcd/peripheral/i2c/eeprom/device.h */
