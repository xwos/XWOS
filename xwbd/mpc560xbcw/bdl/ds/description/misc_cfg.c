/**
 * @file
 * @brief 设备栈描述：MISC设备
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
#include <xwos/lib/xwbop.h>
#include <xwos/irq.h>
#include <soc_irq.h>
#include <soc_gpio.h>
#include <soc_adc.h>
#include <xwmd/ds/misc/chip.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <bdl/ds/misc/driver.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct soc_irq_cfg mpc560xb_misc_irq_cfgs[] = {
        [0] = {
                .priority = BDL_IRQ_PRIO_LOW,
        },
        [1] = {
                .priority = BDL_IRQ_PRIO_LOW,
        },
        [2] = {
                .priority = BDL_IRQ_PRIO_LOW,
        },
};

const struct xwos_irq_resource mpc560xb_misc_irq_resources[] = {
        [0] = {
                .irqn = IRQ_ADC0_EOC,
                .isr = mpc560xb_adc_eoc_isr,
                .cfg = &mpc560xb_misc_irq_cfgs[0],
                .description = "rsc.irq.adc.eoc",
        },
        [1] = {
                .irqn = IRQ_ADC0_ER,
                .isr = mpc560xb_adc_er_isr,
                .cfg = &mpc560xb_misc_irq_cfgs[1],
                .description = "rsc.irq.adc.er",
        },
        [2] = {
                .irqn = IRQ_ADC0_WD,
                .isr = mpc560xb_adc_wd_isr,
                .cfg = &mpc560xb_misc_irq_cfgs[2],
                .description = "rsc.irq.adc.wd",
        },
};

const struct xwds_resource_clk mpc560xb_misc_clk_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .clkid = MPC560XB_CLK_ADC_0,
                .description = "rsc.clk.adc",
        },
/*
        [1] = {
                .soc = &mpc560xb_soc_cb,
                .clkid = MPC560XB_CLK_EMIOS_0,
                .description = "rsc.clk.emios.0",
        },
*/
};

const struct xwds_resource_gpio mpc560xb_misc_gpio_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_B,
                .pinmask = (BIT(SOC_GPIO_PIN_4) |   /* 8V_DET -- PB4 -- P[0] */
                            BIT(SOC_GPIO_PIN_5) |   /* MIC_OUT -- PB5 -- P[1] */
                            BIT(SOC_GPIO_PIN_6)),   /* MIC_IN -- PB6 -- P[2] */
                .description = "rsc.gpio.adc.ch.set1",
        },
        [1] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_D,
                .pinmask = (BIT(SOC_GPIO_PIN_0) |   /* 12V_DET  --  P[4] */
                            BIT(SOC_GPIO_PIN_1) |   /* 3V3_DET  --  P[5] */
                            BIT(SOC_GPIO_PIN_2) |   /* 3V9_DET  --  P[6] */
                            BIT(SOC_GPIO_PIN_3) |   /* 1V8_DET  --  P[7] */
                            BIT(SOC_GPIO_PIN_4) |   /* BAT_TMPR_ACQ  --  P[8] */
                            BIT(SOC_GPIO_PIN_5) |   /* BAT_DET  --  P[9]  */
                            BIT(SOC_GPIO_PIN_6) |   /* VGPSANT_DET  --  P[10] */
                            BIT(SOC_GPIO_PIN_7) |   /* GPSANT_DET   --  P[11] */
                            BIT(SOC_GPIO_PIN_8) |   /* USB_VBUS_ACQ  --  P[12] */
                            BIT(SOC_GPIO_PIN_9) |   /* 5V_DET  --  P[13] */
                            BIT(SOC_GPIO_PIN_10)|   /* LTEANT_DIV_DET  --  P[14] */
                            BIT(SOC_GPIO_PIN_11)),  /* WAN_ANT_DET  --  P[15] */
                .description = "rsc.gpio.adc.ch.set2",
        },
};

const struct xwds_resources mpc560xb_misc_resources = {
        .irqrsc_array = mpc560xb_misc_irq_resources,
        .irqrsc_num = xw_array_size(mpc560xb_misc_irq_resources),
        .regrsc_array = NULL,
        .regrsc_num = 0,
        .clkrsc_array = mpc560xb_misc_clk_resources,
        .clkrsc_num = xw_array_size(mpc560xb_misc_clk_resources),
        .gpiorsc_array = mpc560xb_misc_gpio_resources,
        .gpiorsc_num = xw_array_size(mpc560xb_misc_gpio_resources),
};

const struct soc_adc_private_cfg mpc560xb_adc_cfg = {
        .mcr.bit = {
                .mode = SOC_ADC_MCR_MODE_ONESHOT, /* One Shot mode */
                .clock_select = SOC_ADC_MCR_CLKSEL_HALF, /* 32MHz */
        },
        .ncmr = {
                [0] = (BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(6) |
                       BIT(7) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) |
                       BIT(13) | BIT(14) | BIT(15)),
                [1] = 0,
                [2] = 0,
        },
        .jcmr = {
                [0] = 0,
                [1] = 0,
                [2] = 0,
        },
        .ctr = {
                [0].bit = {
                        .inplatch = 1,
                        .offshift = 0,
                        .inpcmp = 3,
                        .inpsamp = 255,
                },
                [1].u32 = 0,
                [2].u32 = 0,
        },
};

const struct mpc560xb_misc_private_cfg mpc560xb_misc_cfg = {
        .adc = &mpc560xb_adc_cfg,
};

struct mpc560xb_misc_drvdata mpc560xb_misc_drvdata;

struct xwds_misc mpc560xb_misc_cb = {
        .dev = {
                .name = "mpc560xb.misc",
                .id = 1,
                .resources = &mpc560xb_misc_resources,
                .drv = xwds_static_cast(struct xwds_driver *, &mpc560xb_misc_drv),
                .data = &mpc560xb_misc_drvdata,
        },
        .misccfg = &mpc560xb_misc_cfg,
};
