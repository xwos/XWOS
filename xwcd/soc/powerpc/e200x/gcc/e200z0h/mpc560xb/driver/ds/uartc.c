/**
 * @file
 * @brief 设备栈驱动：MPC560xB UART
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#include <xwos/lib/xwlog.h>
#include <xwos/irq.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/clock.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/uart/general.h>
#include <driver/ds/uart.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_uartc_check_desc(struct xwds_uartc * uartc);

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_stop(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_resume(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_cfg(struct xwds_uartc * uartc,
                              const struct xwds_uart_cfg * cfg);

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_tx(struct xwds_uartc * uartc, xwu8_t byte);

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_rx(struct xwds_uartc * uartc, xwu8_t * rxbuf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct xwds_uartc_driver mpc560xb_uartc_drv = {
        .base = {
                .name = "mpc560xb.linflex.uart",
                .probe = mpc560xb_uartc_drv_probe,
                .remove = NULL,
                .start = mpc560xb_uartc_drv_start,
                .stop = mpc560xb_uartc_drv_stop,
                .suspend = mpc560xb_uartc_drv_suspend,
                .resume =  mpc560xb_uartc_drv_resume,
        },
        .cfg = mpc560xb_uartc_drv_cfg,
        .tx = mpc560xb_uartc_drv_tx,
        .rx = mpc560xb_uartc_drv_rx,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_uartc_check_desc(struct xwds_uartc * uartc)
{
        const struct xwds_resources * resources;
        const struct xwds_uart_cfg * cfg;
        xwer_t rc;

        resources = uartc->dev.resources;
        if (__unlikely(is_err_or_null(resources))) {
                rc = -EINVAL;
        } else {
                cfg = uartc->cfg;
                if (__unlikely(is_err_or_null(cfg)))
                        rc = -EINVAL;
                else
                        rc = OK;
        }
        return rc;
}

/******** ******** common driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_uartc_drv_probe(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;

        uartc = xwds_static_cast(struct xwds_uartc *, dev);
        return mpc560xb_uartc_check_desc(uartc);
}

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_start(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_resources * resources;
        volatile struct LINFLEX_tag * linflex_reg;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_uart_cfg * cfg;
        xwssz_t i, j;
        xwer_t rc;

        uartc = xwds_static_cast(struct xwds_uartc *, dev);
        resources = uartc->dev.resources;
        linflex_reg = resources->regrsc_array[0].base;
        cfg = uartc->cfg;

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

        /* request GPIOs */
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

        /* setup uart */
        linflex_reg->LINCR1.B.INIT = 1;
        linflex_reg->LINCR1.B.SLEEP = 0;

        linflex_reg->UARTCR.B.UART = 1;
        /* word length */
        linflex_reg->UARTCR.B.WL = cfg->bus.bit.bits;

        if (SODS_UART_PARITY_NONE == cfg->bus.bit.parity) {
                linflex_reg->UARTCR.B.PCE = 0;
        } else {
                linflex_reg->UARTCR.B.PCE = 1; /* enable parity */
                if (SODS_UART_PARITY_ODD == cfg->bus.bit.parity) {
                        linflex_reg->UARTCR.B.OP = 1;
                } else {
                        linflex_reg->UARTCR.B.OP = 0;
                }
        }

        if (SODS_UART_MODE_TX & cfg->bus.bit.mode) {
                linflex_reg->UARTCR.B.TDFL = 0;
                linflex_reg->UARTCR.B.TXEN = 1;
        }
        if (SODS_UART_MODE_RX & cfg->bus.bit.mode) {
                linflex_reg->UARTCR.B.RDFL = 0;
                linflex_reg->UARTCR.B.RXEN = 1;
        }
        linflex_reg->LINIER.B.DTIE = 1; /* enable tx completion interrupt */
        linflex_reg->LINIER.B.DRIE = 1; /* enable rx interrupt */

        /* baudrate */
        xwu32_t clkfqcy;
        xwsz_t bufnum;
        xwu32_t uartdiv100m;
        xwu32_t mantissa;
        xwu32_t fraction;

        clkrsc = &resources->clkrsc_array[0];
        bufnum = 1;
        rc = xwds_clk_getfqcy(clkrsc->soc, clkrsc->clkid, &clkfqcy, &bufnum);
        if (__unlikely(rc < 0)) {
                goto err_setup_bd;
        }
        /* This is ((100 * clkfqcy) / (16 * (cfg->baudrate))) */
        uartdiv100m = ((25 * clkfqcy) / (cfg->baudrate << 2));
        mantissa = uartdiv100m / 100;
        fraction = uartdiv100m - (100 * mantissa);
        fraction = ((fraction * 16 + 50) / 100);
        linflex_reg->LINFBRR.B.DIV_F = fraction & (0x0F);
        linflex_reg->LINIBRR.B.DIV_M = mantissa;

        /* clear all flags */
        linflex_reg->LINSR.R = 0x0000F2BF;
        linflex_reg->UARTSR.R = 0xFFE7;
        /* enable irqs */
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

        /* enable uart */
        linflex_reg->LINCR1.B.INIT = 0;

        return OK;

err_en_irq:
err_setup_bd:
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
xwer_t mpc560xb_uartc_drv_stop(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_resources * resources;
        volatile struct LINFLEX_tag * linflex_reg;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        xwssz_t j;
        xwer_t rc;

        uartc = xwds_static_cast(struct xwds_uartc *, dev);
        resources = uartc->dev.resources;
        linflex_reg = resources->regrsc_array[0].base;

        /* stop */
        linflex_reg->LINCR1.B.INIT = 1;
        linflex_reg->LINIER.B.DTIE = 0; /* disable tx completion interrupt */
        linflex_reg->LINIER.B.DRIE = 0; /* disable rx interrupt */

        /* disable IRQs */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
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
err_irq_disable:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_suspend(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_resources * resources;
        volatile struct LINFLEX_tag * linflex_reg;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        xwssz_t j;
        xwer_t rc;

        uartc = xwds_static_cast(struct xwds_uartc *, dev);
        resources = uartc->dev.resources;
        linflex_reg = resources->regrsc_array[0].base;

        /* stop */
        linflex_reg->LINCR1.B.INIT = 1;
        linflex_reg->LINIER.B.DTIE = 0; /* disable tx completion interrupt */
        linflex_reg->LINIER.B.DRIE = 0; /* disable rx interrupt */

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
xwer_t mpc560xb_uartc_drv_resume(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_resources * resources;
        volatile struct LINFLEX_tag * linflex_reg;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t i, j;
        xwer_t rc;

        uartc = xwds_static_cast(struct xwds_uartc *, dev);
        resources = uartc->dev.resources;
        linflex_reg = resources->regrsc_array[0].base;

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

        /* request GPIOs */
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

        /* enable uart */
        linflex_reg->UARTSR.R = 0xFFE7;
        linflex_reg->LINIER.B.DTIE = 1; /* enable tx completion interrupt */
        linflex_reg->LINIER.B.DRIE = 1; /* enable rx interrupt */
        linflex_reg->LINCR1.B.INIT = 0;

        return OK;

err_req_gpios:
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
        }
err_req_clks:
        return rc;
}

/******** ******** uart driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_uartc_drv_cfg(struct xwds_uartc * uartc,
                              const struct xwds_uart_cfg * cfg)
{
        XWOS_UNUSED(uartc);
        XWOS_UNUSED(cfg);
        return -EPERM;
}

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_tx(struct xwds_uartc * uartc, xwu8_t byte)
{
        const struct xwds_resources * resources;
        volatile struct LINFLEX_tag * linflex_reg;

        resources = uartc->dev.resources;
        linflex_reg = resources->regrsc_array[0].base;
        linflex_reg->BDRL.B.DATA0 = byte;
        return OK;
}

static __xwbsp_code
xwer_t mpc560xb_uartc_drv_rx(struct xwds_uartc * uartc, xwu8_t * rxbuf)
{
        const struct xwds_resources * resources;
        volatile struct LINFLEX_tag * linflex_reg;

        resources = uartc->dev.resources;
        linflex_reg = resources->regrsc_array[0].base;
        linflex_reg->UARTSR.B.DRF = 1;
        *rxbuf = (xwu8_t)linflex_reg->BDRM.B.DATA4;
        linflex_reg->UARTSR.B.RMB = 1;
        return OK;
}

/******** ******** uart isr ******** ********/
__xwbsp_isr
void mpc560xb_uartc_rx_isr(void)
{
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_uartc * uartc;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        uartc = irqdata.data;
        xwds_uartc_lib_rx_isr(uartc);
}

__xwbsp_isr
void mpc560xb_uartc_tx_isr(void)
{
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_uartc * uartc;
        const struct xwds_resources * resources;
        volatile struct LINFLEX_tag * linflex_reg;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        uartc = irqdata.data;
        resources = uartc->dev.resources;
        linflex_reg = resources->regrsc_array[0].base;
        linflex_reg->UARTSR.B.DTF = 1;
        xwds_uartc_lib_tx_isr(uartc);
}

__xwbsp_isr
void mpc560xb_uartc_err_isr(void)
{
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_uartc * uartc;
        const struct xwds_resources * resources;
        volatile struct LINFLEX_tag * linflex_reg;
        xwu32_t sr;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        uartc = irqdata.data;
        resources = uartc->dev.resources;
        linflex_reg = resources->regrsc_array[0].base;
        sr = linflex_reg->UARTSR.R;
        xwisrlogf(ERR, "<%s:%d> error! sr: 0x%x", uartc->dev.name, uartc->dev.id, sr);
        linflex_reg->UARTSR.R = 0x0000FFE1;
}
