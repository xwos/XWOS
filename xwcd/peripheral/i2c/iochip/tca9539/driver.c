/**
 * @file
 * @brief 玄武设备栈：I2C IO扩展芯片：TCA9539：驱动
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
#include <xwos/lib/xwlog.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/peripheral/i2c/iochip/tca9539/device.h>
#include <xwcd/peripheral/i2c/iochip/tca9539/driver.h>

#define LOGTAG "TCA9539"

static __xwbsp_code
xwer_t xwds_tca9539_write(struct xwds_tca9539 * tca9539,
                          xwu8_t reg, xwu8_t data,
                          xwtm_t to)
{
        xwer_t rc;
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg;
        xwu8_t txdata[2];

        bus = tca9539->iochip.bc.i2cp.bus;
        txdata[0] = (xwu8_t)reg;
        txdata[1] = (xwu8_t)data;
        msg.addr = tca9539->iochip.bc.i2cp.addr;
        msg.flag = ((xwu16_t)XWDS_I2C_F_START | (xwu16_t)XWDS_I2C_F_STOP);
        msg.data = txdata;
        msg.size = sizeof(txdata);
        rc = xwds_i2cm_xfer(bus, &msg, 1U, to);
        return rc;
}

static __xwbsp_code
xwer_t xwds_tca9539_read(struct xwds_tca9539 * tca9539,
                         xwu8_t reg, xwu8_t * buf,
                         xwtm_t to)
{
        xwer_t rc;
        struct xwds_i2cm * bus;
        struct xwds_i2c_msg msg[2];

        bus = tca9539->iochip.bc.i2cp.bus;
        msg[0].addr = tca9539->iochip.bc.i2cp.addr;
        msg[0].flag = (xwu16_t)XWDS_I2C_F_START;
        msg[0].data = &reg;
        msg[0].size = 1U;
        msg[1].addr = msg[0].addr;
        msg[1].flag = ((xwu16_t)XWDS_I2C_F_START | (xwu16_t)XWDS_I2C_F_RD |
                       (xwu16_t)XWDS_I2C_F_STOP);
        msg[1].data = buf;
        msg[1].size = 1U;
        rc = xwds_i2cm_xfer(bus, msg, 2U, to);
        return rc;
}

/******** ******** base driver ******** ********/
__xwbsp_code
xwer_t xwds_tca9539_drv_start(struct xwds_device * dev)
{
        struct xwds_tca9539 * tca9539;
        const struct xwds_resource_gpio * gpiorsc;
        xwer_t rc;

        tca9539 = xwds_cast(struct xwds_tca9539 *, dev);
        gpiorsc = tca9539->gpiorsc.rst;
        if (NULL != gpiorsc) {
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (rc < 0) {
                        goto err_gpio_req_rst;
                }
                xwds_tca9539_reset(tca9539, true);
        }
        gpiorsc = tca9539->gpiorsc.irq;
        if (NULL != gpiorsc) {
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (rc < 0) {
                        goto err_gpio_req_irq;
                }
        }
        return XWOK;

err_gpio_req_irq:
        gpiorsc = tca9539->gpiorsc.rst;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
err_gpio_req_rst:
        return rc;
}

__xwbsp_code
xwer_t xwds_tca9539_drv_stop(struct xwds_device * dev)
{
        struct xwds_tca9539 * tca9539;
        const struct xwds_resource_gpio * gpiorsc;

        tca9539 = xwds_cast(struct xwds_tca9539 *, dev);
        gpiorsc = tca9539->gpiorsc.irq;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
        gpiorsc = tca9539->gpiorsc.rst;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
__xwbsp_code
xwer_t xwds_tca9539_drv_suspend(struct xwds_device * dev)
{
        return xwds_tca9539_drv_stop(dev);
}

__xwbsp_code
xwer_t xwds_tca9539_drv_resume(struct xwds_device * dev)
{
        return xwds_tca9539_drv_start(dev);
}
#endif

/******** ******** gpio operation driver ******** ********/
__xwbsp_code
xwer_t xwds_tca9539_drv_gpio_req(struct xwds_iochip * iochip,
                                 xwid_t port, xwsq_t pinmask,
                                 xwtm_t to)
{
        XWOS_UNUSED(iochip);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);
        XWOS_UNUSED(to);
        return XWOK;
}

__xwbsp_code
xwer_t xwds_tca9539_drv_gpio_rls(struct xwds_iochip * iochip,
                                 xwid_t port, xwsq_t pinmask,
                                 xwtm_t to)
{
        XWOS_UNUSED(iochip);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);
        XWOS_UNUSED(to);
        return XWOK;
}

__xwbsp_code
xwer_t xwds_tca9539_drv_gpio_cfg(struct xwds_iochip * iochip,
                                 xwid_t port, xwsq_t pinmsk, void * cfg,
                                 xwtm_t to)
{
        struct xwds_tca9539 * tca9539 = xwds_cast(struct xwds_tca9539 *, iochip);
        const struct xwds_tca9539_pincfg * pincfg = cfg;
        xwer_t rc;
        xwu8_t oldval;
        xwu8_t newval;
        xwu8_t cfgval;
        xwu8_t regmsk;

        regmsk = (xwu8_t)pinmsk & 0xFFU;
        if (XWDS_TCA9539_PIN_DIRECTION_IN == pincfg->direction) {
                cfgval = regmsk;
        } else {
                cfgval = regmsk ^ regmsk;
        }
        if (1U == port) {
                rc = xwds_tca9539_read(tca9539, XWDS_TCA9539_REG_P1_CFG, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                newval |= cfgval;
                rc = xwds_tca9539_write(tca9539, XWDS_TCA9539_REG_P1_CFG, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        } else {
                rc = xwds_tca9539_read(tca9539, XWDS_TCA9539_REG_P0_CFG, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                newval |= cfgval;
                rc = xwds_tca9539_write(tca9539, XWDS_TCA9539_REG_P0_CFG, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        }
        return XWOK;

err_write:
err_read:
        return rc;
}

__xwbsp_code
xwer_t xwds_tca9539_drv_gpio_set(struct xwds_iochip * iochip,
                                 xwid_t port, xwsq_t pinmsk,
                                 xwtm_t to)
{
        struct xwds_tca9539 * tca9539 = xwds_cast(struct xwds_tca9539 *, iochip);
        xwer_t rc;
        xwu8_t oldval;
        xwu8_t newval;
        xwu8_t regmsk;

        regmsk = (xwu8_t)pinmsk & 0xFFU;
        if (1U == port) {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P1_OUTPUT, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                newval |= regmsk;
                rc = xwds_tca9539_write(tca9539,
                                        XWDS_TCA9539_REG_P1_OUTPUT, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        } else {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P0_OUTPUT, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                newval |= regmsk;
                rc = xwds_tca9539_write(tca9539,
                                        XWDS_TCA9539_REG_P0_OUTPUT, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        }
        return XWOK;

err_write:
err_read:
        return rc;
}

__xwbsp_code
xwer_t xwds_tca9539_drv_gpio_reset(struct xwds_iochip * iochip,
                                   xwid_t port, xwsq_t pinmsk,
                                   xwtm_t to)
{
        struct xwds_tca9539 * tca9539 = xwds_cast(struct xwds_tca9539 *, iochip);
        xwer_t rc;
        xwu8_t oldval;
        xwu8_t newval;
        xwu8_t regmsk;

        regmsk = (xwu8_t)pinmsk & 0xFFU;
        if (1U == port) {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P1_OUTPUT, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                rc = xwds_tca9539_write(tca9539,
                                        XWDS_TCA9539_REG_P1_OUTPUT, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        } else {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P0_OUTPUT, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                rc = xwds_tca9539_write(tca9539,
                                        XWDS_TCA9539_REG_P0_OUTPUT, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        }
        return XWOK;

err_write:
err_read:
        return rc;
}

__xwbsp_code
xwer_t xwds_tca9539_drv_gpio_toggle(struct xwds_iochip * iochip,
                                    xwid_t port, xwsq_t pinmsk,
                                    xwtm_t to)
{
        struct xwds_tca9539 * tca9539 = xwds_cast(struct xwds_tca9539 *, iochip);
        xwer_t rc;
        xwu8_t oldval;
        xwu8_t newval;
        xwu8_t regmsk;

        regmsk = (xwu8_t)pinmsk & 0xFFU;
        if (1U == port) {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P1_OUTPUT, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                newval ^= regmsk;
                rc = xwds_tca9539_write(tca9539,
                                        XWDS_TCA9539_REG_P1_OUTPUT, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        } else {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P0_OUTPUT, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                newval ^= regmsk;
                rc = xwds_tca9539_write(tca9539,
                                        XWDS_TCA9539_REG_P0_OUTPUT, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        }
        return XWOK;

err_write:
err_read:
        return rc;
}

__xwbsp_code
xwer_t xwds_tca9539_drv_gpio_output(struct xwds_iochip * iochip,
                                    xwid_t port, xwsq_t pinmsk, xwsq_t out,
                                    xwtm_t to)
{
        struct xwds_tca9539 * tca9539 = xwds_cast(struct xwds_tca9539 *, iochip);
        xwer_t rc;
        xwu8_t oldval;
        xwu8_t newval;
        xwu8_t regmsk;

        regmsk = (xwu8_t)pinmsk & 0xFFU;
        if (1U == port) {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P1_OUTPUT, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                newval |= (regmsk & (xwu8_t)out);
                rc = xwds_tca9539_write(tca9539,
                                        XWDS_TCA9539_REG_P1_OUTPUT, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        } else {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P0_OUTPUT, &oldval, to);
                if (rc < 0) {
                        goto err_read;
                }
                newval = oldval & (~regmsk);
                newval |= (regmsk & (xwu8_t)out);
                rc = xwds_tca9539_write(tca9539,
                                        XWDS_TCA9539_REG_P0_OUTPUT, newval, to);
                if (rc < 0) {
                        goto err_write;
                }
        }
        return XWOK;

err_write:
err_read:
        return rc;
}

__xwbsp_code
xwer_t xwds_tca9539_drv_gpio_input(struct xwds_iochip * iochip,
                                   xwid_t port, xwsq_t pinmsk, xwsq_t * in,
                                   xwtm_t to)
{
        struct xwds_tca9539 * tca9539 = xwds_cast(struct xwds_tca9539 *, iochip);
        xwer_t rc;
        xwu8_t regval;
        xwu8_t regmsk;

        regmsk = (xwu8_t)pinmsk & 0xFFU;
        if (1U == port) {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P1_INPUT, &regval, to);
                if (rc < 0) {
                        goto err_read;
                }
                *in = regval & regmsk;
        } else {
                rc = xwds_tca9539_read(tca9539,
                                       XWDS_TCA9539_REG_P0_INPUT, &regval, to);
                if (rc < 0) {
                        goto err_read;
                }
                *in = regval & regmsk;
        }
        return XWOK;

err_read:
        return rc;
}

__xwbsp_rodata
const struct xwds_iochip_driver xwds_tca9539_drv = {
        .base = {
                .name = "xwds.tca9539",
                .probe = NULL,
                .remove = NULL,
                .start = xwds_tca9539_drv_start,
                .stop = xwds_tca9539_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = xwds_tca9539_drv_suspend,
                .resume =  xwds_tca9539_drv_resume,
#endif
        },
        .gpio_req = xwds_tca9539_drv_gpio_req,
        .gpio_rls = xwds_tca9539_drv_gpio_rls,
        .gpio_cfg = xwds_tca9539_drv_gpio_cfg,
        .gpio_set = xwds_tca9539_drv_gpio_set,
        .gpio_reset = xwds_tca9539_drv_gpio_reset,
        .gpio_toggle = xwds_tca9539_drv_gpio_toggle,
        .gpio_output = xwds_tca9539_drv_gpio_output,
        .gpio_input = xwds_tca9539_drv_gpio_input,
};

/******** ******** TCA9539 APIs ******** ********/
__xwbsp_api
void xwds_tca9539_reset(struct xwds_tca9539 * tca9539, bool active)
{
        const struct xwds_resource_gpio * gpiorsc;

        gpiorsc = tca9539->gpiorsc.rst;
        if (gpiorsc) {
                if (active) {
                        xwds_gpio_reset(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                } else {
                        xwds_gpio_set(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                }
        }
}

__xwbsp_api
xwer_t xwds_tca9539_run(struct xwds_tca9539 * tca9539, xwds_eirq_f eisr, xwtm_t to)
{
        const struct xwds_tca9539_cfg * chipcfg;
        const struct xwds_resource_gpio * gpiorsc;
        xwer_t rc;

        chipcfg = (const struct xwds_tca9539_cfg *)tca9539->iochip.gpio.cfg;
        rc = xwds_tca9539_write(tca9539,
                                XWDS_TCA9539_REG_P0_CFG, chipcfg->p0.direction.u8,
                                to);
        if (rc < 0) {
                goto err_write;
        }
        rc = xwds_tca9539_write(tca9539,
                                XWDS_TCA9539_REG_P1_CFG, chipcfg->p1.direction.u8,
                                to);
        if (rc < 0) {
                goto err_write;
        }
        rc = xwds_tca9539_write(tca9539,
                                XWDS_TCA9539_REG_P0_OUTPUT, chipcfg->p0.out.u8,
                                to);
        if (rc < 0) {
                goto err_write;
        }
        rc = xwds_tca9539_write(tca9539,
                                XWDS_TCA9539_REG_P1_OUTPUT, chipcfg->p1.out.u8,
                                to);
        if (rc < 0) {
                goto err_write;
        }
        gpiorsc = tca9539->gpiorsc.irq;
        if (NULL != gpiorsc) {
                rc = xwds_eirq_req(gpiorsc->soc,
                                   gpiorsc->port, gpiorsc->pinmask, tca9539->eirq,
                                   XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                                   eisr, tca9539);
                if (rc < 0) {
                        goto err_eirq_req;
                }
        }
        return XWOK;

err_eirq_req:
err_write:
        return rc;
}

__xwbsp_api
void xwds_tca9539_quit(struct xwds_tca9539 * tca9539)
{
        const struct xwds_resource_gpio * gpiorsc;

        gpiorsc = tca9539->gpiorsc.irq;
        if (NULL != gpiorsc) {
                xwds_eirq_rls(gpiorsc->soc,
                              gpiorsc->port, gpiorsc->pinmask, tca9539->eirq);
        }
}

__xwbsp_api
void xwds_tca9539_dump(struct xwds_tca9539 * tca9539, xwtm_t to)
{
        xwer_t rc;
        xwu8_t val[8];

        rc = xwds_tca9539_read(tca9539,
                               XWDS_TCA9539_REG_P0_INPUT, &val[0], to);
        if (rc < 0) {
                goto err_read;
        }
        rc = xwds_tca9539_read(tca9539,
                               XWDS_TCA9539_REG_P1_INPUT, &val[1], to);
        if (rc < 0) {
                goto err_read;
        }
        rc = xwds_tca9539_read(tca9539,
                               XWDS_TCA9539_REG_P0_OUTPUT, &val[2], to);
        if (rc < 0) {
                goto err_read;
        }
        rc = xwds_tca9539_read(tca9539,
                               XWDS_TCA9539_REG_P1_OUTPUT, &val[3], to);
        if (rc < 0) {
                goto err_read;
        }
        rc = xwds_tca9539_read(tca9539,
                               XWDS_TCA9539_REG_P0_POLINV, &val[4], to);
        if (rc < 0) {
                goto err_read;
        }
        rc = xwds_tca9539_read(tca9539,
                               XWDS_TCA9539_REG_P1_POLINV, &val[5], to);
        if (rc < 0) {
                goto err_read;
        }
        rc = xwds_tca9539_read(tca9539,
                               XWDS_TCA9539_REG_P0_CFG, &val[6], to);
        if (rc < 0) {
                goto err_read;
        }
        rc = xwds_tca9539_read(tca9539,
                               XWDS_TCA9539_REG_P1_CFG, &val[7], to);
        if (rc < 0) {
                goto err_read;
        }
        xwlogf(INFO, LOGTAG,
               "[ID:%d] 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\r\n",
               tca9539->iochip.bc.i2cp.dev.id,
               val[0], val[1], val[2], val[3],
               val[4], val[5], val[6], val[7]);
err_read:
        return;
}
