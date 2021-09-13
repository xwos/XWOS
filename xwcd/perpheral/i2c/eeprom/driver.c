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

#include <xwos/standard.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/i2c/perpheral.h>
#include <xwcd/perpheral/i2c/eeprom/device.h>
#include <xwcd/perpheral/i2c/eeprom/driver.h>

/******** ******** base driver ******** ********/
static
xwer_t xwds_eeprom_check_desc(struct xwds_eeprom * eeprom)
{
        const struct xwds_eeprom_parameter * parameter;
        xwsz_t page_size;
        xwsz_t total;
        xwer_t rc;

        parameter = &eeprom->parameter;
        page_size = parameter->page_size;
        total = parameter->total;
        if (__xwcc_unlikely(total % page_size)) {
                rc = -EINVAL;
        } else {
                rc = XWOK;
        }
        return rc;
}

xwer_t xwds_eeprom_drv_start(struct xwds_device * dev)
{
        struct xwds_eeprom * eeprom;
        const struct xwds_resource_gpio * gpiorsc;
        xwer_t rc;

        eeprom = xwds_cast(struct xwds_eeprom *, dev);
        rc = xwds_eeprom_check_desc(eeprom);
        if (rc < 0) {
                goto err_chk_desc;
        }
        /* request GPIO resources */
        gpiorsc = eeprom->pwr_gpiorsc;
        if (NULL != gpiorsc) {
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (rc < 0) {
                        goto err_pwrgpio_req;
                }
        }
        gpiorsc = eeprom->wp_gpiorsc;
        if (NULL != gpiorsc) {
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (rc < 0) {
                        goto err_wpgpio_req;
                }
        }
        return XWOK;

err_wpgpio_req:
        gpiorsc = eeprom->pwr_gpiorsc;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
err_pwrgpio_req:
err_chk_desc:
        return rc;
}

xwer_t xwds_eeprom_drv_stop(struct xwds_device * dev)
{
        struct xwds_eeprom * eeprom;
        const struct xwds_resource_gpio * gpiorsc;

        eeprom = xwds_cast(struct xwds_eeprom *, dev);
        /* release GPIO resources */
        gpiorsc = eeprom->wp_gpiorsc;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
        gpiorsc = eeprom->pwr_gpiorsc;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }

        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_eeprom_drv_suspend(struct xwds_device * dev)
{
        return xwds_eeprom_drv_stop(dev);
}

xwer_t xwds_eeprom_drv_resume(struct xwds_device * dev)
{
        return xwds_eeprom_drv_start(dev);
}
#endif

/******** ******** I2C EEPROM operations ******** ********/
/**
 * @brief EEPROM API：开启EEPROM的电源
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @retrun 错误码
 */
xwer_t xwds_eeprom_power_on(struct xwds_eeprom * eeprom)
{
        xwer_t rc;

        if (eeprom->pwr_gpiorsc) {
                const struct xwds_resource_gpio * gpiorsc = eeprom->pwr_gpiorsc;
                rc = xwds_gpio_set(gpiorsc->soc,
                                   gpiorsc->port,
                                   gpiorsc->pinmask);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief EEPROM API：关闭EEPROM的电源
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @retrun 错误码
 */
xwer_t xwds_eeprom_power_off(struct xwds_eeprom * eeprom)
{
        xwer_t rc;

        if (eeprom->pwr_gpiorsc) {
                const struct xwds_resource_gpio * gpiorsc = eeprom->pwr_gpiorsc;
                rc = xwds_gpio_reset(gpiorsc->soc,
                                     gpiorsc->port,
                                     gpiorsc->pinmask);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief EEPROM API：开启EEPROM的写保护
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @retrun 错误码
 */
xwer_t xwds_eeprom_wp_enable(struct xwds_eeprom * eeprom)
{
        xwer_t rc;

        if (eeprom->wp_gpiorsc) {
                const struct xwds_resource_gpio * gpiorsc = eeprom->wp_gpiorsc;
                rc = xwds_gpio_set(gpiorsc->soc,
                                   gpiorsc->port,
                                   gpiorsc->pinmask);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief EEPROM API：关闭EEPROM的写保护
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @retrun 错误码
 */
xwer_t xwds_eeprom_wp_disable(struct xwds_eeprom * eeprom)
{
        xwer_t rc;

        if (eeprom->wp_gpiorsc) {
                const struct xwds_resource_gpio * gpiorsc = eeprom->wp_gpiorsc;
                rc = xwds_gpio_reset(gpiorsc->soc,
                                     gpiorsc->port,
                                     gpiorsc->pinmask);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief EEPROM API：写一个字节到EEPROM
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[in] data: 数据
 * @param[in] addr: 地址
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_putc(struct xwds_eeprom * eeprom,
                        xwu8_t data, xwptr_t addr,
                        xwtm_t * xwtm)
{
        const struct xwds_eeprom_driver * drv;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_eeprom_driver *, eeprom->i2cp.dev.drv);
        if ((drv) && (drv->putc)) {
                rc = drv->putc(eeprom, data, addr, xwtm);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief EEPROM API：从EEPROM中读取一个字节
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in] addr: 地址
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_getc(struct xwds_eeprom * eeprom,
                        xwu8_t * buf, xwptr_t addr,
                        xwtm_t * xwtm)
{
        const struct xwds_eeprom_driver * drv;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_eeprom_driver *, eeprom->i2cp.dev.drv);
        if ((drv) && (drv->getc)) {
                rc = drv->getc(eeprom, buf, addr, xwtm);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief EEPROM API：写一页数据到EEPROM
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[in] data: 待写入的数据的缓冲区
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据的大小
 * + (O) 作为输出时，返回实际写入的数据大小
 * @param[in] pgidx: 页的序号
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_pgwrite(struct xwds_eeprom * eeprom,
                           xwu8_t * data, xwsz_t * size, xwsq_t pgidx,
                           xwtm_t * xwtm)
{
        const struct xwds_eeprom_driver * drv;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_eeprom_driver *, eeprom->i2cp.dev.drv);
        if ((drv) && (drv->pgwrite)) {
                rc = drv->pgwrite(eeprom, data, size, pgidx, xwtm);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief EEPROM API：从EEPROM读一页数据
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据的大小
 * + (O) 作为输出时，返回实际写入的数据大小
 * @param[in] pgidx: 页的序号
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_pgread(struct xwds_eeprom * eeprom,
                          xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                          xwtm_t * xwtm)
{
        const struct xwds_eeprom_driver * drv;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_eeprom_driver *, eeprom->i2cp.dev.drv);
        if ((drv) && (drv->pgread)) {
                rc = drv->pgread(eeprom, buf, size, pgidx, xwtm);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief EEPROM API：复位I2C EEPROM
 * @param[in] eeprom: I2C EEPROM对象的指针
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @retrun 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 */
xwer_t xwds_eeprom_reset(struct xwds_eeprom * eeprom, xwtm_t * xwtm)
{
        struct xwds_i2c_msg msg;
        xwu8_t dummy;
        xwer_t rc;

        dummy = 0xFF;
        msg.addr = 0xFE;
        msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_WR;
        msg.data = &dummy;
        msg.size = 1;
        rc = xwds_i2cm_xfer(eeprom->i2cp.bus, &msg, xwtm);
        if (-EADDRNOTAVAIL == rc) {
                msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_WR | XWDS_I2C_F_STOP;
                msg.data = NULL;
                msg.size = 0;
                rc = xwds_i2cm_xfer(eeprom->i2cp.bus, &msg, xwtm);
                if (-EADDRNOTAVAIL == rc) {
                        rc = XWOK;
                }
        }
        return rc;
}
