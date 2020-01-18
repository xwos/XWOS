/**
 * @file
 * @brief 基于设备栈的EEPROM统一的驱动接口
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwcd/perpheral/ds/i2c/eeprom/common/driver.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** I2C EEPROM operations ******** ********/
/**
 * @brief XWBSP API：开启EEPROM的电源
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_power_on(struct xwds_i2cp_eeprom * eeprom)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_PWR_ON);
        return rc;
}

/**
 * @brief XWBSP API：关闭EEPROM的电源
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_power_off(struct xwds_i2cp_eeprom * eeprom)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_PWR_OFF);
        return rc;
}

/**
 * @brief XWBSP API：开启EEPROM的写保护
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_wp_enable(struct xwds_i2cp_eeprom * eeprom)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_WP_EN);
        return rc;
}

/**
 * @brief XWBSP API：关闭EEPROM的写保护
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_wp_disable(struct xwds_i2cp_eeprom * eeprom)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_WP_DS);
        return rc;
}

/**
 * @brief XWBSP API：写一个字节到EEPROM
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param data: (I) 数据
 * @param addr: (I) 地址
 * @param xwtm: (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_putc(struct xwds_i2cp_eeprom * eeprom,
                             xwu8_t data, xwptr_t addr,
                             xwtm_t * xwtm)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_PUTC,
                             data, addr, xwtm);
        return rc;
}

/**
 * @brief XWBSP API：从EEPROM中读取一个字节
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param data: (O) 指向返回数据的缓冲区的指针
 * @param addr: (I) 地址
 * @param xwtm: (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_getc(struct xwds_i2cp_eeprom * eeprom,
                             xwu8_t * buf, xwptr_t addr,
                             xwtm_t * xwtm)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_GETC,
                             buf, addr, xwtm);
        return rc;
}

/**
 * @brief XWBSP API：写一页数据到EEPROM
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param data: (I) 数据数组的指针
 * @param size: (I) 数据的大小
 * @param seq: (I) 页的序号
 * @param xwtm: (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_pgwrite(struct xwds_i2cp_eeprom * eeprom,
                                xwu8_t data[], xwsz_t size, xwsq_t seq,
                                xwtm_t * xwtm)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_PGWR,
                             data, size, seq, xwtm);
        return rc;
}

/**
 * @brief XWBSP API：从EEPROM读一页数据
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param buf: (O) 指向数组缓冲区的指针
 * @param size: (I) 缓冲区的大小
 *              (O) 实际读取的数据大小
 * @param seq: (I) 页的序号
 * @param xwtm: (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_pgread(struct xwds_i2cp_eeprom * eeprom,
                               xwu8_t buf[], xwsz_t * size, xwsq_t seq,
                               xwtm_t * xwtm)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_PGRD,
                             buf, size, seq, xwtm);
        return rc;
}

/**
 * @brief XWBSP API：复位I2C EEPROM
 * @param eeprom: (I) I2C EEPROM对象的指针
 * @param xwtm: (I) 期望的阻塞等待时间
 *              (O) 函数返回时，剩余的期望值
 * @retrun 错误码
 */
__xwbsp_api
xwer_t xwds_i2cp_eeprom_reset(struct xwds_i2cp_eeprom * eeprom, xwtm_t * xwtm)
{
        xwer_t rc;

        rc = xwds_i2cp_ioctl(&eeprom->i2cp, SODS_I2CP_EEPROM_IOC_RESET, xwtm);
        return rc;
}
