/**
 * @file
 * @brief ATMEL小密度EEPROM(AT24C01/02/04/08/16)驱动
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
#include <string.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/i2c/perpheral.h>
#include <xwcd/perpheral/i2c/eeprom/device.h>
#include <xwcd/perpheral/i2c/eeprom/driver.h>

static __xwbsp_code
xwer_t at24sd_putc(struct xwds_eeprom * eeprom, xwu8_t data, xwptr_t addr,
                   xwtm_t * xwtm);

static __xwbsp_code
xwer_t at24sd_getc(struct xwds_eeprom * eeprom, xwu8_t * buf, xwptr_t addr,
                   xwtm_t * xwtm);

static __xwbsp_code
xwer_t at24sd_pgwrite(struct xwds_eeprom * eeprom, xwu8_t data[],
                      xwsz_t * size, xwsq_t pgidx, xwtm_t * xwtm);

static __xwbsp_code
xwer_t at24sd_pgread(struct xwds_eeprom * eeprom,
                     xwu8_t buf[], xwsz_t * size, xwsq_t pgidx,
                     xwtm_t * xwtm);


const struct xwds_eeprom_driver at24sd_drv = {
        .i2cp = {
                .base = {
                        .name = "xwds.eeprom",
                        .probe = NULL,
                        .remove = NULL,
                        .start = xwds_eeprom_drv_start,
                        .stop = xwds_eeprom_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                        .suspend = xwds_eeprom_drv_suspend,
                        .resume =  xwds_eeprom_drv_resume,
#endif
                },
        },
        .putc = at24sd_putc,
        .getc = at24sd_getc,
        .pgwrite = at24sd_pgwrite,
        .pgread = at24sd_pgread,
};

static
xwer_t at24sd_putc(struct xwds_eeprom * eeprom, xwu8_t data, xwptr_t addr,
                   xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu8_t txdata[2];
        xwu16_t i2cpaddr;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        txdata[0] = (xwu8_t)addr & 0xFF;
        txdata[1] = data;
        msg.addr = (xwu16_t)((i2cpaddr & 0x00F0) | (((addr >> 8) & 0x7) << 1));
        msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_WR | XWDS_I2C_F_STOP;
        msg.data = txdata;
        msg.size = sizeof(txdata);
        rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        return rc;
}

static
xwer_t at24sd_getc(struct xwds_eeprom * eeprom, xwu8_t * buf, xwptr_t addr,
                   xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu8_t txdata;
        xwu16_t i2cpaddr;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        txdata = (xwu8_t)(addr & 0xFF);

        msg.addr = (xwu16_t)((i2cpaddr & 0x00F0) | (((addr >> 8) & 0x7) << 1));
        msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_WR;
        msg.data = &txdata;
        msg.size = 1;
        rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        if (XWOK == rc) {
                msg.addr = (xwu16_t)((i2cpaddr & 0x00F0) |
                                     (((addr >> 8) & 0x7) << 1));
                msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_RD | XWDS_I2C_F_STOP;
                msg.data = buf;
                msg.size = 1;
                rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        }/* else {} */
        return rc;
}

static
xwer_t at24sd_pgwrite(struct xwds_eeprom * eeprom,
                      xwu8_t * data, xwsz_t * size, xwsq_t pgidx,
                      xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        const struct xwds_eeprom_parameter * parameter;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu16_t i2cpaddr;
        xwu16_t addr;
        xwsz_t wrsz = *size;
        xwu8_t txdata[wrsz + 1];

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        parameter = &eeprom->parameter;

        addr = (xwu16_t)((pgidx * parameter->page_size) & 0x0FFF);
        txdata[0] = (xwu8_t)(addr & 0xFF);
        memcpy(&txdata[1], data, wrsz);
        msg.addr = (xwu16_t)((i2cpaddr & 0x00F0) | (((addr >> 8) & 0x7) << 1));
        msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_WR | XWDS_I2C_F_STOP;
        msg.data = txdata;
        msg.size = wrsz + 1;
        rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        if (rc < 0) {
                *size = 0;
        }

        return rc;
}

static
xwer_t at24sd_pgread(struct xwds_eeprom * eeprom,
                     xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                     xwtm_t * xwtm)
{
        struct xwds_i2cm * bus;
        const struct xwds_eeprom_parameter * parameter;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu16_t i2cpaddr;
        xwu16_t addr;
        xwu8_t txdata;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        parameter = &eeprom->parameter;

        addr = (xwu16_t)((pgidx * parameter->page_size) & 0x0FFF);
        txdata = (xwu8_t)(addr & 0xFF);
        msg.addr = (xwu16_t)((i2cpaddr & 0x00F0) |
                             (((addr >> 8) & 0x7) << 1));
        msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_WR;
        msg.data = &txdata;
        msg.size = 1;
        rc = xwds_i2cm_xfer(bus, &msg, xwtm);
        if (XWOK == rc) {
                msg.addr = (xwu16_t)((i2cpaddr & 0x00F0) |
                                     (((addr >> 8) & 0x7) << 1));
                msg.flag = XWDS_I2C_F_START | XWDS_I2C_F_RD | XWDS_I2C_F_STOP;
                msg.data = buf;
                msg.size = *size;
                rc = xwds_i2cm_xfer(bus, &msg, xwtm);
                if (XWOK == rc) {
                        *size = msg.size;
                }/* else {} */
        }/* else {} */
        return rc;
}
