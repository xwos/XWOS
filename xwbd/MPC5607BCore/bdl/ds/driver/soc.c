/**
 * @file
 * @brief 设备栈驱动：MPC560xB SOC
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

#include <string.h>
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/irq.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/soc/clock.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>
#include <xwcd/ds/soc/dma.h>
#include <soc.h>
#include <soc_me.h>
#include <soc_clk.h>
#include <soc_gpio.h>
#include <soc_eirq.h>
#include <soc_dma.h>
#include <bdl/ds/driver/soc.h>

#if defined(XWCDCFG_ds_SOC_DMA) && (1 == XWCDCFG_ds_SOC_DMA)
#  if defined(MPC5607B)
#    define EDMA_CH_ISR(x)                                              \
__xwbsp_isr                                                             \
void mpc560xb_edma_ch##x##_isr(void)                                    \
{                                                                       \
        xwirq_t irqn;                                                   \
        struct soc_irq_data irqdata;                                    \
        struct xwds_soc * soc;                                          \
        xwsid_t ch;                                                     \
                                                                        \
        xwos_irq_get_id(&irqn);                                         \
        xwos_irq_get_data(irqn, &irqdata);                              \
        soc = irqdata.data;                                             \
        ch = x;                                                         \
        if ((soc->dma.chcbs) && (soc->dma.chcbs[ch])) {                 \
                if (soc->dma.chcbargs) {                                \
                        soc->dma.chcbs[ch](soc, (xwid_t)ch, 0,          \
                                           soc->dma.chcbargs[ch]);      \
                } else {                                                \
                        soc->dma.chcbs[ch](soc, (xwid_t)ch, 0, NULL);   \
                }                                                       \
        }                                                               \
        if (EDMA.TCD[ch].COM.DONE) {                                    \
                EDMA.CDSBR.R = (xwu8_t)ch;                              \
        }                                                               \
        EDMA.CIRQR.R = (xwu8_t)ch;                                      \
}
#  endif
#endif

static __xwbsp_code
xwer_t mpc560xb_soc_check_desc(struct xwds_soc * soc);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_stop(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_resume(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_clk_req(struct xwds_soc * soc, xwid_t id);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_clk_rls(struct xwds_soc * soc, xwid_t id);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_clk_getfqcy(struct xwds_soc * soc, xwid_t id,
                                    xwu32_t * buf, xwsz_t * num);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_cfg(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                 void * cfg);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_set(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_reset(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_toggle(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_output(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                    xwsq_t out);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_input(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                   xwsq_t * in);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                 xwid_t eiid, xwsq_t eiflag);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                 xwid_t eiid);

#if defined(XWCDCFG_ds_SOC_DMA) && (1 == XWCDCFG_ds_SOC_DMA)
static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_req(struct xwds_soc * soc, xwid_t ch);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_rls(struct xwds_soc * soc, xwid_t ch);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_cfg(struct xwds_soc * soc, xwid_t ch,
                                void * cfg);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_enable(struct xwds_soc * soc, xwid_t ch);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_disable(struct xwds_soc * soc, xwid_t ch);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_start(struct xwds_soc * soc, xwid_t ch);

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_stop(struct xwds_soc * soc, xwid_t ch);
#endif

__xwbsp_rodata const struct xwds_soc_driver mpc560xb_soc_drv = {
        .base = {
                .name = "mpc560xb.soc",
                .probe = mpc560xb_soc_drv_probe,
                .remove = NULL,
                .start = mpc560xb_soc_drv_start,
                .stop = mpc560xb_soc_drv_stop,
                .suspend = mpc560xb_soc_drv_suspend,
                .resume = mpc560xb_soc_drv_resume,
        },
        .clk_req = mpc560xb_soc_drv_clk_req,
        .clk_rls = mpc560xb_soc_drv_clk_rls,
        .clk_getfqcy = mpc560xb_soc_drv_clk_getfqcy,

        .gpio_req = mpc560xb_soc_drv_gpio_req,
        .gpio_rls = mpc560xb_soc_drv_gpio_rls,
        .gpio_cfg = mpc560xb_soc_drv_gpio_cfg,
        .gpio_set = mpc560xb_soc_drv_gpio_set,
        .gpio_reset = mpc560xb_soc_drv_gpio_reset,
        .gpio_toggle = mpc560xb_soc_drv_gpio_toggle,
        .gpio_output = mpc560xb_soc_drv_gpio_output,
        .gpio_input = mpc560xb_soc_drv_gpio_input,

        .eirq_req = mpc560xb_soc_drv_eirq_req,
        .eirq_rls = mpc560xb_soc_drv_eirq_rls,

#if defined(XWCDCFG_ds_SOC_DMA) && (1 == XWCDCFG_ds_SOC_DMA)
        .dma_req = mpc560xb_soc_drv_dma_req,
        .dma_rls = mpc560xb_soc_drv_dma_rls,
        .dma_cfg = mpc560xb_soc_drv_dma_cfg,
        .dma_enable = mpc560xb_soc_drv_dma_enable,
        .dma_disable = mpc560xb_soc_drv_dma_disable,
        .dma_start = mpc560xb_soc_drv_dma_start,
        .dma_stop = mpc560xb_soc_drv_dma_stop,
#endif
};

static __xwbsp_code
xwer_t mpc560xb_soc_check_desc(struct xwds_soc * soc)
{
        const struct xwds_resources * resources;
        const struct soc_sys_cfg * syscfg;
        const struct soc_gpio_pincfg (*pincfgs)[SOC_GPIO_PIN_NUM];
        xwer_t rc;

        resources = soc->dev.resources;
        syscfg = soc->xwccfg;
        pincfgs = soc->gpio.cfg;
        if (is_err_or_null(resources)) {
                rc = -EINVAL;
        } else if (is_err_or_null(syscfg)) {
                rc = -EINVAL;
        } else if (is_err_or_null(pincfgs)) {
                rc = -EINVAL;
        } else {
                rc = XWOK;
        }
        return rc;
}

/******** ******** common driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_soc_drv_probe(struct xwds_device * dev)
{
        struct xwds_soc * soc;

        soc = xwds_cast(struct xwds_soc *, dev);
        return mpc560xb_soc_check_desc(soc);
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_start(struct xwds_device * dev)
{
        struct xwds_soc * soc;
        const struct xwds_resources * resources;
        const struct soc_sys_cfg * syscfg;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t i, j;
        xwer_t rc;

        soc = xwds_cast(struct xwds_soc *, dev);
        resources = soc->dev.resources;
        syscfg = soc->xwccfg;

        /* start clock controllers: do_nothing(); */
        /* start GPIO controllers: do_nothing() */

        /* request irqs */
        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr, soc, NULL);
                if (__xwcc_unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                irqrsc = &resources->irqrsc_array[j];
                                xwos_irq_release(irqrsc->irqn);
                        }
                        goto err_req_irqs;
                }
        }

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

        /* Pad Selection for Multiplexed Inputs */
        for (i = 0; i < (xwssz_t)SOC_PADSEL_NUM; i++) {
                SIU.PSMI[i].R = syscfg->padsel[i];
        }

#if defined(XWCDCFG_ds_SOC_DMA) && (1 == XWCDCFG_ds_SOC_DMA)
#  if defined(MPC5607B)
        /* DMAC setup */
        const struct soc_dmac_private_cfg * dmaccfg;

        dmaccfg = soc->dma.ccfg;
        EDMA.CR.R = dmaccfg->cr.u32;
        for (i = 0; i < (xwssz_t)SOC_DMAC_CHANNEL_NUM; i++) {
                EDMA.CPR[i].R = dmaccfg->cpr[i].u8;
        }
#  endif
#endif

        /* clear all EIRQ flags */
        WKUP.WISR.R = 0x1FFFFFFFU;
        SIU.ISR.R = 0x00FFFFFFU;

        /* enable irqs */
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
        return XWOK;

err_en_irq:
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
xwer_t mpc560xb_soc_drv_stop(struct xwds_device * dev)
{
        struct xwds_soc * soc;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        xwssz_t j;
        xwer_t rc;

        soc = xwds_cast(struct xwds_soc *, dev);
        resources = soc->dev.resources;

        /* disable IRQs */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_disable(irqrsc->irqn);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_irq_disable;
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
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_release(irqrsc->irqn);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_irq_release;
                }
        }
        return XWOK;

err_irq_release:
err_clk_rls:
err_gpio_rls:
err_irq_disable:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_suspend(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_resume(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

/******** ******** clock driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_soc_drv_clk_req(struct xwds_soc * soc, xwid_t id)
{
        xwreg_t cpuirq;
        xwer_t rc;
        xwu32_t idx;

        XWOS_UNUSED(soc);
        rc = XWOK;
        idx = MPC5_ME_RUNMODE - MPC5_ME_MODE_RUN0;
        xwos_cpuirq_save_lc(&cpuirq);
        switch (id) {
        case MPC560XB_CLK_FXOSC:
                ME.RUN[idx].B.FXOSCON = 1;
                break;
        case MPC560XB_CLK_FIRC:
                ME.RUN[idx].B.FIRCON = 1;
                break;
        case MPC560XB_CLK_SCOSC:
                CGM.SXOSC_CTL.B.OSCON = 1;
                break;
        case MPC560XB_CLK_SIRC:
                CGM.SIRC_CTL.B.SIRCON_STDBY = 1;
                break;
        case MPC560XB_CLK_FMPLL:
                ME.RUN[idx].B.FMPLLON = 1;
                break;
        case MPC560XB_CLK_SYSCLK:
                rc = -ENOSYS;
                break;
        case MPC560XB_CLK_PC1:
                CGM.SC_DC[0].B.DE = 1;
                break;
        case MPC560XB_CLK_PC2:
                CGM.SC_DC[1].B.DE = 1;
                break;
        case MPC560XB_CLK_PC3:
                CGM.SC_DC[2].B.DE = 1;
                break;
        case MPC560XB_CLK_ADC_0:
                ME.PCTL[32].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;

#if defined(MPC5607B)
        case MPC560XB_CLK_ADC_1:
                ME.PCTL[33].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
#endif

        case MPC560XB_CLK_CAN_SAMPLER:
                ME.PCTL[60].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_CMU:
                ME.PCTL[104].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_CTU:
                ME.PCTL[57].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_DMA_MUX:
                ME.PCTL[23].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_DSPI_0:
                ME.PCTL[4].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_DSPI_1:
                ME.PCTL[5].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_DSPI_2:
                ME.PCTL[6].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_DSPI_3:
                ME.PCTL[7].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;

#if defined(MPC5607B)
        case MPC560XB_CLK_DSPI_4:
                ME.PCTL[8].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_DSPI_5:
                ME.PCTL[9].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
#endif

        case MPC560XB_CLK_EMIOS_0:
                ME.PCTL[72].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_EMIOS_1:
                ME.PCTL[73].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_FLEXCAN_0:
                ME.PCTL[16].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_FLEXCAN_1:
                ME.PCTL[17].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_FLEXCAN_2:
                ME.PCTL[18].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_FLEXCAN_3:
                ME.PCTL[19].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_FLEXCAN_4:
                ME.PCTL[20].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_FLEXCAN_5:
                ME.PCTL[21].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_I2C:
                ME.PCTL[44].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_0:
                ME.PCTL[48].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_1:
                ME.PCTL[49].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_2:
                ME.PCTL[50].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_3:
                ME.PCTL[51].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;

#if defined(MPC5607B)
        case MPC560XB_CLK_LINFLEX_4:
                ME.PCTL[52].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_5:
                ME.PCTL[53].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_6:
                ME.PCTL[54].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_7:
                ME.PCTL[55].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_8:
                ME.PCTL[12].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
        case MPC560XB_CLK_LINFLEX_9:
                ME.PCTL[13].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                break;
#endif

        case MPC560XB_CLK_PIT:
                ME.PCTL[92].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE;
                ME.PCTL[92].B.LP_CFG = MPC5_ME_LPPC_ACTIVE_HALT;
                break;
        case MPC560XB_CLK_RTC:
                ME.PCTL[91].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE_ALL;
                ME.PCTL[91].B.LP_CFG = MPC5_ME_LPPC_ACTIVE_ALL_LPM;
                break;
        case MPC560XB_CLK_SIUL:
                ME.PCTL[68].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE_ALL;
                break;
        case MPC560XB_CLK_WKUP:
                ME.PCTL[69].B.RUN_CFG = MPC5_ME_RUNPC_ACTIVE_ALL;
                ME.PCTL[69].B.LP_CFG = MPC5_ME_LPPC_ACTIVE_ALL_LPM;
                break;
        default:
                rc = -EINVAL;
                break;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        if (XWOK == rc) {
                soc_me_enter_mode(MPC5_ME_RUNMODE);
        }
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_clk_rls(struct xwds_soc * soc, xwid_t id)
{
        xwreg_t cpuirq;
        xwer_t rc;
        xwsq_t idx;

        XWOS_UNUSED(soc);
        rc = XWOK;
        idx = MPC5_ME_RUNMODE - MPC5_ME_MODE_RUN0;
        xwos_cpuirq_save_lc(&cpuirq);
        switch (id) {
        case MPC560XB_CLK_FXOSC:
                ME.RUN[idx].B.FXOSCON = 0;
                break;
        case MPC560XB_CLK_FIRC:
                ME.RUN[idx].B.FIRCON = 0;
                break;
        case MPC560XB_CLK_SCOSC:
                CGM.SXOSC_CTL.B.OSCON = 0;
                break;
        case MPC560XB_CLK_SIRC:
                CGM.SIRC_CTL.B.SIRCON_STDBY = 0;
                break;
        case MPC560XB_CLK_FMPLL:
                ME.RUN[idx].B.FMPLLON = 0;
                break;
        case MPC560XB_CLK_SYSCLK:
                rc = -ENOSYS;
                break;
        case MPC560XB_CLK_PC1:
                CGM.SC_DC[0].B.DE = 0;
                break;
        case MPC560XB_CLK_PC2:
                CGM.SC_DC[1].B.DE = 0;
                break;
        case MPC560XB_CLK_PC3:
                CGM.SC_DC[2].B.DE = 0;
                break;
        case MPC560XB_CLK_ADC_0:
                ME.PCTL[32].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;

#if defined(MPC5607B)
        case MPC560XB_CLK_ADC_1:
                ME.PCTL[33].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
#endif

        case MPC560XB_CLK_CAN_SAMPLER:
                ME.PCTL[60].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_CMU:
                ME.PCTL[104].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_CTU:
                ME.PCTL[57].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_DMA_MUX:
                ME.PCTL[23].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_DSPI_0:
                ME.PCTL[4].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_DSPI_1:
                ME.PCTL[5].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_DSPI_2:
                ME.PCTL[6].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_DSPI_3:
                ME.PCTL[7].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;

#if defined(MPC5607B)
        case MPC560XB_CLK_DSPI_4:
                ME.PCTL[8].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_DSPI_5:
                ME.PCTL[9].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
#endif

        case MPC560XB_CLK_EMIOS_0:
                ME.PCTL[72].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_EMIOS_1:
                ME.PCTL[73].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_FLEXCAN_0:
                ME.PCTL[16].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_FLEXCAN_1:
                ME.PCTL[17].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_FLEXCAN_2:
                ME.PCTL[18].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_FLEXCAN_3:
                ME.PCTL[19].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_FLEXCAN_4:
                ME.PCTL[20].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_FLEXCAN_5:
                ME.PCTL[21].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_I2C:
                ME.PCTL[44].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_0:
                ME.PCTL[48].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_1:
                ME.PCTL[49].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_2:
                ME.PCTL[50].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_3:
                ME.PCTL[51].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;

#if defined(MPC5607B)
        case MPC560XB_CLK_LINFLEX_4:
                ME.PCTL[52].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_5:
                ME.PCTL[53].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_6:
                ME.PCTL[54].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_7:
                ME.PCTL[55].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_8:
                ME.PCTL[12].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_LINFLEX_9:
                ME.PCTL[13].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
#endif

        case MPC560XB_CLK_PIT:
                ME.PCTL[92].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                ME.PCTL[92].B.LP_CFG = MPC5_ME_LPPC_FROZEN;
                break;
        case MPC560XB_CLK_RTC:
                ME.PCTL[91].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                ME.PCTL[91].B.LP_CFG = MPC5_ME_LPPC_FROZEN;
                break;
        case MPC560XB_CLK_SIUL:
                ME.PCTL[68].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                break;
        case MPC560XB_CLK_WKUP:
                ME.PCTL[69].B.RUN_CFG = MPC5_ME_RUNPC_FROZEN;
                ME.PCTL[69].B.LP_CFG = MPC5_ME_LPPC_FROZEN;
                break;
        default:
                rc = -EINVAL;
                break;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        if (XWOK == rc) {
                soc_me_enter_mode(MPC5_ME_RUNMODE);
        }
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_clk_getfqcy(struct xwds_soc * soc, xwid_t id,
                                    xwu32_t * buf, xwsz_t * num)
{
        xwu32_t sysclksrc, sysclk, pclk1, pclk2, pclk3;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_UNUSED(soc);
        if (*num < 1) {
                rc = -EINVAL;
                goto err_inval;
        }
        rc = XWOK;

        /* system clock */
        xwos_cpuirq_save_lc(&cpuirq);
        sysclksrc = ME.GS.B.S_SYSCLK;
        if (MPC5_ME_GS_SYSCLK_IRC == sysclksrc)
                sysclk = SOCCFG_CLK_FIRC_VALUE;
        else if (MPC5_ME_GS_SYSCLK_IRCDIV == sysclksrc)
                sysclk = SOCCFG_CLK_FIRC_VALUE / SOCCFG_CLK_FIRC_DIV;
        else if (MPC5_ME_GS_SYSCLK_XOSC == sysclksrc)
                sysclk = SOCCFG_CLK_FXOSC_VALUE;
        else if (MPC5_ME_GS_SYSCLK_XOSCDIV == sysclksrc)
                sysclk = SOCCFG_CLK_FXOSC_VALUE / SOCCFG_CLK_FXOSC_DIV;
        else
                sysclk = SOCCFG_CLK_FMPLL_VALUE;

        /* APB clock */
        pclk1 = sysclk / (xwu32_t)(CGM.SC_DC[0].B.DIV + 1);
        pclk2 = sysclk / (xwu32_t)(CGM.SC_DC[1].B.DIV + 1);
        pclk3 = sysclk / (xwu32_t)(CGM.SC_DC[2].B.DIV + 1);

        switch (id) {
        case MPC560XB_CLK_FXOSC:
                *buf = SOCCFG_CLK_FXOSC_VALUE;
                break;
        case MPC560XB_CLK_FIRC:
                *buf = SOCCFG_CLK_FIRC_VALUE;
                break;
        case MPC560XB_CLK_SCOSC:
                *buf = SOCCFG_CLK_SCOSC_VALUE;
                break;
        case MPC560XB_CLK_SIRC:
                *buf = SOCCFG_CLK_SIRC_VALUE;
                break;
        case MPC560XB_CLK_FMPLL:
                *buf = SOCCFG_CLK_FMPLL_VALUE;
                break;
        case MPC560XB_CLK_SYSCLK:
                *buf = sysclk;
                break;
        case MPC560XB_CLK_PC1:
                *buf = pclk1;
                break;
        case MPC560XB_CLK_PC2:
                *buf = pclk2;
                break;
        case MPC560XB_CLK_PC3:
                *buf = pclk3;
                break;
        case MPC560XB_CLK_ADC_0:
#if defined(MPC5607B)
        case MPC560XB_CLK_ADC_1:
#endif
        case MPC560XB_CLK_CTU:
        case MPC560XB_CLK_EMIOS_0:
        case MPC560XB_CLK_EMIOS_1:
                *buf = pclk3;
                break;
        case MPC560XB_CLK_CAN_SAMPLER:
        case MPC560XB_CLK_DMA_MUX:
        case MPC560XB_CLK_SIUL:
        case MPC560XB_CLK_WKUP:
        case MPC560XB_CLK_PIT:
        case MPC560XB_CLK_RTC:
        case MPC560XB_CLK_CMU:
                *buf = sysclk;
                break;
        case MPC560XB_CLK_DSPI_0:
        case MPC560XB_CLK_DSPI_1:
        case MPC560XB_CLK_DSPI_2:
        case MPC560XB_CLK_DSPI_3:
#if defined(MPC5607B)
        case MPC560XB_CLK_DSPI_4:
        case MPC560XB_CLK_DSPI_5:
#endif
        case MPC560XB_CLK_FLEXCAN_0:
        case MPC560XB_CLK_FLEXCAN_1:
        case MPC560XB_CLK_FLEXCAN_2:
        case MPC560XB_CLK_FLEXCAN_3:
        case MPC560XB_CLK_FLEXCAN_4:
        case MPC560XB_CLK_FLEXCAN_5:
                *buf = pclk2;
                break;
        case MPC560XB_CLK_I2C:
        case MPC560XB_CLK_LINFLEX_0:
        case MPC560XB_CLK_LINFLEX_1:
        case MPC560XB_CLK_LINFLEX_2:
        case MPC560XB_CLK_LINFLEX_3:
#if defined(MPC5607B)
        case MPC560XB_CLK_LINFLEX_4:
        case MPC560XB_CLK_LINFLEX_5:
        case MPC560XB_CLK_LINFLEX_6:
        case MPC560XB_CLK_LINFLEX_7:
        case MPC560XB_CLK_LINFLEX_8:
        case MPC560XB_CLK_LINFLEX_9:
#endif
                *buf = pclk1;
                break;
        default:
                rc = -EINVAL;
                break;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        if (XWOK == rc) {
                *num = 1;
        } else {
        }
err_inval:
        return rc;
}

/******** ******** GPIO driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        const struct soc_gpio_pincfg (*pincfgs)[SOC_GPIO_PIN_NUM];
        const struct soc_gpio_pincfg * pc;
        xwssq_t pin;
        xwu32_t idx;

        XWOS_UNUSED(soc);
        pincfgs = soc->gpio.cfg;
        do {
                pin = xwbop_ffs(xwsq_t, pinmask);
                if (pin >= 0) {
                        pc = &pincfgs[port][pin];
                        idx = port * SOC_GPIO_PIN_NUM + (xwu32_t)pin;
                        SIU.GPDO[idx].R = pc->dout;
                        SIU.PCR[idx].R = pc->cfg.u16;
                        xwbop_c0m(xwsq_t, &pinmask, (xwsq_t)XWBOP_BIT(pin));
                }
        } while (pinmask);
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        xwssq_t pin;
        xwu32_t idx;

        XWOS_UNUSED(soc);
        do {
                pin = xwbop_ffs(xwsq_t, pinmask);
                if (pin >= 0) {
                        idx = port * SOC_GPIO_PIN_NUM + (xwu32_t)pin;
                        SIU.PCR[idx].R = SOC_GPIO_PIN_DCFG;
                        SIU.GPDO[idx].R = 0;
                        xwbop_c0m(xwsq_t, &pinmask, (xwsq_t)XWBOP_BIT(pin));
                }
        } while (pinmask);
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_cfg(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                 void * cfg)
{
        const struct soc_gpio_pincfg * pc;
        xwssq_t pin;
        xwu32_t idx;

        XWOS_UNUSED(soc);
        pc = cfg;
        do {
                pin = xwbop_ffs(xwsq_t, pinmask);
                if (pin >= 0) {
                        idx = port * SOC_GPIO_PIN_NUM + (xwu32_t)pin;
                        SIU.PCR[idx].R = pc->cfg.u16;
                        xwbop_c0m(xwsq_t, &pinmask, (xwsq_t)XWBOP_BIT(pin));
                }/* else {} */
        } while (pinmask);
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_set(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        xwssq_t pin;
        xwu32_t idx;

        XWOS_UNUSED(soc);
        do {
                pin = xwbop_ffs(xwsq_t, pinmask);
                if (pin >= 0) {
                        idx = port * SOC_GPIO_PIN_NUM + (xwu32_t)pin;
                        SIU.GPDO[idx].R = SOC_GPIO_OUT_HIGH;
                        xwbop_c0m(xwsq_t, &pinmask, (xwsq_t)XWBOP_BIT(pin));
                }/* else {} */
        } while (pinmask);
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_reset(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        xwssq_t pin;
        xwu32_t idx;

        XWOS_UNUSED(soc);
        do {
                pin = xwbop_ffs(xwsq_t, pinmask);
                if (pin >= 0) {
                        idx = port * SOC_GPIO_PIN_NUM + (xwu32_t)pin;
                        SIU.GPDO[idx].R = SOC_GPIO_OUT_LOW;
                        xwbop_c0m(xwsq_t, &pinmask, (xwsq_t)XWBOP_BIT(pin));
                }/* else {} */
        } while (pinmask);
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_toggle(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        xwu32_t mask;
        xwsq_t idx;

        XWOS_UNUSED(soc);

        idx = port >> 1U;
        mask = xwbop_rbit(xwu32_t, (xwu32_t)pinmask);
        if (port & 1U) {
                mask = (mask >> 16U) & 0xFFFF;
        }/* else {} */
        SIU.PGPDO[idx].R ^= mask;
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_output(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                    xwsq_t out)
{
        xwu32_t mask, value;

        XWOS_UNUSED(soc);
        out &= pinmask;
        mask = (xwbop_rbit(xwu32_t, (xwu32_t)pinmask) >> 16U) & 0x0000FFFFU;
        value = (xwbop_rbit(xwu32_t, (xwu32_t)out) >> 16U) & 0x0000FFFFU;
        SIU.MPGPDO[port].R = (((xwu32_t)mask) << 16) | ((xwu32_t)value);
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_gpio_input(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                   xwsq_t * in)
{
        xwu32_t value;
        xwu32_t idx, sft;

        XWOS_UNUSED(soc);
        idx = port >> 1U;
        sft = port & 1U ? SOC_GPIO_PIN_NUM : 0;
        value = xwbop_rbit(xwu32_t, SIU.PGPDI[idx].R);
        value >>= sft;
        *in = (xwsq_t)value & pinmask;
        return XWOK;
}

/******** ******** EIRQ driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_soc_drv_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                 xwid_t eiid, xwsq_t eiflag)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);

        if (eiid <= SOC_WKUP_MAX) {
                if (XWDS_SOC_EIF_TM_RISING & eiflag) {
                        WKUP.WIREER.R |= XWBOP_BIT(eiid);
                } else {
                        WKUP.WIREER.R &= ~XWBOP_BIT(eiid);
                }
                if (XWDS_SOC_EIF_TM_FALLING & eiflag) {
                        WKUP.WIFEER.R |= XWBOP_BIT(eiid);
                } else {
                        WKUP.WIFEER.R &= ~XWBOP_BIT(eiid);
                }
                WKUP.IRER.R |= XWBOP_BIT(eiid);
                WKUP.WIFER.R |= XWBOP_BIT(eiid);
                if (XWDS_SOC_EIF_WKUP & eiflag) {
                        WKUP.WRER.R |= XWBOP_BIT(eiid);
                } else {
                        WKUP.WRER.R &= ~XWBOP_BIT(eiid);
                }
        } else {
                eiid -= SOC_EIRQ_MIN;
                if (XWDS_SOC_EIF_TM_RISING & eiflag) {
                        SIU.IREER.R |= XWBOP_BIT(eiid);
                } else {
                        SIU.IREER.R &= ~XWBOP_BIT(eiid);
                }
                if (XWDS_SOC_EIF_TM_FALLING & eiflag) {
                        SIU.IFEER.R |= XWBOP_BIT(eiid);
                } else {
                        SIU.IFEER.R &= ~XWBOP_BIT(eiid);
                }
                SIU.IRER.R |= XWBOP_BIT(eiid);
        }
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                                 xwid_t eiid)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);

        if (eiid <= SOC_WKUP_MAX) {
                WKUP.WIREER.R &= ~XWBOP_BIT(eiid);
                WKUP.WIFEER.R &= ~XWBOP_BIT(eiid);
                WKUP.IRER.R &= ~XWBOP_BIT(eiid);
                WKUP.WIFER.R &= ~XWBOP_BIT(eiid);
                WKUP.WRER.R &= ~XWBOP_BIT(eiid);
                WKUP.WISR.R |= XWBOP_BIT(eiid); /**< clear flag once */
        } else {
                eiid -= SOC_EIRQ_MIN;
                SIU.IREER.R &= ~XWBOP_BIT(eiid);
                SIU.IFEER.R &= ~XWBOP_BIT(eiid);
                SIU.IRER.R &= ~XWBOP_BIT(eiid);
                SIU.ISR.R |= XWBOP_BIT(eiid); /**< clear flag once */
        }
        return XWOK;
}

#if defined(XWCDCFG_ds_SOC_DMA) && (1 == XWCDCFG_ds_SOC_DMA)
/******** ******** DMA driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_req(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        EDMA.SEEIR.R = (xwu8_t)ch;
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_rls(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        EDMA.CEEIR.R = (xwu8_t)ch;
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_cfg(struct xwds_soc * soc, xwid_t ch,
                                void * cfg)
{
        struct soc_dmach_private_cfg * chcfg;

        XWOS_UNUSED(soc);
        chcfg = cfg;
        DMAMUX.CHCONFIG[ch].R = chcfg->dmamux.u8;
        memcpy((void *)&EDMA.TCD[ch].COM,
               &chcfg->tcds[0].com,
               sizeof(union soc_dma_tcd));
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_enable(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        EDMA.SERQR.R = (xwu8_t)ch;
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_disable(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        EDMA.CERQR.R = (xwu8_t)ch;
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_start(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        EDMA.SSBR.R = (xwu8_t)ch;
        return XWOK;
}

static __xwbsp_code
xwer_t mpc560xb_soc_drv_dma_stop(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(ch);
        EDMA.CR.B.CX = 1;
        return XWOK;
}
#endif

/******** ******** isr ******** ********/
__xwbsp_isr
void mpc560xb_wkup_irq0_7_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t wisr;

        xwos_irq_get_data(IRQ_WKUP_IRQ0, &irqdata);
        soc = irqdata.data;
        do {
                wisr = WKUP.WISR.R & 0x000000FF;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, wisr);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        WKUP.WISR.R |= XWBOP_BIT(eiid);
                }
        } while (1);
}

__xwbsp_isr
void mpc560xb_wkup_irq8_15_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t wisr;

        xwos_irq_get_data(IRQ_WKUP_IRQ1, &irqdata);
        soc = irqdata.data;
        do {
                wisr = WKUP.WISR.R & 0x0000FF00;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, wisr);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        WKUP.WISR.R |= XWBOP_BIT(eiid);
                }
        } while (1);
}

__xwbsp_isr
void mpc560xb_wkup_irq16_23_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t wisr;

        xwos_irq_get_data(IRQ_WKUP_IRQ2, &irqdata);
        soc = irqdata.data;
        do {
                wisr = WKUP.WISR.R & 0x00FF0000;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, wisr);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        WKUP.WISR.R |= XWBOP_BIT(eiid);
                }
        } while (1);
}

__xwbsp_isr
void mpc560xb_wkup_irq24_28_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t wisr;

        xwos_irq_get_data(IRQ_WKUP_IRQ3, &irqdata);
        soc = irqdata.data;
        do {
                wisr = WKUP.WISR.R & 0x1F000000;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, wisr);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        WKUP.WISR.R |= XWBOP_BIT(eiid);
                }
        } while (1);
}

__xwbsp_isr
void mpc560xb_eirq0_7_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwid_t eiid;
        xwu32_t eisr;
        xws32_t pos;

        xwos_irq_get_data(IRQ_EXT_IRQ0, &irqdata);
        soc = irqdata.data;
        do {
                eisr = SIU.ISR.R & 0xFF;
                pos = xwbop_fls(xwu32_t, eisr);
                if (pos >= 0) {
                        eiid = (xwid_t)(pos + SOC_EIRQ_MIN);
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        SIU.ISR.R |= XWBOP_BIT(pos);
                }
        } while (1);
}

__xwbsp_isr
void mpc560xb_eirq8_15_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwid_t eiid;
        xwu32_t eisr;
        xws32_t pos;

        xwos_irq_get_data(IRQ_EXT_IRQ1, &irqdata);
        soc = irqdata.data;
        do {
                eisr = SIU.ISR.R & 0xFF00;
                pos = xwbop_fls(xwu32_t, eisr);
                if (pos >= 0) {
                        eiid = (xwid_t)(pos + SOC_EIRQ_MIN);
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        SIU.ISR.R |= XWBOP_BIT(pos); /* clear flag */
                }
        } while (1);
}

__xwbsp_isr
void mpc560xb_eirq16_23_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwid_t eiid;
        xwu32_t eisr;
        xws32_t pos;

        xwos_irq_get_data(IRQ_EXT_IRQ2, &irqdata);
        soc = irqdata.data;
        do {
                eisr = SIU.ISR.R & 0xFF0000;
                pos = xwbop_fls(xwu32_t, eisr);
                if (pos >= 0) {
                        eiid = (xwid_t)(pos + SOC_EIRQ_MIN);
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        SIU.ISR.R |= XWBOP_BIT(pos);
                }
        } while (1);
}

#if (defined(XWCDCFG_ds_SOC_DMA)) && (1 == XWCDCFG_ds_SOC_DMA)
#  if defined(MPC5607B)
__xwbsp_isr
void mpc560xb_edma_cmberr_isr(void)
{
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwu32_t erl;
        xwsid_t ch;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        soc = irqdata.data;

        do {
                erl = EDMA.ERL.R;
                ch = (xwsid_t)xwbop_ffs(xwu32_t, erl);
                if (ch < 0) {
                        break;
                }
                if ((soc->dma.chcbs) && (soc->dma.chcbs[ch])) {
                        if (soc->dma.chcbargs) {
                                soc->dma.chcbs[ch](soc, (xwid_t)ch, EDMA.ESR.R,
                                                   soc->dma.chcbargs[ch]);
                        } else {
                                soc->dma.chcbs[ch](soc, (xwid_t)ch, EDMA.ESR.R,
                                                   NULL);
                        }
                }
                EDMA.CER.R = (xwu8_t)ch;
                EDMA.CR.B.HALT = 0;
        } while (1);
}

EDMA_CH_ISR(0)
EDMA_CH_ISR(1)
EDMA_CH_ISR(2)
EDMA_CH_ISR(3)
EDMA_CH_ISR(4)
EDMA_CH_ISR(5)
EDMA_CH_ISR(6)
EDMA_CH_ISR(7)
EDMA_CH_ISR(8)
EDMA_CH_ISR(9)
EDMA_CH_ISR(10)
EDMA_CH_ISR(11)
EDMA_CH_ISR(12)
EDMA_CH_ISR(13)
EDMA_CH_ISR(14)
EDMA_CH_ISR(15)
#  endif
#endif
