/**
 * @file
 * @brief 设备栈驱动：DMA UART0
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/irq.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/clock.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/dma.h>
#include <xwcd/ds/uart/dma.h>
#include <soc.h>
#include <soc_dma.h>
#include <soc_dmauart.h>
#include <bdl/ds/driver/dmauart0.h>

static __xwbsp_code
xwer_t mpc560xb_dmauart0_check_desc(struct xwds_dmauartc * uartc);

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_stop(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_resume(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_cfg(struct xwds_dmauartc * uartc,
                                 const struct xwds_uart_cfg * cfg);

static
void mpc560xb_dmauart0_tx_cb(struct xwds_soc * soc, xwid_t ch, xwu32_t rc,
                             xwds_dma_cbarg_t arg);

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_tx(struct xwds_dmauartc * dmauartc,
                                const xwu8_t * srcmem, xwsz_t * size,
                                xwtm_t * xwtm);

static __xwbsp_code
void mpc560xb_dmauart0_rxdma_cb(struct xwds_soc * soc, xwid_t ch, xwu32_t res,
                                xwds_dma_cbarg_t arg);

static __xwbsp_code
xwer_t mpc560xb_dmauart0_rxdma_setup(struct xwds_dmauartc * dmauartc);

__xwbsp_rodata const struct xwds_dmauartc_driver mpc560xb_dmauart0_drv = {
        .base = {
                .name = "mpc560xb.linflexd.uart",
                .probe = mpc560xb_dmauart0_drv_probe,
                .remove = NULL,
                .start = mpc560xb_dmauart0_drv_start,
                .stop = mpc560xb_dmauart0_drv_stop,
                .suspend = mpc560xb_dmauart0_drv_suspend,
                .resume =  mpc560xb_dmauart0_drv_resume,
        },
        .cfg = mpc560xb_dmauart0_drv_cfg,
        .tx = mpc560xb_dmauart0_drv_tx,
};

static __xwbsp_code
xwer_t mpc560xb_dmauart0_check_desc(struct xwds_dmauartc * dmauartc)
{
        struct mpc560xb_dmauart0_drvdata * drvdata;
        const struct xwds_resources * resources;
        const struct xwds_uart_cfg * cfg;
        xwer_t rc;

        drvdata = dmauartc->dev.data;
        resources = dmauartc->dev.resources;
        if (__xwcc_unlikely(is_err_or_null(resources))) {
                rc = -EINVAL;
                goto err_nores;
        }
        cfg = dmauartc->cfg;
        if (__xwcc_unlikely(is_err_or_null(cfg))) {
                rc = -EINVAL;
                goto err_nocfg;
        }
        if (__xwcc_unlikely(is_err_or_null(drvdata))) {
                rc = -EINVAL;
                goto err_nodrvdata;
        }

        return XWOK;

err_nodrvdata:
err_nocfg:
err_nores:
        return rc;
}

/******** ******** common driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_probe(struct xwds_device * dev)
{
        struct xwds_dmauartc * dmauartc;

        dmauartc = xwds_cast(struct xwds_dmauartc *, dev);
        return mpc560xb_dmauart0_check_desc(dmauartc);
}

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_start(struct xwds_device * dev)
{
        struct xwds_dmauartc * dmauartc;
        struct mpc560xb_dmauart0_drvdata * drvdata;
        __xwcc_unused const struct xwds_resource_dma * txdmarsc;
        __xwcc_unused const struct xwds_resource_dma * rxdmarsc;
        const struct xwds_resources * resources;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_dma * dmarsc;
        const struct xwds_uart_cfg * cfg;
        const const struct soc_dmauart_private_cfg * xwccfg;
        xwssz_t i, j;
        xwer_t rc;

        dmauartc = xwds_cast(struct xwds_dmauartc *, dev);
        resources = dmauartc->dev.resources;
        cfg = dmauartc->cfg;
        xwccfg = cfg->xwccfg;

        drvdata = dmauartc->dev.data;
        xwos_splk_init(&drvdata->tx.splk);
        xwos_cond_init(&drvdata->tx.cond);
        drvdata->tx.rc = -ECANCELED;

        /* Request IRQ resources */
        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr, dev, NULL);
                if (__xwcc_unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                irqrsc = &resources->irqrsc_array[j];
                                xwos_irq_release(irqrsc->irqn);
                        }
                        goto err_req_irqs;
                }
        }

        /* Request clock resources */
        for (i = 0; i < (xwssz_t)resources->clkrsc_num; i++) {
                clkrsc = &resources->clkrsc_array[i];
                rc = xwds_clk_req(clkrsc->soc, clkrsc->clkid);
                if (__xwcc_unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                clkrsc = &resources->clkrsc_array[j];
                                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                        }
                        goto err_req_clks;
                }
        }

        /* Request GPIO resources */
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

        /* Request DMA resources */
        for (i = 0; i < (xwssz_t)resources->dmarsc_num; i++) {
                dmarsc = &resources->dmarsc_array[i];
                rc = xwds_dma_req(dmarsc->soc, dmarsc->ch);
                if (__xwcc_unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                dmarsc = &resources->dmarsc_array[j];
                                xwds_dma_rls(dmarsc->soc, dmarsc->ch);
                        }
                        goto err_req_dmas;
                }
        }

        /* Setup uart */
        LINFLEX_0.LINCR1.B.INIT = 1;
        LINFLEX_0.LINCR1.B.SLEEP = 0;

        LINFLEX_0.UARTCR.B.UART = 1;
        /* word length */
        LINFLEX_0.UARTCR.B.WL0 = cfg->bus.bit.bits;
        /* stop bit */
        LINFLEX_0.GCR.B.STOP = cfg->bus.bit.stopbits? 1 : 0;

        if (XWDS_UART_PARITY_NONE == cfg->bus.bit.parity) {
                LINFLEX_0.UARTCR.B.PCE = 0;
        } else {
                LINFLEX_0.UARTCR.B.PCE = 1; /* enable parity */
                if (XWDS_UART_PARITY_ODD == cfg->bus.bit.parity) {
                        LINFLEX_0.UARTCR.B.PC0 = 1;
                } else {
                        LINFLEX_0.UARTCR.B.PC0 = 0;
                }
        }

        if (XWDS_UART_MODE_TX & cfg->bus.bit.mode) {
                LINFLEX_0.UARTCR.B.TFBM = 1;
                LINFLEX_0.UARTCR.B.TXEN = 1;
                txdmarsc = dmauartc->txdmarsc;
                LINFLEX_0.DMATXE.B.DTE0 = 1;
        }
        if (XWDS_UART_MODE_RX & cfg->bus.bit.mode) {
                LINFLEX_0.UARTCR.B.RFBM = 1;
                LINFLEX_0.UARTCR.B.RXEN = 1;
                rxdmarsc = dmauartc->rxdmarsc;
                LINFLEX_0.DMARXE.B.DRE0 = 1;
        }

        /* baudrate */
        xwu32_t clkfqcy;
        xwsz_t bufnum;
        xwu32_t uartdiv100m;
        xwu32_t mantissa;
        xwu32_t fraction;

        clkrsc = &resources->clkrsc_array[0];
        bufnum = 1;
        rc = xwds_clk_getfqcy(clkrsc->soc, clkrsc->clkid, &clkfqcy, &bufnum);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_setup_bd;
        }
        /* This is ((100 * clkfqcy) / (16 * (cfg->baudrate))) */
        uartdiv100m = ((25 * clkfqcy) / (cfg->baudrate << 2));
        mantissa = uartdiv100m / 100;
        fraction = uartdiv100m - (100 * mantissa);
        fraction = ((fraction * 16 + 50) / 100);
        LINFLEX_0.LINFBRR.B.DIV_F = fraction & (0x0F);
        LINFLEX_0.LINIBRR.B.DIV_M = mantissa;

        /* clear all flags */
        LINFLEX_0.LINSR.R = 0x0000F2BF;
        LINFLEX_0.UARTSR.R = 0xFFE7;
        /* enable IRQs */
        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                xwos_irq_clear(irqrsc->irqn);
                rc = xwos_irq_cfg(irqrsc->irqn, irqrsc->cfg);
                if (XWOK == rc) {
                        rc = xwos_irq_enable(irqrsc->irqn);
                }
                if (__xwcc_unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                irqrsc = &resources->irqrsc_array[j];
                                xwos_irq_disable(irqrsc->irqn);
                        }
                        goto err_en_irq;
                }
        }

        /* enable uart */
        LINFLEX_0.UARTPTO.R = xwccfg->pto;
        LINFLEX_0.LINIER.B.DBEIE = 1;
        LINFLEX_0.LINCR1.B.INIT = 0;
        LINFLEX_0.GCR.B.SR = 1;
        rc = mpc560xb_dmauart0_rxdma_setup(dmauartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_rxdma_setup;
        }

        return XWOK;

err_rxdma_setup:
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                xwos_irq_disable(irqrsc->irqn);
        }
err_en_irq:
err_setup_bd:
        for (j = (xwssz_t)resources->dmarsc_num - 1; j >= 0; j--) {
                dmarsc = &resources->dmarsc_array[j];
                xwds_dma_rls(dmarsc->soc, dmarsc->ch);
        }
err_req_dmas:
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
xwer_t mpc560xb_dmauart0_drv_stop(struct xwds_device * dev)
{
        struct xwds_dmauartc * dmauartc;
        struct mpc560xb_dmauart0_drvdata * drvdata;
        const struct xwds_resources * resources;
        const struct xwds_resource_dma * dmarsc;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        xwssz_t j;
        xwer_t rc;

        dmauartc = xwds_cast(struct xwds_dmauartc *, dev);
        resources = dmauartc->dev.resources;

        /* stop */
        LINFLEX_0.LINCR1.B.INIT = 1;

        /* disable IRQs */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_disable(irqrsc->irqn);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_irq_disable;
                }
        }

        /* release DMA resources */
        for (j = (xwssz_t)resources->dmarsc_num - 1; j >= 0; j--) {
                dmarsc = &resources->dmarsc_array[j];
                rc = xwds_dma_rls(dmarsc->soc, dmarsc->ch);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_dma_rls;
                }
        }

        /* release GPIO resources */
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                rc = xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_gpio_rls;
                }
        }

        /* release clock resources */
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                rc = xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_clk_rls;
                }
        }

        /* release IRQ resources */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_release(irqrsc->irqn);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_irq_rls;
                }
        }

        drvdata = dmauartc->dev.data;
        xwos_cond_fini(&drvdata->tx.cond);
        return XWOK;

err_irq_rls:
err_clk_rls:
err_dma_rls:
err_gpio_rls:
err_irq_disable:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_suspend(struct xwds_device * dev)
{
        struct xwds_dmauartc * dmauartc;
        const struct xwds_resources * resources;
        const struct xwds_resource_dma * dmarsc;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        xwssz_t j;
        xwer_t rc;

        dmauartc = xwds_cast(struct xwds_dmauartc *, dev);
        resources = dmauartc->dev.resources;

        /* stop */
        LINFLEX_0.LINCR1.B.INIT = 1;

        /* release DMA resources */
        for (j = (xwssz_t)resources->dmarsc_num - 1; j >= 0; j--) {
                dmarsc = &resources->dmarsc_array[j];
                rc = xwds_dma_rls(dmarsc->soc, dmarsc->ch);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_dma_rls;
                }
        }

        /* release GPIO resources */
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                rc = xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_gpio_rls;
                }
        }

        /* release clock resources */
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                rc = xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_clk_rls;
                }
        }
        return XWOK;

err_clk_rls:
err_gpio_rls:
err_dma_rls:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_resume(struct xwds_device * dev)
{
        struct xwds_dmauartc * dmauartc;
        const struct xwds_resources * resources;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_dma * dmarsc;
        xwssz_t i, j;
        xwer_t rc;

        dmauartc = xwds_cast(struct xwds_dmauartc *, dev);
        resources = dmauartc->dev.resources;

        /* request clock resources */
        for (i = 0; i < (xwssz_t)resources->clkrsc_num; i++) {
                clkrsc = &resources->clkrsc_array[i];
                rc = xwds_clk_req(clkrsc->soc, clkrsc->clkid);
                if (__xwcc_unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                clkrsc = &resources->clkrsc_array[j];
                                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                        }
                        goto err_req_clks;
                }
        }

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

        /* request DMA resources */
        for (i = 0; i < (xwssz_t)resources->dmarsc_num; i++) {
                dmarsc = &resources->dmarsc_array[i];
                rc = xwds_dma_req(dmarsc->soc, dmarsc->ch);
                if (__xwcc_unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                dmarsc = &resources->dmarsc_array[j];
                                xwds_dma_rls(dmarsc->soc, dmarsc->ch);
                        }
                        goto err_req_dmas;
                }
        }

        /* enable uart */
        LINFLEX_0.LINCR1.B.INIT = 0;
        LINFLEX_0.GCR.B.SR = 1;

        return XWOK;

err_req_dmas:
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

/******** ******** dmauart driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_cfg(struct xwds_dmauartc * dmauartc,
                                 const struct xwds_uart_cfg * cfg)
{
        XWOS_UNUSED(dmauartc);
        XWOS_UNUSED(cfg);
        return -ENOSYS;
}

static
void mpc560xb_dmauart0_tx_cb(struct xwds_soc * soc, xwid_t ch, xwu32_t rc,
                             xwds_dma_cbarg_t arg)
{
        struct xwds_dmauartc * dmauartc;
        struct mpc560xb_dmauart0_drvdata * drvdata;
        xwreg_t flag;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(ch);

        dmauartc = xwds_cast(struct xwds_dmauartc *, arg);
        drvdata = dmauartc->dev.data;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &flag);
        if (-ECANCELED != drvdata->tx.rc) {
                if (rc) {
                        drvdata->tx.rc = -EIO;
                } else {
                        drvdata->tx.rc = XWOK;
                }
        } else {
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, flag);
        xwos_cond_broadcast(&drvdata->tx.cond);
}

static __xwbsp_code
xwer_t mpc560xb_dmauart0_drv_tx(struct xwds_dmauartc * dmauartc,
                                const xwu8_t * srcmem, xwsz_t * size,
                                xwtm_t * xwtm)
{
        union xwos_ulock ulk;
        struct mpc560xb_dmauart0_drvdata * drvdata;
        const struct xwds_resource_dma * txdmarsc;
        struct soc_dmach_private_cfg * xwcdmacfg;
        xwsz_t wrsz;
        xwreg_t flag;
        xwsq_t lkst;
        xwer_t rc;

        wrsz = *size;
        drvdata = dmauartc->dev.data;
        txdmarsc = dmauartc->txdmarsc;
        xwcdmacfg = txdmarsc->xwccfg;
        xwcdmacfg->tcds[0].std.SADDR = (volatile xwu32_t)srcmem;
        xwcdmacfg->tcds[0].std.DADDR = ((volatile xwu32_t)&LINFLEX_0.BDRL) + 3;
        xwcdmacfg->tcds[0].std.SLAST = -((xws32_t)wrsz);
        xwcdmacfg->tcds[0].std.BITER = ((xwu16_t)wrsz);
        xwcdmacfg->tcds[0].std.CITER = ((xwu16_t)wrsz);

        rc = xwds_dma_cfg(txdmarsc->soc, txdmarsc->ch, (void *)&xwcdmacfg,
                          mpc560xb_dmauart0_tx_cb, dmauartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dma_cfg;
        }

        ulk.osal.splk = &drvdata->tx.splk;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &flag);
        drvdata->tx.rc = -EINPROGRESS;
        rc = xwds_dma_enable(txdmarsc->soc, txdmarsc->ch);
        if (XWOK == rc) {
                rc = xwos_cond_timedwait(&drvdata->tx.cond,
                                         ulk, XWOS_LK_SPLK, NULL,
                                         xwtm, &lkst);
                if (XWOK == rc) {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->tx.splk);
                        }
                        rc = drvdata->tx.rc;
                } else {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->tx.splk);
                        }
                        drvdata->tx.rc = -ECANCELED;
                }
        } else {
                drvdata->tx.rc = -ECANCELED;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, flag);
        if (rc < 0) {
                *size = 0;
        }
        return rc;

err_dma_cfg:
        return rc;
}

static __xwbsp_code
void mpc560xb_dmauart0_rxdma_cb(struct xwds_soc * soc, xwid_t ch, xwu32_t res,
                                xwds_dma_cbarg_t arg)
{
        struct xwds_dmauartc * dmauartc;
        const struct xwds_resource_dma * rxdmarsc;
        xwer_t rc;

        XWOS_UNUSED(soc);
        dmauartc = arg;
        rxdmarsc = dmauartc->rxdmarsc;
        if (0 == res) {
                if (EDMA.TCD[ch].COM.DONE) {
                        xwds_dmauartc_drvcb_rxq_pub(dmauartc, 0);
                        do {
                                rc = xwds_dma_enable(rxdmarsc->soc, rxdmarsc->ch);
                        } while (rc < 0);
                } else {
                        xwds_dmauartc_drvcb_rxq_pub(dmauartc, XWDS_DMAUART_RXQ_SIZE);
                }
        } else {
                XWDS_BUG();
        }
}

static __xwbsp_code
xwer_t mpc560xb_dmauart0_rxdma_setup(struct xwds_dmauartc * dmauartc)
{
        const struct xwds_resource_dma * rxdmarsc;
        struct soc_dmach_private_cfg * xwcdmacfg;
        xwer_t rc;

        rxdmarsc = dmauartc->rxdmarsc;
        xwcdmacfg = rxdmarsc->xwccfg;
        xwcdmacfg->tcds[0].std.SADDR = ((volatile xwu32_t)&LINFLEX_0.BDRM) + 3;
        xwcdmacfg->tcds[0].std.DADDR = (volatile xwu32_t)&dmauartc->rxq.mem[0];
        xwcdmacfg->tcds[0].std.DLAST_SGA = -((xws32_t)sizeof(dmauartc->rxq.mem));
        xwcdmacfg->tcds[0].std.BITER = ((xwu16_t)sizeof(dmauartc->rxq.mem));
        xwcdmacfg->tcds[0].std.CITER = ((xwu16_t)sizeof(dmauartc->rxq.mem));

        rc = xwds_dma_cfg(rxdmarsc->soc, rxdmarsc->ch, &xwcdmacfg,
                          mpc560xb_dmauart0_rxdma_cb, dmauartc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dma_cfg;
        }
        rc = xwds_dma_enable(rxdmarsc->soc, rxdmarsc->ch);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dma_en;
        }
        return XWOK;

err_dma_en:
err_dma_cfg:
        return rc;
}

__xwbsp_isr
void mpc560xb_dmauart0_err_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_dmauartc * dmauartc;
        const struct xwds_resource_dma * rxdmarsc;
        xwid_t ch;
        xwsq_t tail;

        xwos_irq_get_data(IRQ_LINFLEX0_ERR, &irqdata);
        dmauartc = irqdata.data;

        if (LINFLEX_0.UARTSR.B.TO) {
                rxdmarsc = dmauartc->rxdmarsc;
                ch = rxdmarsc->ch;
                tail = (xwsq_t)(EDMA.TCD[ch].COM.DADDR - (xwu32_t)dmauartc->rxq.mem);
                if (!(EDMA.IRQRL.R & XWBOP_BIT(ch))) {
                        xwds_dmauartc_drvcb_rxq_pub(dmauartc, tail);
                }/* else {} */
        }
        LINFLEX_0.UARTSR.R = 0x0000FFEF;
}
