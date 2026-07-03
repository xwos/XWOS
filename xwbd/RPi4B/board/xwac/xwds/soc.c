/**
 * @file
 * @brief RPi4B::XWDS::SOC
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

#include <board/std.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h>
#include <xwcd/soc/arm64/v8a/arch_gic2.h>
#include <xwcd/ds/soc/eirq.h>
#include "board/xwac/xwds/device.h"
#include "board/xwac/xwds/soc.h"

struct rpi4bxwds_soc_driver_data {
        struct xwos_splk splk;
};

static
xwer_t rpi4bxwds_soc_drv_probe(struct xwds_device * dev);

static
xwer_t rpi4bxwds_soc_drv_remove(struct xwds_device * dev);

static
xwer_t rpi4bxwds_soc_drv_start(struct xwds_device * dev);

static
xwer_t rpi4bxwds_soc_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t rpi4bxwds_soc_drv_suspend(struct xwds_device * dev);

static
xwer_t rpi4bxwds_soc_drv_resume(struct xwds_device * dev);
#endif

static
xwer_t rpi4bxwds_soc_drv_gpio_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t rpi4bxwds_soc_drv_gpio_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t rpi4bxwds_soc_drv_gpio_cfg(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  void * cfg);

static
xwer_t rpi4bxwds_soc_drv_gpio_set(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t rpi4bxwds_soc_drv_gpio_reset(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask);

static
xwer_t rpi4bxwds_soc_drv_gpio_toggle(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask);

static
xwer_t rpi4bxwds_soc_drv_gpio_output(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask,
                                     xwsq_t out);

static
xwer_t rpi4bxwds_soc_drv_gpio_input(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask,
                                    xwsq_t * in);

static
xwer_t rpi4bxwds_soc_drv_eirq_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid, xwsq_t eiflag);

static
xwer_t rpi4bxwds_soc_drv_eirq_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid);

static void rpi4bxwds_soc_eirq_bank0_isr(void);
static void rpi4bxwds_soc_eirq_bank1_isr(void);
static void rpi4bxwds_soc_eirq_bank2_isr(void);

const struct xwds_soc_driver rpi4bxwds_soc_drv = {
        .base = {
                .name = "rpi4bxwds.soc",
                .probe = rpi4bxwds_soc_drv_probe,
                .remove = rpi4bxwds_soc_drv_remove,
                .start = rpi4bxwds_soc_drv_start,
                .stop = rpi4bxwds_soc_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = rpi4bxwds_soc_drv_suspend,
                .resume = rpi4bxwds_soc_drv_resume,
#endif
        },

        .gpio_req = rpi4bxwds_soc_drv_gpio_req,
        .gpio_rls = rpi4bxwds_soc_drv_gpio_rls,
        .gpio_cfg = rpi4bxwds_soc_drv_gpio_cfg,
        .gpio_set = rpi4bxwds_soc_drv_gpio_set,
        .gpio_reset = rpi4bxwds_soc_drv_gpio_reset,
        .gpio_toggle = rpi4bxwds_soc_drv_gpio_toggle,
        .gpio_output = rpi4bxwds_soc_drv_gpio_output,
        .gpio_read_output = NULL,
        .gpio_input = rpi4bxwds_soc_drv_gpio_input,

        .eirq_req = rpi4bxwds_soc_drv_eirq_req,
        .eirq_rls = rpi4bxwds_soc_drv_eirq_rls,
};

atomic_xwsq_t rpi4bxwds_gpio_pin_state[] = {
        [XWDS_GPIO_PORT_A] = 0,
};

struct rpi4bxwds_soc_driver_data rpi4bxwds_soc_drvdata = {
        .splk = XWOS_SPLK_INITIALIZER,
};

#define RPI4B_EIRQ_NUM  58U
static xwds_eirq_f rpi4bxwds_eirq_isrs[RPI4B_EIRQ_NUM];
static xwds_eirq_arg_t rpi4bxwds_eirq_isrargs[RPI4B_EIRQ_NUM];

struct xwds_soc rpi4bxwds_soc = {
        .dev = {
                .name = "rpi4bxwds.soc",
                .id = 0,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &rpi4bxwds_soc_drv),
                .data = (void *)&rpi4bxwds_soc_drvdata,
        },
        .xwccfg = NULL,
        .gpio = {
                .cfg = NULL,
                .pins = rpi4bxwds_gpio_pin_state,
                .port_num = xw_array_size(rpi4bxwds_gpio_pin_state),
                .pin_num = 58,
        },
        .eirq = {
                .isrs = rpi4bxwds_eirq_isrs,
                .isrargs = rpi4bxwds_eirq_isrargs,
                .num = RPI4B_EIRQ_NUM,
        },
};

/******** ******** base driver ******** ********/
static
xwer_t rpi4bxwds_soc_drv_probe(struct xwds_device * dev)
{
        struct rpi4bxwds_soc_driver_data * drvdata;

        drvdata = dev->data;
        xwos_splk_init(&drvdata->splk);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO0, rpi4bxwds_soc_eirq_bank0_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_GPIO0, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_GPIO0, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_GPIO0);
        armv8a_gic_irq_enable(SOC_VC_IRQ_GPIO0);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO1, rpi4bxwds_soc_eirq_bank1_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_GPIO1, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_GPIO1, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_GPIO1);
        armv8a_gic_irq_enable(SOC_VC_IRQ_GPIO1);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO2, rpi4bxwds_soc_eirq_bank2_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_GPIO2, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_GPIO2, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_GPIO2);
        armv8a_gic_irq_enable(SOC_VC_IRQ_GPIO2);

        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_remove(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_start(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t rpi4bxwds_soc_drv_suspend(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_resume(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}
#endif

/******** ******** GPIO register helper ******** ********/
static
void rpi4bxwds_gpfsel_set(xwu32_t pin, xwu32_t func)
{
        volatile xwu32_t * reg;
        xwu32_t bitpos;
        xwu32_t val;

        bitpos = (pin % 10U) * 3U;
        switch (pin / 10U) {
        case 0U:
                reg = &soc_gpio.gpfsel0.u32;
                break;
        case 1U:
                reg = &soc_gpio.gpfsel1.u32;
                break;
        case 2U:
                reg = &soc_gpio.gpfsel2.u32;
                break;
        case 3U:
                reg = &soc_gpio.gpfsel3.u32;
                break;
        case 4U:
                reg = &soc_gpio.gpfsel4.u32;
                break;
        default:
                reg = &soc_gpio.gpfsel5.u32;
                break;
        }
        val = *reg;
        val &= ~(0x7U << bitpos);
        val |= (func & 0x7U) << bitpos;
        *reg = val;
}

static
void rpi4bxwds_pupd_set(xwu32_t pin, xwu32_t pud)
{
        volatile xwu32_t * reg;
        xwu32_t bitpos;
        xwu32_t val;

        bitpos = (pin % 16U) * 2U;
        switch (pin / 16U) {
        case 0U:
                reg = &soc_gpio.gpio_pup_pdn_cntrl_reg0.u32;
                break;
        case 1U:
                reg = &soc_gpio.gpio_pup_pdn_cntrl_reg1.u32;
                break;
        case 2U:
                reg = &soc_gpio.gpio_pup_pdn_cntrl_reg2.u32;
                break;
        default:
                reg = &soc_gpio.gpio_pup_pdn_cntrl_reg3.u32;
                break;
        }
        val = *reg;
        val &= ~(0x3U << bitpos);
        val |= (pud & 0x3U) << bitpos;
        *reg = val;
}

/******** ******** gpio operation driver ******** ********/
static
xwer_t rpi4bxwds_soc_drv_gpio_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_gpio_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_gpio_cfg(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  void * cfg)
{
        const struct rpi4bxwds_gpio_cfg * gpio_cfg;
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwsq_t copy;
        xwssq_t pin;
        xwreg_t cpuirq;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);

        gpio_cfg = (const struct rpi4bxwds_gpio_cfg *)cfg;
        drvdata = soc->dev.data;
        copy = pinmask;
        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        pin = xwbop_ffs(xwsq_t, copy);
        while (pin >= 0L) {
                rpi4bxwds_gpfsel_set(pin, gpio_cfg->function);
                rpi4bxwds_pupd_set(pin, gpio_cfg->pud);
                copy &= ~XWBOP_BIT(pin);
                pin = xwbop_ffs(xwsq_t, copy);
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_gpio_set(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwu32_t mask_lo;
        xwu32_t mask_hi;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);

        drvdata = soc->dev.data;
        mask_lo = (xwu32_t)(pinmask & 0xFFFFFFFFUL);
        mask_hi = (xwu32_t)((pinmask >> 32U) & 0xFFFFFFFFUL);

        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        if (0U != mask_lo) {
                soc_gpio.gpset0.u32 = mask_lo;
        }
        if (0U != mask_hi) {
                soc_gpio.gpset1.u32 = mask_hi;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_gpio_reset(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwu32_t mask_lo;
        xwu32_t mask_hi;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);

        drvdata = soc->dev.data;
        mask_lo = (xwu32_t)(pinmask & 0xFFFFFFFFUL);
        mask_hi = (xwu32_t)((pinmask >> 32U) & 0xFFFFFFFFUL);

        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        if (0U != mask_lo) {
                soc_gpio.gpclr0.u32 = mask_lo;
        }
        if (0U != mask_hi) {
                soc_gpio.gpclr1.u32 = mask_hi;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_gpio_toggle(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwu32_t mask_lo;
        xwu32_t mask_hi;
        xwu32_t lev_lo;
        xwu32_t lev_hi;
        xwu32_t set_lo;
        xwu32_t set_hi;
        xwu32_t clr_lo;
        xwu32_t clr_hi;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);

        drvdata = soc->dev.data;
        mask_lo = (xwu32_t)(pinmask & 0xFFFFFFFFUL);
        mask_hi = (xwu32_t)((pinmask >> 32U) & 0xFFFFFFFFUL);
        lev_lo = 0U;
        lev_hi = 0U;

        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        if (0U != mask_lo) {
                lev_lo = soc_gpio.gplev0.u32 & mask_lo;
        }
        if (0U != mask_hi) {
                lev_hi = soc_gpio.gplev1.u32 & mask_hi;
        }
        set_lo = mask_lo ^ lev_lo;
        set_hi = mask_hi ^ lev_hi;
        clr_lo = lev_lo;
        clr_hi = lev_hi;
        if (0U != set_lo) {
                soc_gpio.gpset0.u32 = set_lo;
        }
        if (0U != set_hi) {
                soc_gpio.gpset1.u32 = set_hi;
        }
        if (0U != clr_lo) {
                soc_gpio.gpclr0.u32 = clr_lo;
        }
        if (0U != clr_hi) {
                soc_gpio.gpclr1.u32 = clr_hi;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_gpio_output(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask,
                                     xwsq_t out)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwu32_t mask_lo;
        xwu32_t mask_hi;
        xwu32_t out_lo;
        xwu32_t out_hi;
        xwu32_t lev_lo;
        xwu32_t lev_hi;
        xwu32_t set_lo;
        xwu32_t set_hi;
        xwu32_t clr_lo;
        xwu32_t clr_hi;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);

        out &= pinmask;
        drvdata = soc->dev.data;
        mask_lo = (xwu32_t)(pinmask & 0xFFFFFFFFUL);
        mask_hi = (xwu32_t)((pinmask >> 32U) & 0xFFFFFFFFUL);
        out_lo = (xwu32_t)(out & 0xFFFFFFFFUL);
        out_hi = (xwu32_t)((out >> 32U) & 0xFFFFFFFFUL);
        lev_lo = 0U;
        lev_hi = 0U;

        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        if (0U != mask_lo) {
                lev_lo = soc_gpio.gplev0.u32 & mask_lo;
        }
        if (0U != mask_hi) {
                lev_hi = soc_gpio.gplev1.u32 & mask_hi;
        }
        set_lo = out_lo & (~lev_lo);
        set_hi = out_hi & (~lev_hi);
        clr_lo = (~out_lo) & lev_lo & mask_lo;
        clr_hi = (~out_hi) & lev_hi & mask_hi;
        if (0U != set_lo) {
                soc_gpio.gpset0.u32 = set_lo;
        }
        if (0U != set_hi) {
                soc_gpio.gpset1.u32 = set_hi;
        }
        if (0U != clr_lo) {
                soc_gpio.gpclr0.u32 = clr_lo;
        }
        if (0U != clr_hi) {
                soc_gpio.gpclr1.u32 = clr_hi;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_gpio_input(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask,
                                    xwsq_t * in)
{
        xwu32_t mask_lo;
        xwu32_t mask_hi;
        xwsq_t result;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);

        mask_lo = (xwu32_t)(pinmask & 0xFFFFFFFFUL);
        mask_hi = (xwu32_t)((pinmask >> 32U) & 0xFFFFFFFFUL);
        result = 0U;

        if (0U != mask_lo) {
                result |= (xwsq_t)(soc_gpio.gplev0.u32 & mask_lo);
        }
        if (0U != mask_hi) {
                result |= (xwsq_t)(soc_gpio.gplev1.u32 & mask_hi) << 32U;
        }
        *in = result;
        return XWOK;
}

/******** ******** EIRQ operation driver ******** ********/
#define EIRQ_BANK0_PIN_LO   0U
#define EIRQ_BANK0_PIN_HI   27U
#define EIRQ_BANK1_PIN_LO   28U
#define EIRQ_BANK1_PIN_HI   45U
#define EIRQ_BANK2_PIN_LO   46U
#define EIRQ_BANK2_PIN_HI   57U

static inline
xwu32_t rpi4bxwds_eirq_pin_bit(xwsq_t pin)
{
        return (xwu32_t)(1U << (pin & 0x1FU));
}

static
void rpi4bxwds_soc_eirq_bank0_isr(void)
{
        struct xwds_soc * soc;
        xwu32_t eds;
        xwsq_t p;

        soc = &rpi4bxwds_soc;
        eds = soc_gpio.gpeds0.u32;
        soc_gpio.gpeds0.u32 = eds;
        for (p = EIRQ_BANK0_PIN_LO; p <= EIRQ_BANK0_PIN_HI; p++) {
                if ((eds & rpi4bxwds_eirq_pin_bit(p)) &&
                    (soc->eirq.isrs[p])) {
                        soc->eirq.isrs[p](soc, (xwid_t)p, soc->eirq.isrargs[p]);
                }
        }
}

static
void rpi4bxwds_soc_eirq_bank1_isr(void)
{
        struct xwds_soc * soc;
        xwu32_t eds0;
        xwu32_t eds1;
        xwsq_t p;

        soc = &rpi4bxwds_soc;
        eds0 = soc_gpio.gpeds0.u32;
        eds1 = soc_gpio.gpeds1.u32;
        soc_gpio.gpeds0.u32 = eds0;
        soc_gpio.gpeds1.u32 = eds1;
        for (p = EIRQ_BANK1_PIN_LO; p <= 31U; p++) {
                if ((eds0 & rpi4bxwds_eirq_pin_bit(p)) &&
                    (soc->eirq.isrs[p])) {
                        soc->eirq.isrs[p](soc, (xwid_t)p, soc->eirq.isrargs[p]);
                }
        }
        for (p = 32U; p <= EIRQ_BANK1_PIN_HI; p++) {
                if ((eds1 & rpi4bxwds_eirq_pin_bit(p)) &&
                    (soc->eirq.isrs[p])) {
                        soc->eirq.isrs[p](soc, (xwid_t)p, soc->eirq.isrargs[p]);
                }
        }
}

static
void rpi4bxwds_soc_eirq_bank2_isr(void)
{
        struct xwds_soc * soc;
        xwu32_t eds;
        xwsq_t p;

        soc = &rpi4bxwds_soc;
        eds = soc_gpio.gpeds1.u32;
        soc_gpio.gpeds1.u32 = eds;
        for (p = EIRQ_BANK2_PIN_LO; p <= EIRQ_BANK2_PIN_HI; p++) {
                if ((eds & rpi4bxwds_eirq_pin_bit(p)) &&
                    (soc->eirq.isrs[p])) {
                        soc->eirq.isrs[p](soc, (xwid_t)p, soc->eirq.isrargs[p]);
                }
        }
}

static
xwer_t rpi4bxwds_soc_drv_eirq_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid, xwsq_t eiflag)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwu32_t bit;
        xwreg_t cpuirq;

        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);

        drvdata = soc->dev.data;
        bit = rpi4bxwds_eirq_pin_bit(eiid);
        if (eiid < (xwid_t)32U) {
                xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
                if (XWDS_SOC_EIF_TM_RISING & eiflag) {
                        soc_gpio.gparen0.u32 |= bit;
                } else {
                        soc_gpio.gparen0.u32 &= ~bit;
                }
                if (XWDS_SOC_EIF_TM_FALLING & eiflag) {
                        soc_gpio.gpafen0.u32 |= bit;
                } else {
                        soc_gpio.gpafen0.u32 &= ~bit;
                }
                if (XWDS_SOC_EIF_TM_HIGH & eiflag) {
                        soc_gpio.gphen0.u32 |= bit;
                } else {
                        soc_gpio.gphen0.u32 &= ~bit;
                }
                if (XWDS_SOC_EIF_TM_LOW & eiflag) {
                        soc_gpio.gplen0.u32 |= bit;
                } else {
                        soc_gpio.gplen0.u32 &= ~bit;
                }
                xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        } else {
                xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
                if (XWDS_SOC_EIF_TM_RISING & eiflag) {
                        soc_gpio.gparen1.u32 |= bit;
                } else {
                        soc_gpio.gparen1.u32 &= ~bit;
                }
                if (XWDS_SOC_EIF_TM_FALLING & eiflag) {
                        soc_gpio.gpafen1.u32 |= bit;
                } else {
                        soc_gpio.gpafen1.u32 &= ~bit;
                }
                if (XWDS_SOC_EIF_TM_HIGH & eiflag) {
                        soc_gpio.gphen1.u32 |= bit;
                } else {
                        soc_gpio.gphen1.u32 &= ~bit;
                }
                if (XWDS_SOC_EIF_TM_LOW & eiflag) {
                        soc_gpio.gplen1.u32 |= bit;
                } else {
                        soc_gpio.gplen1.u32 &= ~bit;
                }
                xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        }
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_eirq_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwu32_t bit;
        xwreg_t cpuirq;

        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);

        drvdata = soc->dev.data;
        bit = rpi4bxwds_eirq_pin_bit(eiid);
        if (eiid < (xwid_t)32U) {
                xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
                soc_gpio.gparen0.u32 &= ~bit;
                soc_gpio.gpafen0.u32 &= ~bit;
                soc_gpio.gphen0.u32 &= ~bit;
                soc_gpio.gplen0.u32 &= ~bit;
                soc_gpio.gpeds0.u32 = bit;
                xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        } else {
                xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
                soc_gpio.gparen1.u32 &= ~bit;
                soc_gpio.gpafen1.u32 &= ~bit;
                soc_gpio.gphen1.u32 &= ~bit;
                soc_gpio.gplen1.u32 &= ~bit;
                soc_gpio.gpeds1.u32 = bit;
                xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        }
        return XWOK;
}
