/**
 * @file
 * @brief 设备栈驱动：I2C master controllor
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
#include <soc.h>
#include <soc_irq.h>
#include <soc_i2c.h>
#include <xwos/lib/xwlog.h>
#include <xwos/irq.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/clock.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/i2c/master.h>
#include <driver/ds/i2cm.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define MPC560XB_I2CM_RETRY_MAX                (32)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_i2cm_check_desc(struct xwds_i2cm * i2cm);

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_remove(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_stop(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_resume(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_xfer(struct xwds_i2cm * i2cm, struct xwds_i2c_msg * msg,
                              xwtm_t * xwtm);

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_reset(struct xwds_i2cm * i2cm, xwtm_t * xwtm);

static __xwbsp_isr
void mpc560xb_i2cm_txfsm_7bitaddr(struct xwds_i2cm * i2cm);

static __xwbsp_isr
void mpc560xb_i2cm_rxfsm_7bitaddr(struct xwds_i2cm * i2cm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct xwds_i2cm_driver mpc560xb_i2cm_drv = {
        .base = {
                .name = "mpc560xb.i2c.m",
                .probe = mpc560xb_i2cm_drv_probe,
                .remove = mpc560xb_i2cm_drv_remove,
                .start = mpc560xb_i2cm_drv_start,
                .stop = mpc560xb_i2cm_drv_stop,
                .suspend = mpc560xb_i2cm_drv_suspend,
                .resume =  mpc560xb_i2cm_drv_resume,
        },
        .xfer = mpc560xb_i2cm_drv_xfer,
        .reset = mpc560xb_i2cm_drv_reset,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_i2cm_check_desc(struct xwds_i2cm * i2cm)
{
        const struct xwds_resources * resources;
        struct mpc560xb_i2cm_drvdata * drvdata;
        const struct xwds_i2cm_cfg * cfg;
        const struct soc_i2c_private_cfg * xwccfg;
        xwer_t rc;

        resources = i2cm->dev.resources;
        drvdata = i2cm->dev.data;
        cfg = i2cm->cfg;
        if (__unlikely(is_err_or_null(resources))) {
                rc = -EINVAL;
        } else if (__unlikely(is_err_or_null(drvdata))) {
                rc = -EINVAL;
        } else if (__unlikely(is_err_or_null(cfg))) {
                rc = -EINVAL;
        } else {
                xwccfg = cfg->xwccfg;
                if (__unlikely(is_err_or_null(xwccfg))) {
                        rc = -EINVAL;
                } else {
                        rc = OK;
                }
        }
        return rc;
}

/******** ******** common driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_probe(struct xwds_device * dev)
{
        struct xwds_i2cm * i2cm;
        struct mpc560xb_i2cm_drvdata * drvdata;
        xwer_t rc;

        i2cm = xwds_static_cast(struct xwds_i2cm *, dev);
        rc = mpc560xb_i2cm_check_desc(i2cm);
        if (__likely(OK == rc)) {
                drvdata = dev->data;

                xwosal_splk_init(&drvdata->lock);
                xwosal_cdt_init(&drvdata->cdt);
                drvdata->xmsg.ptr = NULL;
                drvdata->dpos = 0;
                I2C.IBCR.B.MDIS = 1;
        }
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_remove(struct xwds_device * dev)
{
        struct mpc560xb_i2cm_drvdata * drvdata;

        drvdata = dev->data;
        xwosal_cdt_destroy(&drvdata->cdt);
        return OK;
}

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_start(struct xwds_device * dev)
{
        struct xwds_i2cm * i2cm;
        const struct xwds_resources * resources;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_i2cm_cfg * cfg;
        const struct soc_i2c_private_cfg * xwccfg;
        xwssz_t i, j;
        xwer_t rc;

        i2cm = xwds_static_cast(struct xwds_i2cm *, dev);
        resources = i2cm->dev.resources;
        cfg = i2cm->cfg;
        xwccfg = cfg->xwccfg;

        /* request irqs */
        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr,
                                      XWOS_UNUSED_ARGUMENT, dev);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                irqrsc = &resources->irqrsc_array[j];
                                xwos_irq_release(irqrsc->irqn);
                        }
                        goto err_req_irqs;
                }
        }

        /* request clock */
        for (i = 0; i < (xwssz_t)resources->clkrsc_num; i++) {
                clkrsc = &resources->clkrsc_array[i];
                rc = xwds_clk_req(clkrsc->soc, clkrsc->clkid);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                clkrsc = &resources->clkrsc_array[j];
                                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                        }
                        goto err_req_clks;
                }
        }

        /* install gpios */
        for (i = 0; i < (xwssz_t)resources->gpiorsc_num; i++) {
                gpiorsc = &resources->gpiorsc_array[i];
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                gpiorsc = &resources->gpiorsc_array[j];
                                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port,
                                              gpiorsc->pinmask);
                        }
                        goto err_req_gpios;
                }
        }

        /* setup clock */
        I2C.IBFD.R = xwccfg->bfdr;

        /* clear all i2c flags */
        I2C.IBSR.R = 0x12;

        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                xwos_irq_clear(irqrsc->irqn);
                rc = xwos_irq_cfg(irqrsc->irqn, irqrsc->cfg);
                if (OK == rc) {
                        rc = xwos_irq_enable(irqrsc->irqn);
                }
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                irqrsc = &resources->irqrsc_array[j];
                                xwos_irq_disable(irqrsc->irqn);
                        }
                        goto err_en_irq;
                }
        }
        I2C.IBCR.B.MDIS = 0; /* enable I2C */

        return OK;

err_en_irq:
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
err_req_gpios:
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
        }
err_req_clks:
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                xwos_irq_release(irqrsc->irqn);
        }
err_req_irqs:
        return rc;
}


static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_stop(struct xwds_device * dev)
{
        struct xwds_i2cm * i2cm;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        xwssz_t j;
        xwer_t rc;

        i2cm = xwds_static_cast(struct xwds_i2cm *, dev);
        resources = i2cm->dev.resources;

        I2C.IBCR.B.MDIS = 1; /* disable I2C */

        /* disable IRQs */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_disable(irqrsc->irqn);
                if (__unlikely(rc < 0)) {
                        goto err_irqc_disalbe;
                }
        }

        /* release GPIOs */
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                rc = xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__unlikely(rc < 0)) {
                        goto err_gpio_release;
                }
        }

        /* release clocks */
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                rc = xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                if (__unlikely(rc < 0)) {
                        goto err_clk_release;
                }
        }

        /* release IRQs */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_release(irqrsc->irqn);
                if (__unlikely(rc < 0)) {
                        goto err_irq_release;
                }
        }

        return OK;

err_irq_release:
err_clk_release:
err_gpio_release:
err_irqc_disalbe:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_suspend(struct xwds_device * dev)
{
        struct xwds_i2cm * i2cm;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        xwssz_t j;
        xwer_t rc;

        i2cm = xwds_static_cast(struct xwds_i2cm *, dev);
        resources = i2cm->dev.resources;

        I2C.IBCR.B.MDIS = 1; /* disable I2C */
        /* release GPIOs */
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                rc = xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__unlikely(rc < 0)) {
                        goto err_gpio_release;
                }
        }

        /* release clocks */
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                rc = xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                if (__unlikely(rc < 0)) {
                        goto err_clk_release;
                }
        }
        return OK;

err_clk_release:
err_gpio_release:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_resume(struct xwds_device * dev)
{
        struct xwds_i2cm * i2cm;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        xwssz_t i, j;
        xwer_t rc;

        i2cm = xwds_static_cast(struct xwds_i2cm *, dev);
        resources = i2cm->dev.resources;

        /* request clocks */
        for (i = 0; i < (xwssz_t)resources->clkrsc_num; i++) {
                clkrsc = &resources->clkrsc_array[i];
                rc = xwds_clk_req(clkrsc->soc, clkrsc->clkid);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                clkrsc = &resources->clkrsc_array[j];
                                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                        }
                        goto err_req_clks;
                }
        }

        /* install GPIOs */
        for (i = 0; i < (xwssz_t)resources->gpiorsc_num; i++) {
                gpiorsc = &resources->gpiorsc_array[i];
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                gpiorsc = &resources->gpiorsc_array[j];
                                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port,
                                              gpiorsc->pinmask);
                        }
                        goto err_req_gpios;
                }
        }
        I2C.IBCR.B.MDIS = 0; /* enable I2C */

        return OK;

err_req_gpios:
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
        }
err_req_clks:
        return rc;
}

/******** ******** i2cm driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_reset(struct xwds_i2cm * i2cm, xwtm_t * xwtm)
{
        struct mpc560xb_i2cm_drvdata * drvdata;
        xwreg_t cpuirq;

        XWOS_UNUSED(xwtm);
        drvdata = i2cm->dev.data;

        xwosal_splk_lock_cpuirqsv(&drvdata->lock, &cpuirq);
        I2C.IBCR.R = 0x80;
        I2C.IBSR.R = 0x12;
        drvdata->xmsg.rc = -EAGAIN;
        drvdata->xmsg.ptr = NULL;
        drvdata->dpos = 0;
        I2C.IBCR.B.MDIS = 0;
        xwosal_splk_unlock_cpuirqrs(&drvdata->lock, cpuirq);
        xwosal_cdt_unicast(xwosal_cdt_get_id(&drvdata->cdt));
        return OK;
}

static __xwbsp_code
xwer_t mpc560xb_i2cm_drv_xfer(struct xwds_i2cm * i2cm, struct xwds_i2c_msg * msg,
                              xwtm_t * xwtm)
{
        struct mpc560xb_i2cm_drvdata * drvdata;
        xwreg_t cpuirq;
        union xwlk_ulock ulk;
        xwsq_t lkstate;
        __maybe_unused xwu8_t ibsr;
        xwer_t rc;

        drvdata = i2cm->dev.data;
        ulk.osal.splk = &drvdata->lock;
        xwosal_splk_lock_cpuirqsv(&drvdata->lock, &cpuirq);
        drvdata->xmsg.ptr = msg;
        /* Generate START */
        if (I2C.IBCR.B.MS) {
                I2C.IBCR.R |= (SOC_I2C_IBCR_RSTA_MASK | SOC_I2C_IBCR_TXRX_MASK);
        } else {
                I2C.IBCR.R |= (SOC_I2C_IBCR_MSSL_MASK | SOC_I2C_IBCR_TXRX_MASK);
        }
        while (0 == I2C.IBSR.B.IBB) {
        }
        if (is_err_or_null(msg)) {
                I2C.IBCR.B.MS = 0;
                rc = drvdata->xmsg.rc;
        } else if (0 == msg->size) {
                I2C.IBCR.B.MS = 0;
                rc = OK;
        } else if (XWDS_I2C_F_10BITADDR & msg->flags) {
                I2C.IBCR.B.MS = 0;
                rc = -ENOSYS;
        } else {
                drvdata->dpos = 0;
                if (XWDS_I2C_F_RD & msg->flags) {
                        if (msg->size > 1) {
                                I2C.IBCR.B.NOACK = 0;
                        } else {
                                I2C.IBCR.B.NOACK = 1;
                        }
                        I2C.IBDR.R = (xwu8_t)(msg->addr & 0xFF) | ((xwu8_t)1);
                } else {
                        I2C.IBDR.R = (xwu8_t)(msg->addr & 0xFF) & ((xwu8_t)~1);
                }
                I2C.IBCR.B.IBIE = 1; /* enable irq */
                rc = xwosal_cdt_timedwait(xwosal_cdt_get_id(&drvdata->cdt),
                                          ulk, XWLK_TYPE_SPLK, NULL,
                                          xwtm, &lkstate);
                if (OK == rc) {
                        rc = drvdata->xmsg.rc;
                } else {
                        if (XWLK_STATE_UNLOCKED == lkstate) {
                                xwosal_splk_lock(&drvdata->lock);
                        }
                }
        }
        drvdata->xmsg.ptr = NULL;
        xwosal_splk_unlock_cpuirqrs(&drvdata->lock, cpuirq);
        return rc;
}

/******** ******** i2c isr ******** ********/
static __xwbsp_isr
void mpc560xb_i2cm_txfsm_7bitaddr(struct xwds_i2cm * i2cm)
{
        struct mpc560xb_i2cm_drvdata * drvdata;
        struct xwds_i2c_msg * msg;

        drvdata = i2cm->dev.data;
        msg = drvdata->xmsg.ptr;
        if ((1 == I2C.IBSR.B.TCF) && (0 == I2C.IBSR.B.RXAK)) {
                if (drvdata->dpos == msg->size) {
                        if (XWDS_I2C_F_STOP & msg->flags) {
                                I2C.IBCR.B.MS = 0;
                        }
                        drvdata->xmsg.rc = OK;
                        I2C.IBCR.B.IBIE = 0;
                        xwosal_cdt_unicast(xwosal_cdt_get_id(&drvdata->cdt));
                } else {
                        I2C.IBDR.R = msg->buf[drvdata->dpos];
                        drvdata->dpos++;
                }
        } else {
                /* Rx NO ACK */
                drvdata->xmsg.rc = -EIO;
                I2C.IBCR.B.IBIE = 0;
                xwosal_cdt_unicast(xwosal_cdt_get_id(&drvdata->cdt));
        }
}

static __xwbsp_isr
void mpc560xb_i2cm_rxfsm_7bitaddr(struct xwds_i2cm * i2cm)
{
        struct mpc560xb_i2cm_drvdata * drvdata;
        struct xwds_i2c_msg * msg;
        xwsz_t rest;
        __maybe_unused xwu8_t ibdr;

        drvdata = i2cm->dev.data;
        msg = drvdata->xmsg.ptr;

        if (1 == I2C.IBSR.B.TCF) {
                if (1 == I2C.IBCR.B.TX) {
                        if (1 == I2C.IBSR.B.RXAK) {
                                drvdata->xmsg.rc = -EIO;
                                I2C.IBCR.B.IBIE = 0;
                                xwosal_cdt_unicast(xwosal_cdt_get_id(&drvdata->cdt));
                        } else {
                                I2C.IBCR.B.TX = 0;
                                xwmb_read(xwu8_t, ibdr, &I2C.IBDR.R);
                        }
                } else {
                        rest = msg->size - drvdata->dpos;
                        if (2 == rest) {
                                I2C.IBCR.B.NOACK = 1;
                        } else if (1 == rest) {
                                I2C.IBCR.B.MS = 0;
                        } else {
                        }
                        msg->buf[drvdata->dpos] = I2C.IBDR.R;
                        drvdata->dpos++;
                        if (drvdata->dpos == msg->size) {
                                drvdata->xmsg.rc = OK;
                                xwosal_cdt_unicast(xwosal_cdt_get_id(&drvdata->cdt));
                        }
                }
        }
}

__xwbsp_isr
void mpc560xb_i2cm_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_i2cm * i2cm;
        struct mpc560xb_i2cm_drvdata * drvdata;
        struct xwds_i2c_msg * msg;

        xwos_irq_get_data(IRQ_I2C0_SR, &irqdata);
        i2cm =irqdata.data;
        drvdata = i2cm->dev.data;

        if (I2C.IBSR.B.IAAS) {
                /* TODO: Addressed as a slave */
                I2C.IBSR.B.IBIF = 1;
        } else if (I2C.IBSR.B.IBAL) {
                /* TODO: Arbitration Lost */
                I2C.IBSR.B.IBAL = 1;
                I2C.IBSR.B.IBIF = 1;
        } else if ((I2C.IBIC.R) && (0 == I2C.IBSR.B.IBB)) {
                I2C.IBSR.B.IBIF = 1;
                /* TODO: bus idle IRQ */
        } else {
                msg = drvdata->xmsg.ptr;
                I2C.IBSR.B.IBIF = 1;
                xwosal_splk_lock_cpuirq(&drvdata->lock);
                if (is_err_or_null(msg)) {
                        /* communication is interruptted. Clear all flags and
                           generate STOP. */
                        I2C.IBSR.R = 0x12;
                        I2C.IBCR.B.MS = 0;
                } else {
                        if (XWDS_I2C_F_RD & msg->flags) {
                                mpc560xb_i2cm_rxfsm_7bitaddr(i2cm);
                        } else {
                                mpc560xb_i2cm_txfsm_7bitaddr(i2cm);
                        }
                }
                xwosal_splk_unlock_cpuirq(&drvdata->lock);
        }
}
