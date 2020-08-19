/**
 * @file
 * @brief ATMEL中等密度EEPROM(AT24C32/64)驱动
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <string.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/i2c/perpheral.h>
#include <xwcd/perpheral/ds/i2c/eeprom/common/driver.h>
#include <xwcd/perpheral/ds/i2c/eeprom/chip/at24mdd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t at24mdd_check_desc(struct xwds_i2cp_eeprom * eeprom);

static __xwbsp_code
xwer_t at24mdd_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t at24mdd_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t at24mdd_drv_stop(struct xwds_device * dev);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwbsp_code
xwer_t at24mdd_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t at24mdd_drv_resume(struct xwds_device * dev);
#endif /* XWMDCFG_ds_PM */

static __xwbsp_code
xwer_t at24mdd_putc(struct xwds_i2cp_eeprom * eeprom, xwu8_t data, xwptr_t addr,
                    xwtm_t * xwtm);

static __xwbsp_code
xwer_t at24mdd_getc(struct xwds_i2cp_eeprom * eeprom, xwu8_t * buf, xwptr_t addr,
                    xwtm_t * xwtm);

static __xwbsp_code
xwer_t at24mdd_pgwrite(struct xwds_i2cp_eeprom * eeprom, xwu8_t data[],
                       xwsz_t size, xwsq_t seq, xwtm_t * xwtm);

static __xwbsp_code
xwer_t at24mdd_pgread(struct xwds_i2cp_eeprom * eeprom,
                      xwu8_t buf[], xwsz_t * size, xwsq_t seq,
                      xwtm_t * xwtm);

static __xwbsp_code
xwer_t at24mdd_reset(struct xwds_i2cp_eeprom * eeprom, xwtm_t * xwtm);

static __xwbsp_code
xwer_t at24mdd_drv_ioctl(struct xwds_i2cp * i2cp, xwsq_t cmd, va_list args);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct xwds_i2cp_driver at24mdd_drv = {
        .base = {
                .name = "at24mdd",
                .probe = at24mdd_drv_probe,
                .remove = NULL,
                .start = at24mdd_drv_start,
                .stop = at24mdd_drv_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
                .suspend = at24mdd_drv_suspend,
                .resume =  at24mdd_drv_resume,
#endif /* XWMDCFG_ds_PM */
        },
        .ioctl = at24mdd_drv_ioctl,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t at24mdd_check_desc(struct xwds_i2cp_eeprom * eeprom)
{
        const struct xwds_resources * resources;
        const struct xwds_i2cp_eeprom_cfg * cfg;
        xwsz_t page_size;
        xwsz_t total;
        xwer_t rc;

        resources = eeprom->i2cp.dev.resources;
        cfg = eeprom->cfg;
        if (__xwcc_unlikely(is_err_or_null(resources))) {
                rc = -EINVAL;
        } else if (__xwcc_unlikely(is_err_or_null(cfg))) {
                rc = -EINVAL;
        } else {
                page_size = cfg->page_size;
                total = cfg->total;
                if (__xwcc_unlikely(total % page_size)) {
                        rc = -EINVAL;
                } else {
                        rc = XWOK;
                }
        }
        return rc;
}

/******** ******** base driver ******** ********/
static __xwbsp_code
xwer_t at24mdd_drv_probe(struct xwds_device * dev)
{
        struct xwds_i2cp * i2cp;
        struct xwds_i2cp_eeprom * eeprom;
        xwer_t rc;

        i2cp = xwds_static_cast(struct xwds_i2cp *, dev);
        eeprom = xwds_static_cast(struct xwds_i2cp_eeprom *, i2cp);
        rc = at24mdd_check_desc(eeprom);
        return rc;
}

static __xwbsp_code
xwer_t at24mdd_drv_start(struct xwds_device * dev)
{
        struct xwds_i2cp * i2cp;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t i, j;
        xwer_t rc;

        i2cp = xwds_static_cast(struct xwds_i2cp *, dev);
        resources = i2cp->dev.resources;

        /* request GPIO resources */
        for (i = 0; i < (xwssz_t)resources->gpiorsc_num; i++) {
                gpiorsc = &resources->gpiorsc_array[i];
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__xwcc_unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                gpiorsc = &resources->gpiorsc_array[j];
                                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port,
                                              gpiorsc->pinmask);
                        }
                        goto err_req_gpios;
                }
        }
        return XWOK;

err_req_gpios:
        return rc;
}

static __xwbsp_code
xwer_t at24mdd_drv_stop(struct xwds_device * dev)
{
        struct xwds_i2cp * i2cp;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t j;
        xwer_t rc;

        i2cp = xwds_static_cast(struct xwds_i2cp *, dev);
        resources = i2cp->dev.resources;

        /* release GPIO resources */
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                rc = xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_gpio_release;
                }
        }
        return XWOK;

err_gpio_release:
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwbsp_code
xwer_t at24mdd_drv_suspend(struct xwds_device * dev)
{
        return at24mdd_drv_stop(dev);
}

static __xwbsp_code
xwer_t at24mdd_drv_resume(struct xwds_device * dev)
{
        return at24mdd_drv_start(dev);
}
#endif /* XWMDCFG_ds_PM */

/******** ******** AT24MDD driver ******** ********/
static __xwbsp_code
xwer_t at24mdd_power_on(struct xwds_i2cp_eeprom * eeprom)
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

static __xwbsp_code
xwer_t at24mdd_power_off(struct xwds_i2cp_eeprom * eeprom)
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

static __xwbsp_code
xwer_t at24mdd_wp_enable(struct xwds_i2cp_eeprom * eeprom)
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

static __xwbsp_code
xwer_t at24mdd_wp_disable(struct xwds_i2cp_eeprom * eeprom)
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

static __xwbsp_code
xwer_t at24mdd_putc(struct xwds_i2cp_eeprom * eeprom, xwu8_t data, xwptr_t addr,
                    xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu8_t txdata[3];
        xwu16_t i2cpaddr;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        txdata[0] = (xwu8_t)((addr >> 8) & 0x1F);
        txdata[1] = (xwu8_t)(addr & 0xFF);
        txdata[2] = data;
        msg.addr = (xwu16_t)(i2cpaddr & 0x00FE);
        msg.flags = XWDS_I2C_F_STOP;
        msg.buf = txdata;
        msg.size = sizeof(txdata);
        rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        return rc;
}

static __xwbsp_code
xwer_t at24mdd_getc(struct xwds_i2cp_eeprom * eeprom, xwu8_t * buf, xwptr_t addr,
                    xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu8_t txdata[2];
        xwu16_t i2cpaddr;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        txdata[0] = (xwu8_t)((addr >> 8) & 0x1F);
        txdata[1] = (xwu8_t)(addr & 0xFF);
        msg.addr = (xwu16_t)(i2cpaddr & 0x00FE);
        msg.flags = XWDS_I2C_F_NONE;
        msg.buf = txdata;
        msg.size = 2;
        rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        if (XWOK == rc) {
                msg.addr = (xwu16_t)(i2cpaddr & 0x00FE);
                msg.flags = XWDS_I2C_F_RD | XWDS_I2C_F_STOP;
                msg.buf = buf;
                msg.size = 1;
                rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        }/* else {} */
        return rc;
}

static __xwbsp_code
xwer_t at24mdd_pgwrite(struct xwds_i2cp_eeprom * eeprom,
                       xwu8_t data[], xwsz_t size, xwsq_t seq,
                       xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        const struct xwds_i2cp_eeprom_cfg * cfg;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu16_t i2cpaddr;
        xwu16_t addr;
        xwu8_t txdata[size + 2];

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        cfg = eeprom->cfg;

        addr = (xwu16_t)((seq * cfg->page_size) & 0x1FFF);
        txdata[0] = (xwu8_t)(addr >> 8);
        txdata[1] = (xwu8_t)(addr & 0xFF);
        memcpy(&txdata[2], data, size);
        msg.addr = (xwu16_t)(i2cpaddr & 0x00FE);
        msg.flags = XWDS_I2C_F_STOP;
        msg.buf = txdata;
        msg.size = size + 2;
        rc = xwds_i2cm_xfer(bus, &msg, xwtm);

        return rc;
}

static __xwbsp_code
xwer_t at24mdd_pgread(struct xwds_i2cp_eeprom * eeprom,
                      xwu8_t buf[], xwsz_t * size, xwsq_t seq,
                      xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        const struct xwds_i2cp_eeprom_cfg * cfg;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu16_t i2cpaddr;
        xwu16_t addr;
        xwu8_t txdata[2];

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        cfg = eeprom->cfg;

        addr = (xwu16_t)((seq * cfg->page_size) & 0x1FFF);
        txdata[0] = (xwu8_t)(addr >> 8);
        txdata[1] = (xwu8_t)(addr & 0xFF);
        msg.addr = (xwu16_t)(i2cpaddr & 0x00FE);
        msg.flags = XWDS_I2C_F_NONE;
        msg.buf = txdata;
        msg.size = 2;
        rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        if (XWOK == rc) {
                msg.addr = (xwu16_t)(i2cpaddr & 0x00FE);
                msg.flags = XWDS_I2C_F_RD | XWDS_I2C_F_STOP;
                msg.buf = buf;
                msg.size = *size;
                rc = xwds_i2cm_xfer(bus, &msg, xwtm);
                if (XWOK == rc) {
                        *size = msg.size;
                }/* else {} */
        }/* else {} */
        return rc;
}

static __xwbsp_code
xwer_t at24mdd_reset(struct xwds_i2cp_eeprom * eeprom, xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        xwer_t rc;

        bus = eeprom->i2cp.bus;
        rc = xwds_i2cm_xfer(bus, NULL, xwtm);
        return rc;
}

static __xwbsp_code
xwer_t at24mdd_drv_ioctl(struct xwds_i2cp * i2cp, xwsq_t cmd, va_list args)
{
        struct xwds_i2cp_eeprom * eeprom;
        xwer_t rc;
        xwu8_t byte;
        xwu8_t * databuf;
        xwptr_t addr;
        xwsz_t wrsz;
        xwsz_t * rdsz;
        xwsq_t seq;
        xwtm_t * xwtm;

        eeprom = xwds_static_cast(struct xwds_i2cp_eeprom *, i2cp);
        switch (cmd) {
        case XWDS_I2CP_EEPROM_IOC_PWR_ON:
                rc = at24mdd_power_on(eeprom);
                break;
        case XWDS_I2CP_EEPROM_IOC_PWR_OFF:
                rc = at24mdd_power_off(eeprom);
                break;
        case XWDS_I2CP_EEPROM_IOC_WP_EN:
                rc = at24mdd_wp_enable(eeprom);
                break;
        case XWDS_I2CP_EEPROM_IOC_WP_DS:
                rc = at24mdd_wp_disable(eeprom);
                break;
        case XWDS_I2CP_EEPROM_IOC_PUTC:
                byte = (xwu8_t)va_arg(args, int);
                addr = va_arg(args, xwptr_t);
                xwtm = va_arg(args, xwtm_t *);
                rc = at24mdd_putc(eeprom, byte, addr, xwtm);
                break;
        case XWDS_I2CP_EEPROM_IOC_GETC:
                databuf = va_arg(args, xwu8_t *);
                addr = va_arg(args, xwptr_t);
                xwtm = va_arg(args, xwtm_t *);
                rc = at24mdd_getc(eeprom, databuf, addr, xwtm);
                break;
        case XWDS_I2CP_EEPROM_IOC_PGWR:
                databuf = va_arg(args, xwu8_t *);
                wrsz = va_arg(args, xwsz_t);
                seq = va_arg(args, xwsq_t);
                xwtm = va_arg(args, xwtm_t *);
                rc = at24mdd_pgwrite(eeprom, databuf, wrsz, seq, xwtm);
                break;
        case XWDS_I2CP_EEPROM_IOC_PGRD:
                databuf = va_arg(args, xwu8_t *);
                rdsz = va_arg(args, xwsz_t *);
                seq = va_arg(args, xwsq_t);
                xwtm = va_arg(args, xwtm_t *);
                rc = at24mdd_pgread(eeprom, databuf, rdsz, seq, xwtm);
                break;
        case XWDS_I2CP_EEPROM_IOC_RESET:
                xwtm = va_arg(args, xwtm_t *);
                rc = at24mdd_reset(eeprom, xwtm);
                break;
        default:
                rc = -ENOSYS;
                break;
        }

        return rc;
}
