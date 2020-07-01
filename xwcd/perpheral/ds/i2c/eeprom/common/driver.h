/**
 * @file
 * @brief 基于设备栈的EEPROM统一的驱动接口
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

#ifndef __xwcd_perpheral_ds_i2c_eeprom_common_driver_h__
#define __xwcd_perpheral_ds_i2c_eeprom_common_driver_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/ds/i2c/perpheral.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWDS_I2CP_EEPROM_MAGIC          'e'

#define XWDS_I2CP_EEPROM_IOC_PWR_ON     __IOC(__IOC_CTRL, XWDS_I2CP_EEPROM_MAGIC, 3, 0)
#define XWDS_I2CP_EEPROM_IOC_PWR_OFF    __IOC(__IOC_CTRL, XWDS_I2CP_EEPROM_MAGIC, 4, 0)
#define XWDS_I2CP_EEPROM_IOC_WP_EN      __IOC(__IOC_CTRL, XWDS_I2CP_EEPROM_MAGIC, 5, 0)
#define XWDS_I2CP_EEPROM_IOC_WP_DS      __IOC(__IOC_CTRL, XWDS_I2CP_EEPROM_MAGIC, 6, 0)
#define XWDS_I2CP_EEPROM_IOC_PUTC       __IOC(__IOC_WRITE, XWDS_I2CP_EEPROM_MAGIC, 0, 1)
#define XWDS_I2CP_EEPROM_IOC_GETC       __IOC(__IOC_READ, XWDS_I2CP_EEPROM_MAGIC, 0, 1)
#define XWDS_I2CP_EEPROM_IOC_PGWR       __IOC(__IOC_WRITE, XWDS_I2CP_EEPROM_MAGIC, 1, 0)
#define XWDS_I2CP_EEPROM_IOC_PGRD       __IOC(__IOC_READ, XWDS_I2CP_EEPROM_MAGIC, 1, 0)
#define XWDS_I2CP_EEPROM_IOC_RESET      __IOC(__IOC_CTRL, XWDS_I2CP_EEPROM_MAGIC, 2, 0)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief EEPROM GPIO resource enumeration
 */
enum xwds_i2cp_eeprom_rsc_gpio_idx_em {
        XWDS_I2CP_EEPROM_RSC_GPIO_IDX_PWR = 0,
        XWDS_I2CP_EEPROM_RSC_GPIO_IDX_WP
};

struct xwds_i2cp_eeprom_cfg {
        xwsz_t page_size;
        xwsz_t total;
};

struct xwds_i2cp_eeprom {
        struct xwds_i2cp i2cp; /**< public: xwds_i2cp */
        const struct xwds_i2cp_eeprom_cfg * cfg; /**< 配置 */
        const struct xwds_resource_gpio * pwr_gpiorsc; /**< 电源开关GPIO */
        const struct xwds_resource_gpio * wp_gpiorsc; /**< 写保护GPIO */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** I2C EEPROM operations ******** ********/
__xwbsp_api
xwer_t xwds_i2cp_eeprom_power_on(struct xwds_i2cp_eeprom * eeprom);

__xwbsp_api
xwer_t xwds_i2cp_eeprom_power_off(struct xwds_i2cp_eeprom * eeprom);

__xwbsp_api
xwer_t xwds_i2cp_eeprom_wp_enable(struct xwds_i2cp_eeprom * eeprom);

__xwbsp_api
xwer_t xwds_i2cp_eeprom_wp_disable(struct xwds_i2cp_eeprom * eeprom);

__xwbsp_api
xwer_t xwds_i2cp_eeprom_putc(struct xwds_i2cp_eeprom * eeprom,
                             xwu8_t data, xwptr_t addr,
                             xwtm_t * xwtm);

__xwbsp_api
xwer_t xwds_i2cp_eeprom_getc(struct xwds_i2cp_eeprom * eeprom,
                             xwu8_t * buf, xwptr_t addr,
                             xwtm_t * xwtm);

__xwbsp_api
xwer_t xwds_i2cp_eeprom_pgwrite(struct xwds_i2cp_eeprom * eeprom,
                                xwu8_t data[], xwsz_t size, xwsq_t seq,
                                xwtm_t * xwtm);

__xwbsp_api
xwer_t xwds_i2cp_eeprom_pgread(struct xwds_i2cp_eeprom * eeprom,
                               xwu8_t buf[], xwsz_t * size, xwsq_t seq,
                               xwtm_t * xwtm);

__xwbsp_api
xwer_t xwds_i2cp_eeprom_reset(struct xwds_i2cp_eeprom * eeprom, xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwcd/perpheral/ds/i2c/eeprom/common/driver.h */
