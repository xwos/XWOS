/**
 * @file
 * @brief 设备栈描述：SOC
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
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/chip.h>
#include <bdl/ds/description/s32k14xdkp.h>
#include <soc.h>
#include <soc_clk.h>
#include <soc_eirq.h>
#include <soc_gpio.h>
#include <soc_dma.h>
#include <driver/ds/soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct soc_irq_cfg s32k14x_soc_irq_cfgs[] = {
        [0] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [1] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [2] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [3] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [4] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [5] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [6] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [7] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [8] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [9] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [10] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [11] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [12] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [13] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [14] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [15] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_LOW),
                },
        },
        [16] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_HIGH),
                },
        },
        [17] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_HIGH),
                },
        },
        [18] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_HIGH),
                },
        },
        [19] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_HIGH),
                },
        },
        [20] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_HIGH),
                },
        },
        [21] = {
                .irqcfg = {
                        .priority = (ARCH_IRQ_PRIO_6 | ARCH_IRQ_SUBPRIO_HIGH),
                },
        },
};

const struct xwos_irq_resource s32k14x_soc_irq_resources[] = {
        [0] = {
                .irqn = SOC_IRQ_DMA0,
                .isr = s32k14x_edma_ch0_isr,
                .cfg = &s32k14x_soc_irq_cfgs[0],
                .description = "rsc.irq.edma.ch0",
        },
        [1] = {
                .irqn = SOC_IRQ_DMA1,
                .isr = s32k14x_edma_ch1_isr,
                .cfg = &s32k14x_soc_irq_cfgs[1],
                .description = "rsc.irq.edma.ch1",
        },
        [2] = {
                .irqn = SOC_IRQ_DMA2,
                .isr = s32k14x_edma_ch2_isr,
                .cfg = &s32k14x_soc_irq_cfgs[2],
                .description = "rsc.irq.edma.ch2",
        },
        [3] = {
                .irqn = SOC_IRQ_DMA3,
                .isr = s32k14x_edma_ch3_isr,
                .cfg = &s32k14x_soc_irq_cfgs[3],
                .description = "rsc.irq.edma.ch3",
        },
        [4] = {
                .irqn = SOC_IRQ_DMA4,
                .isr = s32k14x_edma_ch4_isr,
                .cfg = &s32k14x_soc_irq_cfgs[4],
                .description = "rsc.irq.edma.ch4",
        },
        [5] = {
                .irqn = SOC_IRQ_DMA5,
                .isr = s32k14x_edma_ch5_isr,
                .cfg = &s32k14x_soc_irq_cfgs[5],
                .description = "rsc.irq.edma.ch5",
        },
        [6] = {
                .irqn = SOC_IRQ_DMA6,
                .isr = s32k14x_edma_ch6_isr,
                .cfg = &s32k14x_soc_irq_cfgs[6],
                .description = "rsc.irq.edma.ch6",
        },
        [7] = {
                .irqn = SOC_IRQ_DMA7,
                .isr = s32k14x_edma_ch7_isr,
                .cfg = &s32k14x_soc_irq_cfgs[7],
                .description = "rsc.irq.edma.ch7",
        },
        [8] = {
                .irqn = SOC_IRQ_DMA8,
                .isr = s32k14x_edma_ch8_isr,
                .cfg = &s32k14x_soc_irq_cfgs[8],
                .description = "rsc.irq.edma.ch8",
        },
        [9] = {
                .irqn = SOC_IRQ_DMA9,
                .isr = s32k14x_edma_ch9_isr,
                .cfg = &s32k14x_soc_irq_cfgs[9],
                .description = "rsc.irq.edma.ch9",
        },
        [10] = {
                .irqn = SOC_IRQ_DMA10,
                .isr = s32k14x_edma_ch10_isr,
                .cfg = &s32k14x_soc_irq_cfgs[10],
                .description = "rsc.irq.edma.ch10",
        },
        [11] = {
                .irqn = SOC_IRQ_DMA11,
                .isr = s32k14x_edma_ch11_isr,
                .cfg = &s32k14x_soc_irq_cfgs[11],
                .description = "rsc.irq.edma.ch11",
        },
        [12] = {
                .irqn = SOC_IRQ_DMA12,
                .isr = s32k14x_edma_ch12_isr,
                .cfg = &s32k14x_soc_irq_cfgs[12],
                .description = "rsc.irq.edma.ch12",
        },
        [13] = {
                .irqn = SOC_IRQ_DMA13,
                .isr = s32k14x_edma_ch13_isr,
                .cfg = &s32k14x_soc_irq_cfgs[13],
                .description = "rsc.irq.edma.ch13",
        },
        [14] = {
                .irqn = SOC_IRQ_DMA14,
                .isr = s32k14x_edma_ch14_isr,
                .cfg = &s32k14x_soc_irq_cfgs[14],
                .description = "rsc.irq.edma.ch14",
        },
        [15] = {
                .irqn = SOC_IRQ_DMA15,
                .isr = s32k14x_edma_ch15_isr,
                .cfg = &s32k14x_soc_irq_cfgs[15],
                .description = "rsc.irq.edma.ch15",
        },
        [16] = {
                .irqn = SOC_IRQ_DMA_Error,
                .isr = s32k14x_edma_cmberr_isr,
                .cfg = &s32k14x_soc_irq_cfgs[16],
                .description = "rsc.irq.edma.err",
        },
        [17] = {
                .irqn = SOC_IRQ_PORTA,
                .isr = s32k14x_porta_isr,
                .cfg = &s32k14x_soc_irq_cfgs[17],
                .description = "rsc.irq.port.a",
        },
        [18] = {
                .irqn = SOC_IRQ_PORTB,
                .isr = s32k14x_portb_isr,
                .cfg = &s32k14x_soc_irq_cfgs[18],
                .description = "rsc.irq.port.b",
        },
        [19] = {
                .irqn = SOC_IRQ_PORTC,
                .isr = s32k14x_portc_isr,
                .cfg = &s32k14x_soc_irq_cfgs[19],
                .description = "rsc.irq.port.c",
        },
        [20] = {
                .irqn = SOC_IRQ_PORTD,
                .isr = s32k14x_portd_isr,
                .cfg = &s32k14x_soc_irq_cfgs[20],
                .description = "rsc.irq.port.d",
        },
        [21] = {
                .irqn = SOC_IRQ_PORTE,
                .isr = s32k14x_porte_isr,
                .cfg = &s32k14x_soc_irq_cfgs[21],
                .description = "rsc.irq.port.e",
        },

};

const struct xwds_resource_clk s32k14x_soc_clk_resources[] = {
        [0] = {
                .soc = &s32k14x_soc_cb,
                .clkid = S32K14X_MCLK_DMA,
                .description = "rsc.clk.m.edma",
        },
        [1] = {
                .soc = &s32k14x_soc_cb,
                .clkid = S32K14X_MCLK_DMAMUX,
                .description = "rsc.clk.m.dmamux",
        },
        [2] = {
                .soc = &s32k14x_soc_cb,
                .clkid = S32K14X_MCLK_PORTA,
                .description = "rsc.clk.m.port.a",
        },
        [3] = {
                .soc = &s32k14x_soc_cb,
                .clkid = S32K14X_MCLK_PORTB,
                .description = "rsc.clk.m.port.b",
        },
        [4] = {
                .soc = &s32k14x_soc_cb,
                .clkid = S32K14X_MCLK_PORTC,
                .description = "rsc.clk.m.port.c",
        },
        [5] = {
                .soc = &s32k14x_soc_cb,
                .clkid = S32K14X_MCLK_PORTD,
                .description = "rsc.clk.m.port.d",
        },
        [6] = {
                .soc = &s32k14x_soc_cb,
                .clkid = S32K14X_MCLK_PORTE,
                .description = "rsc.clk.m.port.e",
        },

};

const struct xwds_resources s32k14x_soc_resources = {
        .irqrsc_array = s32k14x_soc_irq_resources,
        .irqrsc_num = xw_array_size(s32k14x_soc_irq_resources),
        .regrsc_array = NULL,
        .regrsc_num = 0,
        .clkrsc_array = s32k14x_soc_clk_resources,
        .clkrsc_num = xw_array_size(s32k14x_soc_clk_resources),
        .pwrrsc_array = NULL,
        .pwrrsc_num = 0,
        .gpiorsc_array = NULL,
        .gpiorsc_num = 0,
        .dmarsc_array = NULL,
        .dmarsc_num = 0,
};

const struct soc_sys_cfg s32k14x_soc_cfg = {
        .sim = {
                .chipctl.bit = {
                        .adc_interleave_en = 0,
                        .clkoutsel = SOC_CLK_SIM_CHIPCTL_CLKOUTSEL_SCG_CLKOUT,
                        .clkoutdiv = 0,
                        .clkouten = 0,
                        .traceclk_sel = 0,
                        .pdb_bb_sel = 0,
                        .adc_supply = 0,
                        .adc_supplyen = 1,
                        .sramu_reten = 1,
                        .sraml_reten = 1,
                },
        },
        .smc = {
                .pmctrl.bit = {
                        .stopm = 2,
                        .runm = 0,
                },
                .stopctrl.bit = {
                        .stopo = 1,
                },
        },
        .pmc = {
                .lvdsc1.bit = {
                        .lvdre = 0,
                        .lvdie = 1,
                },
                .lvdsc2.bit = {
                        .lvwie = 0,
                },
                .regsc.bit = {
                        .biasen = 1,
                        .clkbiasdis = 1,
                        .lpodis = 0,
                },
        },
};

__xwmd_rodata const
struct soc_gpio_portcfg s32k14x_gpio_portcfgs[SOC_GPIO_PORT_NUM] = {
        [SOC_GPIO_PORT_A] = {
                .pincfg = {
                        [SOC_GPIO_PIN_0] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_1] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_2] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_3] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_4] = { /* JTAG_TMS/SWD_DIO */
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_EN,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_AM7,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_5] = { /* RESET_b */
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_EN,
                                        .pfe = SOC_GPIO_PFE_EN,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_AM7,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_6] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_7] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_8] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_9] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_10] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_11] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_12] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_13] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_14] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_15] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_16] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_17] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_18] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_19] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_20] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_21] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_22] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_23] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_24] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_25] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_26] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_27] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_28] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_29] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_30] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_31] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                },
                .dfer.bit = {
                        .pin0 = 0,
                        .pin1 = 0,
                        .pin2 = 0,
                        .pin3 = 0,
                        .pin4 = 0,
                        .pin5 = 0,
                        .pin6 = 0,
                        .pin7 = 0,
                        .pin8 = 0,
                        .pin9 = 0,
                        .pin10 = 0,
                        .pin11 = 0,
                        .pin12 = 0,
                        .pin13 = 0,
                        .pin14 = 0,
                        .pin15 = 0,
                        .pin16 = 0,
                        .pin17 = 0,
                        .pin18 = 0,
                        .pin19 = 0,
                        .pin20 = 0,
                        .pin21 = 0,
                        .pin22 = 0,
                        .pin23 = 0,
                        .pin24 = 0,
                        .pin25 = 0,
                        .pin26 = 0,
                        .pin27 = 0,
                        .pin28 = 0,
                        .pin29 = 0,
                        .pin30 = 0,
                        .pin31 = 0,
                },
                .dfcr.bit = {
                        .cs = SOC_GPIO_DFCS_LPO,
                },
                .dfwr.bit = {
                        .filt = 0,
                },
        },
        [SOC_GPIO_PORT_B] = {
                .pincfg = {
                        [SOC_GPIO_PIN_0] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_1] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_2] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_3] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_4] = { /* Red LED */
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_GPIO,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_OUTPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_5] = { /* Green LED */
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_GPIO,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_OUTPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_6] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_7] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_8] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_9] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_10] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_11] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_12] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_13] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_14] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_15] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_16] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_17] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_18] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_19] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_20] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_21] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_22] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_23] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_24] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_25] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_26] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_27] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_28] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_29] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_30] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_31] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                },
                .dfer.bit = {
                        .pin0 = 0,
                        .pin1 = 0,
                        .pin2 = 0,
                        .pin3 = 0,
                        .pin4 = 0,
                        .pin5 = 0,
                        .pin6 = 0,
                        .pin7 = 0,
                        .pin8 = 0,
                        .pin9 = 0,
                        .pin10 = 0,
                        .pin11 = 0,
                        .pin12 = 0,
                        .pin13 = 0,
                        .pin14 = 0,
                        .pin15 = 0,
                        .pin16 = 0,
                        .pin17 = 0,
                        .pin18 = 0,
                        .pin19 = 0,
                        .pin20 = 0,
                        .pin21 = 0,
                        .pin22 = 0,
                        .pin23 = 0,
                        .pin24 = 0,
                        .pin25 = 0,
                        .pin26 = 0,
                        .pin27 = 0,
                        .pin28 = 0,
                        .pin29 = 0,
                        .pin30 = 0,
                        .pin31 = 0,
                },
                .dfcr.bit = {
                        .cs = SOC_GPIO_DFCS_LPO,
                },
                .dfwr.bit = {
                        .filt = 0,
                },
        },
        [SOC_GPIO_PORT_C] = {
                .pincfg = {
                        [SOC_GPIO_PIN_0] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_1] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_2] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_3] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_4] = { /* JTAG_TCLK/SWD_CLK */
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PD,
                                        .pe = SOC_GPIO_PE_EN,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_AM7,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_5] = { /* JTAG_TDI */
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_EN,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_AM7,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_6] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_7] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_8] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_9] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_10] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_11] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_12] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_13] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_14] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_15] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_16] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_17] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_18] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_19] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_20] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_21] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_22] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_23] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_24] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_25] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_26] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_27] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_28] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_29] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_30] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_31] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                },
                .dfer.bit = {
                        .pin0 = 0,
                        .pin1 = 0,
                        .pin2 = 0,
                        .pin3 = 0,
                        .pin4 = 0,
                        .pin5 = 0,
                        .pin6 = 0,
                        .pin7 = 0,
                        .pin8 = 0,
                        .pin9 = 0,
                        .pin10 = 0,
                        .pin11 = 0,
                        .pin12 = 0,
                        .pin13 = 0,
                        .pin14 = 0,
                        .pin15 = 0,
                        .pin16 = 0,
                        .pin17 = 0,
                        .pin18 = 0,
                        .pin19 = 0,
                        .pin20 = 0,
                        .pin21 = 0,
                        .pin22 = 0,
                        .pin23 = 0,
                        .pin24 = 0,
                        .pin25 = 0,
                        .pin26 = 0,
                        .pin27 = 0,
                        .pin28 = 0,
                        .pin29 = 0,
                        .pin30 = 0,
                        .pin31 = 0,
                },
                .dfcr.bit = {
                        .cs = SOC_GPIO_DFCS_LPO,
                },
                .dfwr.bit = {
                        .filt = 0,
                },
        },
        [SOC_GPIO_PORT_D] = {
                .pincfg = {
                        [SOC_GPIO_PIN_0] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_1] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_2] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_3] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_4] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_5] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_6] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_7] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_8] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_9] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_10] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_11] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_12] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_13] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_14] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_15] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_16] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_17] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_18] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_19] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_20] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_21] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_22] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_23] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_24] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_25] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_26] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_27] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_28] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_29] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_30] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_31] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                },
                .dfer.bit = {
                        .pin0 = 0,
                        .pin1 = 0,
                        .pin2 = 0,
                        .pin3 = 0,
                        .pin4 = 0,
                        .pin5 = 0,
                        .pin6 = 0,
                        .pin7 = 0,
                        .pin8 = 0,
                        .pin9 = 0,
                        .pin10 = 0,
                        .pin11 = 0,
                        .pin12 = 0,
                        .pin13 = 0,
                        .pin14 = 0,
                        .pin15 = 0,
                        .pin16 = 0,
                        .pin17 = 0,
                        .pin18 = 0,
                        .pin19 = 0,
                        .pin20 = 0,
                        .pin21 = 0,
                        .pin22 = 0,
                        .pin23 = 0,
                        .pin24 = 0,
                        .pin25 = 0,
                        .pin26 = 0,
                        .pin27 = 0,
                        .pin28 = 0,
                        .pin29 = 0,
                        .pin30 = 0,
                        .pin31 = 0,
                },
                .dfcr.bit = {
                        .cs = SOC_GPIO_DFCS_LPO,
                },
                .dfwr.bit = {
                        .filt = 0,
                },
        },
        [SOC_GPIO_PORT_E] = {
                .pincfg = {
                        [SOC_GPIO_PIN_0] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_1] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_2] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_3] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_4] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_5] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_6] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_7] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_8] = { /* Blue LED */
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_GPIO,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_OUTPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_9] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_10] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_11] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_12] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_13] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_14] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_15] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_16] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_17] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_18] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_19] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_20] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_21] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_22] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_23] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_24] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_25] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_26] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_27] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_28] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_29] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_30] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                        [SOC_GPIO_PIN_31] = {
                                .pcr.bit = {
                                        .ps = SOC_GPIO_PS_PU,
                                        .pe = SOC_GPIO_PE_DS,
                                        .pfe = SOC_GPIO_PFE_DS,
                                        .dse = SOC_GPIO_DSE_LOW,
                                        .mux = SOC_GPIO_MUX_ANALOG,
                                        .lk = 0,
                                },
                                .io.bit = {
                                        .dov = SOC_GPIO_OUT_LOW,
                                        .pdd = SOC_GPIO_PDD_INPUT,
                                        .pid = SOC_GPIO_PID_DS,
                                },
                        },
                },
                .dfer.bit = {
                        .pin0 = 0,
                        .pin1 = 0,
                        .pin2 = 0,
                        .pin3 = 0,
                        .pin4 = 0,
                        .pin5 = 0,
                        .pin6 = 0,
                        .pin7 = 0,
                        .pin8 = 0,
                        .pin9 = 0,
                        .pin10 = 0,
                        .pin11 = 0,
                        .pin12 = 0,
                        .pin13 = 0,
                        .pin14 = 0,
                        .pin15 = 0,
                        .pin16 = 0,
                        .pin17 = 0,
                        .pin18 = 0,
                        .pin19 = 0,
                        .pin20 = 0,
                        .pin21 = 0,
                        .pin22 = 0,
                        .pin23 = 0,
                        .pin24 = 0,
                        .pin25 = 0,
                        .pin26 = 0,
                        .pin27 = 0,
                        .pin28 = 0,
                        .pin29 = 0,
                        .pin30 = 0,
                        .pin31 = 0,
                },
                .dfcr.bit = {
                        .cs = SOC_GPIO_DFCS_LPO,
                },
                .dfwr.bit = {
                        .filt = 0,
                },
        },
};

__atomic  xwsq_t s32k14x_gpio_pinstatus[SOC_GPIO_PORT_NUM] = {
        [SOC_GPIO_PORT_A] = 0,
        [SOC_GPIO_PORT_B] = 0,
        [SOC_GPIO_PORT_C] = 0,
        [SOC_GPIO_PORT_D] = 0,
        [SOC_GPIO_PORT_E] = 0,
};

__xwds_soc_eirq_tbl_qualifier
xwds_eirq_f s32k14x_eirq_isrs[SOC_EIRQ_NUM] = {
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,

        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,

        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,

        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,

        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,
};

__xwds_soc_eirq_tbl_qualifier
xwds_eirq_arg_t s32k14x_eirq_isrargs[SOC_EIRQ_NUM] = {
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_A * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,

        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_B * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,

        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_C * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,

        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_D * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,

        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_0] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_1] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_2] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_3] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_4] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_5] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_6] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_7] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_8] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_9] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_10] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_11] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_12] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_13] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_14] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_15] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_16] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_17] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_18] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_19] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_20] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_21] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_22] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_23] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_24] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_25] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_26] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_27] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_28] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_29] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_30] = NULL,
        [SOC_GPIO_PORT_E * SOC_GPIO_PIN_NUM + SOC_GPIO_PIN_31] = NULL,
};

const struct soc_dmac_private_cfg s32k14x_dmac_cfg = {
        .cr.bit = {
                .edbg = 1,
                .erca = 1,
                .hoe = 1,
                .clm = 0,
                .emlm = 0,
        },
        .cpr = {
                [0].bit = {
                        .chpri = 0,
                        .dpa = 0,
                        .ecp = 0,
                },
                [1].bit = {
                        .chpri = 1,
                        .dpa = 0,
                        .ecp = 0,
                },
                [2].bit = {
                        .chpri = 2,
                        .dpa = 0,
                        .ecp = 0,
                },
                [3].bit = {
                        .chpri = 3,
                        .dpa = 0,
                        .ecp = 0,
                },
                [4].bit = {
                        .chpri = 4,
                        .dpa = 0,
                        .ecp = 0,
                },
                [5].bit = {
                        .chpri = 5,
                        .dpa = 0,
                        .ecp = 0,
                },
                [6].bit = {
                        .chpri = 6,
                        .dpa = 0,
                        .ecp = 0,
                },
                [7].bit = {
                        .chpri = 7,
                        .dpa = 0,
                        .ecp = 0,
                },
                [8].bit = {
                        .chpri = 8,
                        .dpa = 0,
                        .ecp = 0,
                },
                [9].bit = {
                        .chpri = 9,
                        .dpa = 0,
                        .ecp = 0,
                },
                [10].bit = {
                        .chpri = 10,
                        .dpa = 0,
                        .ecp = 0,
                },
                [11].bit = {
                        .chpri = 11,
                        .dpa = 0,
                        .ecp = 0,
                },
                [12].bit = {
                        .chpri = 12,
                        .dpa = 0,
                        .ecp = 0,
                },
                [13].bit = {
                        .chpri = 13,
                        .dpa = 0,
                        .ecp = 0,
                },
                [14].bit = {
                        .chpri = 14,
                        .dpa = 0,
                        .ecp = 0,
                },
                [15].bit = {
                        .chpri = 15,
                        .dpa = 0,
                        .ecp = 0,
                },
        },
};

__atomic DECLARE_BITMAP(s32k14x_dma_channel_status, SOC_DMAC_CHANNEL_NUM) = {
        0,
};

__xwds_soc_dma_tbl_qualifier
xwds_dma_f s32k14x_dma_cbcb_array[SOC_DMAC_CHANNEL_NUM] = {
        [0] = NULL,
        [1] = NULL,
        [2] = NULL,
        [3] = NULL,
        [4] = NULL,
        [5] = NULL,
        [6] = NULL,
        [7] = NULL,
        [8] = NULL,
        [9] = NULL,
        [10] = NULL,
        [11] = NULL,
        [12] = NULL,
        [13] = NULL,
        [14] = NULL,
        [15] = NULL,
};

__xwds_soc_dma_tbl_qualifier
xwds_dma_cbarg_t s32k14x_dma_chcbarg_array[SOC_DMAC_CHANNEL_NUM] = {
        [0] = NULL,
        [1] = NULL,
        [2] = NULL,
        [3] = NULL,
        [4] = NULL,
        [5] = NULL,
        [6] = NULL,
        [7] = NULL,
        [8] = NULL,
        [9] = NULL,
        [10] = NULL,
        [11] = NULL,
        [12] = NULL,
        [13] = NULL,
        [14] = NULL,
        [15] = NULL,
};

struct xwds_soc s32k14x_soc_cb = {
        .dev = {
                .name = "s32k14x.soc",
                .id = 1,
                .resources = &s32k14x_soc_resources,
                .drv = xwds_static_cast(struct xwds_driver *, &s32k14x_soc_drv),
                .data = NULL,
        },
        .xwccfg = &s32k14x_soc_cfg,
        .gpio = {
                .cfg = (void *)s32k14x_gpio_portcfgs,
                .pins = s32k14x_gpio_pinstatus,
                .port_num = SOC_GPIO_PORT_NUM,
                .pin_num = SOC_GPIO_PIN_NUM,
        },
        .clk = {
                .num = S32K14X_CLK_NUM,
        },
        .eirq = {
                .isrs = s32k14x_eirq_isrs,
                .isrargs = s32k14x_eirq_isrargs,
                .num = SOC_EIRQ_NUM,
        },
        .dma = {
                .ccfg = (void *)&s32k14x_dmac_cfg,
                .chstatus = s32k14x_dma_channel_status,
                .ch_num = SOC_DMAC_CHANNEL_NUM,
                .chcbs = s32k14x_dma_cbcb_array,
                .chcbargs = s32k14x_dma_chcbarg_array,
        },
};
