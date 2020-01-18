/**
 * @file
 * @brief 设备栈描述：RTC
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
#include <soc_irq.h>
#include <soc_clk.h>
#include <soc_gpio.h>
#include <soc_rtc.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/clock.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <driver/ds/rtc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct soc_irq_cfg mpc560xb_rtc_irq_cfgs[] = {
        [0] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
        [1] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
};

const struct xwos_irq_resource mpc560xb_rtc_irq_resources[] = {
        [0] = {
                .irqn = IRQ_RTC,
                .isr = mpc560xb_rtc_isr,
                .cfg = &mpc560xb_rtc_irq_cfgs[0],
                .description = "rsc.irq.rtc",
        },
        [1] = {
                .irqn = IRQ_API,
                .isr = mpc560xb_api_isr,
                .cfg = &mpc560xb_rtc_irq_cfgs[1],
                .description = "rsc.irq.api",
        },
};

const struct xwds_resource_reg mpc560xb_rtc_reg_resources[] = {
        [0] = {
                .base = (void *)(&RTC),
                .size = sizeof(struct RTC_tag),
                .description = "rsc.reg.rtc",
        },
};

const struct xwds_resource_clk mpc560xb_rtc_clk_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .clkid = MPC560XB_CLK_RTC,
                .description = "rsc.clk.rtc",
        },
};

const struct xwds_resources mpc560xb_rtc_resources = {
        .irqrsc_array = mpc560xb_rtc_irq_resources,
        .irqrsc_num = xw_array_size(mpc560xb_rtc_irq_resources),
        .regrsc_array = mpc560xb_rtc_reg_resources,
        .regrsc_num = xw_array_size(mpc560xb_rtc_reg_resources),
        .clkrsc_array = mpc560xb_rtc_clk_resources,
        .clkrsc_num = xw_array_size(mpc560xb_rtc_clk_resources),
        .gpiorsc_array = NULL,
        .gpiorsc_num = 0,
};

const struct soc_rtc_private_cfg mpc560xb_rtc_cfg = {
#ifdef DEBUG_RTC
        .u.bit = {
                .reserved = 0,
                .rtcie = SOC_RTC_RTCIE_ENABLED,
                .frzen = SOC_RTC_FRZEN_ENABLED,
                .rovren = SOC_RTC_ROVREN_DISABLED,
                .rtcval = 1,
                .apien = SOC_RTC_APIEN_DISABLED,
                .apiie = SOC_RTC_APIIE_DISABLED,
                .clksel = SOC_RTC_CLKSEL_SIRC,
                .div512en = SOC_RTC_DIV512EN_DISABLED,
                .div32en = SOC_RTC_DIV32EN_ENABLED, /* 160Hz */
                .apival = 1000, /* 100s */
        },
#else
        .u.bit = {
                .reserved = 0,
                .rtcie = SOC_RTC_RTCIE_ENABLED,
                .frzen = SOC_RTC_FRZEN_ENABLED,
                .rovren = SOC_RTC_ROVREN_DISABLED,
                .rtcval = 0xFFF, /* 5 day */
                .apien = SOC_RTC_APIEN_DISABLED,
                .apiie = SOC_RTC_APIIE_DISABLED,
                .clksel = SOC_RTC_CLKSEL_SIRC, /* 5120 HZ  */
                .div512en = SOC_RTC_DIV512EN_ENABLED, /* Clock scale is 100ms */
                .div32en = SOC_RTC_DIV32EN_DISABLED,
                .apival = 1000, /* 100s */
        },
#endif
};

struct xwds_misc mpc560xb_rtc_cb = {
        .dev = {
                .name = "mpc560xb.rtc",
                .id = 1,
                .resources = &mpc560xb_rtc_resources,
                .drv = xwds_static_cast(struct xwds_driver *, &mpc560xb_rtc_drv),
                .data = NULL,
        },
        .misccfg = &mpc560xb_rtc_cfg,
};
