/**
 * @file
 * @brief 设备栈描述：SPI主机
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
#include <soc_spi.h>
#include <xwos/lib/xwbop.h>
#include <xwos/irq.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/spi/master.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <driver/ds/spim.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct soc_irq_cfg mpc560xb_spim1_irq_cfgs[] = {
        [0] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
        [1] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
        [2] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
};

const struct xwos_irq_resource mpc560xb_spim1_irq_resources[] = {
        [0] = {
                .irqn = IRQ_DSPI1_SR_TFUF_RFOF,
                .isr = mpc560xb_spim_isr_tfuf_rfof,
                .cfg = &mpc560xb_spim1_irq_cfgs[0],
                .description = "rsc.irq.dspi.1.tfuf_rfof",
        },
        [1] = {
                .irqn = IRQ_DSPI1_SR_TFFF,
                .isr = mpc560xb_spim_isr_tfff,
                .cfg = &mpc560xb_spim1_irq_cfgs[1],
                .description = "rsc.irq.dspi.1.tfff",
        },
        [2] = {
                .irqn = IRQ_DSPI1_SR_RFDF,
                .isr = mpc560xb_spim_isr_rfdf,
                .cfg = &mpc560xb_spim1_irq_cfgs[2],
                .description = "rsc.irq.dspi.1.rfdf",
        },
};

const struct xwds_resource_reg mpc560xb_spim1_reg_resources[] = {
        [0] = {
                .base = (void *)(&DSPI_1),
                .size = sizeof(struct DSPI_tag),
                .description = "rsc.reg.dspi.1",
        },
};

const struct xwds_resource_clk mpc560xb_spim1_clk_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .clkid = MPC560XB_CLK_DSPI_1,
                .description = "rsc.clk.dspi.1",
        },
};

const struct xwds_resource_gpio mpc560xb_spim1_gpio_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_C,
                .pinmask = (BIT(SOC_GPIO_PIN_2) | /* CLK */
                            BIT(SOC_GPIO_PIN_4) | /* SIN */
                            BIT(SOC_GPIO_PIN_5)), /* SOUT */
                .description = "rsc.gpio.dspi.1.bus",
        },
        [1] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_C,
                .pinmask = BIT(SOC_GPIO_PIN_3), /* CS0 */
                .description = "rsc.gpio.dspi.1.cs0",
        },
};

const struct xwds_resources mpc560xb_spim1_resources = {
        .irqrsc_array = mpc560xb_spim1_irq_resources,
        .irqrsc_num = xw_array_size(mpc560xb_spim1_irq_resources),
        .regrsc_array = mpc560xb_spim1_reg_resources,
        .regrsc_num = xw_array_size(mpc560xb_spim1_reg_resources),
        .clkrsc_array = mpc560xb_spim1_clk_resources,
        .clkrsc_num = xw_array_size(mpc560xb_spim1_clk_resources),
        .gpiorsc_array = mpc560xb_spim1_gpio_resources,
        .gpiorsc_num = xw_array_size(mpc560xb_spim1_gpio_resources),
};

const struct xwds_spim_cspin mpc560xb_spim1_cspins[] = {
        [0] = {
                .csres = &mpc560xb_spim1_gpio_resources[1],
                .inactive = 1,
        },
};

const struct soc_spi_private_cfg mpc560xb_spim1_xwccfg = {
        .clkdiv.bit = {
                .pcssck = SOC_SPI_CLKDIV_PCSSCK_1,
                .cssck = SOC_SPI_CLKDIV_CSSCK_64,
                .pasc = SOC_SPI_CLKDIV_PASC_1,
                .asc = SOC_SPI_CLKDIV_ASC_64,
                .pdt = SOC_SPI_CLKDIV_PDT_1,
                .dt = SOC_SPI_CLKDIV_DT_32,
                .pbr = SOC_SPI_CLKDIV_PBR_2,
                .br = SOC_SPI_CLKDIV_BR_64,
                .dbr = true,
        },
        .continuous_sck = false,
};

const struct xwds_spim_cfg mpc560xb_spim1_cfg = {
        .bus.bit = {
                .wire = SODS_SPI_WIRE_4,
                .direction = SODS_SPI_DIR_TX | SODS_SPI_DIR_RX,
        },
        .cspins = mpc560xb_spim1_cspins,
        .cspins_num = xw_array_size(mpc560xb_spim1_cspins),
        .xwccfg = (void *)&mpc560xb_spim1_xwccfg,
};

struct mpc560xb_spim_drvdata mpc560xb_spim1_drvdata;

struct xwds_spim mpc560xb_spim1_cb = {
        /* attributes */
        .dev = {
                .name = "mpc560xb.spi.m",
                .id = 1,
                .resources = &mpc560xb_spim1_resources,
                .drv = xwds_static_cast(struct xwds_driver *, &mpc560xb_spim_drv),
                .data = &mpc560xb_spim1_drvdata,
        },
        .cfg = (void *)&mpc560xb_spim1_cfg,
};
