/**
 * @file
 * @brief 板级描述层：初始化
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
#include <xwos/osal/irq.h>
#include <soc_osc.h>
#include <soc_sim.h>
#include <soc_ics.h>
#include <bdl/board_init.h>

static __xwbsp_init_code
void bdl_clk_init(void);

__xwbsp_init_code
void board_lowlevel_init(void)
{
        bdl_clk_init();
}

/**
 * @brief init board
 */
__xwbsp_init_code
void board_init(void)
{
}

static __xwbsp_init_code
void bdl_clk_init(void)
{
#if defined(BRDCFG_ICS_OSC) && (1 == BRDCFG_ICS_OSC)
        soc_osc.cr.bit.hgo = BRDCFG_ICS_OSC_HG;
  #if (BRDCFG_ICS_OSC_VALUE >= 4000000U)
        soc_osc.cr.bit.range = 1;
  #else
        soc_osc.cr.bit.range = 0;
  #endif
        soc_osc.cr.bit.oscos = BRDCFG_ICS_OSC_IS_CRYSTAL;
        soc_osc.cr.bit.oscsten = BRDCFG_ICS_OSC_STEN;
        soc_osc.cr.bit.oscen = 1;
        while (!soc_osc.cr.bit.oscinit)
                ; /* do_nothing(); */

        xwu32_t rdiv = 0;
        switch(BRDCFG_ICS_OSC_VALUE) {
        case 8000000U:
        case 10000000U:
                /* 8MHz or 10MHz */
                rdiv = 3; /* now the divided frequency is 8000K/256 = 31.25K */
                          /* now the divided frequency is 10000K/256 = 39.0625K */
                break;
        case 4000000U:
                /* 4MHz */
                rdiv = 2; /* now the divided frequency is 4000/128 = 31.25K */
                break;
        case 16000000U:
        case 20000000U:
                /* 16MHz */
                rdiv = 4; /* now the divided frequency is 16000/512 = 31.25K */
                          /* now the divided frequency is 20000K/512 = 39.0625K */
                break;
        case 32000U:
            /* 32KHz */
                rdiv = 0;
                break;
        default:
                rdiv = 0;
                break;
        }
        soc_ics.c1.bit.rdiv = rdiv;
#endif
        soc_ics.c1.bit.irefsten = BRDCFG_ICS_IRC_STEN;
        soc_ics.c1.bit.irclken = BRDCFG_ICS_IRC;
#if (SOC_CLKM_FEI == BRDCFG_ICS_MODE)
        soc_ics.c1.bit.irefs = 1;
        soc_ics.c1.bit.clks = 0;
        while (!(soc_ics.s.bit.irefst))
                ;/* do_nothing(); */
        /* wait for FLL to lock */
        while (!(soc_ics.s.bit.lock))
                ;/* do_nothing(); */
#elif (SOC_CLKM_FEE == BRDCFG_ICS_MODE)
        soc_ics.c1.bit.irefs = 0;
        soc_ics.c1.bit.clks = 0;
        while (soc_ics.s.bit.irefst)
                ;/* do_nothing(); */
        /* wait for FLL to lock */
        while (!(soc_ics.s.bit.lock))
                ;/* do_nothing(); */
#elif (SOC_CLKM_FBI == BRDCFG_ICS_MODE)
        soc_ics.c2.bit.lp = 0;
        soc_ics.c1.bit.irefs = 1;
        soc_ics.c1.bit.clks = 1;
        while (!(soc_ics.s.bit.irefst))
                ;/* do_nothing(); */
        /* wait for FLL to lock */
        while (!(soc_ics.s.bit.lock))
                ;/* do_nothing(); */
#elif (SOC_CLKM_FBILP == BRDCFG_ICS_MODE)
        soc_ics.c2.bit.lp = 1;
        soc_ics.c1.bit.irefs = 1;
        soc_ics.c1.bit.clks = 1;
#elif (SOC_CLKM_FBE == BRDCFG_ICS_MODE)
        soc_ics.c2.bit.lp = 0;
        soc_ics.c1.bit.irefs = 0;
        soc_ics.c1.bit.clks = 2;
        while (soc_ics.s.bit.irefst)
                ;/* do_nothing(); */
        /* wait for FLL to lock */
        while (!(soc_ics.s.bit.lock))
                ;/* do_nothing(); */
#elif (SOC_CLKM_FBELP == BRDCFG_ICS_MODE)
        soc_ics.c2.bit.lp = 1;
        soc_ics.c1.bit.irefs = 0;
        soc_ics.c1.bit.clks = 2;
#else
  #error "Unknown clock mode !"
#endif
        /* clear Loss of lock sticky bit */
        soc_ics.s.bit.lols = 1;

        soc_sim.clkdiv.word = 0x01100000; /* div2 = 2; div3 = 2; div1 = 1; */
        soc_ics.c2.bit.bdiv = 0; /* bdiv = 1; */
}
