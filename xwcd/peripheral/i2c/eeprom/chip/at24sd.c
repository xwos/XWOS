/**
 * @file
 * @brief 小密度(1Ki/2Ki/4Ki/8Ki/16Ki bits)EEPROM驱动
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

static __xwbsp_code
xwer_t at24sd_putc(struct xwds_eeprom * eeprom, xwu8_t data, xwsq_t addr, xwtm_t to);

static __xwbsp_code
xwer_t at24sd_getc(struct xwds_eeprom * eeprom, xwu8_t * buf, xwsq_t addr, xwtm_t to);

static __xwbsp_code
xwer_t at24sd_pgwrite(struct xwds_eeprom * eeprom, xwu8_t data[],
                      xwsz_t * size, xwsq_t pgidx,
                      xwtm_t to);

static __xwbsp_code
xwer_t at24sd_pgread(struct xwds_eeprom * eeprom,
                     xwu8_t buf[], xwsz_t * size, xwsq_t pgidx,
                     xwtm_t to);

__xwbsp_rodata
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

static __xwbsp_code
xwer_t at24sd_putc(struct xwds_eeprom * eeprom, xwu8_t data, xwsq_t addr, xwtm_t to)
{
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg;
        xwer_t rc;
        xwu8_t txdata[2];
        xwu16_t i2cpaddr;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        txdata[0] = (xwu8_t)(addr & (xwu8_t)0xFF);
        txdata[1] = data;
        msg.addr = ((i2cpaddr & (xwu16_t)0x0078) |
                    (((addr >> (xwsq_t)8) & (xwu16_t)0x7)));
        msg.flag = ((xwu16_t)XWDS_I2C_F_START | (xwu16_t)XWDS_I2C_F_STOP);
        msg.data = txdata;
        msg.size = sizeof(txdata);
        rc = xwds_i2cm_xfer(bus, &msg, 1U, to);
        return rc;
}

static __xwbsp_code
xwer_t at24sd_getc(struct xwds_eeprom * eeprom, xwu8_t * buf, xwsq_t addr, xwtm_t to)
{
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg[2];
        xwer_t rc;
        xwu8_t txdata;
        xwu16_t i2cpaddr;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        txdata = (xwu8_t)(addr & (xwu8_t)0xFF);
        msg[0].addr = ((i2cpaddr & (xwu16_t)0x0078) |
                       (((addr >> (xwsq_t)8) & (xwu16_t)0x7)));
        msg[0].flag = (xwu16_t)XWDS_I2C_F_START;
        msg[0].data = &txdata;
        msg[0].size = 1U;
        msg[1].addr = msg[0].addr;
        msg[1].flag = ((xwu16_t)XWDS_I2C_F_START | (xwu16_t)XWDS_I2C_F_RD |
                       (xwu16_t)XWDS_I2C_F_STOP);
        msg[1].data = buf;
        msg[1].size = 1U;
        rc = xwds_i2cm_xfer(bus, msg, 2U, to);
        return rc;
}

static __xwbsp_code
xwer_t at24sd_pgwrite(struct xwds_eeprom * eeprom,
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
        xwu8_t txdata[wrsz + 1];

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        parameter = &eeprom->parameter;
        addr = ((pgidx * parameter->page_size) & (xwsq_t)0x7FF);
        txdata[0] = (xwu8_t)(addr & (xwu8_t)0xFF);
        memcpy(&txdata[1], data, wrsz);
        msg.addr = ((i2cpaddr & (xwu16_t)0x0078) |
                    (((addr >> (xwsq_t)8) & (xwu16_t)0x7)));
        msg.flag = (xwu16_t)XWDS_I2C_F_START | (xwu16_t)XWDS_I2C_F_STOP;
        msg.data = txdata;
        msg.size = wrsz + 1U;
        rc = xwds_i2cm_xfer(bus, &msg, 1U, to);
        if (rc < 0) {
                *size = 0;
        }

        return rc;
}

static __xwbsp_code
xwer_t at24sd_pgread(struct xwds_eeprom * eeprom,
                     xwu8_t * buf, xwsz_t * size, xwsq_t pgidx,
                     xwtm_t to)
{
        struct xwds_i2cm * bus;
        const struct xwds_eeprom_parameter * parameter;
        struct xwds_i2c_msg msg[2];
        xwer_t rc;
        xwu16_t i2cpaddr;
        xwsq_t addr;
        xwu8_t txdata;

        i2cpaddr = eeprom->i2cp.addr;
        bus = eeprom->i2cp.bus;
        parameter = &eeprom->parameter;
        addr = ((pgidx * parameter->page_size) & (xwsq_t)0x7FF);
        txdata = (xwu8_t)(addr & (xwu8_t)0xFF);
        msg[0].addr = ((i2cpaddr & (xwu16_t)0x0078) |
                       (((addr >> (xwsq_t)8) & (xwu16_t)0x7)));
        msg[0].flag = (xwu16_t)XWDS_I2C_F_START;
        msg[0].data = &txdata;
        msg[0].size = 1U;
        msg[1].addr = msg[0].addr;
        msg[1].flag = ((xwu16_t)XWDS_I2C_F_START | (xwu16_t)XWDS_I2C_F_RD |
                       (xwu16_t)XWDS_I2C_F_STOP);
        msg[1].data = buf;
        msg[1].size = *size;
        rc = xwds_i2cm_xfer(bus, msg, 2U, to);
        if (XWOK == rc) {
                *size = msg[1].size;
        } else {
                *size = 0;
        }
        return rc;
}
