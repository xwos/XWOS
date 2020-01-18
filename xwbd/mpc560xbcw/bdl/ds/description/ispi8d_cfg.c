/**
 * @file
 * @brief 设备栈描述：SPI 8-bit从机模式通讯
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
#include <soc_gpio.h>
#include <xwos/lib/xwbop.h>
#include <xwos/irq.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/spi/ispi8d.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <driver/ds/ispi8d.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct soc_irq_cfg mpc560xb_ispi8d1_irq_cfgs[] = {
        [0] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
        [1] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
};

const struct xwos_irq_resource mpc560xb_ispi8d1_irq_resources[] = {
        [0] = {
                .irqn = IRQ_DSPI1_SR_TFUF_RFOF,
                .isr = mpc560xb_ispi8d_isr_tfuf_rfof,
                .cfg = &mpc560xb_ispi8d1_irq_cfgs[0],
                .description = "rsc.irq.dspi.1.tfut_rfof",
        },
        [1] = {
                .irqn = IRQ_DSPI1_SR_RFDF,
                .isr = mpc560xb_ispi8d_isr_rfdf,
                .cfg = &mpc560xb_ispi8d1_irq_cfgs[1],
                .description = "rsc.irq.dspi.1.rfdf",
        },
};

const struct xwds_resource_reg mpc560xb_ispi8d1_reg_resources[] = {
        [0] = {
                .base = (void *)(&DSPI_1),
                .size = sizeof(struct DSPI_tag),
                .description = "rsc.reg.dspi.1",
        },
};

const struct xwds_resource_clk mpc560xb_ispi8d1_clk_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .clkid = MPC560XB_CLK_DSPI_1,
                .description = "rsc.clk.dspi.1",
        },
};

const struct xwds_resource_gpio mpc560xb_ispi8d1_gpio_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_C,
                .pinmask = (BIT(SOC_GPIO_PIN_2) | /* CLK */
                            BIT(SOC_GPIO_PIN_3) | /* CS */
                            BIT(SOC_GPIO_PIN_4) | /* SIN */
                            BIT(SOC_GPIO_PIN_5)), /* SOUT */
                .description = "rsc.gpio.dspi.1.bus",
        },
        [1] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_E,
                .pinmask = BIT(SOC_GPIO_PIN_11), /* INT2Master */
                .description = "rsc.gpio.dspi.1.int_master",
        },
};

const struct xwds_resources mpc560xb_ispi8d1_resources = {
        .irqrsc_array = mpc560xb_ispi8d1_irq_resources,
        .irqrsc_num = xw_array_size(mpc560xb_ispi8d1_irq_resources),
        .regrsc_array = mpc560xb_ispi8d1_reg_resources,
        .regrsc_num = xw_array_size(mpc560xb_ispi8d1_reg_resources),
        .clkrsc_array = mpc560xb_ispi8d1_clk_resources,
        .clkrsc_num = xw_array_size(mpc560xb_ispi8d1_clk_resources),
        .gpiorsc_array = mpc560xb_ispi8d1_gpio_resources,
        .gpiorsc_num = xw_array_size(mpc560xb_ispi8d1_gpio_resources),
};

const struct xwds_ispi8d_cfg mpc560xb_ispi8d1_cfg = {
        .bus.bit = {
                .wire = SODS_SPI_WIRE_4,
                .direction = SODS_SPI_DIR_TX | SODS_SPI_DIR_RX,
                .cpol = SODS_ISPI8D_CPOL_HIGH,
                .cpha = SODS_ISPI8D_CPHA_LSTC,
                .frmodr = SODS_ISPI8D_FO_LSB_FIRST,
                .csmode = SODS_ISPI8D_CS_HWMODE | SODS_ISPI8D_CS_INACTIVE_HIGH,
                .ntfmode = SODS_ISPI8D_NTFM_LOW,
        },
        .xwccfg = NULL,
};

struct xwds_ispi8d mpc560xb_ispi8d1_cb = {
        .dev = {
                .name = "mpc560xb.dspi.ispi8d",
                .id = 1,
                .resources = &mpc560xb_ispi8d1_resources,
                .drv = xwds_static_cast(struct xwds_driver *, &mpc560xb_ispi8d_drv),
                .data = NULL,
        },
        .cfg = &mpc560xb_ispi8d1_cfg,
};
