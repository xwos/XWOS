/**
  ******************************************************************************
  * File Name          : SYS.c
  * Description        : This file provides code for the configuration
  *                      of the SYS instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* USER CODE BEGIN 0 */
#include "dma.h"
#include "gpio.h"
#include "fmc.h"

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/irq.h>
#include <xwmd/ds/soc/chip.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/eirq.h>
#include <xwmd/ds/soc/dma.h>
#include <bm/stm32cube/xwds/stm32cube.h>

static
xwer_t stm32cube_soc_drv_start(struct xwds_device * dev);

static
xwer_t stm32cube_soc_drv_stop(struct xwds_device * dev);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static
xwer_t stm32cube_soc_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32cube_soc_drv_resume(struct xwds_device * dev);
#endif /* XWMDCFG_ds_PM */

#if defined(XWMDCFG_ds_SOC_GPIO) && (1 == XWMDCFG_ds_SOC_GPIO)
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
#endif /* XWMDCFG_ds_SOC_GPIO */

#if defined(XWMDCFG_ds_SOC_EIRQ) && (1 == XWMDCFG_ds_SOC_EIRQ)
static
xwer_t stm32cube_soc_drv_eirq_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid, xwsq_t eiflag);

static
xwer_t stm32cube_soc_drv_eirq_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid);
#endif /* XWMDCFG_ds_SOC_EIRQ */

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
static
xwer_t stm32cube_soc_drv_dma_req(struct xwds_soc * soc, xwid_t ch);

static
xwer_t stm32cube_soc_drv_dma_rls(struct xwds_soc * soc, xwid_t ch);

static
xwer_t stm32cube_soc_drv_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg);

static
xwer_t stm32cube_soc_drv_dma_enable(struct xwds_soc * soc, xwid_t ch);

static
xwer_t stm32cube_soc_drv_dma_disable(struct xwds_soc * soc, xwid_t ch);
#endif /* XWMDCFG_ds_SOC_DMA */

#if defined(XWMDCFG_ds_SOC_ERAM) && (1 == XWMDCFG_ds_SOC_ERAM)
static
xwer_t stm32cube_soc_drv_eram_tst(struct xwds_soc * soc, xwptr_t * erraddr);
#endif /* XWMDCFG_ds_SOC_ERAM */

const struct xwds_soc_driver stm32cube_soc_drv = {
        .base = {
                .name = "stm32cube.soc",
                .probe = NULL,
                .remove = NULL,
                .start = stm32cube_soc_drv_start,
                .stop = stm32cube_soc_drv_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
                .suspend = stm32cube_soc_drv_suspend,
                .resume = stm32cube_soc_drv_resume,
#endif /* XWMDCFG_ds_PM */
        },
#if defined(XWMDCFG_ds_SOC_CLK) && (1 == XWMDCFG_ds_SOC_CLK)
        .clk_req = stm32cube_soc_drv_clk_req,
        .clk_rls = stm32cube_soc_drv_clk_rls,
        .clk_getfqcy = stm32cube_soc_drv_clk_getfqcy,
#endif /* XWMDCFG_ds_SOC_CLK */

#if defined(XWMDCFG_ds_SOC_PWR) && (1 == XWMDCFG_ds_SOC_PWR)
        .pwr_req = NULL,
        .pwr_rls = NULL,
        .pwr_getvltg = NULL,
#endif /* XWMDCFG_ds_SOC_PWR */

#if defined(XWMDCFG_ds_SOC_GPIO) && (1 == XWMDCFG_ds_SOC_GPIO)
        .gpio_req = stm32cube_soc_drv_gpio_req,
        .gpio_rls = stm32cube_soc_drv_gpio_rls,
        .gpio_cfg = stm32cube_soc_drv_gpio_cfg,
        .gpio_set = stm32cube_soc_drv_gpio_set,
        .gpio_reset = stm32cube_soc_drv_gpio_reset,
        .gpio_toggle = stm32cube_soc_drv_gpio_toggle,
        .gpio_output = stm32cube_soc_drv_gpio_output,
        .gpio_input = stm32cube_soc_drv_gpio_input,
#endif /* XWMDCFG_ds_SOC_GPIO */

#if defined(XWMDCFG_ds_SOC_EIRQ) && (1 == XWMDCFG_ds_SOC_EIRQ)
        .eirq_req = stm32cube_soc_drv_eirq_req,
        .eirq_rls = stm32cube_soc_drv_eirq_rls,
#endif /* XWMDCFG_ds_SOC_EIRQ */

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
        .dma_req = stm32cube_soc_drv_dma_req,
        .dma_rls = stm32cube_soc_drv_dma_rls,
        .dma_cfg = stm32cube_soc_drv_dma_cfg,
        .dma_enable = stm32cube_soc_drv_dma_enable,
        .dma_disable = stm32cube_soc_drv_dma_disable,
        .dma_start = NULL,
        .dma_stop = NULL,
#endif /* XWMDCFG_ds_SOC_DMA */

#if defined(XWMDCFG_ds_SOC_ERAM) && (1 == XWMDCFG_ds_SOC_ERAM)
        .eram_tst = stm32cube_soc_drv_eram_tst,
#endif /* XWMDCFG_ds_SOC_ERAM */

        .ioctl = NULL,
};

struct stm32cube_soc_cfg {
  struct {
    GPIO_TypeDef * const register_map[11];
  } gpio;
  struct {
    xwu32_t exti_port_map[11];
    xwu32_t exti_line_map[16];
  } eirq;
  struct {
    DMA_TypeDef * ch2dma_map[16];
    xwu32_t ch2stream_map[16];
  } dma;
};

struct stm32cube_soc_cfg stm32cube_soc_cfg = {
  .gpio = {
    .register_map = {
      [SODS_GPIO_PORT_A] = (GPIO_TypeDef *)GPIOA_BASE,
      [SODS_GPIO_PORT_B] = (GPIO_TypeDef *)GPIOB_BASE,
      [SODS_GPIO_PORT_C] = (GPIO_TypeDef *)GPIOC_BASE,
      [SODS_GPIO_PORT_D] = (GPIO_TypeDef *)GPIOD_BASE,
      [SODS_GPIO_PORT_E] = (GPIO_TypeDef *)GPIOE_BASE,
      [SODS_GPIO_PORT_F] = (GPIO_TypeDef *)GPIOF_BASE,
      [SODS_GPIO_PORT_G] = (GPIO_TypeDef *)GPIOG_BASE,
      [SODS_GPIO_PORT_H] = (GPIO_TypeDef *)GPIOH_BASE,
      [SODS_GPIO_PORT_I] = (GPIO_TypeDef *)GPIOI_BASE,
      [SODS_GPIO_PORT_J] = (GPIO_TypeDef *)GPIOJ_BASE,
      [SODS_GPIO_PORT_K] = (GPIO_TypeDef *)GPIOK_BASE,
    },
  },
  .eirq = {
    .exti_port_map = {
      [SODS_GPIO_PORT_A] = LL_SYSCFG_EXTI_PORTA,
      [SODS_GPIO_PORT_B] = LL_SYSCFG_EXTI_PORTB,
      [SODS_GPIO_PORT_C] = LL_SYSCFG_EXTI_PORTC,
      [SODS_GPIO_PORT_D] = LL_SYSCFG_EXTI_PORTD,
      [SODS_GPIO_PORT_E] = LL_SYSCFG_EXTI_PORTE,
      [SODS_GPIO_PORT_F] = LL_SYSCFG_EXTI_PORTF,
      [SODS_GPIO_PORT_G] = LL_SYSCFG_EXTI_PORTG,
      [SODS_GPIO_PORT_H] = LL_SYSCFG_EXTI_PORTH,
      [SODS_GPIO_PORT_I] = LL_SYSCFG_EXTI_PORTI,
      [SODS_GPIO_PORT_J] = LL_SYSCFG_EXTI_PORTJ,
      [SODS_GPIO_PORT_K] = LL_SYSCFG_EXTI_PORTK,
    },
    .exti_line_map = {
      [SODS_EIRQ(0)] = LL_SYSCFG_EXTI_LINE0,
      [SODS_EIRQ(1)] = LL_SYSCFG_EXTI_LINE1,
      [SODS_EIRQ(2)] = LL_SYSCFG_EXTI_LINE2,
      [SODS_EIRQ(3)] = LL_SYSCFG_EXTI_LINE3,
      [SODS_EIRQ(4)] = LL_SYSCFG_EXTI_LINE4,
      [SODS_EIRQ(5)] = LL_SYSCFG_EXTI_LINE5,
      [SODS_EIRQ(6)] = LL_SYSCFG_EXTI_LINE6,
      [SODS_EIRQ(7)] = LL_SYSCFG_EXTI_LINE7,
      [SODS_EIRQ(8)] = LL_SYSCFG_EXTI_LINE8,
      [SODS_EIRQ(9)] = LL_SYSCFG_EXTI_LINE9,
      [SODS_EIRQ(10)] = LL_SYSCFG_EXTI_LINE10,
      [SODS_EIRQ(11)] = LL_SYSCFG_EXTI_LINE11,
      [SODS_EIRQ(12)] = LL_SYSCFG_EXTI_LINE12,
      [SODS_EIRQ(13)] = LL_SYSCFG_EXTI_LINE13,
      [SODS_EIRQ(14)] = LL_SYSCFG_EXTI_LINE14,
      [SODS_EIRQ(15)] = LL_SYSCFG_EXTI_LINE15,
    },
  },
  .dma = {
    .ch2dma_map = {
      [SODS_DMA_CH(0)] = DMA1,
      [SODS_DMA_CH(1)] = DMA1,
      [SODS_DMA_CH(2)] = DMA1,
      [SODS_DMA_CH(3)] = DMA1,
      [SODS_DMA_CH(4)] = DMA1,
      [SODS_DMA_CH(5)] = DMA1,
      [SODS_DMA_CH(6)] = DMA1,
      [SODS_DMA_CH(7)] = DMA1,
      [SODS_DMA_CH(8)] = DMA2,
      [SODS_DMA_CH(9)] = DMA2,
      [SODS_DMA_CH(10)] = DMA2,
      [SODS_DMA_CH(11)] = DMA2,
      [SODS_DMA_CH(12)] = DMA2,
      [SODS_DMA_CH(13)] = DMA2,
      [SODS_DMA_CH(14)] = DMA2,
      [SODS_DMA_CH(15)] = DMA2,
    },
    .ch2stream_map = {
      [SODS_DMA_CH(0)] = LL_DMA_STREAM_0,
      [SODS_DMA_CH(1)] = LL_DMA_STREAM_1,
      [SODS_DMA_CH(2)] = LL_DMA_STREAM_2,
      [SODS_DMA_CH(3)] = LL_DMA_STREAM_3,
      [SODS_DMA_CH(4)] = LL_DMA_STREAM_4,
      [SODS_DMA_CH(5)] = LL_DMA_STREAM_5,
      [SODS_DMA_CH(6)] = LL_DMA_STREAM_6,
      [SODS_DMA_CH(7)] = LL_DMA_STREAM_7,
      [SODS_DMA_CH(8)] = LL_DMA_STREAM_0,
      [SODS_DMA_CH(9)] = LL_DMA_STREAM_1,
      [SODS_DMA_CH(10)] = LL_DMA_STREAM_2,
      [SODS_DMA_CH(11)] = LL_DMA_STREAM_3,
      [SODS_DMA_CH(12)] = LL_DMA_STREAM_4,
      [SODS_DMA_CH(13)] = LL_DMA_STREAM_5,
      [SODS_DMA_CH(14)] = LL_DMA_STREAM_6,
      [SODS_DMA_CH(15)] = LL_DMA_STREAM_7,
    },
  },
};

__atomic xwsq_t stm32cube_gpio_pin_state[] = {
  [SODS_GPIO_PORT_A] = 0,
  [SODS_GPIO_PORT_B] = 0,
  [SODS_GPIO_PORT_C] = 0,
  [SODS_GPIO_PORT_D] = 0,
  [SODS_GPIO_PORT_E] = 0,
  [SODS_GPIO_PORT_F] = 0,
  [SODS_GPIO_PORT_G] = 0,
  [SODS_GPIO_PORT_H] = 0,
  [SODS_GPIO_PORT_I] = 0,
  [SODS_GPIO_PORT_J] = 0,
  [SODS_GPIO_PORT_K] = 0,
};

__xwds_soc_eirq_tbl_qualifier xwds_eirq_f stm32cube_eirq_isr[] = {
  [SODS_EIRQ(0)] = NULL,
  [SODS_EIRQ(1)] = NULL,
  [SODS_EIRQ(2)] = NULL,
  [SODS_EIRQ(3)] = NULL,
  [SODS_EIRQ(4)] = NULL,
  [SODS_EIRQ(5)] = NULL,
  [SODS_EIRQ(6)] = NULL,
  [SODS_EIRQ(7)] = NULL,
  [SODS_EIRQ(8)] = NULL,
  [SODS_EIRQ(9)] = NULL,
  [SODS_EIRQ(10)] = NULL,
  [SODS_EIRQ(11)] = NULL,
  [SODS_EIRQ(12)] = NULL,
  [SODS_EIRQ(13)] = NULL,
  [SODS_EIRQ(14)] = NULL,
  [SODS_EIRQ(15)] = NULL,
  [SODS_EIRQ(16)] = NULL,
  [SODS_EIRQ(17)] = NULL,
  [SODS_EIRQ(18)] = NULL,
  [SODS_EIRQ(19)] = NULL,
  [SODS_EIRQ(20)] = NULL,
  [SODS_EIRQ(21)] = NULL,
  [SODS_EIRQ(22)] = NULL,
};

__xwds_soc_eirq_tbl_qualifier xwds_eirq_arg_t stm32cube_eirq_israrg[] = {
  [SODS_EIRQ(0)] = NULL,
  [SODS_EIRQ(1)] = NULL,
  [SODS_EIRQ(2)] = NULL,
  [SODS_EIRQ(3)] = NULL,
  [SODS_EIRQ(4)] = NULL,
  [SODS_EIRQ(5)] = NULL,
  [SODS_EIRQ(6)] = NULL,
  [SODS_EIRQ(7)] = NULL,
  [SODS_EIRQ(8)] = NULL,
  [SODS_EIRQ(9)] = NULL,
  [SODS_EIRQ(10)] = NULL,
  [SODS_EIRQ(11)] = NULL,
  [SODS_EIRQ(12)] = NULL,
  [SODS_EIRQ(13)] = NULL,
  [SODS_EIRQ(14)] = NULL,
  [SODS_EIRQ(15)] = NULL,
  [SODS_EIRQ(16)] = NULL,
  [SODS_EIRQ(17)] = NULL,
  [SODS_EIRQ(18)] = NULL,
  [SODS_EIRQ(19)] = NULL,
  [SODS_EIRQ(20)] = NULL,
  [SODS_EIRQ(21)] = NULL,
  [SODS_EIRQ(22)] = NULL,
};

__atomic DECLARE_BITMAP(stm32cube_dma_channel_state_map, 16) = {0,};

__xwds_soc_dma_tbl_qualifier
xwds_dma_f stm32cube_dma_cbcb_array[16] = {
  [SODS_DMA_CH(0)] = NULL,
  [SODS_DMA_CH(1)] = NULL,
  [SODS_DMA_CH(2)] = NULL,
  [SODS_DMA_CH(3)] = NULL,
  [SODS_DMA_CH(4)] = NULL,
  [SODS_DMA_CH(5)] = NULL,
  [SODS_DMA_CH(6)] = NULL,
  [SODS_DMA_CH(7)] = NULL,
  [SODS_DMA_CH(8)] = NULL,
  [SODS_DMA_CH(9)] = NULL,
  [SODS_DMA_CH(10)] = NULL,
  [SODS_DMA_CH(11)] = NULL,
  [SODS_DMA_CH(12)] = NULL,
  [SODS_DMA_CH(13)] = NULL,
  [SODS_DMA_CH(14)] = NULL,
  [SODS_DMA_CH(15)] = NULL,
};

__xwds_soc_dma_tbl_qualifier
xwds_dma_cbarg_t stm32cube_dma_chcbarg_array[16] = {
  [SODS_DMA_CH(0)] = NULL,
  [SODS_DMA_CH(1)] = NULL,
  [SODS_DMA_CH(2)] = NULL,
  [SODS_DMA_CH(3)] = NULL,
  [SODS_DMA_CH(4)] = NULL,
  [SODS_DMA_CH(5)] = NULL,
  [SODS_DMA_CH(6)] = NULL,
  [SODS_DMA_CH(7)] = NULL,
  [SODS_DMA_CH(8)] = NULL,
  [SODS_DMA_CH(9)] = NULL,
  [SODS_DMA_CH(10)] = NULL,
  [SODS_DMA_CH(11)] = NULL,
  [SODS_DMA_CH(12)] = NULL,
  [SODS_DMA_CH(13)] = NULL,
  [SODS_DMA_CH(14)] = NULL,
  [SODS_DMA_CH(15)] = NULL,
};

struct xwds_soc stm32cube_soc_cb = {
  /* attributes */
  .dev = {
    .name = "stm32cube.soc",
    .id = 0,
    .resources = NULL,
    .drv = xwds_static_cast(struct xwds_driver *, &stm32cube_soc_drv),
    .data = NULL,
  },
  .xwccfg = (void *)&stm32cube_soc_cfg,
  .gpio = {
    .cfg = NULL,
    .pins = stm32cube_gpio_pin_state,
    .port_num = xw_array_size(stm32cube_gpio_pin_state),
    .pin_num = 16,
  },
  .eirq = {
    .isrs = stm32cube_eirq_isr,
    .isrargs = stm32cube_eirq_israrg,
    .num = xw_array_size(stm32cube_eirq_isr),
  },
  .dma = {
    .ccfg = NULL,
    .chstatus = stm32cube_dma_channel_state_map,
    .ch_num = 16,
    .chcbs = stm32cube_dma_cbcb_array,
    .chcbargs = stm32cube_dma_chcbarg_array,
  },
};
/* USER CODE END 0 */

/* SYS init function */
void MX_SYS_Init(void)
{

}

/* USER CODE BEGIN 1 */

/******** ******** common driver ******** ********/
static
xwer_t stm32cube_soc_drv_start(struct xwds_device * dev)
{

  XWOS_UNUSED(dev);

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SDRAM_Init();

  return OK;
}

static
xwer_t stm32cube_soc_drv_stop(struct xwds_device * dev)
{
  XWOS_UNUSED(dev);

  MX_SDRAM_Deinit();
  MX_DMA_Deinit();
  MX_GPIO_Deinit();

  return OK;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static
xwer_t stm32cube_soc_drv_suspend(struct xwds_device * dev)
{
  XWOS_UNUSED(dev);
  MX_SDRAM_Suspend();
  MX_DMA_Suspend();
  MX_GPIO_Suspend();
  return OK;
}

static
xwer_t stm32cube_soc_drv_resume(struct xwds_device * dev)
{
  XWOS_UNUSED(dev);
  MX_GPIO_Resume();
  MX_DMA_Resume();
  MX_SDRAM_Resume();
  return OK;
}
#endif /* XWMDCFG_ds_PM */

#if defined(XWMDCFG_ds_SOC_GPIO) && (1 == XWMDCFG_ds_SOC_GPIO)
/******** ******** gpio operation driver ******** ********/
static
xwer_t stm32cube_soc_drv_gpio_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
  XWOS_UNUSED(soc);
  XWOS_UNUSED(port);
  XWOS_UNUSED(pinmask);
  return OK;
}

static
xwer_t stm32cube_soc_drv_gpio_rls(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask)
{
  XWOS_UNUSED(soc);
  XWOS_UNUSED(port);
  XWOS_UNUSED(pinmask);
  return OK;
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
  soc_cpuirq_save_lc(&cpuirq);
  es = LL_GPIO_Init(gpio, llcfg);
  soc_cpuirq_restore_lc(cpuirq);
  if (SUCCESS == es) {
    rc = OK;
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
  soc_cpuirq_save_lc(&cpuirq);
  LL_GPIO_SetOutputPin(gpio, pinmask);
  soc_cpuirq_restore_lc(cpuirq);
  return OK;
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
  soc_cpuirq_save_lc(&cpuirq);
  LL_GPIO_ResetOutputPin(gpio, pinmask);
  soc_cpuirq_restore_lc(cpuirq);
  return OK;
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
  soc_cpuirq_save_lc(&cpuirq);
  LL_GPIO_TogglePin(gpio, pinmask);
  soc_cpuirq_restore_lc(cpuirq);
  return OK;
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
  soc_cpuirq_save_lc(&cpuirq);
  regval = LL_GPIO_ReadOutputPort(gpio);
  regval &= (xwu32_t)(~(out ^ pinmask));
  regval |= (xwu32_t)(out & pinmask);
  LL_GPIO_WriteOutputPort(gpio, regval);
  soc_cpuirq_restore_lc(cpuirq);
  return OK;
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
  return OK;
}
#endif /* XWMDCFG_ds_SOC_GPIO */

#if defined(XWMDCFG_ds_SOC_EIRQ) && (1 == XWMDCFG_ds_SOC_EIRQ)
/******** ******** EIRQ operation driver ******** ********/
static
xwer_t stm32cube_soc_drv_eirq_req(struct xwds_soc * soc,
                                  xwid_t port, xwsq_t pinmask,
                                  xwid_t eiid, xwsq_t eiflag)
{
  const struct stm32cube_soc_cfg * xwccfg;
  LL_EXTI_InitTypeDef eicfg;
  xwu32_t syscfg_exti_port;
  xwu32_t syscfg_exti_line;
  uint32_t cubelibrc;
  xwer_t rc;

  xwccfg = soc->xwccfg;

  syscfg_exti_port = xwccfg->eirq.exti_port_map[port];
  if (BIT(eiid) == pinmask) {
    syscfg_exti_line = xwccfg->eirq.exti_line_map[eiid];
    LL_SYSCFG_SetEXTISource(syscfg_exti_port, syscfg_exti_line);
    eicfg.Line_0_31 = pinmask;
    eicfg.LineCommand = ENABLE;
    eicfg.Mode = LL_EXTI_MODE_IT;
    eicfg.Trigger = LL_EXTI_TRIGGER_NONE;
    if (SODS_SOC_EIF_TM_RISING & eiflag) {
      eicfg.Trigger |= LL_EXTI_TRIGGER_RISING;
    } else {
      eicfg.Trigger &= ~(LL_EXTI_TRIGGER_RISING);
    }
    if (SODS_SOC_EIF_TM_FALLING & eiflag) {
      eicfg.Trigger |= LL_EXTI_TRIGGER_FALLING;
    } else {
      eicfg.Trigger &= ~(LL_EXTI_TRIGGER_FALLING);
    }
    cubelibrc = LL_EXTI_Init(&eicfg);
    if (SUCCESS == cubelibrc) {
      rc = OK;
    } else {
      rc = -EINVAL;
    }
  } else {
    rc = -EINVAL;
  }
  return rc;
}

static
xwer_t stm32cube_soc_drv_eirq_rls(struct xwds_soc * soc,
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

  return OK;
}

#endif /* XWMDCFG_ds_SOC_EIRQ */

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
/******** ******** DMA driver ******** ********/
static
xwer_t stm32cube_soc_drv_dma_req(struct xwds_soc * soc, xwid_t ch)
{
  return OK;
}

static
xwer_t stm32cube_soc_drv_dma_rls(struct xwds_soc * soc, xwid_t ch)
{
  return OK;
}

static
xwer_t stm32cube_soc_drv_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg)
{
  const struct stm32cube_soc_cfg * xwccfg;
  LL_DMA_InitTypeDef * cubecfg;
  DMA_TypeDef * dma;
  uint32_t stream;
  uint32_t cuberc;
  xwer_t rc;

  xwccfg = soc->xwccfg;
  dma = xwccfg->dma.ch2dma_map[ch];
  stream = xwccfg->dma.ch2stream_map[ch];
  cubecfg = cfg;
  if (LL_DMA_IsEnabledStream(dma, stream)) {
    rc = -EPERM;
  } else {
    if (cubecfg) {
      cuberc = LL_DMA_Init(dma, stream, cubecfg);
      if (SUCCESS == cuberc) {
        rc = OK;
      } else {
        rc = -EINVAL;
      }
    } else {
      rc = OK;
    }
  }
  return rc;
}

static
xwer_t stm32cube_soc_drv_dma_enable(struct xwds_soc * soc, xwid_t ch)
{
  const struct stm32cube_soc_cfg * xwccfg;
  DMA_TypeDef * dma;
  uint32_t stream;
  xwer_t rc;

  xwccfg = soc->xwccfg;
  dma = xwccfg->dma.ch2dma_map[ch];
  stream = xwccfg->dma.ch2stream_map[ch];
  if (LL_DMA_IsEnabledStream(dma, stream)) {
    rc = -EALREADY;
  } else {
    switch (stream) {
      case LL_DMA_STREAM_0:
        LL_DMA_ClearFlag_HT0(dma);
        LL_DMA_ClearFlag_TC0(dma);
        LL_DMA_ClearFlag_TE0(dma);
        LL_DMA_ClearFlag_DME0(dma);
        LL_DMA_ClearFlag_FE0(dma);
        break;
      case LL_DMA_STREAM_1:
        LL_DMA_ClearFlag_HT1(dma);
        LL_DMA_ClearFlag_TC1(dma);
        LL_DMA_ClearFlag_TE1(dma);
        LL_DMA_ClearFlag_DME1(dma);
        LL_DMA_ClearFlag_FE1(dma);
        break;
      case LL_DMA_STREAM_2:
        LL_DMA_ClearFlag_HT2(dma);
        LL_DMA_ClearFlag_TC2(dma);
        LL_DMA_ClearFlag_TE2(dma);
        LL_DMA_ClearFlag_DME2(dma);
        LL_DMA_ClearFlag_FE2(dma);
        break;
      case LL_DMA_STREAM_3:
        LL_DMA_ClearFlag_HT3(dma);
        LL_DMA_ClearFlag_TC3(dma);
        LL_DMA_ClearFlag_TE3(dma);
        LL_DMA_ClearFlag_DME3(dma);
        LL_DMA_ClearFlag_FE3(dma);
        break;
      case LL_DMA_STREAM_4:
        LL_DMA_ClearFlag_HT4(dma);
        LL_DMA_ClearFlag_TC4(dma);
        LL_DMA_ClearFlag_TE4(dma);
        LL_DMA_ClearFlag_DME4(dma);
        LL_DMA_ClearFlag_FE4(dma);
        break;
      case LL_DMA_STREAM_5:
        LL_DMA_ClearFlag_HT5(dma);
        LL_DMA_ClearFlag_TC5(dma);
        LL_DMA_ClearFlag_TE5(dma);
        LL_DMA_ClearFlag_DME5(dma);
        LL_DMA_ClearFlag_FE5(dma);
        break;
      case LL_DMA_STREAM_6:
        LL_DMA_ClearFlag_HT6(dma);
        LL_DMA_ClearFlag_TC6(dma);
        LL_DMA_ClearFlag_TE6(dma);
        LL_DMA_ClearFlag_DME6(dma);
        LL_DMA_ClearFlag_FE6(dma);
        break;
      case LL_DMA_STREAM_7:
        LL_DMA_ClearFlag_HT7(dma);
        LL_DMA_ClearFlag_TC7(dma);
        LL_DMA_ClearFlag_TE7(dma);
        LL_DMA_ClearFlag_DME7(dma);
        LL_DMA_ClearFlag_FE7(dma);
        break;
    }
    LL_DMA_EnableStream(dma, stream);
    rc = OK;
  }
  return rc;
}

static
xwer_t stm32cube_soc_drv_dma_disable(struct xwds_soc * soc, xwid_t ch)
{
  const struct stm32cube_soc_cfg * xwccfg;
  DMA_TypeDef * dma;
  uint32_t stream;
  xwer_t rc;

  xwccfg = soc->xwccfg;
  dma = xwccfg->dma.ch2dma_map[ch];
  stream = xwccfg->dma.ch2stream_map[ch];
  if (LL_DMA_IsEnabledStream(dma, stream)) {
    LL_DMA_DisableStream(dma, stream);
    rc = OK;
  } else {
    rc = -EALREADY;
  }

  switch (stream) {
    case LL_DMA_STREAM_0:
      LL_DMA_ClearFlag_HT0(dma);
      LL_DMA_ClearFlag_TC0(dma);
      LL_DMA_ClearFlag_TE0(dma);
      LL_DMA_ClearFlag_DME0(dma);
      LL_DMA_ClearFlag_FE0(dma);
      break;
    case LL_DMA_STREAM_1:
      LL_DMA_ClearFlag_HT1(dma);
      LL_DMA_ClearFlag_TC1(dma);
      LL_DMA_ClearFlag_TE1(dma);
      LL_DMA_ClearFlag_DME1(dma);
      LL_DMA_ClearFlag_FE1(dma);
      break;
    case LL_DMA_STREAM_2:
      LL_DMA_ClearFlag_HT2(dma);
      LL_DMA_ClearFlag_TC2(dma);
      LL_DMA_ClearFlag_TE2(dma);
      LL_DMA_ClearFlag_DME2(dma);
      LL_DMA_ClearFlag_FE2(dma);
      break;
    case LL_DMA_STREAM_3:
      LL_DMA_ClearFlag_HT3(dma);
      LL_DMA_ClearFlag_TC3(dma);
      LL_DMA_ClearFlag_TE3(dma);
      LL_DMA_ClearFlag_DME3(dma);
      LL_DMA_ClearFlag_FE3(dma);
      break;
    case LL_DMA_STREAM_4:
      LL_DMA_ClearFlag_HT4(dma);
      LL_DMA_ClearFlag_TC4(dma);
      LL_DMA_ClearFlag_TE4(dma);
      LL_DMA_ClearFlag_DME4(dma);
      LL_DMA_ClearFlag_FE4(dma);
      break;
    case LL_DMA_STREAM_5:
      LL_DMA_ClearFlag_HT5(dma);
      LL_DMA_ClearFlag_TC5(dma);
      LL_DMA_ClearFlag_TE5(dma);
      LL_DMA_ClearFlag_DME5(dma);
      LL_DMA_ClearFlag_FE5(dma);
      break;
    case LL_DMA_STREAM_6:
      LL_DMA_ClearFlag_HT6(dma);
      LL_DMA_ClearFlag_TC6(dma);
      LL_DMA_ClearFlag_TE6(dma);
      LL_DMA_ClearFlag_DME6(dma);
      LL_DMA_ClearFlag_FE6(dma);
      break;
    case LL_DMA_STREAM_7:
      LL_DMA_ClearFlag_HT7(dma);
      LL_DMA_ClearFlag_TC7(dma);
      LL_DMA_ClearFlag_TE7(dma);
      LL_DMA_ClearFlag_DME7(dma);
      LL_DMA_ClearFlag_FE7(dma);
      break;
  }
  return rc;
}

#endif /* XWMDCFG_ds_SOC_DMA */

#if defined(XWMDCFG_ds_SOC_ERAM) && (1 == XWMDCFG_ds_SOC_ERAM)
/******** ******** ERAM ******** ********/
static
xwer_t stm32cube_soc_drv_eram_tst(struct xwds_soc * soc, xwptr_t * erraddr)
{
  xwsq_t i;
  xwsz_t * origin;
  xwsz_t size;
  xwer_t rc;

  rc = OK;
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
#endif /* XWMDCFG_ds_SOC_ERAM */

/******** ******** ISR ******** ********/
void stm32cube_exti_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwsid_t eiid;
  xwu32_t pr;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  do {
    pr = LL_EXTI_ReadFlag_0_31(LL_EXTI_LINE_ALL);
    eiid = (xwsid_t)xwbop_fls(xwu32_t, pr);
    if (eiid >= 0) {
      if (!is_err_or_null(soc->eirq.isrs[eiid])) {
        soc->eirq.isrs[eiid](soc, (xwid_t)eiid,
                             soc->eirq.isrargs[eiid]);
      }/* else {} */
      LL_EXTI_ClearFlag_0_31(BIT(eiid));
    } else {
      break;
    }
  } while (true);
}

void stm32cube_dma1_stream0_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE0(DMA1)) {
    LL_DMA_ClearFlag_FE0(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME0(DMA1)) {
    LL_DMA_ClearFlag_DME0(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE0(DMA1)) {
    LL_DMA_ClearFlag_TE0(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT0(DMA1)) {
    LL_DMA_ClearFlag_HT0(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC0(DMA1)) {
    LL_DMA_ClearFlag_TC0(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[0])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[0](soc, SODS_DMA_CH(0), dmarc,
                        soc->dma.chcbargs[0]);
    } else {
      soc->dma.chcbs[0](soc, SODS_DMA_CH(0), dmarc, NULL);
    }
  }
}

void stm32cube_dma1_stream1_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE1(DMA1)) {
    LL_DMA_ClearFlag_FE1(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME1(DMA1)) {
    LL_DMA_ClearFlag_DME1(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE1(DMA1)) {
    LL_DMA_ClearFlag_TE1(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT1(DMA1)) {
    LL_DMA_ClearFlag_HT1(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC1(DMA1)) {
    LL_DMA_ClearFlag_TC1(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[1])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[1](soc, SODS_DMA_CH(1), dmarc,
                        soc->dma.chcbargs[1]);
    } else {
      soc->dma.chcbs[1](soc, SODS_DMA_CH(1), dmarc, NULL);
    }
  }
}

void stm32cube_dma1_stream2_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE2(DMA1)) {
    LL_DMA_ClearFlag_FE2(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME2(DMA1)) {
    LL_DMA_ClearFlag_DME2(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE2(DMA1)) {
    LL_DMA_ClearFlag_TE2(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT2(DMA1)) {
    LL_DMA_ClearFlag_HT2(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC2(DMA1)) {
    LL_DMA_ClearFlag_TC2(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[2])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[2](soc, SODS_DMA_CH(2), dmarc,
                        soc->dma.chcbargs[2]);
    } else {
      soc->dma.chcbs[2](soc, SODS_DMA_CH(2), dmarc, NULL);
    }
  }
}

void stm32cube_dma1_stream3_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE3(DMA1)) {
    LL_DMA_ClearFlag_FE3(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME3(DMA1)) {
    LL_DMA_ClearFlag_DME3(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE3(DMA1)) {
    LL_DMA_ClearFlag_TE3(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT3(DMA1)) {
    LL_DMA_ClearFlag_HT3(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC3(DMA1)) {
    LL_DMA_ClearFlag_TC3(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[3])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[3](soc, SODS_DMA_CH(3), dmarc,
                        soc->dma.chcbargs[3]);
    } else {
      soc->dma.chcbs[3](soc, SODS_DMA_CH(3), dmarc, NULL);
    }
  }
}

void stm32cube_dma1_stream4_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE4(DMA1)) {
    LL_DMA_ClearFlag_FE4(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME4(DMA1)) {
    LL_DMA_ClearFlag_DME4(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE4(DMA1)) {
    LL_DMA_ClearFlag_TE4(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT4(DMA1)) {
    LL_DMA_ClearFlag_HT4(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC4(DMA1)) {
    LL_DMA_ClearFlag_TC4(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[4])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[4](soc, SODS_DMA_CH(4), dmarc,
                        soc->dma.chcbargs[4]);
    } else {
      soc->dma.chcbs[4](soc, SODS_DMA_CH(4), dmarc, NULL);
    }
  }
}

void stm32cube_dma1_stream5_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE5(DMA1)) {
    LL_DMA_ClearFlag_FE5(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME5(DMA1)) {
    LL_DMA_ClearFlag_DME5(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE5(DMA1)) {
    LL_DMA_ClearFlag_TE5(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT5(DMA1)) {
    LL_DMA_ClearFlag_HT5(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC5(DMA1)) {
    LL_DMA_ClearFlag_TC5(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[5])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[5](soc, SODS_DMA_CH(5), dmarc,
                        soc->dma.chcbargs[5]);
    } else {
      soc->dma.chcbs[5](soc, SODS_DMA_CH(5), dmarc, NULL);
    }
  }
}

void stm32cube_dma1_stream6_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE6(DMA1)) {
    LL_DMA_ClearFlag_FE6(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME6(DMA1)) {
    LL_DMA_ClearFlag_DME6(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE6(DMA1)) {
    LL_DMA_ClearFlag_TE6(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT6(DMA1)) {
    LL_DMA_ClearFlag_HT6(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC6(DMA1)) {
    LL_DMA_ClearFlag_TC6(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[6])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[6](soc, SODS_DMA_CH(6), dmarc,
                        soc->dma.chcbargs[6]);
    } else {
      soc->dma.chcbs[6](soc, SODS_DMA_CH(6), dmarc, NULL);
    }
  }
}

void stm32cube_dma1_stream7_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE7(DMA1)) {
    LL_DMA_ClearFlag_FE7(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME7(DMA1)) {
    LL_DMA_ClearFlag_DME7(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE7(DMA1)) {
    LL_DMA_ClearFlag_TE7(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT7(DMA1)) {
    LL_DMA_ClearFlag_HT7(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC7(DMA1)) {
    LL_DMA_ClearFlag_TC7(DMA1);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[7])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[7](soc, SODS_DMA_CH(7), dmarc,
                        soc->dma.chcbargs[7]);
    } else {
      soc->dma.chcbs[7](soc, SODS_DMA_CH(7), dmarc, NULL);
    }
  }
}

void stm32cube_dma2_stream0_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE0(DMA2)) {
    LL_DMA_ClearFlag_FE0(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME0(DMA2)) {
    LL_DMA_ClearFlag_DME0(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE0(DMA2)) {
    LL_DMA_ClearFlag_TE0(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT0(DMA2)) {
    LL_DMA_ClearFlag_HT0(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC0(DMA2)) {
    LL_DMA_ClearFlag_TC0(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[8])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[0](soc, SODS_DMA_CH(8), dmarc,
                        soc->dma.chcbargs[8]);
    } else {
      soc->dma.chcbs[0](soc, SODS_DMA_CH(8), dmarc, NULL);
    }
  }
}

void stm32cube_dma2_stream1_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE1(DMA2)) {
    LL_DMA_ClearFlag_FE1(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME1(DMA2)) {
    LL_DMA_ClearFlag_DME1(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE1(DMA2)) {
    LL_DMA_ClearFlag_TE1(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT1(DMA2)) {
    LL_DMA_ClearFlag_HT1(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC1(DMA2)) {
    LL_DMA_ClearFlag_TC1(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[9])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[9](soc, SODS_DMA_CH(9), dmarc,
                        soc->dma.chcbargs[9]);
    } else {
      soc->dma.chcbs[9](soc, SODS_DMA_CH(9), dmarc, NULL);
    }
  }
}

void stm32cube_dma2_stream2_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE2(DMA2)) {
    LL_DMA_ClearFlag_FE2(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME2(DMA2)) {
    LL_DMA_ClearFlag_DME2(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE2(DMA2)) {
    LL_DMA_ClearFlag_TE2(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT2(DMA2)) {
    LL_DMA_ClearFlag_HT2(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC2(DMA2)) {
    LL_DMA_ClearFlag_TC2(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[10])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[10](soc, SODS_DMA_CH(10), dmarc,
                         soc->dma.chcbargs[10]);
    } else {
      soc->dma.chcbs[10](soc, SODS_DMA_CH(10), dmarc, NULL);
    }
  }
}

void stm32cube_dma2_stream3_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE3(DMA2)) {
    LL_DMA_ClearFlag_FE3(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME3(DMA2)) {
    LL_DMA_ClearFlag_DME3(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE3(DMA2)) {
    LL_DMA_ClearFlag_TE3(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT3(DMA2)) {
    LL_DMA_ClearFlag_HT3(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC3(DMA2)) {
    LL_DMA_ClearFlag_TC3(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[11])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[11](soc, SODS_DMA_CH(11), dmarc,
                         soc->dma.chcbargs[11]);
    } else {
      soc->dma.chcbs[11](soc, SODS_DMA_CH(11), dmarc, NULL);
    }
  }
}

void stm32cube_dma2_stream4_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE4(DMA2)) {
    LL_DMA_ClearFlag_FE4(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME4(DMA2)) {
    LL_DMA_ClearFlag_DME4(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE4(DMA2)) {
    LL_DMA_ClearFlag_TE4(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT4(DMA2)) {
    LL_DMA_ClearFlag_HT4(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC4(DMA2)) {
    LL_DMA_ClearFlag_TC4(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[12])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[12](soc, SODS_DMA_CH(12), dmarc,
                         soc->dma.chcbargs[12]);
    } else {
      soc->dma.chcbs[12](soc, SODS_DMA_CH(12), dmarc, NULL);
    }
  }
}

void stm32cube_dma2_stream5_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE5(DMA2)) {
    LL_DMA_ClearFlag_FE5(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME5(DMA2)) {
    LL_DMA_ClearFlag_DME5(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE5(DMA2)) {
    LL_DMA_ClearFlag_TE5(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT5(DMA2)) {
    LL_DMA_ClearFlag_HT5(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC5(DMA2)) {
    LL_DMA_ClearFlag_TC5(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[13])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[13](soc, SODS_DMA_CH(13), dmarc,
                         soc->dma.chcbargs[13]);
    } else {
      soc->dma.chcbs[13](soc, SODS_DMA_CH(13), dmarc, NULL);
    }
  }
}

void stm32cube_dma2_stream6_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE6(DMA2)) {
    LL_DMA_ClearFlag_FE6(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME6(DMA2)) {
    LL_DMA_ClearFlag_DME6(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE6(DMA2)) {
    LL_DMA_ClearFlag_TE6(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT6(DMA2)) {
    LL_DMA_ClearFlag_HT6(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC6(DMA2)) {
    LL_DMA_ClearFlag_TC6(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[14])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[14](soc, SODS_DMA_CH(14), dmarc,
                         soc->dma.chcbargs[14]);
    } else {
      soc->dma.chcbs[14](soc, SODS_DMA_CH(14), dmarc, NULL);
    }
  }
}

void stm32cube_dma2_stream7_isr(void)
{
  xwirq_t irqn;
  struct soc_irq_data irqdata;
  struct xwds_soc * soc;
  xwu32_t dmarc;

  xwos_irq_get_id(&irqn);
  xwos_irq_get_data(irqn, &irqdata);
  soc = irqdata.data;

  dmarc = 0;
  if (LL_DMA_IsActiveFlag_FE7(DMA2)) {
    LL_DMA_ClearFlag_FE7(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_FE;
  }
  if (LL_DMA_IsActiveFlag_DME7(DMA2)) {
    LL_DMA_ClearFlag_DME7(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_DME;
  }
  if (LL_DMA_IsActiveFlag_TE7(DMA2)) {
    LL_DMA_ClearFlag_TE7(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TE;
  }
  if (LL_DMA_IsActiveFlag_HT7(DMA2)) {
    LL_DMA_ClearFlag_HT7(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_HT;
  }
  if (LL_DMA_IsActiveFlag_TC7(DMA2)) {
    LL_DMA_ClearFlag_TC7(DMA2);
    dmarc |= STM32CUBE_SOC_DMA_RC_TC;
  }

  if ((soc->dma.chcbs) && (soc->dma.chcbs[15])) {
    if (soc->dma.chcbargs) {
      soc->dma.chcbs[15](soc, SODS_DMA_CH(15), dmarc,
                         soc->dma.chcbargs[15]);
    } else {
      soc->dma.chcbs[15](soc, SODS_DMA_CH(15), dmarc, NULL);
    }
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
