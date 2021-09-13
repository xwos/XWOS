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
#include <xwos/osal/irq.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/soc/gpio.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/soc.h>
#include <bm/stm32cube/cubemx/Core/Inc/dma.h>
#include <bm/stm32cube/cubemx/Core/Inc/gpio.h>
#include <bm/stm32cube/cubemx/Core/Inc/quadspi.h>

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
#endif

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
#endif
        },

        .gpio_req = stm32cube_soc_drv_gpio_req,
        .gpio_rls = stm32cube_soc_drv_gpio_rls,
        .gpio_cfg = stm32cube_soc_drv_gpio_cfg,
        .gpio_set = stm32cube_soc_drv_gpio_set,
        .gpio_reset = stm32cube_soc_drv_gpio_reset,
        .gpio_toggle = stm32cube_soc_drv_gpio_toggle,
        .gpio_output = stm32cube_soc_drv_gpio_output,
        .gpio_input = stm32cube_soc_drv_gpio_input,
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
                        [XWDS_GPIO_PORT_H] = (GPIO_TypeDef *)GPIOH_BASE,
                        [XWDS_GPIO_PORT_I] = (GPIO_TypeDef *)GPIOI_BASE,
                        [XWDS_GPIO_PORT_J] = (GPIO_TypeDef *)GPIOJ_BASE,
                        [XWDS_GPIO_PORT_K] = (GPIO_TypeDef *)GPIOK_BASE,
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
        [XWDS_GPIO_PORT_H] = 0,
        [XWDS_GPIO_PORT_I] = 0,
        [XWDS_GPIO_PORT_J] = 0,
        [XWDS_GPIO_PORT_K] = 0,
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
};

/******** ******** base driver ******** ********/
static
xwer_t stm32cube_soc_drv_start(struct xwds_device * dev)
{

        XWOS_UNUSED(dev);

        MX_GPIO_Init();
        MX_DMA_Init();
        MX_W25Q_Init();

        return XWOK;
}

static
xwer_t stm32cube_soc_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);

        MX_W25Q_DeInit();
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
#endif

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
