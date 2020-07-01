/**
 * @file
 * @brief 设备栈描述：UART
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/irq.h>
#include <soc_irq.h>
#include <soc_gpio.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/uart/general.h>
#include <bdl/ds/description/mpc560xbdkp.h>
#include <driver/ds/uart.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct soc_irq_cfg mpc560xb_uart0_irq_cfgs[] = {
        [0] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
        [1] = {
                .priority = BDL_IRQ_PRIO_HIGH,
        },
        [2] = {
                .priority = BDL_IRQ_PRIO_MIDDLE,
        },
};

const struct xwos_irq_resource mpc560xb_uart0_irq_resources[] = {
        [0] = {
                .irqn = IRQ_LINFLEX0_RXI,
                .isr = mpc560xb_uart_rx_isr,
                .cfg = &mpc560xb_uart0_irq_cfgs[0],
                .description = "rsc.irq.linflex.0.rxi",
        },
        [1] = {
                .irqn = IRQ_LINFLEX0_TXI,
                .isr = mpc560xb_uart_tx_isr,
                .cfg = &mpc560xb_uart0_irq_cfgs[1],
                .description = "rsc.irq.linflex.0.txi",
        },
        [2] = {
                .irqn = IRQ_LINFLEX0_ERR,
                .isr = mpc560xb_uart_err_isr,
                .cfg = &mpc560xb_uart0_irq_cfgs[2],
                .description = "rsc.irq.linflex.0.err",
        },
};

const struct xwds_resource_reg mpc560xb_uart0_reg_resources[] = {
        [0] = {
                .base = (void *)(&LINFLEX_0),
                .size = sizeof(struct LINFLEX_tag),
                .description = "rsc.reg.linflex.0",
        },
};

const struct xwds_resource_clk mpc560xb_uart0_clk_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .clkid = MPC560XB_CLK_LINFLEX_0,
                .description = "rsc.clk.linflex.0",
        },
};

const struct xwds_resource_gpio mpc560xb_uart0_gpio_resources[] = {
        [0] = {
                .soc = &mpc560xb_soc_cb,
                .port = SOC_GPIO_PORT_B,
                .pinmask = BIT(SOC_GPIO_PIN_2) | BIT(SOC_GPIO_PIN_3),
                .description = "rsc.gpio.linflex.0",
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
};

const struct xwds_uart_cfg mpc560xb_uart0_cfg = {
        .baudrate = BRDCFG_AMQ_UART_BAUDRATE,
        .bus.bit = {
                .bits = XWDS_UART_BITS_9,
                .stopbits = XWDS_UART_STOPBITS_1_0,
                .parity = XWDS_UART_PARITY_NONE,
                .hfc = XWDS_UART_HFC_NONE,
                .mode = XWDS_UART_MODE_TX | XWDS_UART_MODE_RX,
        },
        .xwccfg = NULL,
};

struct xwds_uartc mpc560xb_uart0_cb = {
        /* attributes */
        .dev = {
                .name = "mpc560xb_uart",
                .id = 1,
                .resources = &mpc560xb_uart0_resources,
                .drv = xwds_static_cast(struct xwds_driver *, &mpc560xb_uart_drv),
                .data = NULL,
        },
        .cfg = &mpc560xb_uart0_cfg,
};
