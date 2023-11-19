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
 */

#include <xwos/standard.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/peripheral/i2c/eeprom/device.h>
#include <xwcd/peripheral/i2c/eeprom/driver.h>

/******** ******** base driver ******** ********/
static __xwbsp_code
xwer_t xwds_eeprom_check_desc(struct xwds_eeprom * eeprom)
{
        const struct xwds_eeprom_parameter * parameter;
        xwsz_t page_size;
        xwsz_t total;
        xwer_t rc;

        parameter = &eeprom->parameter;
        page_size = parameter->page_size;
        total = parameter->total;
        if (total % page_size) {
                rc = -EINVAL;
        } else {
                rc = XWOK;
        }
        return rc;
}

__xwbsp_code
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

__xwbsp_code
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
__xwbsp_code
xwer_t xwds_eeprom_drv_suspend(struct xwds_device * dev)
{
        return xwds_eeprom_drv_stop(dev);
}

__xwbsp_code
xwer_t xwds_eeprom_drv_resume(struct xwds_device * dev)
{
        return xwds_eeprom_drv_start(dev);
}
#endif

/******** ******** I2C EEPROM APIs ******** ********/
__xwbsp_api
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

__xwbsp_api
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

__xwbsp_api
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

__xwbsp_api
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

__xwbsp_api
xwer_t xwds_eeprom_putc(struct xwds_eeprom * eeprom,
                        xwu8_t data, xwsq_t addr,
                        xwtm_t to)
{
        const struct xwds_eeprom_driver * drv;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_eeprom_driver *, eeprom->i2cp.dev.drv);
        if ((drv) && (drv->putc)) {
                rc = drv->putc(eeprom, data, addr, to);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwbsp_api
xwer_t xwds_eeprom_getc(struct xwds_eeprom * eeprom,
                        xwu8_t * buf, xwsq_t addr,
                        xwtm_t to)
{
        const struct xwds_eeprom_driver * drv;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_eeprom_driver *, eeprom->i2cp.dev.drv);
        if ((drv) && (drv->getc)) {
                rc = drv->getc(eeprom, buf, addr, to);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwbsp_api
xwer_t xwds_eeprom_pgwrite(struct xwds_eeprom * eeprom,
                           xwu8_t * data, xwsz_t * size, xwsq_t pgidx,
                           xwtm_t to)
{
        const struct xwds_eeprom_driver * drv;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_eeprom_driver *, eeprom->i2cp.dev.drv);
        if ((drv) && (drv->pgwrite)) {
                rc = drv->pgwrite(eeprom, data, size, pgidx, to);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwbsp_api
xwer_t xwds_eeprom_pgread(struct xwds_eeprom * eeprom,
                          xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                          xwtm_t to)
{
        const struct xwds_eeprom_driver * drv;
        xwer_t rc;

        drv = xwds_cast(const struct xwds_eeprom_driver *, eeprom->i2cp.dev.drv);
        if ((drv) && (drv->pgread)) {
                rc = drv->pgread(eeprom, buf, size, pgidx, to);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwbsp_api
xwer_t xwds_eeprom_reset(struct xwds_eeprom * eeprom, xwtm_t to)
{
        struct xwds_i2c_msg msg;
        xwu8_t dummy;
        xwer_t rc;

        dummy = 0xFF;
        msg.addr = 0xFE;
        msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_WR;
        msg.data = &dummy;
        msg.size = 1;
        rc = xwds_i2cm_xfer(eeprom->i2cp.bus, &msg, to);
        if (-EADDRNOTAVAIL == rc) {
                msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_WR | XWDS_I2C_F_STOP;
                msg.data = NULL;
                msg.size = 0;
                rc = xwds_i2cm_xfer(eeprom->i2cp.bus, &msg, to);
                if (-EADDRNOTAVAIL == rc) {
                        rc = XWOK;
                }
        }
        return rc;
}
