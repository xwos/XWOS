/**
 * @file
 * @brief S32K14x clock
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
#include <soc.h>
#include <soc_clk.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化LPO
 * @note
 * - 32K_LPO: enabled
 * - 1K_LPO: enabled
 * - RTC_CLK: 32KHz
 * - LPO_CLK: 32KHz
 */
__xwbsp_init_code
void soc_clk_lpo_init(void)
{
        SIM->LPOCLKS = SIM_LPOCLKS_RTCCLKSEL(1) |
                       SIM_LPOCLKS_LPOCLKSEL(2) |
                       SIM_LPOCLKS_LPO32KCLKEN(1) |
                       SIM_LPOCLKS_LPO1KCLKEN(1);
        /* RTCCLKSEL = 1: 32KHz LPO as RTC_CLK */
        /* LPOCLKSEL = 2: 32KHz LPO as LPO_CLK */
        /* LPO32KCLKEN = 1: enable 32KHz LPO */
        /* LPO1KCLKEN = 1: enable 1KHz LPO */
}

/**
 * @brief 初始化SIRC振荡器
 * @note
 * - SIRC clock: 8MHz
 * - SIRCDIV1 clock: 8MHz
 * - SIRCDIV2 clock: 8MHz
 */
__xwbsp_init_code
void soc_clk_sirc_enable(void)
{
        SCG->SIRCDIV = 0x00000101;
        /* SIRCDIV1 = 1; SIRCDIV2 = 1 */

        SCG->SIRCCFG = 0x0000001;
        /* RANGE = 1: High (8MHz) */

        /* Ensure SIRCCSR unlocked */
        while (SCG->SIRCCSR & SCG_SIRCCSR_LK_MASK) {
        }

        SCG->SIRCCSR = 0x00000001;
        /* LK = 0:          SIRCCSR can be written */
        /* SIRCLPEN = 0:    SIRC disabled in VLP modes */
        /* SIRCSTEN = 0:    SIRC disabled in Stop modes */
        /* SIRCEN = 1:      enable SIRC */

        /* Wait for sys OSC clk valid */
        while (!(SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK)) {
        }
}

/**
 * @brief 关闭SIRC振荡器
 */
__xwbsp_init_code
void soc_clk_sirc_disable(void)
{
        /* Ensure SIRCCSR unlocked */
        while (SCG->SIRCCSR & SCG_SIRCCSR_LK_MASK) {
        }

        SCG->SIRCCSR = 0x00000000;
        /* Wait for sys OSC clk valid */
        while (SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK) {
        }
}

/**
 * @brief 初始化FIRC振荡器
 * @note
 * - FIRC clock: 48MHz
 * - FIRCDIV1 clock: 48MHz
 * - FIRCDIV2 clock: 24MHz
 */
__xwbsp_init_code
void soc_clk_firc_enable(void)
{
        SCG->FIRCDIV = 0x00000201;
        /* FIRCDIV1 = 1; FIRCDIV2 = 2 */

        SCG->FIRCCFG = 0x0000000;
        /* RANGE = 0: (48MHz) */

        /* Ensure FIRCCSR unlocked */
        while (SCG->FIRCCSR & SCG_FIRCCSR_LK_MASK) {
        }

        SCG->FIRCCSR = 0x00000001;
        /* LK = 0:          FIRCCSR can be written */
        /* FIRCREGOFF = 1:    FIRC disabled in Stop modes */
        /* FIRCEN = 1:      enable FIRC */

        /* Wait for sys OSC clk valid */
        while (!(SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK)) {
        }
}

/**
 * @brief 关闭FIRC振荡器
 */
__xwbsp_init_code
void soc_clk_firc_disable(void)
{
        /* Ensure FIRCCSR unlocked */
        while (SCG->FIRCCSR & SCG_FIRCCSR_LK_MASK) {
        }

        SCG->FIRCCSR = 0x00000000;
        /* Wait for sys OSC clk valid */
        while (SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK) {
        }
}

/**
 * @brief 初始化8MHz外部晶振
 */
__xwbsp_init_code
void soc_clk_xwsc_init_8mhz(void)
{
        SCG->SOSCDIV = 0x00000101;
        /* SOSCDIV1 & SOSCDIV2 = 1: divide by 1 */

        SCG->SOSCCFG = 0x00000024;
        /* Range=2: Medium freq (SOSC betw 1MHz-8MHz) */
        /* HGO=0:   Config xtal osc for low power */
        /* EREFS=0: Input is external XTAL */

        /* Ensure SOSCCSR unlocked */
        while (SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK) {
        }

        SCG->SOSCCSR = 0x00000001;
        /* LK = 0:          SOSCCSR can be written */
        /* SOSCCMRE = 0:    OSC CLK monitor IRQ if enabled */
        /* SOSCCM = 0:      OSC CLK monitor disabled */
        /* SOSCERCLKEN = 0: Sys OSC 3V ERCLK output clk disabled */
        /* SOSCLPEN = 0:    Sys OSC disabled in VLP modes */
        /* SOSCSTEN = 0:    Sys OSC disabled in Stop modes */
        /* SOSCEN = 1:      enable oscillator */

        /* Wait for sys OSC clk valid */
        while (!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)) {
        }
}

/**
 * @brief 关闭外部晶振
 */
__xwbsp_init_code
void soc_clk_xwsc_disable(void)
{
        /* Ensure SOSCCSR unlocked */
        while (SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK) {
        }

        SCG->SOSCCSR = 0x00000000;
        /* LK = 0:          SOSCCSR can be written */
        /* SOSCCMRE = 0:    OSC CLK monitor IRQ if enabled */
        /* SOSCCM = 0:      OSC CLK monitor disabled */
        /* SOSCERCLKEN = 0: Sys OSC 3V ERCLK output clk disabled */
        /* SOSCLPEN = 0:    Sys OSC disabled in VLP modes */
        /* SOSCSTEN = 0:    Sys OSC disabled in Stop modes */
        /* SOSCEN = 1:      Enable oscillator */

        /* Wait for sys OSC clk valid */
        while (SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) {
        }
}

/**
 * @brief 初始化SPLL为160MHz
 * @note
 * - VCO_CLK = SPLL_SOURCE / (PREDIV + 1) X (MULT + 16) = 320MHz
 * - SPLL = VCO_CLK / 2 = 160MHz
 * - SPLLDIV1 clock: 80MHz
 * - SPLLDIV2 clock: 40MHz
 */
__xwbsp_init_code
void soc_clk_spll_init_160mhz(void)
{
        /* Ensure SPLLCSR unlocked */
        while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) {
        }

        SCG->SPLLCSR = 0x04000000;  /* SPLLEN = 0: SPLL is disabled (default) */
        SCG->SPLLDIV = 0x00000302;  /* SPLLDIV1 = 2; SPLLDIV2 = 4 */
        SCG->SPLLCFG = 0x00180000;
        /* PREDIV = 0: Divide SOSC_CLK by 0 + 1 = 1 */
        /* MULT=24: Multiply sys pll by 16 + 24 = 40 */
        /* SPLL_CLK = 8MHz / 1 * 40 / 2 = 160 MHz */

        /* Ensure SPLLCSR unlocked */
        while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) {
        }

        SCG->SPLLCSR = 0x00000001;
        /* LK = 0:       SPLLCSR can be written */
        /* SPLLCMRE = 0: SPLL CLK monitor IRQ if enabled */
        /* SPLLCM = 0:   SPLL CLK monitor disabled */
        /* SPLLSTEN = 0: SPLL disabled in Stop modes */
        /* SPLLEN = 1:   Enable SPLL */

        /* Wait for SPLL valid */
        while (!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)) {
        }
}

/**
 * @brief 初始化SPLL为112MHz
 * @note
 * - VCO_CLK = SPLL_SOURCE / (PREDIV + 1) X (MULT + 16) = 224MHz
 * - SPLL = VCO_CLK / 2 = 112MHz
 * - SPLLDIV1 clock: 56MHz
 * - SPLLDIV2 clock: 28MHz
 */
__xwbsp_init_code
void soc_clk_spll_init_112mhz(void)
{
        /* Ensure SPLLCSR unlocked */
        while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) {
        }

        SCG->SPLLCSR = 0x04000000;  /* SPLLEN = 0: SPLL is disabled (default) */
        SCG->SPLLDIV = 0x00000302;  /* SPLLDIV1 = 2; SPLLDIV2 = 4 */
        SCG->SPLLCFG = 0x000C0000;
        /* PREDIV = 0: Divide SOSC_CLK by 0 + 1 = 1 */
        /* MULT=24: Multiply sys pll by 16 + 12 = 28 */
        /* SPLL_CLK = 8MHz / 1 * 28 / 2 = 112MHz */

        /* Ensure SPLLCSR unlocked */
        while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) {
        }

        SCG->SPLLCSR = 0x00000001;
        /* LK = 0:       SPLLCSR can be written */
        /* SPLLCMRE = 0: SPLL CLK monitor IRQ if enabled */
        /* SPLLCM = 0:   SPLL CLK monitor disabled */
        /* SPLLSTEN = 0: SPLL disabled in Stop modes */
        /* SPLLEN = 1:   Enable SPLL */

        /* Wait for SPLL valid */
        while (!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)) {
        }
}

__xwbsp_init_code
void soc_clk_spll_disable(void)
{
        /* Ensure SPLLCSR unlocked */
        while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK) {
        }

        SCG->SPLLCSR = 0x00000000;
        /* LK = 0:       SPLLCSR can be written */
        /* SPLLCMRE = 0: SPLL CLK monitor IRQ if enabled */
        /* SPLLCM = 0:   SPLL CLK monitor disabled */
        /* SPLLSTEN = 0: SPLL disabled in Stop modes */
        /* SPLLEN = 0:   disable SPLL */

        /* Wait for SPLL invalid */
        while (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) {
        }
}

/**
 * @brief 初始化Normal RUN mode的时钟频率为80MHz
 * @note
 * - prediv sys clock: 160MHz
 * - sys clock: 80MHz
 * - core clock: 80MHz
 * - bus clock: 40MHz
 * - flash clock = 26.67MHz
 */
__xwbsp_init_code
void soc_clk_nrun_init_80mhz(void)
{
        SCG->RCCR = SCG_RCCR_SCS(6) |     /* PLL as clock source */
                    SCG_RCCR_DIVCORE(1) | /* DIVCORE = 2, Core clock = 80 MHz */
                    SCG_RCCR_DIVBUS(1) |  /* DIVBUS = 2, bus clock = 40 MHz */
                    SCG_RCCR_DIVSLOW(2);  /* DIVSLOW = 3, flash clock = 26.67 MHz */

        /* Wait for sys clk src == SPLL */
        while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 6) {
        }
}

/**
 * @brief 初始化Normal RUN mode的时钟频率为8MHz
 * @note
 * - prediv sys clock: 8MHz
 * - sys clock: 8MHz
 * - core clock: 8MHz
 * - bus clock: 4MHz
 * - flash clock = 2MHz
 */
__xwbsp_init_code
void soc_clk_nrun_init_8mhz(void)
{
        SCG->RCCR = SCG_RCCR_SCS(2) |     /* SIRC as clock source */
                    SCG_RCCR_DIVCORE(0) | /* DIVCORE = 1, Core clock = 8 MHz */
                    SCG_RCCR_DIVBUS(1) |  /* DIVBUS = 2, bus clock = 4 MHz */
                    SCG_RCCR_DIVSLOW(3);  /* DIVSLOW = 4, flash clock = 2 MHz */

        /* Wait for sys clk src == SPLL */
        while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 2) {
        }
}

/**
 * @brief 初始化HSRUN mode的时钟频率为112MHz
 * @note
 * - prediv sys clock: 112MHz
 * - sys clock: 112MHz
 * - core clock: 112MHz
 * - bus clock: 56MHz
 * - flash clock = 28MHz
 */
__xwbsp_init_code
void soc_clk_hsrun_init_112mhz(void)
{
        SCG->HCCR = SCG_HCCR_SCS(6) |     /* PLL as clock source */
                    SCG_HCCR_DIVCORE(0) | /* DIVCORE = 1, Core clock = 112 MHz */
                    SCG_HCCR_DIVBUS(1) |  /* DIVBUS = 2, bus clock = 56 MHz */
                    SCG_HCCR_DIVSLOW(3);  /* DIVSLOW = 4, flash clock = 28 MHz */

        /* Wait for sys clk src == SPLL */
        while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 6) {
        }
}

/**
 * @brief 初始化VLR mode的时钟频率为4MHz
 * @note
 * - prediv sys clock: 8MHz
 * - sys clock: 4MHz
 * - core clock: 4MHz
 * - bus clock: 4MHz
 * - flash clock = 1MHz
 */
__xwbsp_init_code
void soc_clk_vlpr_init_4mhz(void)
{
        SCG->VCCR = SCG_VCCR_SCS(2) |     /* SIRC as clock source */
                    SCG_VCCR_DIVCORE(1) | /* DIVCORE = 2, Core clock = 4 MHz */
                    SCG_VCCR_DIVBUS(0) |  /* DIVBUS = 1, bus clock = 4 MHz */
                    SCG_VCCR_DIVSLOW(3);  /* DIVSLOW = 4, flash clock = 1 MHz */

        /* Wait for sys clk src == SPLL */
        while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 2) {
        }
}
