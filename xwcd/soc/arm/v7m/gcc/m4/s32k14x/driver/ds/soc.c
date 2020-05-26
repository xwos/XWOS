/**
 * @file
 * @brief 设备栈驱动：S32K14x SOC
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
#include <xwos/lib/string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/irq.h>
#include <soc_irq.h>
#include <soc.h>
#include <soc_clk.h>
#include <soc_pwr.h>
#include <soc_gpio.h>
#include <soc_dma.h>
#include <soc_eirq.h>
#include <soc_tmr.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/chip.h>
#include <xwmd/ds/soc/clock.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/eirq.h>
#include <xwmd/ds/soc/dma.h>
#include <driver/ds/soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
#define EDMA_CH_ISR(x)                                                  \
__xwbsp_isr                                                             \
void s32k14x_edma_ch##x##_isr(void)                                     \
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
        if (DMA->TCD[ch].CSR & DMA_TCD_CSR_DONE_MASK)                   \
                DMA->CDNE = (xwu8_t)(ch & 0xF);                         \
        DMA->CINT = (xwu8_t)(ch & 0xF);                                 \
}
#endif /* XWMDCFG_ds_SOC_DMA */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_code
xwer_t s32k14x_soc_check_desc(struct xwds_soc * soc);

static __xwds_code
xwer_t s32k14x_soc_drv_probe(struct xwds_device * dev);

static __xwds_code
xwer_t s32k14x_soc_drv_start(struct xwds_device * dev);

static __xwds_code
xwer_t s32k14x_soc_drv_stop(struct xwds_device * dev);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_code
xwer_t s32k14x_soc_drv_suspend(struct xwds_device * dev);

static __xwds_code
xwer_t s32k14x_soc_drv_resume(struct xwds_device * dev);
#endif /* XWMDCFG_ds_PM */

static __xwds_code
xwer_t s32k14x_soc_drv_clk_req(struct xwds_soc * soc, xwid_t id);

static __xwds_code
xwer_t s32k14x_soc_drv_clk_rls(struct xwds_soc * soc, xwid_t id);

static __xwds_code
xwer_t s32k14x_soc_drv_clk_getfqcy(struct xwds_soc * soc, xwid_t id,
                                   xwu32_t * buf, xwsz_t * num);

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_req(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask);

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_rls(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask);

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_cfg(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask,
                                void * cfg);

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_set(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask);

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_reset(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_toggle(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_output(struct xwds_soc * soc,
                                   xwid_t port, xwsq_t pinmask,
                                   xwsq_t out);

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_input(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwsq_t * in);

static __xwds_code
xwer_t s32k14x_soc_drv_eirq_req(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask,
                                xwid_t eiid, xwsq_t eiflag);

static __xwds_code
xwer_t s32k14x_soc_drv_eirq_rls(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask,
                                xwid_t eiid);

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
static __xwds_code
xwer_t s32k14x_soc_drv_dma_req(struct xwds_soc * soc, xwid_t ch);

static __xwds_code
xwer_t s32k14x_soc_drv_dma_rls(struct xwds_soc * soc, xwid_t ch);

static __xwds_code
xwer_t s32k14x_soc_drv_dma_cfg(struct xwds_soc * soc, xwid_t ch,
                               void * cfg);

static __xwds_code
xwer_t s32k14x_soc_drv_dma_enable(struct xwds_soc * soc, xwid_t ch);

static __xwds_code
xwer_t s32k14x_soc_drv_dma_disable(struct xwds_soc * soc, xwid_t ch);

static __xwds_code
xwer_t s32k14x_soc_drv_dma_start(struct xwds_soc * soc, xwid_t ch);

static __xwds_code
xwer_t s32k14x_soc_drv_dma_stop(struct xwds_soc * soc, xwid_t ch);
#endif /* XWMDCFG_ds_SOC_DMA */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_soc_driver s32k14x_soc_drv = {
        .base = {
                .name = "s32k14x.soc",
                .probe = s32k14x_soc_drv_probe,
                .remove = NULL,
                .start = s32k14x_soc_drv_start,
                .stop = s32k14x_soc_drv_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
                .suspend = s32k14x_soc_drv_suspend,
                .resume = s32k14x_soc_drv_resume,
#endif /* XWMDCFG_ds_PM */
        },
        .clk_req = s32k14x_soc_drv_clk_req,
        .clk_rls = s32k14x_soc_drv_clk_rls,
        .clk_getfqcy = s32k14x_soc_drv_clk_getfqcy,

        .gpio_req = s32k14x_soc_drv_gpio_req,
        .gpio_rls = s32k14x_soc_drv_gpio_rls,
        .gpio_cfg = s32k14x_soc_drv_gpio_cfg,
        .gpio_set = s32k14x_soc_drv_gpio_set,
        .gpio_reset = s32k14x_soc_drv_gpio_reset,
        .gpio_toggle = s32k14x_soc_drv_gpio_toggle,
        .gpio_output = s32k14x_soc_drv_gpio_output,
        .gpio_input = s32k14x_soc_drv_gpio_input,

        .eirq_req = s32k14x_soc_drv_eirq_req,
        .eirq_rls = s32k14x_soc_drv_eirq_rls,

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
        .dma_req = s32k14x_soc_drv_dma_req,
        .dma_rls = s32k14x_soc_drv_dma_rls,
        .dma_cfg = s32k14x_soc_drv_dma_cfg,
        .dma_enable = s32k14x_soc_drv_dma_enable,
        .dma_disable = s32k14x_soc_drv_dma_disable,
        .dma_start = s32k14x_soc_drv_dma_start,
        .dma_stop = s32k14x_soc_drv_dma_stop,
#endif /* XWMDCFG_ds_SOC_DMA */

        .ioctl = NULL,
};

__xwds_rodata const xwu32_t s32k14x_clk_prediv_table[] = {
        0, 1, 2, 4, 8, 16, 32, 64,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_code
xwer_t s32k14x_soc_check_desc(struct xwds_soc * soc)
{
        const struct xwds_resources * resources;
        const struct soc_sys_cfg * syscfg;
        const struct soc_gpio_portcfg * portcfg;
        xwer_t rc;

        resources = soc->dev.resources;
        syscfg = soc->xwccfg;
        portcfg = soc->gpio.cfg;
        if (is_err_or_null(resources)) {
                rc = -EINVAL;
        } else if (is_err_or_null(syscfg)) {
                rc = -EINVAL;
        } else if (is_err_or_null(portcfg)) {
                rc = -EINVAL;
        } else {
                rc = OK;
        }
        return rc;
}

/******** ******** common driver ******** ********/
static __xwds_code
xwer_t s32k14x_soc_drv_probe(struct xwds_device * dev)
{
        struct xwds_soc * soc;

        soc = xwds_static_cast(struct xwds_soc *, dev);
        return s32k14x_soc_check_desc(soc);
}

static __xwds_code
xwer_t s32k14x_soc_drv_start(struct xwds_device * dev)
{
        struct xwds_soc * soc;
        const struct soc_sys_cfg * syscfg;
        const struct xwds_resources * resources;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t i, j;
        xwer_t rc;

        soc = xwds_static_cast(struct xwds_soc *, dev);
        resources = soc->dev.resources;
        syscfg = soc->xwccfg;

        /* request IRQ resources */
        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr,
                                      XWOS_UNUSED_ARGUMENT, soc);
                if (__unlikely(rc < 0)) {
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
                if (__unlikely(rc < 0)) {
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
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                gpiorsc = &resources->gpiorsc_array[j];
                                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port,
                                              gpiorsc->pinmask);
                        }
                        goto err_req_gpios;
                }
        }

        /* SOC setup */
        SIM->CHIPCTL = syscfg->sim.chipctl.u32;
        SMC->STOPCTRL = syscfg->smc.stopctrl.u32;
        SMC->PMCTRL = syscfg->smc.pmctrl.u32;
        PMC->LVDSC1 = syscfg->pmc.lvdsc1.u8;
        PMC->LVDSC2 = syscfg->pmc.lvdsc2.u8;
        PMC->REGSC = syscfg->pmc.regsc.u8;

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
        /* DMAC setup */
        const struct soc_dmac_private_cfg * dmaccfg;

        dmaccfg = soc->dma.ccfg;
        DMA->CR = dmaccfg->cr.u32;
        for (i = 0; i < (xwssz_t)SOC_DMAC_CHANNEL_NUM; i++) {
                DMA->DCHPRI[i] = dmaccfg->cpr[i].u8;
        }
#endif /* XWMDCFG_ds_SOC_DMA */

        /* PORT setup */
        const struct soc_gpio_portcfg * portcfg;
        portcfg = soc->gpio.cfg;

        PORTA->ISFR = 0xFFFFFFFFU;
        PORTB->ISFR = 0xFFFFFFFFU;
        PORTC->ISFR = 0xFFFFFFFFU;
        PORTD->ISFR = 0xFFFFFFFFU;
        PORTE->ISFR = 0xFFFFFFFFU;

        PORTA->DFCR = portcfg[SOC_GPIO_PORT_A].dfcr.u32;
        PORTA->DFWR = portcfg[SOC_GPIO_PORT_A].dfwr.u32;
        PORTA->DFER = portcfg[SOC_GPIO_PORT_A].dfer.u32;

        PORTB->DFCR = portcfg[SOC_GPIO_PORT_B].dfcr.u32;
        PORTB->DFWR = portcfg[SOC_GPIO_PORT_B].dfwr.u32;
        PORTB->DFER = portcfg[SOC_GPIO_PORT_B].dfer.u32;

        PORTC->DFCR = portcfg[SOC_GPIO_PORT_C].dfcr.u32;
        PORTC->DFWR = portcfg[SOC_GPIO_PORT_C].dfwr.u32;
        PORTC->DFER = portcfg[SOC_GPIO_PORT_C].dfer.u32;

        PORTD->DFCR = portcfg[SOC_GPIO_PORT_D].dfcr.u32;
        PORTD->DFWR = portcfg[SOC_GPIO_PORT_D].dfwr.u32;
        PORTD->DFER = portcfg[SOC_GPIO_PORT_D].dfer.u32;

        PORTE->DFCR = portcfg[SOC_GPIO_PORT_E].dfcr.u32;
        PORTE->DFWR = portcfg[SOC_GPIO_PORT_E].dfwr.u32;
        PORTE->DFER = portcfg[SOC_GPIO_PORT_E].dfer.u32;

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

static __xwds_code
xwer_t s32k14x_soc_drv_stop(struct xwds_device * dev)
{
        struct xwds_soc * soc;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwos_irq_resource * irqrsc;
        xwssz_t j;
        xwer_t rc;

        soc = xwds_static_cast(struct xwds_soc *, dev);
        resources = soc->dev.resources;

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
        DMA->CR |= DMA_CR_CX_MASK;
#endif /* XWMDCFG_ds_SOC_DMA */

        /* disable IRQs */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_disable(irqrsc->irqn);
                if (__unlikely(rc < 0)) {
                        goto err_irq_disable;
                }
        }

        /* release GPIO resources */
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                rc = xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__unlikely(rc < 0)) {
                        goto err_gpio_rls;
                }
        }

        /* release clock resources */
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                rc = xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                if (__unlikely(rc < 0)) {
                        goto err_clk_rls;
                }
        }

        /* release IRQ resources */
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >=0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                rc = xwos_irq_release(irqrsc->irqn);
                if (__unlikely(rc < 0)) {
                        goto err_irq_release;
                }
        }
        return OK;

err_irq_release:
err_clk_rls:
err_gpio_rls:
err_irq_disable:
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_code
xwer_t s32k14x_soc_drv_suspend(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_resume(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return OK;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** clock operations ******** ********/
static __xwds_code
xwer_t s32k14x_soc_drv_clk_req(struct xwds_soc * soc, xwid_t id)
{
        xwreg_t cpuirq;
        xwer_t rc;
        xwu32_t runm, sysclksrc;

        XWOS_UNUSED(soc);
        rc = OK;
        soc_cpuirq_save_lc(&cpuirq);
        runm = (SMC->PMCTRL & SMC_PMCTRL_RUNM_MASK) >> SMC_PMCTRL_RUNM_SHIFT;
        switch (runm) {
        case SOC_PWR_SMC_PMCTRL_RUNM_NORMAL:
                sysclksrc = (SCG->RCCR & SCG_RCCR_SCS_MASK) >> SCG_RCCR_SCS_SHIFT;
                break;
        case SOC_PWR_SMC_PMCTRL_RUNM_VLP:
                sysclksrc = (SCG->VCCR & SCG_VCCR_SCS_MASK) >> SCG_VCCR_SCS_SHIFT;
                break;
        case SOC_PWR_SMC_PMCTRL_RUNM_HS:
                sysclksrc = (SCG->HCCR & SCG_HCCR_SCS_MASK) >> SCG_HCCR_SCS_SHIFT;
                break;
        default:
                rc = -EFAULT;
                goto err_syserr;
                break;
        }
        switch (id) {
        case S32K14X_SCLK_PREDIV_SYS:
        case S32K14X_SCLK_CORE:
        case S32K14X_SCLK_SYS:
        case S32K14X_SCLK_BUS:
        case S32K14X_SCLK_FLASH:
        case S32K14X_SCLK_SPLL:
        case S32K14X_SCLK_SOSC:
        case S32K14X_SCLK_LPO_RTC_CLK:
        case S32K14X_SCLK_SCG_CLKOUT:
        case S32K14X_SCLK_LPO32K:
        case S32K14X_SCLK_LPO128K:
        case S32K14X_SCLK_LPO:
        case S32K14X_SCLK_VCO:
        case S32K14X_SCLK_SPLLDIV1_CLK:
        case S32K14X_SCLK_SPLLDIV2_CLK:
        case S32K14X_SCLK_FIRCDIV1_CLK:
        case S32K14X_SCLK_FIRCDIV2_CLK:
        case S32K14X_SCLK_SIRCDIV1_CLK:
        case S32K14X_SCLK_SIRCDIV2_CLK:
        case S32K14X_SCLK_SOSCDIV1_CLK:
        case S32K14X_SCLK_SOSCDIV2_CLK:
                rc = -EPERM;
                break;
        case S32K14X_SCLK_FIRC:
                soc_clk_firc_enable();
                break;
        case S32K14X_SCLK_SIRC:
                soc_clk_sirc_enable();
                break;
        case S32K14X_SCLK_RTC_CLKOUT:
                /* TODO */
                break;
        case S32K14X_SCLK_CLKOUT:
                SIM->CHIPCTL |= SIM_CHIPCTL_CLKOUTEN_MASK;
                break;

        /* module clocks */
        case S32K14X_MCLK_SRAM:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_CMU:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_PMC:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_SIM:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_RCM:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_MSCM:
                SIM->PLATCGC |= SIM_PLATCGC_CGCMSCM_MASK;
                break;
        case S32K14X_MCLK_MPU:
                SIM->PLATCGC |= SIM_PLATCGC_CGCMPU_MASK;
                break;
        case S32K14X_MCLK_DMA:
                SIM->PLATCGC |= SIM_PLATCGC_CGCDMA_MASK;
                break;
        case S32K14X_MCLK_ERM:
                SIM->PLATCGC |= SIM_PLATCGC_CGCERM_MASK;
                break;
        case S32K14X_MCLK_EIM:
                SIM->PLATCGC |= SIM_PLATCGC_CGCEIM_MASK;
                break;
        case S32K14X_MCLK_FTFC:
                PCC->PCCn[PCC_FTFC_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_TRGMUX:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_DMAMUX:
                PCC->PCCn[PCC_DMAMUX_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FlexCAN0:
                PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FlexCAN1:
                PCC->PCCn[PCC_FlexCAN1_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FTM3:
                PCC->PCCn[PCC_FTM3_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FTM3_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_ADC1:
                PCC->PCCn[PCC_ADC1_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_ADC1_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FlexCAN2:
#if SOC_CHIP_S32K >= 144
                PCC->PCCn[PCC_FlexCAN2_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_LPSPI0:
                PCC->PCCn[PCC_LPSPI0_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPSPI0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPSPI1:
                PCC->PCCn[PCC_LPSPI1_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPSPI1_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPSPI2:
#if SOC_CHIP_S32K >= 144
                PCC->PCCn[PCC_LPSPI2_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPSPI2_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_PDB1:
                PCC->PCCn[PCC_PDB1_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_CRC:
                PCC->PCCn[PCC_CRC_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PDB0:
                PCC->PCCn[PCC_PDB0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPIT:
                PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FTM0:
                PCC->PCCn[PCC_FTM0_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FTM0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FTM1:
                PCC->PCCn[PCC_FTM1_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FTM1_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FTM2:
                PCC->PCCn[PCC_FTM2_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FTM2_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_ADC0:
                PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_RTC:
                PCC->PCCn[PCC_RTC_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPTMR0:
                PCC->PCCn[PCC_LPTMR0_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPTMR0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTA:
                PCC->PCCn[PCC_PORTA_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTB:
                PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTC:
                PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTD:
                PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTE:
                PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_SAI0:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_SAI0_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_SAI1:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_SAI1_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FlexIO:
                PCC->PCCn[PCC_FlexIO_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FlexIO_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_WDOG:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_EWM:
                PCC->PCCn[PCC_EWM_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPI2C0:
                PCC->PCCn[PCC_LPI2C0_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPI2C0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPI2C1:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_LPI2C1_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPI2C1_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_LPUART0:
                PCC->PCCn[PCC_LPUART0_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPUART0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPUART1:
                PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPUART2:
#if SOC_CHIP_S32K >= 144
                PCC->PCCn[PCC_LPUART2_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_LPUART2_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM4:
#if SOC_CHIP_S32K >= 146
                PCC->PCCn[PCC_FTM4_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FTM4_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM5:
#if SOC_CHIP_S32K >= 146
                PCC->PCCn[PCC_FTM5_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FTM5_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM6:
#if SOC_CHIP_S32K >= 146
                PCC->PCCn[PCC_FTM6_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FTM6_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM7:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_FTM7_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_FTM7_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_CMP0:
                PCC->PCCn[PCC_CMP0_INDEX] |= PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_QSPI:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_QSPI_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_ENET:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_ENET_INDEX] |= PCC_PCCn_PCS(sysclksrc);
                PCC->PCCn[PCC_ENET_INDEX] |= PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        default:
                rc = -EINVAL;
                break;
        }

err_syserr:
        soc_cpuirq_restore_lc(cpuirq);
        return rc;
}

static __xwds_code
xwer_t s32k14x_soc_drv_clk_rls(struct xwds_soc * soc, xwid_t id)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_UNUSED(soc);
        rc = OK;
        soc_cpuirq_save_lc(&cpuirq);
        switch (id) {
        case S32K14X_SCLK_PREDIV_SYS:
        case S32K14X_SCLK_CORE:
        case S32K14X_SCLK_SYS:
        case S32K14X_SCLK_BUS:
        case S32K14X_SCLK_FLASH:
        case S32K14X_SCLK_SPLL:
        case S32K14X_SCLK_SOSC:
        case S32K14X_SCLK_LPO_RTC_CLK:
        case S32K14X_SCLK_SCG_CLKOUT:
        case S32K14X_SCLK_LPO32K:
        case S32K14X_SCLK_LPO128K:
        case S32K14X_SCLK_LPO:
        case S32K14X_SCLK_VCO:
        case S32K14X_SCLK_SPLLDIV1_CLK:
        case S32K14X_SCLK_SPLLDIV2_CLK:
        case S32K14X_SCLK_FIRCDIV1_CLK:
        case S32K14X_SCLK_FIRCDIV2_CLK:
        case S32K14X_SCLK_SIRCDIV1_CLK:
        case S32K14X_SCLK_SIRCDIV2_CLK:
        case S32K14X_SCLK_SOSCDIV1_CLK:
        case S32K14X_SCLK_SOSCDIV2_CLK:
                rc = -EPERM;
                break;
        case S32K14X_SCLK_FIRC:
                soc_clk_firc_disable();
                break;
        case S32K14X_SCLK_SIRC:
                soc_clk_sirc_disable();
                break;
        case S32K14X_SCLK_RTC_CLKOUT:
                /* TODO */
                break;
        case S32K14X_SCLK_CLKOUT:
                SIM->CHIPCTL &= ~SIM_CHIPCTL_CLKOUTEN_MASK;
                break;

        /* module clocks */
        case S32K14X_MCLK_SRAM:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_CMU:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_PMC:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_SIM:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_RCM:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_MSCM:
                SIM->PLATCGC &= ~SIM_PLATCGC_CGCMSCM_MASK;
                break;
        case S32K14X_MCLK_MPU:
                SIM->PLATCGC &= ~SIM_PLATCGC_CGCMPU_MASK;
                break;
        case S32K14X_MCLK_DMA:
                SIM->PLATCGC &= ~SIM_PLATCGC_CGCDMA_MASK;
                break;
        case S32K14X_MCLK_ERM:
                SIM->PLATCGC &= ~SIM_PLATCGC_CGCERM_MASK;
                break;
        case S32K14X_MCLK_EIM:
                SIM->PLATCGC &= ~SIM_PLATCGC_CGCEIM_MASK;
                break;
        case S32K14X_MCLK_FTFC:
                PCC->PCCn[PCC_FTFC_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_TRGMUX:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_DMAMUX:
                PCC->PCCn[PCC_DMAMUX_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FlexCAN0:
                PCC->PCCn[PCC_FlexCAN0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FlexCAN1:
                PCC->PCCn[PCC_FlexCAN1_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FTM3:
                PCC->PCCn[PCC_FTM3_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_ADC1:
                PCC->PCCn[PCC_ADC1_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FlexCAN2:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_FlexCAN2_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_LPSPI0:
                PCC->PCCn[PCC_LPSPI0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPSPI1:
                PCC->PCCn[PCC_LPSPI1_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPSPI2:
#if SOC_CHIP_S32K >= 144
                PCC->PCCn[PCC_LPSPI2_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_PDB1:
                PCC->PCCn[PCC_PDB1_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_CRC:
                PCC->PCCn[PCC_CRC_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PDB0:
                PCC->PCCn[PCC_PDB0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPIT:
                PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FTM0:
                PCC->PCCn[PCC_FTM0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FTM1:
                PCC->PCCn[PCC_FTM1_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_FTM2:
                PCC->PCCn[PCC_FTM2_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_ADC0:
                PCC->PCCn[PCC_ADC0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_RTC:
                PCC->PCCn[PCC_RTC_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPTMR0:
                PCC->PCCn[PCC_LPTMR0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTA:
                PCC->PCCn[PCC_PORTA_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTB:
                PCC->PCCn[PCC_PORTB_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTC:
                PCC->PCCn[PCC_PORTC_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTD:
                PCC->PCCn[PCC_PORTD_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_PORTE:
                PCC->PCCn[PCC_PORTE_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_SAI0:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_SAI0_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_SAI1:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_SAI1_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FlexIO:
                PCC->PCCn[PCC_FlexIO_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_WDOG:
                rc = -EPERM;
                break;
        case S32K14X_MCLK_EWM:
                PCC->PCCn[PCC_EWM_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPI2C0:
                PCC->PCCn[PCC_LPI2C0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPI2C1:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_LPI2C1_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_LPUART0:
                PCC->PCCn[PCC_LPUART0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPUART1:
                PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_LPUART2:
#if SOC_CHIP_S32K >= 144
                PCC->PCCn[PCC_LPUART2_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM4:
#if SOC_CHIP_S32K >= 146
                PCC->PCCn[PCC_FTM4_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM5:
#if SOC_CHIP_S32K >= 146
                PCC->PCCn[PCC_FTM5_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM6:
#if SOC_CHIP_S32K >= 146
                PCC->PCCn[PCC_FTM6_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM7:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_FTM7_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_CMP0:
                PCC->PCCn[PCC_CMP0_INDEX] &= ~PCC_PCCn_CGC_MASK;
                break;
        case S32K14X_MCLK_QSPI:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_QSPI_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_ENET:
#if SOC_CHIP_S32K >= 148
                PCC->PCCn[PCC_ENET_INDEX] &= ~PCC_PCCn_CGC_MASK;
#else
                rc = -ENOENT;
#endif
                break;
        default:
                rc = -EINVAL;
                break;
        }
        soc_cpuirq_restore_lc(cpuirq);
        return rc;
}

static __xwds_code
xwer_t s32k14x_soc_drv_clk_getfqcy(struct xwds_soc * soc, xwid_t id,
                                   xwu32_t * buf, xwsz_t * num)
{
        xwreg_t cpuirq;
        xwer_t rc;
        xwsz_t array_num;
        xwu32_t runm, sysclksrc, divcore, divbus, divslow;
        xwu32_t xwscdiv1, xwscdiv2, xwscdiv1_clk, xwscdiv2_clk;
        xwu32_t sircdiv1, sircdiv2, sircdiv1_clk, sircdiv2_clk;
        xwu32_t fircdiv1, fircdiv2, fircdiv1_clk, fircdiv2_clk;
        xwu32_t splldiv1, splldiv2, splldiv1_clk, splldiv2_clk;
        xwu32_t sys_clk, core_clk, bus_clk, slow_clk, lpo_rtc_clk, scg_clkout;
        xwu32_t pllvco, pllclk, pllmul, pllprediv;

        XWOS_UNUSED(soc);
        rc = OK;
        array_num = 0;
        if (*num < SOC_CLK_FQCYIDX_NUM) {
                rc = -EINVAL;
                goto err_inval;
        }
        soc_cpuirq_save_lc(&cpuirq);
        runm = ((SMC->PMCTRL & SMC_PMCTRL_RUNM_MASK) >> SMC_PMCTRL_RUNM_SHIFT);
        switch (runm) {
        case SOC_PWR_SMC_PMCTRL_RUNM_NORMAL:
                sysclksrc = (SCG->RCCR & SCG_RCCR_SCS_MASK) >> SCG_RCCR_SCS_SHIFT;
                divcore = (SCG->RCCR & SCG_RCCR_DIVCORE_MASK) >> SCG_RCCR_DIVCORE_SHIFT;
                divcore++;
                divbus = (SCG->RCCR & SCG_RCCR_DIVBUS_MASK) >> SCG_RCCR_DIVBUS_SHIFT;
                divbus++;
                divslow = (SCG->RCCR & SCG_RCCR_DIVSLOW_MASK) >> SCG_RCCR_DIVSLOW_SHIFT;
                divslow++;
                break;
        case SOC_PWR_SMC_PMCTRL_RUNM_VLP:
                sysclksrc = (SCG->VCCR & SCG_VCCR_SCS_MASK) >> SCG_VCCR_SCS_SHIFT;
                divcore = (SCG->VCCR & SCG_VCCR_DIVCORE_MASK) >> SCG_VCCR_DIVCORE_SHIFT;
                divcore++;
                divbus = (SCG->VCCR & SCG_VCCR_DIVBUS_MASK) >> SCG_VCCR_DIVBUS_SHIFT;
                divbus++;
                divslow = (SCG->VCCR & SCG_VCCR_DIVSLOW_MASK) >> SCG_VCCR_DIVSLOW_SHIFT;
                divslow++;
                break;
        case SOC_PWR_SMC_PMCTRL_RUNM_HS:
                sysclksrc = (SCG->HCCR & SCG_HCCR_SCS_MASK) >> SCG_HCCR_SCS_SHIFT;
                divcore = (SCG->HCCR & SCG_HCCR_DIVCORE_MASK) >> SCG_HCCR_DIVCORE_SHIFT;
                divcore++;
                divbus = (SCG->HCCR & SCG_HCCR_DIVBUS_MASK) >> SCG_HCCR_DIVBUS_SHIFT;
                divbus++;
                divslow = (SCG->HCCR & SCG_HCCR_DIVSLOW_MASK) >> SCG_HCCR_DIVSLOW_SHIFT;
                divslow++;
                break;
        default:
                rc = -EFAULT;
                goto err_syserr;
                break;
        }

        if (SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) {
                pllmul = ((SCG->SPLLCFG & SCG_SPLLCFG_MULT_MASK) >>
                          SCG_SPLLCFG_MULT_SHIFT) + 16;
                pllprediv = ((SCG->SPLLCFG & SCG_SPLLCFG_PREDIV_MASK) >>
                             SCG_SPLLCFG_PREDIV_SHIFT) + 1;
                pllvco = SOCCFG_CLK_SOSC_VALUE / pllprediv * pllmul;
                pllclk = pllvco >> 1;
        } else {
                pllvco = 0;
                pllclk = 0;
        }
        switch (sysclksrc) {
        case SOC_CLK_SCG_XCCR_SCS_SOSC:
                sys_clk = SOCCFG_CLK_SOSC_VALUE;
                break;
        case SOC_CLK_SCG_XCCR_SCS_SIRC:
                sys_clk = SOCCFG_CLK_SIRC_VALUE;
                break;
        case SOC_CLK_SCG_XCCR_SCS_FIRC:
                sys_clk = SOCCFG_CLK_FIRC_VALUE;
                break;
        case SOC_CLK_SCG_XCCR_SCS_SPLL:
                sys_clk = pllclk;
                break;
        default:
                rc = -EFAULT;
                goto err_syserr;
                break;
        }

        core_clk = sys_clk / divcore;
        bus_clk = sys_clk / divbus;
        slow_clk = sys_clk / divslow;

        xwscdiv1 = (SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV1_MASK) >>
                   SCG_SOSCDIV_SOSCDIV1_SHIFT;
        xwscdiv1 = s32k14x_clk_prediv_table[xwscdiv1];
        if (xwscdiv1) {
                xwscdiv1_clk = SOCCFG_CLK_SOSC_VALUE / xwscdiv1;
        } else {
                xwscdiv1_clk = 0;
        }
        xwscdiv2 = (SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV2_MASK) >>
                   SCG_SOSCDIV_SOSCDIV2_SHIFT;
        xwscdiv2 = s32k14x_clk_prediv_table[xwscdiv2];
        if (xwscdiv2) {
                xwscdiv2_clk = SOCCFG_CLK_SOSC_VALUE / xwscdiv2;
        } else {
                xwscdiv2_clk = 0;
        }

        sircdiv1 = (SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV1_MASK) >>
                   SCG_SIRCDIV_SIRCDIV1_SHIFT;
        sircdiv1 = s32k14x_clk_prediv_table[sircdiv1];
        if (sircdiv1) {
                sircdiv1_clk = SOCCFG_CLK_SIRC_VALUE / sircdiv1;
        } else {
                sircdiv1_clk = 0;
        }
        sircdiv2 = (SCG->SIRCDIV & SCG_SIRCDIV_SIRCDIV2_MASK) >>
                   SCG_SIRCDIV_SIRCDIV2_SHIFT;
        sircdiv2 = s32k14x_clk_prediv_table[sircdiv2];
        if (sircdiv2) {
                sircdiv2_clk = SOCCFG_CLK_SIRC_VALUE / sircdiv2;
        } else {
                sircdiv2_clk = 0;
        }

        fircdiv1 = (SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV1_MASK) >>
                   SCG_FIRCDIV_FIRCDIV1_SHIFT;
        fircdiv1 = s32k14x_clk_prediv_table[fircdiv1];
        if (fircdiv1) {
                fircdiv1_clk = SOCCFG_CLK_FIRC_VALUE / fircdiv1;
        } else {
                fircdiv1_clk = 0;
        }
        fircdiv2 = (SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV2_MASK) >>
                   SCG_FIRCDIV_FIRCDIV2_SHIFT;
        fircdiv2 = s32k14x_clk_prediv_table[fircdiv2];
        if (fircdiv2) {
                fircdiv2_clk = SOCCFG_CLK_FIRC_VALUE / fircdiv2;
        } else {
                fircdiv2_clk = 0;
        }

        splldiv1 = (SCG->SPLLDIV & SCG_SPLLDIV_SPLLDIV1_MASK) >>
                   SCG_SPLLDIV_SPLLDIV1_SHIFT;
        splldiv1 = s32k14x_clk_prediv_table[splldiv1];
        if (splldiv1) {
                splldiv1_clk = pllclk / splldiv1;
        } else {
                splldiv1_clk = 0;
        }
        splldiv2 = (SCG->SPLLDIV & SCG_SPLLDIV_SPLLDIV2_MASK) >>
                   SCG_SPLLDIV_SPLLDIV2_SHIFT;
        splldiv2 = s32k14x_clk_prediv_table[splldiv2];
        if (splldiv2) {
                splldiv2_clk = pllclk / splldiv2;
        } else {
                splldiv2_clk = 0;
        }

        switch ((SIM->LPOCLKS & SIM_LPOCLKS_RTCCLKSEL_MASK) >>
                SIM_LPOCLKS_RTCCLKSEL_SHIFT) {
        case SOC_CLK_SIM_LPOCLKS_RTCCLKSEL_SOSCDIV1_CLK:
                lpo_rtc_clk = xwscdiv1_clk;
                break;
        case SOC_CLK_SIM_LPOCLKS_RTCCLKSEL_LPO32K_CLK:
                lpo_rtc_clk = SOCCFG_CLK_LPO32K_VALUE;
                break;
        case SOC_CLK_SIM_LPOCLKS_RTCCLKSEL_RTC_CLKIN:
                lpo_rtc_clk = SOCCFG_CLK_RTCOSC_VALUE;
                break;
        case SOC_CLK_SIM_LPOCLKS_RTCCLKSEL_FIRCDIV1_CLK:
                lpo_rtc_clk = fircdiv1_clk;
                break;
        default:
                rc = -EFAULT;
                goto err_syserr;
                break;
        }

        switch ((SCG->CLKOUTCNFG & SCG_CLKOUTCNFG_CLKOUTSEL_MASK) >>
                SCG_CLKOUTCNFG_CLKOUTSEL_SHIFT) {
        case SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_SLOW:
                scg_clkout = slow_clk;
                break;
        case SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_SOSC:
                if (SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK) {
                        scg_clkout = SOCCFG_CLK_SOSC_VALUE;
                } else {
                        scg_clkout = 0;
                }
                break;
        case SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_SIRC:
                if (SCG->SIRCCSR & SCG_SIRCCSR_SIRCEN_MASK) {
                        scg_clkout = SOCCFG_CLK_SIRC_VALUE;
                } else {
                        scg_clkout = 0;
                }
                break;
        case SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_FIRC:
                if (SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK) {
                        scg_clkout = SOCCFG_CLK_FIRC_VALUE;
                } else {
                        scg_clkout = 0;
                }
                break;
        case SOC_CLK_SCG_CLKOUTCNFG_CLKOUTSEL_SPLL:
                if (SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) {
                        scg_clkout = pllclk;
                } else {
                        scg_clkout = 0;
                }
                break;
        default:
                scg_clkout = 0;
                break;
        }

        switch (id) {
        case S32K14X_SCLK_PREDIV_SYS:
                buf[array_num] = sys_clk;
                array_num++;
                break;
        case S32K14X_SCLK_CORE:
                buf[array_num] = core_clk;
                array_num++;
                break;
        case S32K14X_SCLK_SYS:
                buf[array_num] = core_clk;
                array_num++;
                break;
        case S32K14X_SCLK_BUS:
                buf[array_num] = bus_clk;
                array_num++;
                break;
        case S32K14X_SCLK_FLASH:
                buf[array_num] = slow_clk;
                array_num++;
                break;
        case S32K14X_SCLK_SPLL:
                if (SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) {
                        buf[array_num] = pllclk;
                        array_num++;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_FIRC:
                if (SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK) {
                        buf[array_num] = SOCCFG_CLK_FIRC_VALUE;
                        array_num++;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_SIRC:
                if (SCG->SIRCCSR & SCG_SIRCCSR_SIRCEN_MASK) {
                        buf[array_num] = SOCCFG_CLK_SIRC_VALUE;
                        array_num++;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_SOSC:
                if (SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK) {
                        buf[array_num] = SOCCFG_CLK_SOSC_VALUE;
                        array_num++;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_LPO_RTC_CLK:
                buf[array_num] = lpo_rtc_clk;
                array_num++;
                break;
        case S32K14X_SCLK_RTC_CLKOUT:
                /* TODO */
                rc = -ESHUTDOWN;
                break;
        case S32K14X_SCLK_LPO32K:
                buf[array_num] = SOCCFG_CLK_LPO32K_VALUE;
                array_num++;
                break;
        case S32K14X_SCLK_LPO128K:
                buf[array_num] = SOCCFG_CLK_LPO128K_VALUE;
                array_num++;
                break;
        case S32K14X_SCLK_SCG_CLKOUT:
                buf[array_num] = scg_clkout;
                array_num++;
                break;
        case S32K14X_SCLK_LPO:
                buf[array_num] = SOCCFG_CLK_LPOCLK_VALUE;
                array_num++;
                break;
        case S32K14X_SCLK_CLKOUT:
                if (SIM->CHIPCTL & SIM_CHIPCTL_CLKOUTEN_MASK) {
                        xwu32_t clkcoutsrc, clkoutdiv;

                        switch ((SIM->CHIPCTL & SIM_CHIPCTL_CLKOUTSEL_MASK) >>
                                SIM_CHIPCTL_CLKOUTSEL_SHIFT) {
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SCG_CLKOUT:
                                clkcoutsrc = scg_clkout;
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SOSC_DIV2_CLK:
                                if (SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK) {
                                        clkcoutsrc = xwscdiv2_clk;
                                } else {
                                        rc = -ESHUTDOWN;
                                }
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SIRC_DIV2_CLK:
                                if (SCG->SIRCCSR & SCG_SIRCCSR_SIRCEN_MASK) {
                                        clkcoutsrc = sircdiv2_clk;
                                } else {
                                        rc = -ESHUTDOWN;
                                }
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_FIRC_DIV2_CLK:
                                if (SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK) {
                                        clkcoutsrc = fircdiv2_clk;
                                } else {
                                        rc = -ESHUTDOWN;
                                }
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_HCLK:
                                clkcoutsrc = core_clk;
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SPLL_DIV2_CLK:
                                if (SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) {
                                        clkcoutsrc = splldiv2_clk;
                                } else {
                                        rc = -ESHUTDOWN;
                                }
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_BUS_CLK:
                                clkcoutsrc = bus_clk;
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_LPO128K_CLK:
                                clkcoutsrc = SOCCFG_CLK_LPO128K_VALUE;
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_LPO_CLK:
                                clkcoutsrc = SOCCFG_CLK_LPOCLK_VALUE;
                                break;
                        case SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_RTC_CLK:
                                clkcoutsrc = lpo_rtc_clk;
                                break;
                        default:
                                rc = -ESHUTDOWN;
                                break;
                        }
                        if (OK == rc) {
                                clkoutdiv = ((SIM->CHIPCTL & SIM_CHIPCTL_CLKOUTDIV_MASK)
                                             >> SIM_CHIPCTL_CLKOUTDIV_SHIFT) + 1;
                                buf[array_num] = clkcoutsrc / clkoutdiv;
                                array_num++;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_VCO:
                if (SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) {
                        buf[array_num] = pllvco;
                        array_num++;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_SPLLDIV1_CLK:
                if (SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) {
                        if (splldiv1) {
                                buf[array_num] = splldiv1_clk;
                                array_num++;
                        } else {
                                rc = -ESHUTDOWN;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_SPLLDIV2_CLK:
                if (SCG->SPLLCSR & SCG_SPLLCSR_SPLLEN_MASK) {
                        if (splldiv2) {
                                buf[array_num] = splldiv2_clk;
                                array_num++;
                        } else {
                                rc = -ESHUTDOWN;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_FIRCDIV1_CLK:
                if (SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK) {
                        if (fircdiv1) {
                                buf[array_num] = fircdiv1_clk;
                                array_num++;
                        } else {
                                rc = -ESHUTDOWN;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_FIRCDIV2_CLK:
                if (SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK) {
                        if (fircdiv2) {
                                buf[array_num] = fircdiv2_clk;
                                array_num++;
                        } else {
                                rc = -ESHUTDOWN;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_SIRCDIV1_CLK:
                if (SCG->SIRCCSR & SCG_SIRCCSR_SIRCEN_MASK) {
                        if (sircdiv1) {
                                buf[array_num] = sircdiv1_clk;
                                array_num++;
                        } else {
                                rc = -ESHUTDOWN;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_SIRCDIV2_CLK:
                if (SCG->SIRCCSR & SCG_SIRCCSR_SIRCEN_MASK) {
                        if (sircdiv2) {
                                buf[array_num] = sircdiv2_clk;
                                array_num++;
                        } else {
                                rc = -ESHUTDOWN;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_SOSCDIV1_CLK:
                if (SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK) {
                        if (xwscdiv1) {
                                buf[array_num] = xwscdiv1_clk;
                                array_num++;
                        } else {
                                rc = -ESHUTDOWN;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_SCLK_SOSCDIV2_CLK:
                if (SCG->SOSCCSR & SCG_SOSCCSR_SOSCEN_MASK) {
                        if (xwscdiv2) {
                                buf[array_num] = xwscdiv2_clk;
                                array_num++;
                        } else {
                                rc = -ESHUTDOWN;
                        }
                } else {
                        rc = -ESHUTDOWN;
                }
                break;

        /* module clocks */
        case S32K14X_MCLK_SRAM:
                buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_CMU:
                buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_PMC:
                buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_SIM:
                buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_RCM:
                buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_LPO128K_VALUE;
                array_num += 3;
                break;
        case S32K14X_MCLK_MSCM:
                buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_MPU:
                buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_DMA:
                buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_ERM:
                buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_EIM:
                buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_FTFC:
                if (PCC->PCCn[PCC_FTFC_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = slow_clk;
                        array_num += 1;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_TRGMUX:
                buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                array_num += 1;
                break;
        case S32K14X_MCLK_DMAMUX:
                if (PCC->PCCn[PCC_DMAMUX_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        array_num += 1;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_FlexCAN0:
                if (PCC->PCCn[PCC_FlexCAN0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                        if (CAN0->CTRL1 & CAN_CTRL1_CLKSRC_MASK) {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = sys_clk;
                        } else {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = xwscdiv2_clk;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_FlexCAN1:
                if (PCC->PCCn[PCC_FlexCAN1_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                        if (CAN1->CTRL1 & CAN_CTRL1_CLKSRC_MASK) {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = sys_clk;
                        } else {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = xwscdiv2_clk;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_FTM3:
                if (PCC->PCCn[PCC_FTM3_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        switch ((FTM3->SC & FTM_SC_CLKS_MASK) >> FTM_SC_CLKS_SHIFT) {
                        case SOC_TMR_FTM_CLKSRC_NONE:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                break;
                        case SOC_TMR_FTM_CLKSRC_SYSCLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_EXT_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv1_clk;
                                break;
                        default:
                                rc = -EFAULT;
                                break;

                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_ADC1:
                if (PCC->PCCn[PCC_ADC1_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_FlexCAN2:
#if SOC_CHIP_S32K >= 144
                if (PCC->PCCn[PCC_FlexCAN2_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                        if (CAN2->CTRL1 & CAN_CTRL1_CLKSRC_MASK) {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = sys_clk;
                        } else {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = xwscdiv2_clk;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_LPSPI0:
                if (PCC->PCCn[PCC_LPSPI0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_LPSPI1:
                if (PCC->PCCn[PCC_LPSPI1_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_LPSPI2:
#if SOC_CHIP_S32K >= 144
                if (PCC->PCCn[PCC_LPSPI2_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_PDB1:
                if (PCC->PCCn[PCC_PDB1_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        array_num += 1;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_CRC:
                if (PCC->PCCn[PCC_CRC_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        array_num += 1;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_PDB0:
                if (PCC->PCCn[PCC_PDB0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        array_num += 1;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_LPIT:
                if (PCC->PCCn[PCC_LPIT_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_FTM0:
                if (PCC->PCCn[PCC_FTM0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv1_clk;
                        switch ((FTM0->SC & FTM_SC_CLKS_MASK) >> FTM_SC_CLKS_SHIFT) {
                        case SOC_TMR_FTM_CLKSRC_NONE:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                break;
                        case SOC_TMR_FTM_CLKSRC_SYSCLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_EXT_CLK:
                                break;
                        default:
                                rc = -EFAULT;
                                break;

                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_FTM1:
                if (PCC->PCCn[PCC_FTM1_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv1_clk;
                        switch ((FTM1->SC & FTM_SC_CLKS_MASK) >> FTM_SC_CLKS_SHIFT) {
                        case SOC_TMR_FTM_CLKSRC_NONE:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                break;
                        case SOC_TMR_FTM_CLKSRC_SYSCLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_EXT_CLK:
                                break;
                        default:
                                rc = -EFAULT;
                                break;

                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_FTM2:
                if (PCC->PCCn[PCC_FTM2_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv1_clk;
                        switch ((FTM2->SC & FTM_SC_CLKS_MASK) >> FTM_SC_CLKS_SHIFT) {
                        case SOC_TMR_FTM_CLKSRC_NONE:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                break;
                        case SOC_TMR_FTM_CLKSRC_SYSCLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_EXT_CLK:
                                break;
                        default:
                                rc = -EFAULT;
                                break;

                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_ADC0:
                if (PCC->PCCn[PCC_ADC0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_RTC:
                if (PCC->PCCn[PCC_RTC_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        if (RTC->CR & RTC_CR_LPOS_MASK) {
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = SOCCFG_CLK_LPO1K_VALUE;
                        } else {
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_LPTMR0:
                if (PCC->PCCn[PCC_LPTMR0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        switch ((LPTMR0->PSR & LPTMR_PSR_PCS_MASK) >>
                                LPTMR_PSR_PCS_MASK) {
                        case SOC_TMR_LPTMR_PSR_PCS_SIRCDIV2_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sircdiv2_clk;
                                break;
                        case SOC_TMR_LPTMR_PSR_PCS_LPO1K_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = SOC_TMR_LPTMR_PSR_PCS_LPO1K_CLK;
                                break;
                        case SOC_TMR_LPTMR_PSR_PCS_RTC_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_LPTMR_PSR_PCS_PCC:
                                break;
                        default:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                rc = -EFAULT;
                                break;
                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_PORTA:
                if (PCC->PCCn[PCC_PORTA_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                        if (PORTA->DFCR & PORT_DFCR_CS_MASK) {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_LPO128K_VALUE;
                        } else {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = bus_clk;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_PORTB:
                if (PCC->PCCn[PCC_PORTB_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                        if (PORTB->DFCR & PORT_DFCR_CS_MASK) {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_LPO128K_VALUE;
                        } else {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = bus_clk;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_PORTC:
                if (PCC->PCCn[PCC_PORTC_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                        if (PORTC->DFCR & PORT_DFCR_CS_MASK) {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_LPO128K_VALUE;
                        } else {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = bus_clk;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_PORTD:
                if (PCC->PCCn[PCC_PORTD_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                        if (PORTD->DFCR & PORT_DFCR_CS_MASK) {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_LPO128K_VALUE;
                        } else {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = bus_clk;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_PORTE:
                if (PCC->PCCn[PCC_PORTE_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                        if (PORTE->DFCR & PORT_DFCR_CS_MASK) {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_LPO128K_VALUE;
                        } else {
                                buf[SOC_CLK_FQCYIDX_ADDITION] = bus_clk;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_SAI0:
#if SOC_CHIP_S32K >= 148
                if (PCC->PCCn[PCC_SAI0_INDEX] & PCC_PCCn_CGC_MASK) {
                        xwu32_t tcr_msel, rcr_msel;

                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        tcr_msel = ((SAI0->TCR2 & SAI_TCR2_MSEL_MASK) >>
                                    SAI_TCR2_MSEL_SHIFT);
                        rcr_msel = ((SAI0->RCR2 & SAI_RCR2_MSEL_MASK) >>
                                    SAI_RCR2_MSEL_SHIFT);
                        switch (tcr_msel) {
                        case SOC_CLK_SAI_xCR2_MSEL_BUS_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = bus_clk;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SAI0_MCLK_IN:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = SOCCFG_CLK_SAI0_MCLK_VALUE;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SOSCDIV1_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = xwscdiv1_clk;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SAI1_MCLK_IN:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = SOCCFG_CLK_SAI1_MCLK_VALUE;
                                break;
                        }
                        switch (rcr_msel) {
                        case SOC_CLK_SAI_xCR2_MSEL_BUS_CLK:
                                buf[SOC_CLK_FQCYIDX_ADDITION] = bus_clk;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SAI0_MCLK_IN:
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_SAI0_MCLK_VALUE;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SOSCDIV1_CLK:
                                buf[SOC_CLK_FQCYIDX_ADDITION] = xwscdiv1_clk;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SAI1_MCLK_IN:
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_SAI1_MCLK_VALUE;
                                break;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_SAI1:
#if SOC_CHIP_S32K >= 148
                if (PCC->PCCn[PCC_SAI1_INDEX] & PCC_PCCn_CGC_MASK) {
                        xwu32_t tcr_msel, rcr_msel;

                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        tcr_msel = ((SAI1->TCR2 & SAI_TCR2_MSEL_MASK) >>
                                    SAI_TCR2_MSEL_SHIFT);
                        rcr_msel = ((SAI1->RCR2 & SAI_RCR2_MSEL_MASK) >>
                                    SAI_RCR2_MSEL_SHIFT);
                        switch (tcr_msel) {
                        case SOC_CLK_SAI_xCR2_MSEL_BUS_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = bus_clk;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SAI0_MCLK_IN:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = SOCCFG_CLK_SAI0_MCLK_VALUE;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SOSCDIV1_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = xwscdiv1_clk;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SAI1_MCLK_IN:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = SOCCFG_CLK_SAI1_MCLK_VALUE;
                                break;
                        }
                        switch (rcr_msel) {
                        case SOC_CLK_SAI_xCR2_MSEL_BUS_CLK:
                                buf[SOC_CLK_FQCYIDX_ADDITION] = bus_clk;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SAI0_MCLK_IN:
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_SAI0_MCLK_VALUE;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SOSCDIV1_CLK:
                                buf[SOC_CLK_FQCYIDX_ADDITION] = xwscdiv1_clk;
                                break;
                        case SOC_CLK_SAI_xCR2_MSEL_SAI1_MCLK_IN:
                                buf[SOC_CLK_FQCYIDX_ADDITION] = SOCCFG_CLK_SAI1_MCLK_VALUE;
                                break;
                        }
                        array_num += 3;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FlexIO:
                if (PCC->PCCn[PCC_FlexIO_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_WDOG:
                buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                buf[SOC_CLK_FQCYIDX_FUNCTION] = SOCCFG_CLK_LPOCLK_VALUE;
                array_num += 2;
                break;
        case S32K14X_MCLK_EWM:
                if (PCC->PCCn[PCC_EWM_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = SOCCFG_CLK_LPOCLK_VALUE;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_LPI2C0:
                if (PCC->PCCn[PCC_LPI2C0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_LPI2C1:
#if SOC_CHIP_S32K >= 148
                if (PCC->PCCn[PCC_LPI2C1_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_LPUART0:
                if (PCC->PCCn[PCC_LPUART0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_LPUART1:
                if (PCC->PCCn[PCC_LPUART1_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_LPUART2:
#if SOC_CHIP_S32K >= 144
                if (PCC->PCCn[PCC_LPUART2_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv2_clk;
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM4:
#if SOC_CHIP_S32K >= 146
                if (PCC->PCCn[PCC_FTM4_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv1_clk;
                        switch ((FTM4->SC & FTM_SC_CLKS_MASK) >> FTM_SC_CLKS_SHIFT) {
                        case SOC_TMR_FTM_CLKSRC_NONE:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                break;
                        case SOC_TMR_FTM_CLKSRC_SYSCLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_EXT_CLK:
                                break;
                        default:
                                rc = -EFAULT;
                                break;

                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM5:
#if SOC_CHIP_S32K >= 146
                if (PCC->PCCn[PCC_FTM5_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv1_clk;
                        switch ((FTM5->SC & FTM_SC_CLKS_MASK) >> FTM_SC_CLKS_SHIFT) {
                        case SOC_TMR_FTM_CLKSRC_NONE:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                break;
                        case SOC_TMR_FTM_CLKSRC_SYSCLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_EXT_CLK:
                                break;
                        default:
                                rc = -EFAULT;
                                break;

                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM6:
#if SOC_CHIP_S32K >= 148
                if (PCC->PCCn[PCC_FTM6_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv1_clk;
                        switch ((FTM6->SC & FTM_SC_CLKS_MASK) >> FTM_SC_CLKS_SHIFT) {
                        case SOC_TMR_FTM_CLKSRC_NONE:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                break;
                        case SOC_TMR_FTM_CLKSRC_SYSCLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_EXT_CLK:
                                break;
                        default:
                                rc = -EFAULT;
                                break;

                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_FTM7:
#if SOC_CHIP_S32K >= 148
                if (PCC->PCCn[PCC_FTM7_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = sys_clk;
                        buf[SOC_CLK_FQCYIDX_FUNCTION] = splldiv1_clk;
                        switch ((FTM7->SC & FTM_SC_CLKS_MASK) >> FTM_SC_CLKS_SHIFT) {
                        case SOC_TMR_FTM_CLKSRC_NONE:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = 0;
                                break;
                        case SOC_TMR_FTM_CLKSRC_SYSCLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = sys_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_FIXFQCY_CLK:
                                buf[SOC_CLK_FQCYIDX_FUNCTION] = lpo_rtc_clk;
                                break;
                        case SOC_TMR_FTM_CLKSRC_EXT_CLK:
                                break;
                        default:
                                rc = -EFAULT;
                                break;

                        }
                        array_num += 2;
                } else {
                        rc = -ESHUTDOWN;
                }
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_CMP0:
                if (PCC->PCCn[PCC_CMP0_INDEX] & PCC_PCCn_CGC_MASK) {
                        buf[SOC_CLK_FQCYIDX_BUS] = bus_clk;
                        array_num += 1;
                } else {
                        rc = -ESHUTDOWN;
                }
                break;
        case S32K14X_MCLK_QSPI:
#if SOC_CHIP_S32K >= 148
                /* TODO */
                rc = -ENOENT;
#else
                rc = -ENOENT;
#endif
                break;
        case S32K14X_MCLK_ENET:
#if SOC_CHIP_S32K >= 148
                /* TODO */
                rc = -ENOENT;
#else
                rc = -ENOENT;
#endif
                break;
        default:
                rc = -EINVAL;
                break;
        }

err_syserr:
        soc_cpuirq_restore_lc(cpuirq);
err_inval:
        *num = array_num;
        return rc;
}

/******** ******** GPIO operations ******** ********/
static __xwds_code
xwer_t s32k14x_soc_drv_gpio_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        volatile PORT_Type * soc_port_reg;
        volatile GPIO_Type * soc_pin_reg;
        const struct soc_gpio_portcfg * portcfg;
        xwssq_t pin;

        soc_port_reg = (volatile PORT_Type *)(PORTA_BASE +
                                              (SOC_GPIO_PORT_REG_OFFSET * port));
        soc_pin_reg = (volatile GPIO_Type *)(PTA_BASE +
                                             (SOC_GPIO_PIN_REG_OFFSET * port));

        portcfg = soc->gpio.cfg;
        do {
                pin = xwbop_ffs(xwsq_t, pinmask);
                if (pin >= 0) {
                        soc_port_reg->PCR[pin] = portcfg[port].pincfg[pin].pcr.u32 & 0xFFFFU;
                        if (portcfg[port].pincfg[pin].io.bit.pid) {
                                soc_pin_reg->PIDR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PIDR &= (xwu32_t)(~BIT(pin));
                        }
                        if (portcfg[port].pincfg[pin].io.bit.pdd) {
                                soc_pin_reg->PDDR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PDDR &= (xwu32_t)(~BIT(pin));
                        }
                        if (portcfg[port].pincfg[pin].io.bit.dov) {
                                soc_pin_reg->PSOR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PCOR |= (xwu32_t)BIT(pin);
                        }
                        xwbop_c0m(xwsq_t, &pinmask, (xwsq_t)BIT(pin));
                }
        } while (pinmask);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask)
{
        volatile PORT_Type * soc_port_reg;
        volatile GPIO_Type * soc_pin_reg;
        const struct soc_gpio_portcfg * portcfg;
        xwssq_t pin;

        soc_port_reg = (volatile PORT_Type *)(PORTA_BASE +
                                              (SOC_GPIO_PORT_REG_OFFSET * port));
        soc_pin_reg = (volatile GPIO_Type *)(PTA_BASE +
                                             (SOC_GPIO_PIN_REG_OFFSET * port));

        portcfg = soc->gpio.cfg;
        do {
                pin = xwbop_ffs(xwsq_t, pinmask);
                if (pin >= 0) {
                        soc_port_reg->PCR[pin] = SOC_GPIO_PORT_PCR_DEFAULT_VALUE;
                        if (portcfg[port].pincfg[pin].io.bit.pid) {
                                soc_pin_reg->PIDR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PIDR &= (xwu32_t)(~BIT(pin));
                        }
                        if (portcfg[port].pincfg[pin].io.bit.pdd) {
                                soc_pin_reg->PDDR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PDDR &= (xwu32_t)(~BIT(pin));
                        }
                        if (portcfg[port].pincfg[pin].io.bit.dov) {
                                soc_pin_reg->PSOR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PCOR |= (xwu32_t)BIT(pin);
                        }
                        xwbop_c0m(xwsq_t, &pinmask, (xwsq_t)BIT(pin));
                }
        } while (pinmask);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_cfg(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask,
                                void * cfg)
{
        volatile PORT_Type * soc_port_reg;
        volatile GPIO_Type * soc_pin_reg;
        const struct soc_gpio_portcfg * portcfg;
        xwssq_t pin;

        XWOS_UNUSED(soc);
        soc_port_reg = (volatile PORT_Type *)(PORTA_BASE +
                                              (SOC_GPIO_PORT_REG_OFFSET * port));
        soc_pin_reg = (volatile GPIO_Type *)(PTA_BASE +
                                             (SOC_GPIO_PIN_REG_OFFSET * port));

        portcfg = cfg;

        soc_port_reg->DFCR = portcfg[port].dfcr.u32;
        soc_port_reg->DFWR = portcfg[port].dfwr.u32;
        soc_port_reg->DFER = portcfg[port].dfer.u32;
        do {
                pin = xwbop_ffs(xwsq_t, pinmask);
                if (pin >= 0) {
                        soc_port_reg->PCR[pin] = portcfg[port].pincfg[pin].pcr.u32;
                        if (portcfg[port].pincfg[pin].io.bit.pid) {
                                soc_pin_reg->PIDR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PIDR &= (xwu32_t)(~BIT(pin));
                        }
                        if (portcfg[port].pincfg[pin].io.bit.pdd) {
                                soc_pin_reg->PDDR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PDDR &= (xwu32_t)(~BIT(pin));
                        }
                        if (portcfg[port].pincfg[pin].io.bit.dov) {
                                soc_pin_reg->PSOR |= (xwu32_t)BIT(pin);
                        } else {
                                soc_pin_reg->PCOR |= (xwu32_t)BIT(pin);
                        }
                        xwbop_c0m(xwsq_t, &pinmask, (xwsq_t)BIT(pin));
                }
        } while (pinmask);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_set(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask)
{
        volatile GPIO_Type * soc_pin_reg;

        XWOS_UNUSED(soc);
        soc_pin_reg = (volatile GPIO_Type *)(PTA_BASE +
                                             (SOC_GPIO_PIN_REG_OFFSET * port));
        soc_pin_reg->PSOR |= (xwu32_t)pinmask;
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_reset(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        volatile GPIO_Type * soc_pin_reg;

        XWOS_UNUSED(soc);
        soc_pin_reg = (volatile GPIO_Type *)(PTA_BASE +
                                             (SOC_GPIO_PIN_REG_OFFSET * port));
        soc_pin_reg->PCOR |= (xwu32_t)pinmask;
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_toggle(struct xwds_soc * soc,
                                   xwid_t port, xwsq_t pinmask)
{
        volatile GPIO_Type * soc_pin_reg;
        xwreg_t cpuirq;

        XWOS_UNUSED(soc);
        soc_pin_reg = (volatile GPIO_Type *)(PTA_BASE +
                                             (SOC_GPIO_PIN_REG_OFFSET * port));

        soc_cpuirq_save_lc(&cpuirq);
        soc_pin_reg->PDOR ^= (xwu32_t)pinmask;
        soc_cpuirq_restore_lc(cpuirq);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_output(struct xwds_soc * soc,
                                   xwid_t port, xwsq_t pinmask,
                                   xwsq_t out)
{
        volatile GPIO_Type * soc_pin_reg;
        xwreg_t cpuirq;

        XWOS_UNUSED(soc);
        soc_pin_reg = (volatile GPIO_Type *)(PTA_BASE +
                                             (SOC_GPIO_PIN_REG_OFFSET * port));

        soc_cpuirq_save_lc(&cpuirq);
        soc_pin_reg->PDOR &= (xwu32_t)(~(out ^ pinmask));
        soc_pin_reg->PDOR |= (xwu32_t)(out & pinmask);
        soc_cpuirq_restore_lc(cpuirq);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_gpio_input(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwsq_t * in)
{
        volatile GPIO_Type * soc_pin_reg;

        XWOS_UNUSED(soc);
        soc_pin_reg = (volatile GPIO_Type *)(PTA_BASE +
                                             (SOC_GPIO_PIN_REG_OFFSET * port));

        *in = (xwsq_t)(soc_pin_reg->PIDR & pinmask);
        return OK;
}

/******** ******** EIRQ operations ******** ********/
static __xwds_code
xwer_t s32k14x_soc_drv_eirq_req(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask,
                                xwid_t eiid, xwsq_t eiflag)
{
        volatile PORT_Type * soc_port_reg;
        xwu32_t irqc;
        xwer_t rc;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(eiid);

        soc_port_reg = (volatile PORT_Type *)(PORTA_BASE +
                                              (SOC_GPIO_PORT_REG_OFFSET * port));
        rc = OK;

        switch (XWDS_SOC_EIF_TM_MASK & eiflag) {
        case XWDS_SOC_EIF_TM_RISING:
                irqc = PORT_PCR_IRQC(SOC_EIRQC_IRQ_RISING);
                break;
        case XWDS_SOC_EIF_TM_FALLING:
                irqc = PORT_PCR_IRQC(SOC_EIRQC_IRQ_FALLING);
                break;
        case XWDS_SOC_EIF_TM_EITHER:
                irqc = PORT_PCR_IRQC(SOC_EIRQC_IRQ_EITHER);
                break;
        case XWDS_SOC_EIF_TM_LOW:
                irqc = PORT_PCR_IRQC(SOC_EIRQC_IRQ_LOW);
                break;
        case XWDS_SOC_EIF_TM_HIGH:
                irqc = PORT_PCR_IRQC(SOC_EIRQC_IRQ_HIGH);
                break;
        case XWDS_SOC_EIF_TM_RISING | XWDS_SOC_EIF_DMA:
                irqc = PORT_PCR_IRQC(SOC_EIRQC_DMA_RISING);
                break;
        case XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_DMA:
                irqc = PORT_PCR_IRQC(SOC_EIRQC_DMA_FALLING);
                break;
        case XWDS_SOC_EIF_TM_EITHER | XWDS_SOC_EIF_DMA:
                irqc = PORT_PCR_IRQC(SOC_EIRQC_DMA_EITHER);
                break;
        default:
                rc = EINVAL;
                goto err_inval;
                break;
        }
        irqc |= PORT_PCR_ISF_MASK;
        soc_port_reg->GICLR = irqc | (pinmask & 0xFFFFU);
        soc_port_reg->GICHR = irqc | ((pinmask >> 16) & 0xFFFFU);

err_inval:
        return rc;
}

static __xwds_code
xwer_t s32k14x_soc_drv_eirq_rls(struct xwds_soc * soc,
                                xwid_t port, xwsq_t pinmask,
                                xwid_t eiid)
{
        volatile PORT_Type * soc_port_reg;
        xwu32_t irqc;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(eiid);

        soc_port_reg = (volatile PORT_Type *)(PORTA_BASE +
                                              (SOC_GPIO_PORT_REG_OFFSET * port));
        irqc = PORT_PCR_IRQC(SOC_EIRQC_DISABLE);
        soc_port_reg->GICLR = irqc | (pinmask & 0xFFFFU);
        soc_port_reg->GICHR = irqc | ((pinmask >> 16) & 0xFFFFU);
        return OK;
}

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
/******** ******** DMA operations ******** ********/
static __xwds_code
xwer_t s32k14x_soc_drv_dma_req(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        DMA->SEEI = (xwu8_t)(ch & 0xF);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_dma_rls(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        DMA->CEEI = (xwu8_t)(ch & 0xF);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_dma_cfg(struct xwds_soc * soc, xwid_t ch,
                               void * cfg)
{
        struct soc_dmach_private_cfg * chcfg;

        XWOS_UNUSED(soc);
        chcfg = cfg;
        DMAMUX->CHCFG[(ch ^ 0x3)] = chcfg->dmamux.u8;
        memcpy((void *)&DMA->TCD[ch], chcfg->tcd, sizeof(struct soc_dma_tcd));
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_dma_enable(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        DMA->SERQ = (xwu8_t)(ch & 0xF);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_dma_disable(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        DMA->CERQ = (xwu8_t)(ch & 0xF);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_dma_start(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        DMA->SSRT = (xwu8_t)(ch & 0xF);
        return OK;
}

static __xwds_code
xwer_t s32k14x_soc_drv_dma_stop(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(ch);
        DMA->TCD[ch].CSR &= ~DMA_TCD_CSR_START_MASK;
        return OK;
}
#endif /* XWMDCFG_ds_SOC_DMA */

/******** ******** isr ******** ********/
__xwbsp_isr
void s32k14x_porta_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t isf;

        xwos_irq_get_data(SOC_IRQ_PORTA, &irqdata);
        soc = irqdata.data;
        do {
                isf = PORTA->ISFR;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, isf);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        PORTA->ISFR |= BIT(eiid);
                }
        } while (1);
}

__xwbsp_isr
void s32k14x_portb_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t isf;

        xwos_irq_get_data(SOC_IRQ_PORTB, &irqdata);
        soc = irqdata.data;
        do {
                isf = PORTB->ISFR;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, isf);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        PORTB->ISFR |= BIT(eiid);
                }
        } while (1);
}

__xwbsp_isr
void s32k14x_portc_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t isf;

        xwos_irq_get_data(SOC_IRQ_PORTC, &irqdata);
        soc = irqdata.data;
        do {
                isf = PORTC->ISFR;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, isf);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        PORTC->ISFR |= BIT(eiid);
                }
        } while (1);
}

__xwbsp_isr
void s32k14x_portd_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t isf;

        xwos_irq_get_data(SOC_IRQ_PORTD, &irqdata);
        soc = irqdata.data;
        do {
                isf = PORTD->ISFR;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, isf);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        PORTD->ISFR |= BIT(eiid);
                }
        } while (1);
}

__xwbsp_isr
void s32k14x_porte_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwsid_t eiid;
        xwu32_t isf;

        xwos_irq_get_data(SOC_IRQ_PORTE, &irqdata);
        soc = irqdata.data;
        do {
                isf = PORTE->ISFR;
                eiid = (xwsid_t)xwbop_fls(xwu32_t, isf);
                if (eiid >= 0) {
                        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                                soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                                                     soc->eirq.isrargs[eiid]);
                        }
                        PORTE->ISFR |= BIT(eiid);
                }
        } while (1);
}

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
__xwbsp_isr
void s32k14x_edma_cmberr_isr(void)
{
        xwirq_t irqn;
        struct soc_irq_data irqdata;
        struct xwds_soc * soc;
        xwu32_t err;
        xwsid_t ch;

        xwos_irq_get_id(&irqn);
        xwos_irq_get_data(irqn, &irqdata);
        soc = irqdata.data;

        do {
                err = DMA->ERR;
                ch = (xwsid_t)xwbop_ffs(xwu32_t, err);
                if (ch < 0) {
                        break;
                }
                if ((soc->dma.chcbs) && (soc->dma.chcbs[ch])) {
                        if (soc->dma.chcbargs) {
                                soc->dma.chcbs[ch](soc, (xwid_t)ch, DMA->ES,
                                                   soc->dma.chcbargs[ch]);
                        } else {
                                soc->dma.chcbs[ch](soc, (xwid_t)ch, DMA->ES,
                                                   NULL);
                        }
                }
                DMA->CERR = (xwu8_t)(ch & 0xF);
                DMA->CR &= ~DMA_CR_HALT_MASK;
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
#endif /* XWMDCFG_ds_SOC_DMA */
