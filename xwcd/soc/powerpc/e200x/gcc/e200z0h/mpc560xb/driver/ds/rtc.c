/**
 * @file
 * @brief 设备栈驱动：MPC560xB Internal RTC
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
#include <xwos/lib/xwlog.h>
#include <xwos/irq.h>
#include <soc.h>
#include <soc_irq.h>
#include <soc_rtc.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/clock.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/misc/chip.h>
#include <driver/ds/rtc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types      ******** ******** *********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_rtc_check_desc(struct xwds_misc * rtc);

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_remove(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_stop(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_resume(struct xwds_device * dev);

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_ioctl(struct xwds_misc * rtc, xwsq_t cmd, va_list args);

static __xwbsp_code
xwer_t mpc560xb_rtc_enable(struct xwds_misc * rtc);

static __xwbsp_code
xwer_t mpc560xb_rtc_disable(struct xwds_misc * rtc);

static __xwbsp_code
xwer_t mpc560xb_rtc_reset(struct xwds_misc * rtc);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct xwds_misc_driver mpc560xb_rtc_drv = {
        .base = {
                .name = "mpc560xb.rtc",
                .probe = mpc560xb_rtc_drv_probe,
                .remove = mpc560xb_rtc_drv_remove,
                .start = mpc560xb_rtc_drv_start,
                .stop = mpc560xb_rtc_drv_stop,
                .suspend = mpc560xb_rtc_drv_suspend,
                .resume = mpc560xb_rtc_drv_resume,
        },
        .ioctl = mpc560xb_rtc_drv_ioctl,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t mpc560xb_rtc_check_desc(struct xwds_misc * rtc)
{
        const struct xwds_resources * resources;
        const struct soc_rtc_private_cfg * cfg;
        xwer_t rc;

        resources = rtc->dev.resources;
        cfg = rtc->misccfg;
        if (__unlikely(is_err_or_null(resources))) {
                rc = -EINVAL;
        } else {
                if(__unlikely(is_err_or_null(cfg))) {
                        rc = -EINVAL;
                } else {
                        rc = OK;
                }
        }

        return rc;
}

/******** ******** common driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_rtc_drv_probe(struct xwds_device * dev)
{
        struct xwds_misc * rtc;

        rtc = xwds_static_cast(struct xwds_misc *, dev);
        return mpc560xb_rtc_check_desc(rtc);
}

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_remove(struct xwds_device * dev)
{
        xwer_t rc;

        XWOS_UNUSED(dev);
        rc = OK;

        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_start(struct xwds_device * dev)
{
        struct xwds_misc * rtc;
        const struct xwds_resources * resources;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t i, j;
        xwer_t rc;

        rtc = xwds_static_cast(struct xwds_misc *, dev);
        resources = rtc->dev.resources;

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
        };

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
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                xwos_irq_release(irqrsc->irqn);
        }
err_req_irqs:
        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_stop(struct xwds_device * dev)
{
        struct xwds_misc * rtc;
        const struct xwds_resources * resources;
        volatile struct RTC_tag * rtcreg;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t j;

        rtc = xwds_static_cast(struct xwds_misc *, dev);
        resources = rtc->dev.resources;
        rtcreg = resources->regrsc_array[0].base;
        rtcreg->RTCC.R = 0;

        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }

        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
        }

        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                xwos_irq_release(irqrsc->irqn);
        }
        return OK;
}

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_suspend(struct xwds_device * dev)
{
        xwer_t rc;

        XWOS_UNUSED(dev);
        rc = OK;

        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_rtc_drv_resume(struct xwds_device * dev)
{
        xwer_t rc;

        XWOS_UNUSED(dev);
        rc = OK;

        return rc;
}

/******** ******** rtc driver ******** ********/
static __xwbsp_code
xwer_t mpc560xb_rtc_drv_ioctl(struct xwds_misc * rtc, xwsq_t cmd, va_list args)
{
        xwer_t rc;

        XWOS_UNUSED(args);
        switch(cmd) {
        case MPC560XB_RTC_IOC_ENABLE:
                rc = mpc560xb_rtc_enable(rtc);
                break;
        case MPC560XB_RTC_IOC_DISABLE:
                rc = mpc560xb_rtc_disable(rtc);
                break;
        case MPC560XB_RTC_IOC_RESET:
                rc = mpc560xb_rtc_reset(rtc);
                break;
        default:
                rc = -ENOSYS;
                break;
        }

        return rc;
}

static __xwbsp_code
xwer_t mpc560xb_rtc_enable(struct xwds_misc * rtc)
{
        const struct xwds_resources * resources;
        volatile struct RTC_tag * rtcreg;
        const struct soc_rtc_private_cfg * xwccfg;

        xwccfg = rtc->misccfg;
        resources = rtc->dev.resources;
        rtcreg = resources->regrsc_array[0].base;

        rtcreg->RTCC.B.APIEN = xwccfg->u.bit.apien;
        rtcreg->RTCC.B.APIIE = xwccfg->u.bit.apiie;
        rtcreg->RTCC.B.RTCIE = xwccfg->u.bit.rtcie;
        rtcreg->RTCC.B.CNTEN = 1;

        return OK;
}

static __xwbsp_code
xwer_t mpc560xb_rtc_disable(struct xwds_misc * rtc)
{
        const struct xwds_resources * resources;
        volatile struct RTC_tag * rtcreg;

        resources = rtc->dev.resources;
        rtcreg = resources->regrsc_array[0].base;

        rtcreg->RTCC.B.CNTEN = 0;
        rtcreg->RTCC.B.APIEN = 0;
        rtcreg->RTCC.B.APIIE = 0;
        rtcreg->RTCC.B.RTCIE = 0;

        return OK;
}

static __xwbsp_code
xwer_t mpc560xb_rtc_reset(struct xwds_misc * rtc)
{
        const struct xwds_resources * resources;
        volatile struct RTC_tag * rtcreg;
        const struct soc_rtc_private_cfg * xwccfg;

        xwccfg = rtc->misccfg;
        resources = rtc->dev.resources;
        rtcreg = resources->regrsc_array[0].base;

        /* RTC Control Register (RTCC) */
        rtcreg->RTCC.B.CNTEN = 0;
        rtcreg->RTCC.R = xwccfg->u.u32;
        rtcreg->RTCC.B.APIEN = 0;
        rtcreg->RTCC.B.APIIE = 0;
        rtcreg->RTCC.B.RTCIE = 0;

        return OK;
}

/******** ******** rtc isr ******** ********/
__xwbsp_isr
void mpc560xb_rtc_isr(void)
{
        /* RTC Interrupt Flag */
        if (RTC.RTCS.B.RTCF) {
                RTC.RTCS.B.RTCF = 1;
        }

        /* Counter Roll Over Interrupt Flag */
        if (RTC.RTCS.B.ROVRF) {
                RTC.RTCS.B.ROVRF = 1;
        }
}

__xwbsp_isr
void mpc560xb_api_isr(void)
{
        /* API Interrupt Flag */
        if (RTC.RTCS.B.APIF) {
                RTC.RTCS.B.APIF = 1;
        }
}
