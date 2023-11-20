/**
 * @file
 * @brief 中等密度(32Ki/64Ki/128Ki/256Ki/512Ki/1Mi/2Mi bits)EEPROM驱动
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
#include <string.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/peripheral/i2c/eeprom/device.h>
#include <xwcd/peripheral/i2c/eeprom/driver.h>

static
xwer_t at24md_putc(struct xwds_eeprom * eeprom, xwu8_t data, xwsq_t addr, xwtm_t to);

static
xwer_t at24md_getc(struct xwds_eeprom * eeprom, xwu8_t * buf, xwsq_t addr, xwtm_t to);

static
xwer_t at24md_pgwrite(struct xwds_eeprom * eeprom,
                      xwu8_t * data, xwsz_t * size, xwsq_t pgidx,
                      xwtm_t to);

static
xwer_t at24md_pgread(struct xwds_eeprom * eeprom,
                     xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                     xwtm_t to);

const struct xwds_eeprom_driver at24md_drv = {
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
        .putc = at24md_putc,
        .getc = at24md_getc,
        .pgwrite = at24md_pgwrite,
        .pgread = at24md_pgread,
};

static
xwer_t at24md_putc(struct xwds_eeprom * eeprom, xwu8_t data, xwsq_t addr, xwtm_t to)
{
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu8_t txdata[3];
        xwu16_t i2cpaddr;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        txdata[0] = (xwu8_t)((addr >> (xwsq_t)8) & (xwu8_t)0xFF);
        txdata[1] = (xwu8_t)(addr & (xwu8_t)0xFF);
        txdata[2] = data;
        msg.addr = ((i2cpaddr & (xwu16_t)0x00F0) |
                    (((addr >> (xwsq_t)16) & (xwu16_t)0x7) << (xwu16_t)1));
        msg.flag = ((xwu16_t)XWDS_I2C_F_START |
                    (xwu16_t)XWDS_I2C_F_WR |
                    (xwu16_t)XWDS_I2C_F_STOP);
        msg.data = txdata;
        msg.size = sizeof(txdata);
        rc = xwds_i2cm_xfer(bus, &msg, to);
        return rc;
}

static
xwer_t at24md_getc(struct xwds_eeprom * eeprom, xwu8_t * buf, xwsq_t addr, xwtm_t to)
{
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu8_t txdata[2];
        xwu16_t i2cpaddr;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        txdata[0] = (xwu8_t)((addr >> (xwsq_t)8) & (xwu8_t)0xFF);
        txdata[1] = (xwu8_t)(addr & (xwu8_t)0xFF);
        msg.addr = ((i2cpaddr & (xwu16_t)0x00F0) |
                    (((addr >> (xwsq_t)16) & (xwu16_t)0x7) << (xwu16_t)1));
        msg.flag = ((xwu16_t)XWDS_I2C_F_START | (xwu16_t)XWDS_I2C_F_WR);
        msg.data = txdata;
        msg.size = 2;
        rc = xwds_i2cm_xfer(bus, &msg, to);
        if (XWOK == rc) {
                msg.addr = ((i2cpaddr & (xwu16_t)0x00F0) |
                            (((addr >> (xwsq_t)16) & (xwu16_t)0x7) << (xwu16_t)1));
                msg.flag = ((xwu16_t)XWDS_I2C_F_START |
                            (xwu16_t)XWDS_I2C_F_RD |
                            (xwu16_t)XWDS_I2C_F_STOP);
                msg.data = buf;
                msg.size = 1;
                rc = xwds_i2cm_xfer(bus, &msg, to);
        }
        return rc;
}

static
xwer_t at24md_pgwrite(struct xwds_eeprom * eeprom,
                      xwu8_t * data, xwsz_t * size, xwsq_t pgidx,
                      xwtm_t to)
{
        struct xwds_i2cm * bus;
        const struct xwds_eeprom_parameter * parameter;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu16_t i2cpaddr;
        xwsq_t addr;
        xwsz_t wrsz = *size;
        xwu8_t txdata[wrsz + 2];

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        parameter = &eeprom->parameter;
        addr = ((pgidx * parameter->page_size) & (xwsq_t)0x7FFFF);
        txdata[0] = (xwu8_t)((addr >> (xwsq_t)8) & (xwu8_t)0xFF);
        txdata[1] = (xwu8_t)(addr & (xwu8_t)0xFF);
        memcpy(&txdata[2], data, wrsz);
        msg.addr = ((i2cpaddr & (xwu16_t)0x00F0) |
                    (((addr >> (xwsq_t)16) & (xwu16_t)0x7) << (xwu16_t)1));
        msg.flag = ((xwu16_t)XWDS_I2C_F_START |
                    (xwu16_t)XWDS_I2C_F_WR |
                    (xwu16_t)XWDS_I2C_F_STOP);
        msg.data = txdata;
        msg.size = wrsz + 2;
        rc = xwds_i2cm_xfer(bus, &msg, to);
        if (rc < 0) {
                *size = 0;
        }
        return rc;
}

static
xwer_t at24md_pgread(struct xwds_eeprom * eeprom,
                     xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                     xwtm_t to)
{
        struct xwds_i2cm * bus;
        const struct xwds_eeprom_parameter * parameter;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu16_t i2cpaddr;
        xwsq_t addr;
        xwsz_t rdsz = *size;
        xwu8_t txdata[2];

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        parameter = &eeprom->parameter;
        addr = ((pgidx * parameter->page_size) & (xwsq_t)0x7FFFF);
        txdata[0] = (xwu8_t)((addr >> (xwsq_t)8) & (xwu8_t)0xFF);
        txdata[1] = (xwu8_t)(addr & (xwu8_t)0xFF);
        msg.addr = ((i2cpaddr & (xwu16_t)0x00F0) |
                    (((addr >> (xwsq_t)16) & (xwu16_t)0x7) << (xwu16_t)1));
        msg.flag = ((xwu16_t)XWDS_I2C_F_START | (xwu16_t)XWDS_I2C_F_WR);
        msg.data = txdata;
        msg.size = 2;
        rc = xwds_i2cm_xfer(bus, &msg, to);
        if (XWOK == rc) {
                msg.addr = ((i2cpaddr & (xwu16_t)0x00F0) |
                            (((addr >> (xwsq_t)16) & (xwu16_t)0x7) << (xwu16_t)1));
                msg.flag = ((xwu16_t)XWDS_I2C_F_START |
                            (xwu16_t)XWDS_I2C_F_RD |
                            (xwu16_t)XWDS_I2C_F_STOP);
                msg.data = buf;
                msg.size = rdsz;
                rc = xwds_i2cm_xfer(bus, &msg, to);
                if (XWOK == rc) {
                        *size = msg.size;
                } else {
                        *size = 0;
                }
        } else {
                *size = 0;
        }
        return rc;
}
