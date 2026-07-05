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
#include <xwcd/ds/soc/dma.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h>
#include "board/xwac/xwds/device.h"
#include "board/xwac/xwds/soc.h"

#define RPI4B_DMA_CH_NUM  15U
#define RPI4B_DMA_CH_VPU  15U

struct rpi4bxwds_soc_driver_data {
        struct {
                struct xwos_splk lock;
        } gpio;
        struct {
                struct xwos_splk lock;
        } eirq;
        struct {
                xwu16_t channels;
                struct xwos_splk lock;
        } dma;
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

static
xwer_t rpi4bxwds_soc_drv_dma_req(struct xwds_soc * soc, xwid_t ch);

static
xwer_t rpi4bxwds_soc_drv_dma_rls(struct xwds_soc * soc, xwid_t ch);

static
xwer_t rpi4bxwds_soc_drv_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg);

static
xwer_t rpi4bxwds_soc_drv_dma_enable(struct xwds_soc * soc, xwid_t ch);

static
xwer_t rpi4bxwds_soc_drv_dma_disable(struct xwds_soc * soc, xwid_t ch);

static
xwer_t rpi4bxwds_soc_drv_dma_start(struct xwds_soc * soc, xwid_t ch);

static
xwer_t rpi4bxwds_soc_drv_dma_stop(struct xwds_soc * soc, xwid_t ch);

static void rpi4bxwds_soc_eirq_bank0_isr(void);
static void rpi4bxwds_soc_eirq_bank1_isr(void);
static void rpi4bxwds_soc_eirq_bank2_isr(void);

static void rpi4bxwds_soc_dma0_isr(void);
static void rpi4bxwds_soc_dma1_isr(void);
static void rpi4bxwds_soc_dma2_isr(void);
static void rpi4bxwds_soc_dma3_isr(void);
static void rpi4bxwds_soc_dma4_isr(void);
static void rpi4bxwds_soc_dma5_isr(void);
static void rpi4bxwds_soc_dma6_isr(void);
static void rpi4bxwds_soc_dma7_8_isr(void);
static void rpi4bxwds_soc_dma9_10_isr(void);
static void rpi4bxwds_soc_dma11_isr(void);
static void rpi4bxwds_soc_dma12_isr(void);
static void rpi4bxwds_soc_dma13_isr(void);
static void rpi4bxwds_soc_dma14_isr(void);

static
void rpi4bxwds_soc_dmach_isr(xwsq_t ch);

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

        .dma_req = rpi4bxwds_soc_drv_dma_req,
        .dma_rls = rpi4bxwds_soc_drv_dma_rls,
        .dma_cfg = rpi4bxwds_soc_drv_dma_cfg,
        .dma_enable = rpi4bxwds_soc_drv_dma_enable,
        .dma_disable = rpi4bxwds_soc_drv_dma_disable,
        .dma_start = rpi4bxwds_soc_drv_dma_start,
        .dma_stop = rpi4bxwds_soc_drv_dma_stop,
};

atomic_xwsq_t rpi4bxwds_gpio_pin_state[] = {
        [XWDS_GPIO_PORT_A] = 0,
};

struct rpi4bxwds_soc_driver_data rpi4bxwds_soc_drvdata = {
        .gpio = {
                .lock = XWOS_SPLK_INITIALIZER,
        },
        .eirq = {
                .lock = XWOS_SPLK_INITIALIZER,
        },
        .dma = {
                .lock = XWOS_SPLK_INITIALIZER,
                .channels = 0U,
        },
};

#define RPI4B_EIRQ_NUM  58U
static xwds_eirq_f rpi4bxwds_eirq_isrs[RPI4B_EIRQ_NUM];
static xwds_eirq_arg_t rpi4bxwds_eirq_isrargs[RPI4B_EIRQ_NUM];

static xwds_dma_f rpi4bxwds_dma_chcbs[RPI4B_DMA_CH_NUM];
static xwds_dma_cbarg_t rpi4bxwds_dma_chcbargs[RPI4B_DMA_CH_NUM];

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
        .dma = {
                .ch_num = RPI4B_DMA_CH_NUM,
                .ccfg = NULL,
                .chcbs = rpi4bxwds_dma_chcbs,
                .chcbargs = rpi4bxwds_dma_chcbargs,
        },
};

/******** ******** base driver ******** ********/
static
xwer_t rpi4bxwds_soc_drv_probe(struct xwds_device * dev)
{
        struct rpi4bxwds_soc_driver_data * drvdata;

        drvdata = dev->data;
        xwos_splk_init(&drvdata->gpio.lock);

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

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA0, rpi4bxwds_soc_dma0_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA0, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA0, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA0);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA0);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA1, rpi4bxwds_soc_dma1_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA1, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA1, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA1);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA1);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA2, rpi4bxwds_soc_dma2_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA2, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA2, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA2);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA2);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA3, rpi4bxwds_soc_dma3_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA3, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA3, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA3);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA3);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA4, rpi4bxwds_soc_dma4_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA4, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA4, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA4);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA4);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA5, rpi4bxwds_soc_dma5_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA5, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA5, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA5);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA5);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA6, rpi4bxwds_soc_dma6_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA6, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA6, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA6);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA6);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA7_8, rpi4bxwds_soc_dma7_8_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA7_8, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA7_8, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA7_8);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA7_8);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA9_10, rpi4bxwds_soc_dma9_10_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA9_10, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA9_10, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA9_10);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA9_10);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA11, rpi4bxwds_soc_dma11_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA11, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA11, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA11);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA11);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA12, rpi4bxwds_soc_dma12_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA12, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA12, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA12);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA12);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA13, rpi4bxwds_soc_dma13_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA13, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA13, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA13);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA13);

        armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA14, rpi4bxwds_soc_dma14_isr);
        armv8a_gic_irq_set_priority(SOC_VC_IRQ_DMA14, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_DMA14, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_DMA14);
        armv8a_gic_irq_enable(SOC_VC_IRQ_DMA14);

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
        xwos_splk_lock_cpuirqsv(&drvdata->gpio.lock, &cpuirq);
        pin = xwbop_ffs(xwsq_t, copy);
        while (pin >= 0L) {
                rpi4bxwds_gpfsel_set(pin, gpio_cfg->function);
                rpi4bxwds_pupd_set(pin, gpio_cfg->pud);
                copy &= ~XWBOP_BIT(pin);
                pin = xwbop_ffs(xwsq_t, copy);
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->gpio.lock, cpuirq);
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

        xwos_splk_lock_cpuirqsv(&drvdata->gpio.lock, &cpuirq);
        if (0U != mask_lo) {
                soc_gpio.gpset0.u32 = mask_lo;
        }
        if (0U != mask_hi) {
                soc_gpio.gpset1.u32 = mask_hi;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->gpio.lock, cpuirq);
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

        xwos_splk_lock_cpuirqsv(&drvdata->gpio.lock, &cpuirq);
        if (0U != mask_lo) {
                soc_gpio.gpclr0.u32 = mask_lo;
        }
        if (0U != mask_hi) {
                soc_gpio.gpclr1.u32 = mask_hi;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->gpio.lock, cpuirq);
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

        xwos_splk_lock_cpuirqsv(&drvdata->gpio.lock, &cpuirq);
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
        xwos_splk_unlock_cpuirqrs(&drvdata->gpio.lock, cpuirq);
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

        xwos_splk_lock_cpuirqsv(&drvdata->gpio.lock, &cpuirq);
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
        xwos_splk_unlock_cpuirqrs(&drvdata->gpio.lock, cpuirq);
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
                xwos_splk_lock_cpuirqsv(&drvdata->eirq.lock, &cpuirq);
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
                xwos_splk_unlock_cpuirqrs(&drvdata->eirq.lock, cpuirq);
        } else {
                xwos_splk_lock_cpuirqsv(&drvdata->eirq.lock, &cpuirq);
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
                xwos_splk_unlock_cpuirqrs(&drvdata->eirq.lock, cpuirq);
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
                xwos_splk_lock_cpuirqsv(&drvdata->eirq.lock, &cpuirq);
                soc_gpio.gparen0.u32 &= ~bit;
                soc_gpio.gpafen0.u32 &= ~bit;
                soc_gpio.gphen0.u32 &= ~bit;
                soc_gpio.gplen0.u32 &= ~bit;
                soc_gpio.gpeds0.u32 = bit;
                xwos_splk_unlock_cpuirqrs(&drvdata->eirq.lock, cpuirq);
        } else {
                xwos_splk_lock_cpuirqsv(&drvdata->eirq.lock, &cpuirq);
                soc_gpio.gparen1.u32 &= ~bit;
                soc_gpio.gpafen1.u32 &= ~bit;
                soc_gpio.gphen1.u32 &= ~bit;
                soc_gpio.gplen1.u32 &= ~bit;
                soc_gpio.gpeds1.u32 = bit;
                xwos_splk_unlock_cpuirqrs(&drvdata->eirq.lock, cpuirq);
        }
        return XWOK;
}

/******** ******** DMA operation driver ******** ********/
static
xwer_t rpi4bxwds_soc_drv_dma_req(struct xwds_soc * soc, xwid_t ch)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwer_t rc;
        xwu16_t chbit;

        drvdata = soc->dev.data;
        chbit = (xwu16_t)(1U << (xwu16_t)ch);
        xwos_splk_lock_cpuirqsv(&drvdata->dma.lock, &cpuirq);
        if (drvdata->dma.channels & chbit) {
                rc = -EBUSY;
                goto err_busy;
        }
        drvdata->dma.channels |= chbit;
        switch (ch) {
        case 0U:
                soc_dma0.cs.b.reset = 1U;
                break;
        case 1U:
                soc_dma1.cs.b.reset = 1U;
                break;
        case 2U:
                soc_dma2.cs.b.reset = 1U;
                break;
        case 3U:
                soc_dma3.cs.b.reset = 1U;
                break;
        case 4U:
                soc_dma4.cs.b.reset = 1U;
                break;
        case 5U:
                soc_dma5.cs.b.reset = 1U;
                break;
        case 6U:
                soc_dma6.cs.b.reset = 1U;
                break;
        case 7U:
                soc_dma7.cs.b.reset = 1U;
                break;
        case 8U:
                soc_dma8.cs.b.reset = 1U;
                break;
        case 9U:
                soc_dma9.cs.b.reset = 1U;
                break;
        case 10U:
                soc_dma10.cs.b.reset = 1U;
                break;
        case 11U:
                soc_dma11.debug.b.reset = 1U;
                break;
        case 12U:
                soc_dma12.debug.b.reset = 1U;
                break;
        case 13U:
                soc_dma13.debug.b.reset = 1U;
                break;
        case 14U:
                soc_dma14.debug.b.reset = 1U;
                break;
        default:
                break;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
        return XWOK;

err_busy:
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
        return rc;
}

static
xwer_t rpi4bxwds_soc_drv_dma_rls(struct xwds_soc * soc, xwid_t ch)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwer_t rc;
        xwu16_t chbit;

        drvdata = soc->dev.data;
        chbit = (xwu16_t)(1U << (xwu16_t)ch);
        xwos_splk_lock_cpuirqsv(&drvdata->dma.lock, &cpuirq);
        if (!(drvdata->dma.channels & chbit)) {
                rc = -EPERM;
                goto err_perm;
        }
        switch (ch) {
        case 0U:
                soc_dma0.cb.u32 = 0U;
                break;
        case 1U:
                soc_dma1.cb.u32 = 0U;
                break;
        case 2U:
                soc_dma2.cb.u32 = 0U;
                break;
        case 3U:
                soc_dma3.cb.u32 = 0U;
                break;
        case 4U:
                soc_dma4.cb.u32 = 0U;
                break;
        case 5U:
                soc_dma5.cb.u32 = 0U;
                break;
        case 6U:
                soc_dma6.cb.u32 = 0U;
                break;
        case 7U:
                soc_dma7.cb.u32 = 0U;
                break;
        case 8U:
                soc_dma8.cb.u32 = 0U;
                break;
        case 9U:
                soc_dma9.cb.u32 = 0U;
                break;
        case 10U:
                soc_dma10.cb.u32 = 0U;
                break;
        case 11U:
                soc_dma11.cb.u32 = 0U;
                break;
        case 12U:
                soc_dma12.cb.u32 = 0U;
                break;
        case 13U:
                soc_dma13.cb.u32 = 0U;
                break;
        case 14U:
                soc_dma14.cb.u32 = 0U;
                break;
        default:
                break;
        }
        drvdata->dma.channels &= ~chbit;
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
        return XWOK;

err_perm:
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
        return rc;
}

static
xwer_t rpi4bxwds_soc_drv_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg)
{
        const struct rpi4bxwds_dma_cfg * dma_cfg;
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwptr_t addr;
        xwer_t rc;
        xwu16_t chbit;

        drvdata = soc->dev.data;
        dma_cfg = (const struct rpi4bxwds_dma_cfg *)cfg;
        chbit = (xwu16_t)(1U << (xwu16_t)ch);
        if (NULL == cfg) {
                rc = -EFAULT;
                goto err_cfg;
        }
        addr = (xwptr_t)&dma_cfg->cb.dma;
        xwos_splk_lock_cpuirqsv(&drvdata->dma.lock, &cpuirq);
        if (!(drvdata->dma.channels & chbit)) {
                rc = -EPERM;
                goto err_perm;
        }
        switch (ch) {
        case 0U:
                soc_dma0.cb.u32 = (xwu32_t)addr;
                soc_dma0.cs.b.priority = dma_cfg->cs_priority;
                soc_dma0.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma0.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma0.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 1U:
                soc_dma1.cb.u32 = (xwu32_t)addr;
                soc_dma1.cs.b.priority = dma_cfg->cs_priority;
                soc_dma1.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma1.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma1.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 2U:
                soc_dma2.cb.u32 = (xwu32_t)addr;
                soc_dma2.cs.b.priority = dma_cfg->cs_priority;
                soc_dma2.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma2.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma2.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 3U:
                soc_dma3.cb.u32 = (xwu32_t)addr;
                soc_dma3.cs.b.priority = dma_cfg->cs_priority;
                soc_dma3.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma3.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma3.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 4U:
                soc_dma4.cb.u32 = (xwu32_t)addr;
                soc_dma4.cs.b.priority = dma_cfg->cs_priority;
                soc_dma4.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma4.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma4.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 5U:
                soc_dma5.cb.u32 = (xwu32_t)addr;
                soc_dma5.cs.b.priority = dma_cfg->cs_priority;
                soc_dma5.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma5.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma5.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 6U:
                soc_dma6.cb.u32 = (xwu32_t)addr;
                soc_dma6.cs.b.priority = dma_cfg->cs_priority;
                soc_dma6.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma6.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma6.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 7U:
                soc_dma7.cb.u32 = (xwu32_t)addr;
                soc_dma7.cs.b.priority = dma_cfg->cs_priority;
                soc_dma7.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma7.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma7.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 8U:
                soc_dma8.cb.u32 = (xwu32_t)addr;
                soc_dma8.cs.b.priority = dma_cfg->cs_priority;
                soc_dma8.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma8.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma8.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 9U:
                soc_dma9.cb.u32 = (xwu32_t)addr;
                soc_dma9.cs.b.priority = dma_cfg->cs_priority;
                soc_dma9.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma9.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma9.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 10U:
                soc_dma10.cb.u32 = (xwu32_t)addr;
                soc_dma10.cs.b.priority = dma_cfg->cs_priority;
                soc_dma10.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma10.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma10.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 11U:
                soc_dma11.cb.u32 = (xwu32_t)(addr >> 5U);
                soc_dma11.cs.b.priority = dma_cfg->cs_priority;
                soc_dma11.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma11.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma11.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 12U:
                soc_dma12.cb.u32 = (xwu32_t)(addr >> 5U);
                soc_dma12.cs.b.priority = dma_cfg->cs_priority;
                soc_dma12.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma12.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma12.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 13U:
                soc_dma13.cb.u32 = (xwu32_t)(addr >> 5U);
                soc_dma13.cs.b.priority = dma_cfg->cs_priority;
                soc_dma13.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma13.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma13.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        case 14U:
                soc_dma14.cb.u32 = (xwu32_t)(addr >> 5U);
                soc_dma14.cs.b.priority = dma_cfg->cs_priority;
                soc_dma14.cs.b.panic_priority = dma_cfg->cs_panic_priority;
                soc_dma14.cs.b.disdebug = dma_cfg->cs_disdebug;
                soc_dma14.cs.b.wait_for_outstanding_writes =
                        dma_cfg->cs_wait_for_outstanding_writes;
                break;
        default:
                break;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
        return XWOK;

err_perm:
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
err_cfg:
        return rc;
}

static
xwer_t rpi4bxwds_soc_drv_dma_enable(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(ch);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_dma_disable(struct xwds_soc * soc, xwid_t ch)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(ch);
        return XWOK;
}

static
xwer_t rpi4bxwds_soc_drv_dma_start(struct xwds_soc * soc, xwid_t ch)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwer_t rc;
        xwu16_t chbit;

        drvdata = soc->dev.data;
        chbit = (xwu16_t)(1U << (xwu16_t)ch);
        xwos_splk_lock_cpuirqsv(&drvdata->dma.lock, &cpuirq);
        if (!(drvdata->dma.channels & chbit)) {
                rc = -EPERM;
                goto err_perm;
        }
        switch (ch) {
        case 0U:
                soc_dma0.cs.b.active = 1U;
                break;
        case 1U:
                soc_dma1.cs.b.active = 1U;
                break;
        case 2U:
                soc_dma2.cs.b.active = 1U;
                break;
        case 3U:
                soc_dma3.cs.b.active = 1U;
                break;
        case 4U:
                soc_dma4.cs.b.active = 1U;
                break;
        case 5U:
                soc_dma5.cs.b.active = 1U;
                break;
        case 6U:
                soc_dma6.cs.b.active = 1U;
                break;
        case 7U:
                soc_dma7.cs.b.active = 1U;
                break;
        case 8U:
                soc_dma8.cs.b.active = 1U;
                break;
        case 9U:
                soc_dma9.cs.b.active = 1U;
                break;
        case 10U:
                soc_dma10.cs.b.active = 1U;
                break;
        case 11U:
                soc_dma11.cs.b.active = 1U;
                break;
        case 12U:
                soc_dma12.cs.b.active = 1U;
                break;
        case 13U:
                soc_dma13.cs.b.active = 1U;
                break;
        case 14U:
                soc_dma14.cs.b.active = 1U;
                break;
        default:
                break;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
        return XWOK;

err_perm:
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
        return rc;
}

static
xwer_t rpi4bxwds_soc_drv_dma_stop(struct xwds_soc * soc, xwid_t ch)
{
        struct rpi4bxwds_soc_driver_data * drvdata;
        xwreg_t cpuirq;

        drvdata = soc->dev.data;
        xwos_splk_lock_cpuirqsv(&drvdata->dma.lock, &cpuirq);
        switch (ch) {
        case 0U:
                soc_dma0.cs.b.active = 0U;
                break;
        case 1U:
                soc_dma1.cs.b.active = 0U;
                break;
        case 2U:
                soc_dma2.cs.b.active = 0U;
                break;
        case 3U:
                soc_dma3.cs.b.active = 0U;
                break;
        case 4U:
                soc_dma4.cs.b.active = 0U;
                break;
        case 5U:
                soc_dma5.cs.b.active = 0U;
                break;
        case 6U:
                soc_dma6.cs.b.active = 0U;
                break;
        case 7U:
                soc_dma7.cs.b.active = 0U;
                break;
        case 8U:
                soc_dma8.cs.b.active = 0U;
                break;
        case 9U:
                soc_dma9.cs.b.active = 0U;
                break;
        case 10U:
                soc_dma10.cs.b.active = 0U;
                break;
        case 11U:
                soc_dma11.cs.b.active = 0U;
                break;
        case 12U:
                soc_dma12.cs.b.active = 0U;
                break;
        case 13U:
                soc_dma13.cs.b.active = 0U;
                break;
        case 14U:
                soc_dma14.cs.b.active = 0U;
                break;
        default:
                break;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->dma.lock, cpuirq);
        return XWOK;
}

/******** ******** DMA ISR ******** ********/
static
void rpi4bxwds_soc_dmach_isr(xwsq_t ch)
{
        struct xwds_soc * soc;
        xwu8_t triggered;

        soc = &rpi4bxwds_soc;
        triggered = 0U;
        switch (ch) {
        case 0U:
                if (soc_dma0.cs.b.intr) {
                        soc_dma0.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 1U:
                if (soc_dma1.cs.b.intr) {
                        soc_dma1.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 2U:
                if (soc_dma2.cs.b.intr) {
                        soc_dma2.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 3U:
                if (soc_dma3.cs.b.intr) {
                        soc_dma3.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 4U:
                if (soc_dma4.cs.b.intr) {
                        soc_dma4.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 5U:
                if (soc_dma5.cs.b.intr) {
                        soc_dma5.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 6U:
                if (soc_dma6.cs.b.intr) {
                        soc_dma6.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 7U:
                if (soc_dma7.cs.b.intr) {
                        soc_dma7.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 8U:
                if (soc_dma8.cs.b.intr) {
                        soc_dma8.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 9U:
                if (soc_dma9.cs.b.intr) {
                        soc_dma9.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 10U:
                if (soc_dma10.cs.b.intr) {
                        soc_dma10.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 11U:
                if (soc_dma11.cs.b.intr) {
                        soc_dma11.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 12U:
                if (soc_dma12.cs.b.intr) {
                        soc_dma12.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 13U:
                if (soc_dma13.cs.b.intr) {
                        soc_dma13.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        case 14U:
                if (soc_dma14.cs.b.intr) {
                        soc_dma14.cs.b.intr = 1U;
                        triggered = 1U;
                }
                break;
        default:
                break;
        }
        if ((triggered) && (soc->dma.chcbs[ch])) {
                soc->dma.chcbs[ch](soc, (xwid_t)ch, 0U, soc->dma.chcbargs[ch]);
        }
}

static
void rpi4bxwds_soc_dma0_isr(void)
{
        rpi4bxwds_soc_dmach_isr(0U);
}

static
void rpi4bxwds_soc_dma1_isr(void)
{
        rpi4bxwds_soc_dmach_isr(1U);
}

static
void rpi4bxwds_soc_dma2_isr(void)
{
        rpi4bxwds_soc_dmach_isr(2U);
}

static
void rpi4bxwds_soc_dma3_isr(void)
{
        rpi4bxwds_soc_dmach_isr(3U);
}

static
void rpi4bxwds_soc_dma4_isr(void)
{
        rpi4bxwds_soc_dmach_isr(4U);
}

static
void rpi4bxwds_soc_dma5_isr(void)
{
        rpi4bxwds_soc_dmach_isr(5U);
}

static
void rpi4bxwds_soc_dma6_isr(void)
{
        rpi4bxwds_soc_dmach_isr(6U);
}

static
void rpi4bxwds_soc_dma7_8_isr(void)
{
        rpi4bxwds_soc_dmach_isr(7U);
        rpi4bxwds_soc_dmach_isr(8U);
}

static
void rpi4bxwds_soc_dma9_10_isr(void)
{
        rpi4bxwds_soc_dmach_isr(9U);
        rpi4bxwds_soc_dmach_isr(10U);
}

static
void rpi4bxwds_soc_dma11_isr(void)
{
        rpi4bxwds_soc_dmach_isr(11U);
}

static
void rpi4bxwds_soc_dma12_isr(void)
{
        rpi4bxwds_soc_dmach_isr(12U);
}

static
void rpi4bxwds_soc_dma13_isr(void)
{
        rpi4bxwds_soc_dmach_isr(13U);
}

static
void rpi4bxwds_soc_dma14_isr(void)
{
        rpi4bxwds_soc_dmach_isr(14U);
}
