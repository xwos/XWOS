/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWOS设备栈：SOC
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include "board/std.h"
#include <xwos/lib/xwbop.h>
#include <xwos/osal/irq.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>
#include "board/xwac/xwds/device.h"
#include "bm/stm32cube/Core/Inc/dma.h"
#include "bm/stm32cube/Core/Inc/gpio.h"
#include "bm/stm32cube/Core/Inc/rtc.h"

struct stm32xwds_soc_cfg {
        struct {
                GPIO_TypeDef * const register_map[11];
        } gpio;
        struct {
                xwu32_t exti_line_map[16];
        } eirq;
};

static
xwer_t stm32xwds_soc_drv_start(struct xwds_device * dev);

static
xwer_t stm32xwds_soc_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32xwds_soc_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32xwds_soc_drv_resume(struct xwds_device * dev);
#endif

static
xwer_t stm32xwds_soc_drv_gpio_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t stm32xwds_soc_drv_gpio_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t stm32xwds_soc_drv_gpio_cfg(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  void * cfg);

static
xwer_t stm32xwds_soc_drv_gpio_set(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t stm32xwds_soc_drv_gpio_reset(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask);

static
xwer_t stm32xwds_soc_drv_gpio_toggle(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask);

static
xwer_t stm32xwds_soc_drv_gpio_output(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask,
                                     xwsq_t out);

static
xwer_t stm32xwds_soc_drv_gpio_input(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask,
                                    xwsq_t * in);

static
xwer_t stm32xwds_soc_drv_eirq_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid, xwsq_t eiflag);

static
xwer_t stm32xwds_soc_drv_eirq_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid);

static
void stm32xwds_soc_axisram_init(void);

static
void stm32xwds_soc_sram4_init(void);

static
void stm32xwds_soc_cache_init(void);

const struct xwds_soc_driver stm32xwds_soc_drv = {
        .base = {
                .name = "stm32xwds.soc",
                .probe = NULL,
                .remove = NULL,
                .start = stm32xwds_soc_drv_start,
                .stop = stm32xwds_soc_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = stm32xwds_soc_drv_suspend,
                .resume = stm32xwds_soc_drv_resume,
#endif
        },

        .gpio_req = stm32xwds_soc_drv_gpio_req,
        .gpio_rls = stm32xwds_soc_drv_gpio_rls,
        .gpio_cfg = stm32xwds_soc_drv_gpio_cfg,
        .gpio_set = stm32xwds_soc_drv_gpio_set,
        .gpio_reset = stm32xwds_soc_drv_gpio_reset,
        .gpio_toggle = stm32xwds_soc_drv_gpio_toggle,
        .gpio_output = stm32xwds_soc_drv_gpio_output,
        .gpio_input = stm32xwds_soc_drv_gpio_input,

        .eirq_req = stm32xwds_soc_drv_eirq_req,
        .eirq_rls = stm32xwds_soc_drv_eirq_rls,
};

struct stm32xwds_soc_cfg stm32xwds_soc_cfg = {
        .gpio = {
                .register_map = {
                        [XWDS_GPIO_PORT_A] = (GPIO_TypeDef *)GPIOA_BASE,
                        [XWDS_GPIO_PORT_B] = (GPIO_TypeDef *)GPIOB_BASE,
                        [XWDS_GPIO_PORT_C] = (GPIO_TypeDef *)GPIOC_BASE,
                        [XWDS_GPIO_PORT_D] = (GPIO_TypeDef *)GPIOD_BASE,
                        [XWDS_GPIO_PORT_E] = (GPIO_TypeDef *)GPIOE_BASE,
                        [XWDS_GPIO_PORT_F] = (GPIO_TypeDef *)GPIOF_BASE,
                        [XWDS_GPIO_PORT_G] = (GPIO_TypeDef *)GPIOG_BASE,
                        [XWDS_GPIO_PORT_H] = (GPIO_TypeDef *)GPIOH_BASE,
                        [XWDS_GPIO_PORT_I] = (GPIO_TypeDef *)GPIOI_BASE,
                        [XWDS_GPIO_PORT_J] = (GPIO_TypeDef *)GPIOJ_BASE,
                        [XWDS_GPIO_PORT_K] = (GPIO_TypeDef *)GPIOK_BASE,
                },
        },
        .eirq = {
                .exti_line_map = {
                        [XWDS_EIRQ(0)] = LL_SYSCFG_EXTI_LINE0,
                        [XWDS_EIRQ(1)] = LL_SYSCFG_EXTI_LINE1,
                        [XWDS_EIRQ(2)] = LL_SYSCFG_EXTI_LINE2,
                        [XWDS_EIRQ(3)] = LL_SYSCFG_EXTI_LINE3,
                        [XWDS_EIRQ(4)] = LL_SYSCFG_EXTI_LINE4,
                        [XWDS_EIRQ(5)] = LL_SYSCFG_EXTI_LINE5,
                        [XWDS_EIRQ(6)] = LL_SYSCFG_EXTI_LINE6,
                        [XWDS_EIRQ(7)] = LL_SYSCFG_EXTI_LINE7,
                        [XWDS_EIRQ(8)] = LL_SYSCFG_EXTI_LINE8,
                        [XWDS_EIRQ(9)] = LL_SYSCFG_EXTI_LINE9,
                        [XWDS_EIRQ(10)] = LL_SYSCFG_EXTI_LINE10,
                        [XWDS_EIRQ(11)] = LL_SYSCFG_EXTI_LINE11,
                        [XWDS_EIRQ(12)] = LL_SYSCFG_EXTI_LINE12,
                        [XWDS_EIRQ(13)] = LL_SYSCFG_EXTI_LINE13,
                        [XWDS_EIRQ(14)] = LL_SYSCFG_EXTI_LINE14,
                        [XWDS_EIRQ(15)] = LL_SYSCFG_EXTI_LINE15,
                },
        },
};

atomic_xwsq_t stm32xwds_gpio_pin_state[] = {
        [XWDS_GPIO_PORT_A] = 0,
        [XWDS_GPIO_PORT_B] = 0,
        [XWDS_GPIO_PORT_C] = 0,
        [XWDS_GPIO_PORT_D] = 0,
        [XWDS_GPIO_PORT_E] = 0,
        [XWDS_GPIO_PORT_F] = 0,
        [XWDS_GPIO_PORT_G] = 0,
        [XWDS_GPIO_PORT_H] = 0,
        [XWDS_GPIO_PORT_I] = 0,
        [XWDS_GPIO_PORT_J] = 0,
        [XWDS_GPIO_PORT_K] = 0,
};

__xwds_soc_eirq_tbl_qualifier xwds_eirq_f stm32xwds_eirq_isr[] = {
        [XWDS_EIRQ(0)] = NULL,
        [XWDS_EIRQ(1)] = NULL,
        [XWDS_EIRQ(2)] = NULL,
        [XWDS_EIRQ(3)] = NULL,
        [XWDS_EIRQ(4)] = NULL,
        [XWDS_EIRQ(5)] = NULL,
        [XWDS_EIRQ(6)] = NULL,
        [XWDS_EIRQ(7)] = NULL,
        [XWDS_EIRQ(8)] = NULL,
        [XWDS_EIRQ(9)] = NULL,
        [XWDS_EIRQ(10)] = NULL,
        [XWDS_EIRQ(11)] = NULL,
        [XWDS_EIRQ(12)] = NULL,
        [XWDS_EIRQ(13)] = NULL,
        [XWDS_EIRQ(14)] = NULL,
        [XWDS_EIRQ(15)] = NULL,
};

__xwds_soc_eirq_tbl_qualifier xwds_eirq_arg_t stm32xwds_eirq_israrg[] = {
        [XWDS_EIRQ(0)] = NULL,
        [XWDS_EIRQ(1)] = NULL,
        [XWDS_EIRQ(2)] = NULL,
        [XWDS_EIRQ(3)] = NULL,
        [XWDS_EIRQ(4)] = NULL,
        [XWDS_EIRQ(5)] = NULL,
        [XWDS_EIRQ(6)] = NULL,
        [XWDS_EIRQ(7)] = NULL,
        [XWDS_EIRQ(8)] = NULL,
        [XWDS_EIRQ(9)] = NULL,
        [XWDS_EIRQ(10)] = NULL,
        [XWDS_EIRQ(11)] = NULL,
        [XWDS_EIRQ(12)] = NULL,
        [XWDS_EIRQ(13)] = NULL,
        [XWDS_EIRQ(14)] = NULL,
        [XWDS_EIRQ(15)] = NULL,
};

struct xwds_soc stm32xwds_soc = {
        /* attributes */
        .dev = {
                .name = "stm32xwds.soc",
                .id = 0,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &stm32xwds_soc_drv),
                .data = NULL,
        },
        .xwccfg = (void *)&stm32xwds_soc_cfg,
        .gpio = {
                .cfg = NULL,
                .pins = stm32xwds_gpio_pin_state,
                .port_num = xw_array_size(stm32xwds_gpio_pin_state),
                .pin_num = 16,
        },
        .eirq = {
                .isrs = stm32xwds_eirq_isr,
                .isrargs = stm32xwds_eirq_israrg,
                .num = xw_array_size(stm32xwds_eirq_isr),
        },
};

/******** ******** base driver ******** ********/
static
xwer_t stm32xwds_soc_drv_start(struct xwds_device * dev)
{

        XWOS_UNUSED(dev);

        MX_GPIO_Init();
        MX_DMA_Init();
        MX_RTC_Init();
        stm32xwds_soc_axisram_init();
        stm32xwds_soc_sram4_init();
        /* 若SDRAM、QSPI Flash等可映射到内存地址上的器件未初始化完成，
         * 开启Cache可能会因为Cache的预取操作导致宕机。
         * 开启Cache必须在上述器件初始化完成之后。*/
        stm32xwds_soc_cache_init();

        return XWOK;
}

static
xwer_t stm32xwds_soc_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);

        MX_DMA_DeInit();
        MX_GPIO_DeInit();

        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32xwds_soc_drv_suspend(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        MX_GPIO_Suspend();
        return XWOK;
}

static
xwer_t stm32xwds_soc_drv_resume(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        MX_GPIO_Resume();
        return XWOK;
}
#endif

extern xwsz_t axisram_mr_origin[];
extern xwsz_t axisram_mr_size[];

extern xwu8_t data4_lma_base[];
extern xwu8_t data4_vma_base[];
extern xwu8_t data4_vma_end[];

static
void stm32xwds_soc_axisram_init(void)
{
        xwsq_t i;
        xwsz_t * origin = (xwsz_t *)axisram_mr_origin;
        xwsz_t n = (xwsz_t)axisram_mr_size / sizeof(xwsz_t);

        for (i = 0; i < n; i++) {
                origin[i] = 0;
        }
}

static
void stm32xwds_soc_sram4_init(void)
{
        xwsz_t count, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = data4_lma_base;
        dst = data4_vma_base;
        if (dst != src) {
                count = (xwsz_t)data4_vma_end - (xwsz_t)data4_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }
}

static
void stm32xwds_soc_cache_init(void)
{
#if defined(BRDCFG_ICACHE) && (1 == BRDCFG_ICACHE)
        SCB_EnableICache();
#endif
#if defined(BRDCFG_DCACHE) && (1 == BRDCFG_DCACHE)
        SCB_EnableDCache();
        SCB_CleanInvalidateDCache();
#endif
}

/******** ******** gpio operation driver ******** ********/
static
xwer_t stm32xwds_soc_drv_gpio_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);
        return XWOK;
}

static
xwer_t stm32xwds_soc_drv_gpio_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);
        return XWOK;
}

static
xwer_t stm32xwds_soc_drv_gpio_cfg(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  void * cfg)
{
        const struct stm32xwds_soc_cfg * xwccfg;
        LL_GPIO_InitTypeDef * llcfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;
        ErrorStatus es;
        xwer_t rc;

        XWOS_UNUSED(pinmask);

        xwccfg = soc->xwccfg;
        llcfg = cfg;
        gpio = xwccfg->gpio.register_map[port];
        xwos_cpuirq_save_lc(&cpuirq);
        es = LL_GPIO_Init(gpio, llcfg);
        xwos_cpuirq_restore_lc(cpuirq);
        if (SUCCESS == es) {
                rc = XWOK;
        } else {
                rc = -EINVAL;
        }
        return rc;
}

static
xwer_t stm32xwds_soc_drv_gpio_set(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        const struct stm32xwds_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;

        xwccfg = soc->xwccfg;

        gpio = xwccfg->gpio.register_map[port];
        xwos_cpuirq_save_lc(&cpuirq);
        LL_GPIO_SetOutputPin(gpio, pinmask);
        xwos_cpuirq_restore_lc(cpuirq);
        return XWOK;
}

static
xwer_t stm32xwds_soc_drv_gpio_reset(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask)
{
        const struct stm32xwds_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;

        xwccfg = soc->xwccfg;

        gpio = xwccfg->gpio.register_map[port];
        xwos_cpuirq_save_lc(&cpuirq);
        LL_GPIO_ResetOutputPin(gpio, pinmask);
        xwos_cpuirq_restore_lc(cpuirq);
        return XWOK;
}

static
xwer_t stm32xwds_soc_drv_gpio_toggle(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask)
{
        const struct stm32xwds_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;

        xwccfg = soc->xwccfg;

        gpio = xwccfg->gpio.register_map[port];
        xwos_cpuirq_save_lc(&cpuirq);
        LL_GPIO_TogglePin(gpio, pinmask);
        xwos_cpuirq_restore_lc(cpuirq);
        return XWOK;
}

static
xwer_t stm32xwds_soc_drv_gpio_output(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask,
                                     xwsq_t out)
{
        const struct stm32xwds_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;
        xwu32_t regval;

        xwccfg = soc->xwccfg;

        gpio = xwccfg->gpio.register_map[port];
        out &= pinmask;
        xwos_cpuirq_save_lc(&cpuirq);
        regval = LL_GPIO_ReadOutputPort(gpio);
        regval &= (xwu32_t)(~(out ^ pinmask));
        regval |= (xwu32_t)(out & pinmask);
        LL_GPIO_WriteOutputPort(gpio, regval);
        xwos_cpuirq_restore_lc(cpuirq);
        return XWOK;
}

static
xwer_t stm32xwds_soc_drv_gpio_input(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask,
                                    xwsq_t * in)
{
        const struct stm32xwds_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwu32_t regval;

        xwccfg = soc->xwccfg;

        gpio = xwccfg->gpio.register_map[port];
        regval = LL_GPIO_ReadInputPort(gpio);
        *in = pinmask & (xwsq_t)regval;
        return XWOK;
}

/******** ******** EIRQ operation driver ******** ********/
static
xwer_t stm32xwds_soc_drv_eirq_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid, xwsq_t eiflag)
{
        const struct stm32xwds_soc_cfg * xwccfg;
        LL_EXTI_InitTypeDef eicfg;
        xwu32_t syscfg_exti_port;
        xwu32_t syscfg_exti_line;
        uint32_t cubelibrc;
        xwer_t rc;

        xwccfg = soc->xwccfg;

        syscfg_exti_port = port;
        if (XWBOP_BIT(eiid) == pinmask) {
                syscfg_exti_line = xwccfg->eirq.exti_line_map[eiid];
                LL_SYSCFG_SetEXTISource(syscfg_exti_port, syscfg_exti_line);
                eicfg.Line_0_31 = pinmask;
                eicfg.LineCommand = ENABLE;
                eicfg.Mode = LL_EXTI_MODE_IT;
                eicfg.Trigger = LL_EXTI_TRIGGER_NONE;
                if (XWDS_SOC_EIF_TM_RISING & eiflag) {
                        eicfg.Trigger |= LL_EXTI_TRIGGER_RISING;
                } else {
                        eicfg.Trigger &= ~(LL_EXTI_TRIGGER_RISING);
                }
                if (XWDS_SOC_EIF_TM_FALLING & eiflag) {
                        eicfg.Trigger |= LL_EXTI_TRIGGER_FALLING;
                } else {
                        eicfg.Trigger &= ~(LL_EXTI_TRIGGER_FALLING);
                }
                cubelibrc = LL_EXTI_Init(&eicfg);
                if (SUCCESS == cubelibrc) {
                        rc = XWOK;
                } else {
                        rc = -EINVAL;
                }
        } else {
                rc = -EINVAL;
        }
        return rc;
}

static
xwer_t stm32xwds_soc_drv_eirq_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid)
{
        LL_EXTI_InitTypeDef eicfg;

        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(eiid);

        eicfg.Line_0_31 = pinmask;
        eicfg.LineCommand = DISABLE;
        eicfg.Mode = LL_EXTI_MODE_IT;
        eicfg.Trigger = LL_EXTI_TRIGGER_NONE;
        LL_EXTI_Init(&eicfg);

        return XWOK;
}

void stm32xwds_soc_cb_eirq_isr(xwid_t eiid)
{
        struct xwds_soc * soc = &stm32xwds_soc;

        if (!is_err_or_null(soc->eirq.isrs[eiid])) {
                soc->eirq.isrs[eiid](soc, (xwid_t)eiid, soc->eirq.isrargs[eiid]);
        }
}
