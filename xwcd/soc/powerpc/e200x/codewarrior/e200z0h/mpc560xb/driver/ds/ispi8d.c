/**
 * @file
 * @brief 设备栈驱动：inter-SPI 8-bit mode communication (slave device driver)
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
#include <xwos/lib/string.h>
#include <xwos/lib/linid.h>
#include <xwos/lib/xwlog.h>
#include <xwos/irq.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/clock.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/spi/ispi8d.h>
#include <driver/ds/ispi8d.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_ispi8d_check_desc(struct xwds_ispi8d * ispi8d);

static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_stop(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_resume(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_notify(struct xwds_ispi8d * ispi8d, xwsq_t state);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct xwds_ispi8d_driver mpc560xb_ispi8d_drv = {
        .base = {
                .name = "mpc560xb.dspi.ispi8d",
                .probe = mpc560xb_ispi8d_drv_probe,
                .remove = NULL,
                .start = mpc560xb_ispi8d_drv_start,
                .stop = mpc560xb_ispi8d_drv_stop,
                .suspend = mpc560xb_ispi8d_drv_suspend,
                .resume =  mpc560xb_ispi8d_drv_resume,
        },
        .notify = mpc560xb_ispi8d_drv_notify,
        .swcs = NULL,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_ispi8d_check_desc(struct xwds_ispi8d * ispi8d)
{
        const struct xwds_resources * resources;
        const struct xwds_ispi8d_cfg * cfg;
        xwer_t rc;

        resources = ispi8d->dev.resources;
        cfg = ispi8d->cfg;
        if (__unlikely(is_err_or_null(resources))) {
                rc = -EINVAL;
        } else if (__unlikely(is_err_or_null(cfg))) {
                rc = -EINVAL;
        } else {
                rc = OK;
        }
        return rc;
}

/******** ******** common driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_probe(struct xwds_device * dev)
{
        struct xwds_ispi8d * ispi8d;
        xwer_t rc;

        ispi8d = xwds_static_cast(struct xwds_ispi8d *, dev);
        rc = mpc560xb_ispi8d_check_desc(ispi8d);
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_start(struct xwds_device * dev)
{
        struct xwds_ispi8d * ispi8d;
        const struct xwds_resources * resources;
        const struct xwds_ispi8d_cfg * cfg;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        struct DSPI_tag * reg;
        xwssz_t i, j;
        xwer_t rc;

        ispi8d = xwds_static_cast(struct xwds_ispi8d *, dev);
        resources = ispi8d->dev.resources;
        reg = resources->regrsc_array[0].base;
        cfg = ispi8d->cfg;

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

        reg->MCR.R = 0x083F4C01;
        /* Master/slave mode select: Slave
           Continuous SCK enable: Unused
           DSPI configuration: SPI
           Freeze in debug mode: ON
           Modified timing format enable: OFF
           Peripheral chip select strobe enable: OFF
           Receive FIFO overflow overwrite enable: OFF
           Peripheral chip select inactive state: High (Slave mode must be high)
           Module disable: ON
           transmit FIFO: ON
           receive FIFO: ON
           Clear TX FIFO: Clear
           Clear RX FIFO: Clear
           Sample point: 1
           Halt: ON
        */

        reg->TCR.R = 0;

        reg->CTAR[0].R = 0x3B000000;
        /* Double Baud Rate: Not used
           Frame Size: 8 bits
           Clock Polarity: defined later
           Clock Phase: defined later
           LSB First: Yes
           PCS to SCK Delay Prescaler: Not used
           After SCK Delay Prescaler: Not used
           Delay after Transfer Prescaler: Not used
           Baud Rate Prescaler: Not used
           PCS to SCK Delay Scaler: Not used
           After SCK Delay Scaler: Not used
           Delay after Transfer Scaler: Not used
           Baud Rate Scaler: Not used
        */

        reg->SR.R = 0x9A0A0000; /* clear IRQ flags */

        reg->CTAR[0].B.CPOL = cfg->bus.bit.cpol; /* clock polarity */
        reg->CTAR[0].B.CPHA = cfg->bus.bit.cpha; /* clock phase */
        reg->CTAR[0].B.LSBFE = cfg->bus.bit.frmodr; /* LSB or MSB */

        /* enable IRQs */
        reg->RSER.R = 0x00020000;
        /* TCF_RE: OFF
           EOQF_RE: OFF
           TFUF_RE: OFF
           TFFF_RE: OFF
           TFFF_DIRS: IRQ
           RFOF_RE: OFF
           RFDF_RE: ON
           RFDF_DIRS: IRQ
        */

        /* enable IRQ */
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

        /* enable spi */
        reg->MCR.B.MDIS = 0;
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
xwer_t mpc560xb_ispi8d_drv_stop(struct xwds_device * dev)
{
        struct xwds_ispi8d * ispi8d;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        volatile struct DSPI_tag * reg;
        xwssz_t j;
        xwer_t rc;

        ispi8d = xwds_static_cast(struct xwds_ispi8d *, dev);
        resources = ispi8d->dev.resources;
        reg = resources->regrsc_array[0].base;

        /* stop */
        reg->MCR.B.MDIS = 1;

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
xwer_t mpc560xb_ispi8d_drv_suspend(struct xwds_device * dev)
{
        struct xwds_ispi8d * ispi8d;
        const struct xwds_resources * resources;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_gpio * gpiorsc;
        volatile struct DSPI_tag * reg;
        xwssz_t j;
        xwer_t rc;

        ispi8d = xwds_static_cast(struct xwds_ispi8d *, dev);
        resources = ispi8d->dev.resources;
        reg = resources->regrsc_array[0].base;

        /* stop */
        reg->MCR.B.MDIS = 1;

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

        return OK;

err_clk_release:
err_gpio_release:
err_irq_disable:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_resume(struct xwds_device * dev)
{
        struct xwds_ispi8d * ispi8d;
        const struct xwds_resources * resources;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_gpio * gpiorsc;
        volatile struct DSPI_tag * reg;
        xwssz_t i, j;
        xwer_t rc;

        ispi8d = xwds_static_cast(struct xwds_ispi8d *, dev);
        resources = ispi8d->dev.resources;
        reg = resources->regrsc_array[0].base;

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

        /* start */
        reg->MCR.B.MDIS = 0;

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

/******** ******** ispi8d driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_ispi8d_drv_notify(struct xwds_ispi8d * ispi8d, xwsq_t state)
{
        const struct xwds_ispi8d_cfg * cfg;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        xwer_t rc;

        cfg = ispi8d->cfg;
        resources = ispi8d->dev.resources;
        rc = xwds_get_gpiorsc(resources->gpiorsc_array, resources->gpiorsc_num,
                              "int_master", &gpiorsc);
        if (__likely(OK == rc)) {
                if (SODS_ISPI8D_NTFM_HIGH == cfg->bus.bit.ntfmode) {
                        if (SODS_ISPI8D_DATA_AVAILABLE == state) {
                                rc = xwds_gpio_set(gpiorsc->soc, gpiorsc->port,
                                                   gpiorsc->pinmask);
                        } else {
                                rc = xwds_gpio_reset(gpiorsc->soc, gpiorsc->port,
                                                     gpiorsc->pinmask);
                        }
                } else {
                        if (SODS_ISPI8D_DATA_AVAILABLE == state) {
                                rc = xwds_gpio_reset(gpiorsc->soc, gpiorsc->port,
                                                     gpiorsc->pinmask);
                        } else {
                                rc = xwds_gpio_set(gpiorsc->soc, gpiorsc->port,
                                                   gpiorsc->pinmask);
                        }
                }
        } else {
        }
        return rc;
}

/******** ******** ispi8d ISR ******** ********/
__xwbsp_isr
void mpc560xb_ispi8d_isr_tfuf_rfof(void)
{
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_ispi8d * ispi8d;
        const struct xwds_resources * resources;
        volatile struct DSPI_tag * reg;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        ispi8d = irqdata.data;
        resources = ispi8d->dev.resources;
        reg = resources->regrsc_array[0].base;

        if (reg->SR.B.TFUF) {
                reg->SR.B.TFUF = 1;
                xwisrlogf(ERR, "TX queue underflow!");
        }
        if (reg->SR.B.RFOF) {
                reg->SR.B.RFOF = 1;
                xwisrlogf(ERR, "RX queue overflow!");
        }
}

__xwbsp_isr
void mpc560xb_ispi8d_isr_rfdf(void)
{
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_ispi8d * ispi8d;
        const struct xwds_resources * resources;
        volatile struct DSPI_tag * reg;
        xwu8_t data;
        struct xwds_ispi8d_txslot * txslot;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        ispi8d = irqdata.data;
        resources = ispi8d->dev.resources;
        reg = resources->regrsc_array[0].base;

        xwosal_splk_lock_cpuirq(&ispi8d->comi.lock);
        switch (ispi8d->comi.state) {
        case SODS_ISPI8D_STATE_IDLE:
                reg->SR.B.TFUF = 1;
                data = (xwu8_t)(reg->POPR.R & 0xFF);
                reg->SR.B.RFDF = 1;
                rc = xwds_ispi8d_lib_sync(ispi8d, data, &txslot);
                if (OK == rc) {
                        reg->PUSHR.R = txslot->size;
                }
                break;
        case SODS_ISPI8D_STATE_SYNC:
                data = (xwu8_t)(reg->POPR.R & 0xFF);
                reg->SR.B.RFDF = 1;
                rc = xwds_ispi8d_lib_swapsize(ispi8d, data, &txslot);
                if ((OK == rc) && (txslot)) {
                        while (reg->SR.B.TFFF) {
                                reg->PUSHR.R = txslot->data[txslot->pos];
                                txslot->pos++;
                                if (txslot->pos == txslot->size) {
                                        break;
                                }
                        }
                }
                reg->RSER.R |= 0x0808; /* enable TFUF & RFOF IRQ */
                break;
        case SODS_ISPI8D_STATE_XDATA:
                data = (xwu8_t)(reg->POPR.R & 0xFF);
                reg->SR.B.RFDF = 1;
                rc = xwds_ispi8d_lib_swapdata(ispi8d, &data, 1, &txslot);
                if ((-EINPROGRESS == rc) && (txslot)) {
                        while (reg->SR.B.TFFF) {
                                reg->PUSHR.R = txslot->data[txslot->pos];
                                txslot->pos++;
                                if (txslot->pos == txslot->size) {
                                        break;
                                }
                        }
                }
                if (OK == rc) {
                        reg->RSER.R = 0x00020000;
                }
                break;
        }
        xwosal_splk_unlock_cpuirq(&ispi8d->comi.lock);
}
