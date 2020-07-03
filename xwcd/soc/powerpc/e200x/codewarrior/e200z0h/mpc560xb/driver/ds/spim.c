/**
 * @file
 * @brief 设备栈驱动：MPC560xB SPI master mode controller
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
#include <xwos/lib/string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/irq.h>
#include <soc.h>
#include <soc_irq.h>
#include <soc_spi.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/lin/id.h>
#include <xwmd/ds/soc/clock.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/spi/master.h>
#include <driver/ds/spim.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_spim_check_desc(struct xwds_spim * spim);

static __xwbsp_code
xwer_t mpc560xb_spim_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_spim_drv_remove(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_spim_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_spim_drv_stop(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_spim_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_spim_drv_resume(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_spim_drv_xfer(struct xwds_spim * spim,
                              struct xwds_spim_msg * msg,
                              xwtm_t * xwtm);
/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct xwds_spim_driver mpc560xb_spim_drv = {
        .base = {
                .name = "mpc560xb.dspi.m",
                .probe = mpc560xb_spim_drv_probe,
                .remove = mpc560xb_spim_drv_remove,
                .start = mpc560xb_spim_drv_start,
                .stop = mpc560xb_spim_drv_stop,
                .suspend = mpc560xb_spim_drv_suspend,
                .resume =  mpc560xb_spim_drv_resume,
        },
        .xfer = mpc560xb_spim_drv_xfer,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_spim_check_desc(struct xwds_spim * spim)
{
        const struct xwds_resources * resources;
        struct mpc560xb_spim_drvdata * drvdata;
        const struct xwds_spim_cfg * cfg;
        xwer_t rc;

        resources = spim->dev.resources;
        drvdata = spim->dev.data;
        cfg = spim->cfg;
        if (__unlikely(is_err_or_null(resources)))
                rc = -EINVAL;
        else if (__unlikely(is_err_or_null(cfg)))
                rc = -EINVAL;
        else if (__unlikely(is_err_or_null(drvdata)))
                rc = -EINVAL;
        else
                rc = OK;
        return rc;
}

/******** ******** common driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_spim_drv_probe(struct xwds_device * dev)
{
        struct xwds_spim * spim;
        struct mpc560xb_spim_drvdata * drvdata;
        xwer_t rc;

        spim = xwds_static_cast(struct xwds_spim *, dev);
        rc = mpc560xb_spim_check_desc(spim);
        if (__likely(OK == rc)) {
                drvdata = dev->data;
                xwosal_cdt_init(&drvdata->cdt);
                xwosal_splk_init(&drvdata->lock);
        }
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_spim_drv_remove(struct xwds_device * dev)
{
        struct mpc560xb_spim_drvdata * drvdata;

        drvdata = dev->data;
        xwosal_cdt_destroy(&drvdata->cdt);
        return OK;
}

static __xwbsp_code
xwer_t mpc560xb_spim_drv_start(struct xwds_device * dev)
{
        struct xwds_spim * spim;
        struct mpc560xb_spim_drvdata * drvdata;
        const struct xwds_resources * resources;
        const struct xwds_spim_cfg * cfg;
        const struct soc_spi_private_cfg * xwccfg;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        struct DSPI_tag * reg;
        xwssz_t i, j;
        xwer_t rc;

        spim = xwds_static_cast(struct xwds_spim *, dev);
        resources = spim->dev.resources;
        drvdata = spim->dev.data;
        reg = resources->regrsc_array[0].base;
        cfg = spim->cfg;
        xwccfg = cfg->xwccfg;

        /* request IRQs */
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

        /* request Clocks */
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

        reg->MCR.R = 0x883F4C01;
        /* Master/slave mode select: Master
           Continuous SCK enable: defined later
           DSPI configuration: SPI
           Freeze in debug mode: ON
           Modified timing format enable: OFF
           Peripheral chip select strobe enable: OFF
           Receive FIFO overflow overwrite enable: OFF
           Peripheral chip select inactive state: High
           Module disable: ON
           transmit FIFO: ON
           receive FIFO: ON
           Clear TX FIFO: Clear
           Clear RX FIFO: Clear
           Sample point: 1
           Halt: ON
        */
        if (xwccfg->continuous_sck) {
                reg->MCR.B.CONT_SCKE = 1;
        } else {
                reg->MCR.B.CONT_SCKE = 0;
        }

        /* setup CS pin */
        for (i = 0; i < (xwssz_t)cfg->cspins_num; i++) {
                if (cfg->cspins[i].inactive) {
                        reg->MCR.R |= BIT(i + 15);
                } else {
                        reg->MCR.R &= ~BIT(i + 15);
                }
        }

        reg->TCR.R = 0;

        reg->CTAR[0].R = 0x38004444;
        reg->CTAR[1].R = 0x38004444;
        reg->CTAR[2].R = 0x38004444;
        reg->CTAR[3].R = 0x38004444;
        reg->CTAR[4].R = 0x38004444;
        reg->CTAR[5].R = 0x38004444;
        /* Double Baud Rate: 0
           Frame Size: defined later
           Clock Polarity: defined later
           Clock Phase: defined later
           LSB First: defined later
           PCS to SCK Delay Prescaler: 1
           After SCK Delay Prescaler: 1
           Delay after Transfer Prescaler: 1
           Baud Rate Prescaler: 2
           PCS to SCK Delay Scaler: 32
           After SCK Delay Scaler: 32
           Delay after Transfer Scaler: 32
           Baud Rate Scaler: 32
        */

        /* setup Clocks */
        for (i = 0; i < 6; i++) {
                reg->CTAR[i].B.PCSSCK = xwccfg->clkdiv.bit.pcssck;
                reg->CTAR[i].B.CSSCK = xwccfg->clkdiv.bit.cssck;
                reg->CTAR[i].B.PASC = xwccfg->clkdiv.bit.pasc;
                reg->CTAR[i].B.ASC = xwccfg->clkdiv.bit.asc;
                reg->CTAR[i].B.PDT = xwccfg->clkdiv.bit.pdt;
                reg->CTAR[i].B.DT = xwccfg->clkdiv.bit.dt;
                reg->CTAR[i].B.PBR = xwccfg->clkdiv.bit.pbr;
                reg->CTAR[i].B.BR = xwccfg->clkdiv.bit.br;
                reg->CTAR[i].B.DBR = xwccfg->clkdiv.bit.dbr;
        }

        /* clear IRQ flags */
        reg->SR.R = 0x9A0A0000;

        /* setup IRQs */
        reg->RSER.R = 0x00000000;
        /* TCF_RE: OFF
           EOQF_RE: OFF
           TFUF_RE: OFF
           TFFF_RE: OFF
           TFFF_DIRS: IRQ
           RFOF_RE: OFF
           RFDF_RE: OFF
           RFDF_DIRS: IRQ
        */

        /* enable IRQs */
        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                xwos_irq_clear(irqrsc->irqn);
                rc = xwos_irq_cfg(irqrsc->irqn, irqrsc->cfg);
                if (OK == rc)
                        rc = xwos_irq_enable(irqrsc->irqn);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                irqrsc = &resources->irqrsc_array[j];
                                xwos_irq_disable(irqrsc->irqn);
                        }
                        goto err_en_irq;
                }
        }

        /* clear driver data */
        drvdata->msg = NULL;
        drvdata->txpos = 0;
        drvdata->rxpos = 0;
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
xwer_t mpc560xb_spim_drv_stop(struct xwds_device * dev)
{
        struct xwds_spim * spim;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        xwssz_t j;
        xwer_t rc;

        spim = xwds_static_cast(struct xwds_spim *, dev);
        resources = spim->dev.resources;

        /* disable IRQs */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_disable(irqrsc->irqn);
                if (__unlikely(rc < 0)) {
                        goto err_irq_disable;
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
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
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
err_irq_disable:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_spim_drv_suspend(struct xwds_device * dev)
{
        struct xwds_spim * spim;
        const struct xwds_resources * resources;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t j;
        xwer_t rc;

        spim = xwds_static_cast(struct xwds_spim *, dev);
        resources = spim->dev.resources;

        /* disable IRQs */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_disable(irqrsc->irqn);
                if (__unlikely(rc < 0)) {
                        goto err_irq_disable;
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

        return OK;

err_clk_release:
err_gpio_release:
err_irq_disable:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_spim_drv_resume(struct xwds_device * dev)
{
        struct xwds_spim * spim;
        const struct xwds_resources * resources;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t i, j;
        xwer_t rc;

        spim = xwds_static_cast(struct xwds_spim *, dev);
        resources = spim->dev.resources;

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

        /* enable IRQs */
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
        return rc;
}

/******** ******** spim driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_spim_drv_xfer(struct xwds_spim * spim,
                              struct xwds_spim_msg * msg,
                              xwtm_t * xwtm)
{
        const struct xwds_resources * resources;
        struct mpc560xb_spim_drvdata * drvdata;
        volatile struct DSPI_tag * reg;
        xwu32_t pushr = 0;
        union xwlk_ulock ulk;
        xwsq_t lockstate;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = OK;
        resources = spim->dev.resources;
        drvdata = spim->dev.data;
        reg = resources->regrsc_array[0].base;

        /* setup format */
        if (msg->flags & XWDS_SPIM_FF_FO_LSB) {
                reg->CTAR[0].B.LSBFE = 1;
        } else {
                reg->CTAR[0].B.LSBFE = 0;
        }

        if (msg->flags & XWDS_SPIM_FF_CPHA_LSTC) {
                reg->CTAR[0].B.CPHA = 1;
        } else {
                reg->CTAR[0].B.CPHA = 0;
        }

        if (msg->flags & XWDS_SPIM_FF_CPOL_HIGH) {
                reg->CTAR[0].B.CPOL = 1;
        } else {
                reg->CTAR[0].B.CPOL = 0;
        }

        if (XWDS_SPIM_FF_WL_8 == (msg->flags & XWDS_SPIM_FF_WL_MSK)) {
                reg->CTAR[0].B.FMSZ = 7;
        } else if (XWDS_SPIM_FF_WL_16 == (msg->flags & XWDS_SPIM_FF_WL_MSK)) {
                reg->CTAR[0].B.FMSZ = 15;
        } else {
                rc = -ENOSYS;
                goto err_fmsz;
        }

        /* clear IRQ flags */
        reg->SR.R = 0x9A0A0000;

        /* enable SPI */
        reg->MCR.B.MDIS = 0;

        /* clear FIFO */
        reg->MCR.B.CLR_TXF = 1;
        reg->MCR.B.CLR_RXF = 1;

        /* start */
        ulk.osal.splk = &drvdata->lock;
        xwosal_splk_lock_cpuirqsv(&drvdata->lock, &cpuirq);
        drvdata->txpos = 0;
        drvdata->rxpos = 0;
        drvdata->msg = msg;
        if (msg->size > 0) {
                reg->RSER.R |= 0x02020000; /* enable TFFF_RE & RFDF_RE IRQs */
                if (msg->flags & XWDS_SPIM_FF_CS_CONTINUOUS) {
                        pushr |= 0x80000000; /* CONT flag */
                }
                if (msg->cs_pin_msk) {
                        pushr |= ((msg->cs_pin_msk << 16) & 0x003F0000);
                }
                if (0 == drvdata->txpos) {
                        pushr |= 0x04000000; /* Clear SPI_TCNT */
                }
                if (1 == (msg->size - drvdata->txpos)) {
                        pushr |= 0x08000000; /* EOQ flag */
                        reg->RSER.B.TFFFRE = 0;
                }
                if (XWDS_SPIM_FF_WL_8 == (msg->flags & XWDS_SPIM_FF_WL_MSK)) {
                        pushr |= msg->txq.d8[0];
                        reg->PUSHR.R = pushr;
                } else {
                        pushr |= msg->txq.d16[0];
                        reg->PUSHR.R = pushr;
                }
                drvdata->txpos++;
                reg->MCR.B.HALT = 0;
                rc = xwosal_cdt_timedwait(xwosal_cdt_get_id(&drvdata->cdt),
                                          ulk, XWLK_TYPE_SPLK, NULL,
                                          xwtm, &lockstate);
                if (XWLK_STATE_UNLOCKED == lockstate) {
                        xwosal_splk_lock(&drvdata->lock);
                }
                drvdata->msg = NULL;
                drvdata->txpos = 0;
                drvdata->rxpos = 0;
        }
        xwosal_splk_unlock_cpuirqrs(&drvdata->lock, cpuirq);
        reg->MCR.B.MDIS = 1;

err_fmsz:
        return rc;
}

/******** ******** spim ISR ******** ********/
__xwbsp_isr
void mpc560xb_spim_isr_tfuf_rfof(void)
{
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_spim * spim;
        const struct xwds_resources * resources;
        volatile struct DSPI_tag * reg;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        spim = irqdata.data;
        SDL_BUG_ON(NULL == spim);
        resources = spim->dev.resources;
        reg = resources->regrsc_array[0].base;

        if (reg->SR.B.TFUF) {
                reg->SR.B.TFUF = 1;
        }
        if (reg->SR.B.RFOF) {
                reg->SR.B.RFOF = 1;
        }
}

__xwbsp_isr
void mpc560xb_spim_isr_tfff(void)
{
        xwreg_t cpuirq;
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_spim * spim;
        struct mpc560xb_spim_drvdata * drvdata;
        const struct xwds_resources * resources;
        volatile struct DSPI_tag * reg;
        struct xwds_spim_msg * msg;
        xwu32_t pushr = 0;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        spim = irqdata.data;
        SDL_BUG_ON(NULL == spim);
        resources = spim->dev.resources;
        reg = resources->regrsc_array[0].base;
        reg->SR.B.TFFF = 1;
        drvdata = spim->dev.data;
        xwosal_splk_lock_cpuirqsv(&drvdata->lock, &cpuirq);
        msg = drvdata->msg;
        if (msg) {
                if (msg->flags & XWDS_SPIM_FF_CS_CONTINUOUS) {
                        pushr |= 0x80000000; /* CONT flag */
                }
                if (msg->cs_pin_msk) {
                        pushr |= ((msg->cs_pin_msk << 16) & 0x003F0000);
                }
                if (0 == drvdata->txpos) {
                        pushr = 0x04000000; /* Clear SPI_TCNT */
                }
                if (1 == (msg->size - drvdata->txpos)) {
                        pushr |= 0x08000000; /* EOQ flag */
                        reg->RSER.B.TFFFRE = 0;
                }
                if (XWDS_SPIM_FF_WL_8 == (msg->flags & XWDS_SPIM_FF_WL_MSK)) {
                        pushr |= msg->txq.d8[drvdata->txpos];
                        reg->PUSHR.R = pushr;
                } else {
                        pushr |= msg->txq.d16[drvdata->txpos];
                        reg->PUSHR.R = pushr;
                }
                drvdata->txpos++;
        } else {
                reg->MCR.B.HALT = 1;
                reg->RSER.B.TFFFRE = 0;
        }
        xwosal_splk_unlock_cpuirqrs(&drvdata->lock, cpuirq);
}

__xwbsp_isr
void mpc560xb_spim_isr_rfdf(void)
{
        xwreg_t cpuirq;
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_spim * spim;
        struct mpc560xb_spim_drvdata * drvdata;
        const struct xwds_resources * resources;
        volatile struct DSPI_tag * reg;
        struct xwds_spim_msg * msg;
        __maybe_unused xwu32_t rxdata;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        spim = irqdata.data;
        SDL_BUG_ON(NULL == spim);
        resources = spim->dev.resources;
        reg = resources->regrsc_array[0].base;
        drvdata = spim->dev.data;
        xwosal_splk_lock_cpuirqsv(&drvdata->lock, &cpuirq);
        msg = drvdata->msg;
        if (msg) {
                if (XWDS_SPIM_FF_WL_8 == (msg->flags & XWDS_SPIM_FF_WL_MSK)) {
                        msg->rxq.d8[drvdata->rxpos] = (xwu8_t)reg->POPR.R;
                        drvdata->rxpos++;
                } else {
                        msg->rxq.d16[drvdata->rxpos] = (xwu16_t)reg->POPR.R;
                        drvdata->rxpos++;
                }
                if (msg->size == drvdata->rxpos) {
                        reg->RSER.B.RFDFRE = 0;
                        if (reg->SR.B.EOQF) {
                                reg->MCR.B.HALT = 1;
                                reg->SR.B.EOQF = 1;
                                xwosal_cdt_unicast(xwosal_cdt_get_id(&drvdata->cdt));
                        }
                }
                reg->SR.B.RFDF = 1;
        } else {
                reg->MCR.B.HALT = 1;
                reg->RSER.B.RFDFRE = 0;
                xwmb_read(xwu32_t, rxdata, &reg->POPR.R);
                reg->SR.B.RFDF = 1;
        }
        xwosal_splk_unlock_cpuirqrs(&drvdata->lock, cpuirq);
}
