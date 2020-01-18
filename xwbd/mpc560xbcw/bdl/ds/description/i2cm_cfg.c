/**
 * @file
 * @brief 设备栈描述：I2C主机
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
#include <soc_i2c.h>
#include <soc_gpio.h>
#include <xwos/lib/xwbop.h>
#include <xwos/irq.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/i2c/master.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <driver/ds/i2cm.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct soc_irq_cfg mpc560xb_i2cm_irq_cfgs[] = {
        [0] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
};

const struct xwos_irq_resource mpc560xb_i2cm_irq_resources[] = {
        [0] = {
                .irqn = IRQ_I2C0_SR,
                .isr = mpc560xb_i2cm_isr,
                .cfg = &mpc560xb_i2cm_irq_cfgs[0],
                .description = "rsc.irq.i2c",
        },
};

const struct xwds_resource_clk mpc560xb_i2cm_clk_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .clkid = MPC560XB_CLK_I2C,
                .description = "rsc.clk.i2c",
        },
};

const struct xwds_resource_gpio mpc560xb_i2cm_gpio_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_A,
                .pinmask = (BIT(SOC_GPIO_PIN_11) | /* SCL */
                            BIT(SOC_GPIO_PIN_10)), /* SDA */
                .description = "rsc.gpio.i2c",
        },
};

const struct xwds_resources mpc560xb_i2cm_resources = {
        .irqrsc_array = mpc560xb_i2cm_irq_resources,
        .irqrsc_num = xw_array_size(mpc560xb_i2cm_irq_resources),
        .regrsc_array = NULL,
        .regrsc_num = 0,
        .clkrsc_array = mpc560xb_i2cm_clk_resources,
        .clkrsc_num = xw_array_size(mpc560xb_i2cm_clk_resources),
        .gpiorsc_array = mpc560xb_i2cm_gpio_resources,
        .gpiorsc_num = xw_array_size(mpc560xb_i2cm_gpio_resources),
};

const struct soc_i2c_private_cfg mpc560xb_i2cm_xwccfg = {
        .bfdr = 0x2D, /* scl divider: 640 (baudrate 100KHz when pclk1 == 64MHz),
                         sda hold: 97
                         scl hold (start): 318
                         scl hold (stop): 321
                         ref: mpc5604bcrm.pdf table 20-7 @p361 */
};

const struct xwds_i2cm_cfg mpc560xb_i2cm_cfg = {
        .xwccfg = (void *)&mpc560xb_i2cm_xwccfg,
};

struct mpc560xb_i2cm_drvdata mpc560xb_i2cm_drvdata;

struct xwds_i2cm mpc560xb_i2cm_cb = {
        .dev = {
                .name = "mpc560xb.i2c.m",
                .id = 1,
                .resources = &mpc560xb_i2cm_resources,
                .drv = xwds_static_cast(struct xwds_driver *, &mpc560xb_i2cm_drv),
                .data = &mpc560xb_i2cm_drvdata,
        },
        .cfg = &mpc560xb_i2cm_cfg,
};
