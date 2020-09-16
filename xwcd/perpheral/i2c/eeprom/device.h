/**
 * @file
 * @brief I2C EEPROM Device
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

#ifndef __xwcd_perpheral_i2c_eeprom_device_h__
#define __xwcd_perpheral_i2c_eeprom_device_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/ds/i2c/perpheral.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum xwds_eeprom_rsc_gpio_idx_em {
        XWDS_EEPROM_RSC_GPIO_IDX_PWR = 0,
        XWDS_EEPROM_RSC_GPIO_IDX_WP
};

struct xwds_eeprom_cfg {
        xwsz_t page_size;
        xwsz_t total;
};

struct xwds_eeprom {
        struct xwds_i2cp i2cp; /**< public: xwds_i2cp */
        const struct xwds_eeprom_cfg * cfg; /**< 配置 */
        const struct xwds_resource_gpio * pwr_gpiorsc; /**< 电源开关GPIO */
        const struct xwds_resource_gpio * wp_gpiorsc; /**< 写保护GPIO */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param eeprom: (I) I2C EEPROM对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_grab()
 */
static __xwds_inline
xwer_t xwds_eeprom_grab(struct xwds_eeprom * eeprom)
{
        return xwds_i2cp_grab(&eeprom->i2cp);
}

/**
 * @brief 减少对象的引用计数
 * @param eeprom: (I) I2C EEPROM对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_put()
 */
static __xwds_inline
xwer_t xwds_eeprom_put(struct xwds_eeprom * eeprom)
{
        return xwds_i2cp_put(&eeprom->i2cp);
}

/**
 * @brief 增加设备运行状态计数器
 * @param eeprom: (I) I2C EEPROM对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_request()
 */
static __xwds_inline
xwer_t xwds_eeprom_request(struct xwds_eeprom * eeprom)
{
        return xwds_i2cp_request(&eeprom->i2cp);
}

/**
 * @brief 减少设备运行状态计数器
 * @param eeprom: (I) I2C EEPROM对象指针
 * @return 错误码
 * @retval @ref xwds_i2cp_release()
 */
static __xwds_inline
xwer_t xwds_eeprom_release(struct xwds_eeprom * eeprom)
{
        return xwds_i2cp_release(&eeprom->i2cp);
}

#endif /* xwcd/perpheral/i2c/eeprom/device.h */