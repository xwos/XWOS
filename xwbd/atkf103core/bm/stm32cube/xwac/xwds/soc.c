/**
 * @file
 * @brief STM32CUBE设备栈：SOC
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

#include <bm/stm32cube/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/soc/gpio.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/soc.h>
#include <bm/stm32cube/cubemx/Core/Inc/dma.h>
#include <bm/stm32cube/cubemx/Core/Inc/gpio.h>
#include <bm/stm32cube/cubemx/Core/Inc/fsmc.h>

struct stm32cube_soc_cfg {
        struct {
                GPIO_TypeDef * const register_map[11];
        } gpio;
};

static
xwer_t stm32cube_soc_drv_start(struct xwds_device * dev);

static
xwer_t stm32cube_soc_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_soc_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32cube_soc_drv_resume(struct xwds_device * dev);
#endif /* XWCDCFG_ds_PM */

static
xwer_t stm32cube_soc_drv_gpio_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t stm32cube_soc_drv_gpio_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t stm32cube_soc_drv_gpio_cfg(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  void * cfg);

static
xwer_t stm32cube_soc_drv_gpio_set(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask);

static
xwer_t stm32cube_soc_drv_gpio_reset(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask);

static
xwer_t stm32cube_soc_drv_gpio_toggle(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask);

static
xwer_t stm32cube_soc_drv_gpio_output(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask,
                                     xwsq_t out);

static
xwer_t stm32cube_soc_drv_gpio_input(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask,
                                    xwsq_t * in);

#if defined(STM32CUBECFG_SRAM) && (1 == STM32CUBECFG_SRAM)
static
xwer_t stm32cube_soc_drv_eram_tst(struct xwds_soc * soc, xwptr_t * erraddr);
#endif /* STM32CUBECFG_SRAM */

extern xwu8_t sram_mr_origin[];
extern xwu8_t sram_mr_size[];

const struct xwds_soc_driver stm32cube_soc_drv = {
        .base = {
                .name = "stm32cube.soc",
                .probe = NULL,
                .remove = NULL,
                .start = stm32cube_soc_drv_start,
                .stop = stm32cube_soc_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = stm32cube_soc_drv_suspend,
                .resume = stm32cube_soc_drv_resume,
#endif /* XWCDCFG_ds_PM */
        },

        .gpio_req = stm32cube_soc_drv_gpio_req,
        .gpio_rls = stm32cube_soc_drv_gpio_rls,
        .gpio_cfg = stm32cube_soc_drv_gpio_cfg,
        .gpio_set = stm32cube_soc_drv_gpio_set,
        .gpio_reset = stm32cube_soc_drv_gpio_reset,
        .gpio_toggle = stm32cube_soc_drv_gpio_toggle,
        .gpio_output = stm32cube_soc_drv_gpio_output,
        .gpio_input = stm32cube_soc_drv_gpio_input,

#if defined(STM32CUBECFG_SRAM) && (1 == STM32CUBECFG_SRAM)
        .eram_tst = stm32cube_soc_drv_eram_tst,
#endif /* STM32CUBECFG_SRAM */
};

struct stm32cube_soc_cfg stm32cube_soc_cfg = {
        .gpio = {
                .register_map = {
                        [XWDS_GPIO_PORT_A] = (GPIO_TypeDef *)GPIOA_BASE,
                        [XWDS_GPIO_PORT_B] = (GPIO_TypeDef *)GPIOB_BASE,
                        [XWDS_GPIO_PORT_C] = (GPIO_TypeDef *)GPIOC_BASE,
                        [XWDS_GPIO_PORT_D] = (GPIO_TypeDef *)GPIOD_BASE,
                        [XWDS_GPIO_PORT_E] = (GPIO_TypeDef *)GPIOE_BASE,
                        [XWDS_GPIO_PORT_F] = (GPIO_TypeDef *)GPIOF_BASE,
                        [XWDS_GPIO_PORT_G] = (GPIO_TypeDef *)GPIOG_BASE,
                },
        },
};

xwsq_a stm32cube_gpio_pin_state[] = {
        [XWDS_GPIO_PORT_A] = 0,
        [XWDS_GPIO_PORT_B] = 0,
        [XWDS_GPIO_PORT_C] = 0,
        [XWDS_GPIO_PORT_D] = 0,
        [XWDS_GPIO_PORT_E] = 0,
        [XWDS_GPIO_PORT_F] = 0,
        [XWDS_GPIO_PORT_G] = 0,
};

struct xwds_soc stm32cube_soc_cb = {
        /* attributes */
        .dev = {
                .name = "stm32cube.soc",
                .id = 0,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &stm32cube_soc_drv),
                .data = NULL,
        },
        .xwccfg = (void *)&stm32cube_soc_cfg,
        .gpio = {
                .cfg = NULL,
                .pins = stm32cube_gpio_pin_state,
                .port_num = xw_array_size(stm32cube_gpio_pin_state),
                .pin_num = 16,
        },
#if defined(STM32CUBECFG_SRAM) && (1 == STM32CUBECFG_SRAM)
        .eram = {
                .origin = (xwptr_t)sram_mr_origin,
                .size = (xwsz_t)sram_mr_size,
        },
#endif /* STM32CUBECFG_SRAM */
};

/******** ******** base driver ******** ********/
static
xwer_t stm32cube_soc_drv_start(struct xwds_device * dev)
{

        XWOS_UNUSED(dev);

        MX_GPIO_Init();
        MX_DMA_Init();
#if defined(STM32CUBECFG_SRAM) && (1 == STM32CUBECFG_SRAM)
        MX_FSMC_Init();
#endif /* STM32CUBECFG_SRAM */

        return XWOK;
}

static
xwer_t stm32cube_soc_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);

#if defined(STM32CUBECFG_SRAM) && (1 == STM32CUBECFG_SRAM)
        MX_FSMC_DeInit();
#endif /* STM32CUBECFG_SRAM */
        MX_DMA_DeInit();
        MX_GPIO_DeInit();

        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_soc_drv_suspend(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

static
xwer_t stm32cube_soc_drv_resume(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}
#endif /* XWCDCFG_ds_PM */

/******** ******** gpio operation driver ******** ********/
static
xwer_t stm32cube_soc_drv_gpio_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);
        return XWOK;
}

static
xwer_t stm32cube_soc_drv_gpio_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        XWOS_UNUSED(soc);
        XWOS_UNUSED(port);
        XWOS_UNUSED(pinmask);
        return XWOK;
}

static
xwer_t stm32cube_soc_drv_gpio_cfg(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  void * cfg)
{
        const struct stm32cube_soc_cfg * xwccfg;
        LL_GPIO_InitTypeDef * llcfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;
        ErrorStatus es;
        xwer_t rc;

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
xwer_t stm32cube_soc_drv_gpio_set(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
        const struct stm32cube_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;

        xwccfg = soc->xwccfg;
        pinmask = (pinmask << GPIO_PIN_MASK_POS) | GPIO_PIN_MASK_POS;
        gpio = xwccfg->gpio.register_map[port];
        xwos_cpuirq_save_lc(&cpuirq);
        LL_GPIO_SetOutputPin(gpio, pinmask);
        xwos_cpuirq_restore_lc(cpuirq);
        return XWOK;
}

static
xwer_t stm32cube_soc_drv_gpio_reset(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask)
{
        const struct stm32cube_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;

        xwccfg = soc->xwccfg;
        pinmask = (pinmask << GPIO_PIN_MASK_POS) | GPIO_PIN_MASK_POS;
        gpio = xwccfg->gpio.register_map[port];
        xwos_cpuirq_save_lc(&cpuirq);
        LL_GPIO_ResetOutputPin(gpio, pinmask);
        xwos_cpuirq_restore_lc(cpuirq);
        return XWOK;
}

static
xwer_t stm32cube_soc_drv_gpio_toggle(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask)
{
        const struct stm32cube_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwreg_t cpuirq;

        xwccfg = soc->xwccfg;
        pinmask = (pinmask << GPIO_PIN_MASK_POS) | GPIO_PIN_MASK_POS;
        gpio = xwccfg->gpio.register_map[port];
        xwos_cpuirq_save_lc(&cpuirq);
        LL_GPIO_TogglePin(gpio, pinmask);
        xwos_cpuirq_restore_lc(cpuirq);
        return XWOK;
}

static
xwer_t stm32cube_soc_drv_gpio_output(struct xwds_soc * soc,
                                     xwid_t port, xwsq_t pinmask,
                                     xwsq_t out)
{
        const struct stm32cube_soc_cfg * xwccfg;
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
xwer_t stm32cube_soc_drv_gpio_input(struct xwds_soc * soc,
                                    xwid_t port, xwsq_t pinmask,
                                    xwsq_t * in)
{
        const struct stm32cube_soc_cfg * xwccfg;
        GPIO_TypeDef * gpio;
        xwu32_t regval;

        xwccfg = soc->xwccfg;

        gpio = xwccfg->gpio.register_map[port];
        regval = LL_GPIO_ReadInputPort(gpio);
        *in = pinmask & (xwsq_t)regval;
        return XWOK;
}

#if defined(STM32CUBECFG_SRAM) && (1 == STM32CUBECFG_SRAM)
/******** ******** ERAM ******** ********/
static
xwer_t stm32cube_soc_drv_eram_tst(struct xwds_soc * soc, xwptr_t * erraddr)
{
        xwsq_t i;
        xwsz_t * origin;
        xwsz_t size;
        xwer_t rc;

        rc = XWOK;
        origin = (xwsz_t *)soc->eram.origin;
        size = soc->eram.size / sizeof(xwsz_t);

        for (i = 0; i < size; i++) {
                origin[i] = i;
        }
        for (i = 0; i < size; i++) {
                if (origin[i] != i) {
                        *erraddr = (xwptr_t)&origin[i];
                        rc = -EIO;
                        break;
                }
        }
        return rc;
}
#endif /* STM32CUBECFG_SRAM */
