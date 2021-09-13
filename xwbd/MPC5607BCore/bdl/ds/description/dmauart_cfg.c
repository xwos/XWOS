/**
 * @file
 * @brief 设备栈描述：DMA方式的UART
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

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/irq.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/uart/dma.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <bdl/ds/driver/dmauart0.h>
#include <soc_gpio.h>
#include <soc_dma.h>
#include <soc_dmauart.h>

const struct soc_irq_cfg mpc560xb_uart0_irq_cfgs[] = {
        [0] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
};

const struct xwos_irq_resource mpc560xb_uart0_irq_resources[] = {
        [0] = {
                .irqn = IRQ_LINFLEX0_ERR,
                .isr = mpc560xb_dmauart0_err_isr,
                .cfg = &mpc560xb_uart0_irq_cfgs[0],
                .description = "rsc.irq.linflexd.err.0",
        },
};

const struct xwds_resource_reg mpc560xb_uart0_reg_resources[] = {
        [0] = {
                .base = (void *)(&LINFLEX_0),
                .size = sizeof(struct LINFLEXD0_tag),
                .description = "rsc.reg.linflexd.0",
        },
};

const struct xwds_resource_clk mpc560xb_uart0_clk_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .clkid = MPC560XB_CLK_LINFLEX_0,
                .description = "rsc.clk.linflexd.0",
        },
};

const struct xwds_resource_gpio mpc560xb_uart0_gpio_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_B,
                .pinmask = XWBOP_BIT(SOC_GPIO_PIN_2) | XWBOP_BIT(SOC_GPIO_PIN_3),
                .description = "rsc.gpio.linflexd.0",
        },
};

union soc_dma_tcd mpc560xb_uart0_rxdmatcds[] __xwcc_aligned(32) = {
        [0].std = {
                .SADDR = ((volatile xwu32_t)&LINFLEX_0.BDRM) + 3,
                .SMOD = 0,
                .SSIZE = SOC_DMA_TCD_SIZE_8BIT,
                .DMOD = 0,
                .DSIZE = SOC_DMA_TCD_SIZE_8BIT,
                .SOFF = 0,
                .NBYTES = 1,
                .SLAST = 0,
                .DADDR = SOC_DMA_TCD_CFG_NULL,
                .CITERE_LINK = 0,
                .CITER = SOC_DMA_TCD_CFG_NULL,
                .DOFF = 1,
                .DLAST_SGA = SOC_DMA_TCD_CFG_NULL,
                .BITERE_LINK = 0,
                .BITER = SOC_DMA_TCD_CFG_NULL,
                .BWC = SOC_DMA_TCD_BWC_NONE,
                .MAJORLINKCH = SOC_DMA_TCD_CFG_NULL,
                .DONE = SOC_DMA_TCD_CFG_NULL,
                .ACTIVE = SOC_DMA_TCD_CFG_NULL,
                .MAJORE_LINK = 0,
                .E_SG = 0,
                .D_REQ = 1,
                .INT_HALF = 1,
                .INT_MAJ = 1,
                .START = 0,
        },
};

struct soc_dmach_private_cfg mpc560xb_uart0_rxdmach_cfg = {
        .dmamux.bit = {
                .enbl = 1,
                .trig = 0,
                .src = SOC_DMAMUX_LINFLEX0_RX,
        },
        .tcds = mpc560xb_uart0_rxdmatcds,
};

union soc_dma_tcd mpc560xb_uart0_txdmatcds[] __xwcc_aligned(32) = {
        [0].std = {
                .SADDR = SOC_DMA_TCD_CFG_NULL,
                .SMOD = 0,
                .SSIZE = SOC_DMA_TCD_SIZE_8BIT,
                .DMOD = 0,
                .DSIZE = SOC_DMA_TCD_SIZE_8BIT,
                .SOFF = 1,
                .NBYTES = 1,
                .SLAST = SOC_DMA_TCD_CFG_NULL,
                .DADDR = ((volatile xwu32_t)&LINFLEX_0.BDRL) + 3,
                .CITERE_LINK = 0,
                .CITER = SOC_DMA_TCD_CFG_NULL,
                .DOFF = 0,
                .DLAST_SGA = 0,
                .BITERE_LINK = 0,
                .BITER = SOC_DMA_TCD_CFG_NULL,
                .BWC = SOC_DMA_TCD_BWC_NONE,
                .MAJORLINKCH = SOC_DMA_TCD_CFG_NULL,
                .DONE = SOC_DMA_TCD_CFG_NULL,
                .ACTIVE = SOC_DMA_TCD_CFG_NULL,
                .MAJORE_LINK = 0,
                .E_SG = 0,
                .D_REQ = 1,
                .INT_HALF = 0,
                .INT_MAJ = 1,
                .START = 0,
        },
};

struct soc_dmach_private_cfg mpc560xb_uart0_txdmach_cfg = {
        .dmamux.bit = {
                .enbl = 1,
                .trig = 0,
                .src = SOC_DMAMUX_LINFLEX0_TX,
        },
        .tcds = mpc560xb_uart0_txdmatcds,
};

const struct xwds_resource_dma mpc560xb_uart0_dma_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .ch = 0,
                .xwccfg = &mpc560xb_uart0_rxdmach_cfg,
                .description = "rsc.dma.linflexd.0.rx",
        },
        [1] = {
                .soc = &mpc560xb_soc_cb,
                .ch = 1,
                .xwccfg = &mpc560xb_uart0_txdmach_cfg,
                .description = "rsc.dma.linflexd.0.tx",
        },
};

const struct xwds_resources mpc560xb_uart0_resources = {
        .irqrsc_array = mpc560xb_uart0_irq_resources,
        .irqrsc_num = xw_array_size(mpc560xb_uart0_irq_resources),
        .regrsc_array = mpc560xb_uart0_reg_resources,
        .regrsc_num = xw_array_size(mpc560xb_uart0_reg_resources),
        .clkrsc_array = mpc560xb_uart0_clk_resources,
        .clkrsc_num = xw_array_size(mpc560xb_uart0_clk_resources),
        .gpiorsc_array = mpc560xb_uart0_gpio_resources,
        .gpiorsc_num = xw_array_size(mpc560xb_uart0_gpio_resources),
        .dmarsc_array = mpc560xb_uart0_dma_resources,
        .dmarsc_num = xw_array_size(mpc560xb_uart0_dma_resources),
};

const struct soc_dmauart_private_cfg mpc560xb_uart0_xwccfg = {
        .pto = 4000,
};

const struct xwds_uart_cfg mpc560xb_uart0_cfg = {
        .baudrate = B2000000,
        .bus.bit = {
                .bits = XWDS_UART_BITS_9,
                .stopbits = XWDS_UART_STOPBITS_1_0,
                .parity = XWDS_UART_PARITY_NONE,
                .hfc = XWDS_UART_HFC_NONE,
                .mode = XWDS_UART_MODE_RX | XWDS_UART_MODE_TX,
        },
        .xwccfg = (void *)&mpc560xb_uart0_xwccfg,
};

struct mpc560xb_dmauart0_drvdata mpc560xb_uart0_drvdata;

struct xwds_dmauartc mpc560xb_uart0_cb = {
        /* attributes */
        .dev = {
                .name = "mpc560xb.linflexd",
                .id = 0,
                .resources = &mpc560xb_uart0_resources,
                .drv = xwds_cast(struct xwds_driver *, &mpc560xb_dmauart0_drv),
                .data = &mpc560xb_uart0_drvdata,
        },
        .cfg = &mpc560xb_uart0_cfg,
        .rxdmarsc = &mpc560xb_uart0_dma_resources[0],
        .txdmarsc = &mpc560xb_uart0_dma_resources[1],
};
