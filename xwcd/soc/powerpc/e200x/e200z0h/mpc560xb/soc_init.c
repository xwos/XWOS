/**
 * @file
 * @brief SOC描述层：初始化
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
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <soc.h>
#include <soc_clk.h>
#include <soc_me.h>
#include <soc_init.h>

#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
static __xwos_init_code
void soc_relocate_isrtable(void);
#endif /* !SOCCFG_RO_IVT */

extern xwu8_t xwos_ivt_lma_base[];
extern xwu8_t xwos_ivt_vma_base[];
extern xwu8_t xwos_ivt_vma_end[];

extern xwu8_t bkup_vma_base[];
extern xwu8_t bkup_vma_end[];
extern xwu8_t bkup_lma_base[];
extern xwu32_t bkup_mr_origin[];
extern xwu32_t bkup_mr_size[];

extern xwu8_t stdby_wkup_vma_base[];
extern xwu8_t stdby_wkup_vma_end[];
extern xwu8_t stdby_wkup_lma_base[];
extern xwu32_t stdby_wkup_mr_origin[];
extern xwu32_t stdby_wkup_mr_size[];

extern xwu32_t data_mr_origin[];
extern xwu32_t data_mr_size[];

__xwbsp_data struct soc_reset_flags soc_reset_flags;
__xwbsp_data struct soc_wkup_flags soc_wkup_flags;

__xwbsp_init_code
void soc_sysclk_init(void)
{
        xwu32_t pllldf;

        /* use RUN0 mode to configure clock */
        ME.MER.R |= 0x00000010;

        /* SIRC */
        CGM.SIRC_CTL.B.SIRCON_STDBY = 1;
        CGM.SIRC_CTL.B.RCDIV = SOCCFG_CLK_SIRC_DIV - 1;
        /* Slow RC oscillator
           Standby mode: ON
           The SIRC clock division factor: SOCCFG_CLK_SIRC_DIV */

        /* SXOSC */
        CGM.SXOSC_CTL.B.OSCON = 0;

        /* Clock Source Configuration */
        CGM.FIRC_CTL.R = 0x00000000;
        CGM.FIRC_CTL.B.RCDIV = SOCCFG_CLK_FIRC_DIV - 1;
        /* IRC Oscillator Trimming: 0
           The 16MHz IRC Clock Divider: SOCCFG_CLK_FIRC_DIV */

        CGM.FXOSC_CTL.B.EOCV = 0x0080;
        CGM.FXOSC_CTL.B.OSCDIV = SOCCFG_CLK_FXOSC_DIV - 1;
        /* Oscillator Output is Disabled as rootclock
           The End of Count Value for XOSC = 0x80
           The XOSC Clock Interrupt: Disabled
           The XOSC Clock Divider: SOCCFG_CLK_FXOSC_DIV */

#if (16000000 == SOCCFG_CLK_FXOSC_VALUE)
        pllldf = (SOCCFG_CLK_FMPLL_VALUE * 16) / (SOCCFG_CLK_FXOSC_VALUE / 1);
        CGM.FMPLL_CR.B.IDF = 1;
        CGM.FMPLL_CR.B.ODF = 2;
        CGM.FMPLL_CR.B.NDIV = pllldf;
        /* XOSC = SOCCFG_CLK_FXOSC_VALUE
           Input Division Factor: 2
           Output Division Factor: 8
           Loop Division Factor: pllldf
           So Vco >= 256MHz, FMPLL = SOCCFG_CLK_FMPLL_VALUE
           Progressive Clock Switching Disabled in Standby Mode */
#elif (8000000 == SOCCFG_CLK_FXOSC_VALUE)
        pllldf = (SOCCFG_CLK_FMPLL_VALUE * 4) / (SOCCFG_CLK_FXOSC_VALUE / 1);
        CGM.FMPLL_CR.B.IDF = 0;
        CGM.FMPLL_CR.B.ODF = 1;
        CGM.FMPLL_CR.B.NDIV = pllldf;
        /* XOSC = SOCCFG_CLK_FXOSC_VALUE
           Input Division Factor: 1
           Output Division Factor: 4
           Loop Division Factor: pllldf
           So Vco >= 256MHz, FMPLL = SOCCFG_CLK_FMPLL_VALUE
           Progressive Clock Switching Disabled in Standby Mode */
#endif

        CGM.FMPLL_MR.R = 0x00000000;
        /* The selected Spread Type: Down Spread
           The Modulation Period for FMPLL: 0
           Frequency modulation for PLL: Disabled
           The Increment Step for FMPLL: 0 */

        ME.RUN[0].R = 0x001F0070;
        /* I/O Power Down Control: Disabled
           Main Voltage Regulator Control: Enabled
           Data Flash Power Down Control: Normal
           Code Flash Power Down Control: Normal
           System PLL: ON
           Crystal Oscillator: ON
           16MHz IRC: ON
           System Clock Source: 16MHz internal RC oscillator */

        /* Enter in RUN[0] mode to update the configuration */
        ME.MCTL.R = 0x40005AF0;
        ME.MCTL.R = 0x4000A50F;
        /* Wait for mode entry to complete */
        while (0x1 == ME.GS.B.S_MTRANS) {
        }
        while (0x4 != ME.GS.B.S_CURRENTMODE) {
        }

        /* Wait for PLL to lock */
        while (0x0 == CGM.FMPLL_CR.B.S_LOCK) {
        }

        /* step 2: switch clock to PLL */
        ME.RUN[0].R = 0x001F0074;
        /* I/O Power Down Control: Disabled
           Main Voltage Regulator Control: Enabled
           Data Flash Power Down Control: Normal
           Code Flash Power Down Control: Normal
           System PLL: ON
           Crystal Oscillator: ON
           16MHz IRC: ON
           System Clock Source: System PLL */

        /* Enter in RUN[0] mode to update the configuration */
        ME.MCTL.R = 0x40005AF0;
        ME.MCTL.R = 0x4000A50F;
        /* Wait for mode entry to complete  */
        while (0x1 == ME.GS.B.S_MTRANS) {
        }

        while (0x4 != ME.GS.B.S_CURRENTMODE) {
        }

        /* Poll global Status Register to get current System Clock  */
        while (0x4 != ME.GS.B.S_SYSCLK) {
        }
}

__xwbsp_init_code
void soc_rgm_init(void)
{
        RGM.FEAR.R = 0x0000;
        /* Code or data flash fatal error: SAFE mode request to ME
           4.5V low-voltage detected: SAFE mode request to ME
           CMU0 clock frequency higher/lower than reference: SAFE mode request to ME
           Oscillator frequency lower than reference: SAFE mode request to ME
           PLL0 fail: SAFE mode request to ME
           Core reset: SAFE mode request to ME
           JTAG initiated reset: SAFE mode request to ME */

        RGM.FESS.R = 0x0000;
        /* Code or data flash fatal error : Start reset sequence from Phase1
           4.5V low-voltage detected : Start reset sequence from Phase1
           CMU0 clock frequency higher/lower than reference : Start reset sequence from Phase1
           Oscillator frequency lower than reference : Start reset sequence from Phase1
           PLL0 fail : Start reset sequence from Phase1
           Core reset : Start reset sequence from Phase1
           JTAG initiated reset : Start reset sequence from Phase1 */

        RGM.STDBY.R = 0x0080;
        /* System Boot Control :Boot from SRAM on STANDBY0 exit */

        RGM.FBRE.R = 0x0000;
        /* Code or data flash fatal error: External Reset asserted
           4.5V low-voltage detected: External Reset asserted
           CMU0 clock frequency higher/lower than reference: External Reset asserted
           Oscillator frequency lower than reference: External Reset asserted
           PLL0 fail: External Reset asserted
           Core reset: External Reset asserted
           JTAG initiated reset: External Reset asserted */

        RGM.FERD.R = 0x0000;
        /* Code or data flash fatal error: Enable reset sequence
           4.5V low-voltage detected: Enable reset sequence
           CMU0 clock frequency higher/lower than reference: Enable reset sequence
           Oscillator frequency lower than reference: Enable reset sequence
           PLL0 fail: Enable reset sequence
           Core reset: Enable reset sequence
           JTAG initiated reset: Enable reset sequence */
}

__xwbsp_init_code
void soc_flash_init(void)
{
        while (0 == CFLASH.MCR.B.DONE) {
        }

        CFLASH.PFAPR.R = 0x0000FFFF;
        /* Master 1 ( Nexus) Prefetch Enable : prefetching
           Master 0 ( Core) Prefetch Enable : prefetching
           Master 1 ( Nexus) Access Control : Read and Write
           Master 0 ( Core) Access Control : Read and Write */

        CFLASH.PFCR0.R = 0x18C780ED;
        /* Address Pipelining Control : 3 additional hold cycles
           Write Wait State Control :- 3 additional wait states
           Read Wait State Control :- 3 additional wait states
           Data Prefetch Enable :- No Prefetching
           Instruction Prefetch Enable :- Prefetching
           Prefetch Limit :- Prefetch on miss or hit
           Bank0, Port 0 Buffer Enable :- Enabled
           Page Configuration - 3 instruction + 1 data */

        CFLASH.PFCR1.R = 0x18C78081;
        /* Address Pipelining Control : 3 additional hold cycles
           Write Wait State Control :- 3 additional wait states
           Read Wait State control :- 3 additional wait states
           Bank1, Port 0 Buffer Enable :- Enabled */
}

__xwbsp_init_code
void soc_ram_init(void)
{
        __xwcc_unused xwu32_t tmp;
        xwu32_t * src;
        xwu32_t cnt;
        xwu32_t i;
        xwu8_t * rlc_src;
        xwu8_t * rlc_dst;

        /* 初始化备份数据 */
#if defined(SOCCFG_BKUP) && (1 == SOCCFG_BKUP)
        if ((!RGM.DES.R) && ((0x0004 & RGM.FES.R) || (!RGM.FES.R))) {
                /* soft-reset & wake-up */
                src = (xwu32_t *)bkup_vma_base;
                cnt = ((xwu32_t)bkup_vma_end - (xwu32_t)bkup_vma_base) >> 2;
                asm volatile("wrteei 1");
                /* Read all backup SRAM once. If there is an ECC error, it will enter
                   in Data Storage Exception. */
                for (i = 0; i < cnt; i++) {
                        xwmb_read(xwu32_t, tmp, &src[i]);
                }
                asm volatile("wrteei 0");
        } else {
                /* Other reset */
                /* fill zero. */
                src = (xwu32_t *)bkup_mr_origin;
                cnt = (xwu32_t)bkup_mr_size;
                XWOS_BUG_ON(cnt & 3); /* Memory must be aligned 4. */
                cnt >>= 2;
                for (i = 0; i < cnt; i++) {
                        src[i] = 0;
                }

                /* relocate backup data */
                rlc_src = bkup_lma_base;
                if (bkup_vma_base != rlc_src) {
                        rlc_dst = bkup_vma_base;
                        cnt = (xwu32_t)bkup_vma_end - (xwu32_t)bkup_vma_base;
                        for (i = 0; i < cnt; i++) {
                                *rlc_dst = *rlc_src;
                                rlc_dst++;
                                rlc_src++;
                        }
                }
        }
#else /* SOCCFG_BKUP */
        /* fill zero. */
        src = (xwu32_t *)bkup_mr_origin;
        cnt = (xwu32_t)bkup_mr_size;
        XWOS_BUG_ON(cnt & 3); /* Memory must be aligned 4. */
        cnt >>= 2;
        for (i = 0; i < cnt; i++) {
                src[i] = 0;
        }

        /* relocate backup data */
        rlc_src = bkup_lma_base;
        if (bkup_vma_base != rlc_src) {
                rlc_dst = bkup_vma_base;
                cnt = (xwu32_t)bkup_vma_end - (xwu32_t)bkup_vma_base;
                for (i = 0; i < cnt; i++) {
                        *rlc_dst = *rlc_src;
                        rlc_dst++;
                        rlc_src++;
                }
        }
#endif /* !SOCCFG_BKUP */

        /* Init standby mode wakeup entry */
        src = (xwu32_t *)stdby_wkup_mr_origin;
        cnt = (xwu32_t)stdby_wkup_mr_size;
        XWOS_BUG_ON(cnt & 3); /* Zone of memory must be aligned 4. */
        cnt >>= 2;
        for (i = 0; i < cnt; i++) {
                src[i] = 0;
        }

        rlc_src = stdby_wkup_lma_base;
        if (stdby_wkup_vma_base != rlc_src) {
                rlc_dst = stdby_wkup_vma_base;
                cnt = (xwu32_t)stdby_wkup_vma_end - (xwu32_t)stdby_wkup_vma_base;
                for (i = 0; i < cnt; i++) {
                        *rlc_dst = *rlc_src;
                        rlc_dst++;
                        rlc_src++;
                }
        }

        /* Fill 0 to rwx region */
        src = (xwu32_t *)data_mr_origin;
        cnt = (xwu32_t)data_mr_size;
        XWOS_BUG_ON(cnt & 3); /* Zone of memory must be aligned 4. */
        cnt >>= 2;
        for (i = 0; i < cnt; i++) {
                src[i] = 0;
        }
}

__xwbsp_init_code
void soc_me_init(void)
{
        /******** step 1: Peripheral RUN Mode Configuration ********/
        ME.MER.R = 0x000025FF;
        /* STANDBY0 Mode : Enabled
           STOP0 Mode : Enabled
           HALT0 Mode : Enabled
           RUN3 Mode : Enabled
           RUN2 Mode : Enabled
           RUN1 Mode : Enabled
           RUN0 Mode : Enabled
           DRUN Mode : Enabled
           SAFE Mode : Enabled
           TEST Mode : Enabled
           RESET Mode : Enabled */

        ME.TEST.R = 0x001F0010;
        /* I/O Power Down Control: Disabled
           Main Voltage Regulator Control: ON
           Data Flash Power Down Control: Normal
           Code Flash Power Down Control: Normal
           System PLL: OFF
           Crystal Oscillator: OFF
           16MHz IRC: ON
           System Clock Source: 16MHz internal RC oscillator */

        ME.SAFE.R  = 0x009F0010;
        /* I/O Power Down Control: Enabled
           Main Voltage Regulator Control: ON
           Data Flash Power Down Control: Normal
           Code Flash Power Down Control: Normal
           System PLL: OFF
           Crystal Oscillator: OFF
           16MHz IRC: ON
           System Clock Source: 16MHz internal RC oscillator */

        ME.DRUN.R = 0x001F0010;
        /* I/O Power Down Control: Disabled
           Main Voltage Regulator Control: ON
           Data Flash Power Down Control: Normal
           Code Flash Power Down Control: Normal
           System PLL: OFF
           Crystal Oscillator: OFF
           16MHz IRC: ON
           System Clock Source: 16MHz internal RC oscillator */

        ME.RUN[1].R = 0x001F0010;
        /* I/O Power Down Control: Disabled
           Main Voltage Regulator Control: ON
           Data Flash Power Down Control: Normal
           Code Flash Power Down Control: Normal
           System PLL: OFF
           Crystal Oscillator: OFF
           16MHz IRC: ON
           System Clock Source: 16MHz internal RC oscillator */

        ME.RUN[2].R = 0x001F0010;
        /* I/O Power Down Control: Disabled
           Main Voltage Regulator Control: ON
           Data Flash Power Down Control: Normal
           Code Flash Power Down Control: Normal
           System PLL: OFF
           Crystal Oscillator: OFF
           16MHz IRC: ON
           System Clock Source: 16MHz internal RC oscillator */

        ME.RUN[3].R = 0x001F0010;
        /* I/O Power Down Control: Disabled
           Main Voltage Regulator Control: ON
           Data Flash Power Down Control: Normal
           Code Flash Power Down Control: Normal
           System PLL: OFF
           Crystal Oscillator: OFF
           16MHz IRC: ON
           System Clock Source: 16MHz internal RC oscillator */

        ME.HALT0.R = 0x001A0074;
        /* I/O Power Down Control: Disabled
           Main Voltage Regulator Control: ON
           Data Flash Power Down Control: Low Power
           Code Flash Power Down Control: Low Power
           System PLL: ON
           Crystal Oscillator: ON
           16MHz IRC: ON
           System Clock Source: System PLL */

        ME.STOP0.R = 0x00850010;
        /* I/O Power Down Control: Enabled
           Main Voltage Regulator Control: OFF
           Data Flash Power Down Control: Power Down
           Code Flash Power Down Control: Power Down
           System PLL: OFF
           Crystal Oscillator: OFF
           16MHz IRC: ON
           System Clock Source: 16MHz internal RC oscillator */

        ME.STANDBY0.R = 0x0085000F;
        /* I/O Power Down Control: Enabled
           Main Voltage Regulator Control: OFF
           Data Flash Power Down Control: Power Down
           Code Flash Power Down Control: Power Down
           System PLL: OFF
           Crystal Oscillator: OFF
           16MHz IRC: OFF
           System Clock Source: Disable System Clock */

        /******** step 2: Peripheral RUN Mode Configuration ********/
        /* Use RUNPC[0] to freeze the peripherals in RUN mode. */
        ME.RUNPC[0].R = 0x00000000;
        /* Peripheral Operation in RUN3  mode: Disabled
           Peripheral Operation in RUN2  mode: Disabled
           Peripheral Operation in RUN1  mode: Disabled
           Peripheral Operation in RUN0  mode: Disabled
           Peripheral Operation in DRUN  mode: Disabled
           Peripheral Operation in SAFE  mode: Disabled
           Peripheral Operation in TEST  mode: Disabled
           Peripheral Operation in RESET  mode: Disabled */

        /* Use RUNPC[1] to activate the peripherals in RUN mode. */
        ME.RUNPC[1].R = 0x000000F6;
        /* Peripheral Operation in RUN3 mode: Enabled
           Peripheral Operation in RUN2 mode: Enabled
           Peripheral Operation in RUN1 mode: Enabled
           Peripheral Operation in RUN0 mode: Enabled
           Peripheral Operation in DRUN mode: Disabled
           Peripheral Operation in SAFE mode: Enabled
           Peripheral Operation in TEST mode: Enabled
           Peripheral Operation in RESET mode: Disabled */

        /* Use RUNPC[2] to activate the peripherals in all RUN mode. */
        ME.RUNPC[2].R = 0x000000FE;
        /* Peripheral Operation in RUN3  mode: Enabled
           Peripheral Operation in RUN2  mode: Enabled
           Peripheral Operation in RUN1  mode: Enabled
           Peripheral Operation in RUN0  mode: Enabled
           Peripheral Operation in DRUN  mode: Enabled
           Peripheral Operation in SAFE  mode: Enabled
           Peripheral Operation in TEST  mode: Enabled
           Peripheral Operation in RESET  mode: Disabled */

        /* RUNPC[3] ~ RUNPC[7] are not used. */
        ME.RUNPC[3].R = 0x00000008;
        /* Peripheral Operation in RUN3  mode: Disabled
           Peripheral Operation in RUN2  mode: Disabled
           Peripheral Operation in RUN1  mode: Disabled
           Peripheral Operation in RUN0  mode: Disabled
           Peripheral Operation in DRUN  mode: Enabled
           Peripheral Operation in SAFE  mode: Disabled
           Peripheral Operation in TEST  mode: Disabled
           Peripheral Operation in RESET  mode: Disabled */

        ME.RUNPC[4].R = 0x00000008;
        /* Peripheral Operation in RUN3  mode: Disabled
           Peripheral Operation in RUN2  mode: Disabled
           Peripheral Operation in RUN1  mode: Disabled
           Peripheral Operation in RUN0  mode: Disabled
           Peripheral Operation in DRUN  mode: Enabled
           Peripheral Operation in SAFE  mode: Disabled
           Peripheral Operation in TEST  mode: Disabled
           Peripheral Operation in RESET  mode: Disabled */

        ME.RUNPC[5].R = 0x00000008;
        /* Peripheral Operation in RUN3  mode: Disabled
           Peripheral Operation in RUN2  mode: Disabled
           Peripheral Operation in RUN1  mode: Disabled
           Peripheral Operation in RUN0  mode: Disabled
           Peripheral Operation in DRUN  mode: Enabled
           Peripheral Operation in SAFE  mode: Disabled
           Peripheral Operation in TEST  mode: Disabled
           Peripheral Operation in RESET  mode: Disabled */

        ME.RUNPC[6].R = 0x00000008;
        /* Peripheral Operation in RUN3  mode: Disabled
           Peripheral Operation in RUN2  mode: Disabled
           Peripheral Operation in RUN1  mode: Disabled
           Peripheral Operation in RUN0  mode: Disabled
           Peripheral Operation in DRUN  mode: Enabled
           Peripheral Operation in SAFE  mode: Disabled
           Peripheral Operation in TEST  mode: Disabled
           Peripheral Operation in RESET  mode: Disabled */

        ME.RUNPC[7].R = 0x00000008;
        /* Peripheral Operation in RUN3  mode: Disabled
           Peripheral Operation in RUN2  mode: Disabled
           Peripheral Operation in RUN1  mode: Disabled
           Peripheral Operation in RUN0  mode: Disabled
           Peripheral Operation in DRUN  mode: Enabled
           Peripheral Operation in SAFE  mode: Disabled
           Peripheral Operation in TEST  mode: Disabled
           Peripheral Operation in RESET  mode: Disabled */

        /******** step 3: Peripheral Low-Power Mode Configuration ********/
        /* Use LPPC[0] to freeze peripherals in all lowpower modes. */
        ME.LPPC[0].R = 0x00000000;
        /* Peripheral Operation in STANDBY0 mode: Disabled
           Peripheral Operation in STOP0 mode: Disabled
           Peripheral Operation in HALT0 mode: Disabled */

        /* Use LPPC[1] to activate peripherals in HALT0 mode. */
        ME.LPPC[1].R = 0x00000100;
        /* Peripheral Operation in STANDBY0 mode: Disabled
           Peripheral Operation in STOP0 mode: Disabled
           Peripheral Operation in HALT0 mode: Enabled */

        /* Use LPPC[2] to activate peripherals in STOP0 mode. */
        ME.LPPC[2].R = 0x00000400;
        /* Peripheral Operation in STANDBY0 mode: Disabled
           Peripheral Operation in STOP0 mode: Enabled
           Peripheral Operation in HALT0 mode: Disabled */

        /* Use LPPC[3] to activate peripherals in STANDBY0 mode. */
        ME.LPPC[3].R = 0x00002000;
        /* Peripheral Operation in STANDBY0 mode: Enabled
           Peripheral Operation in STOP0 mode: Disabled
           Peripheral Operation in HALT0 mode: Disabled */

        /* Use LPPC[2] to activate peripherals in STOP0 mode & HALT0 mode. */
        ME.LPPC[4].R = 0x00000500;
        /* Peripheral Operation in STANDBY0 mode: Disabled
           Peripheral Operation in STOP0 mode: Enabled
           Peripheral Operation in HALT0 mode: Enabled */

        /* Use LPPC[2] to activate peripherals in all low power modes. */
        ME.LPPC[5].R = 0x00002500;
        /* Peripheral Operation in STANDBY0 mode: Enabled
           Peripheral Operation in STOP0 mode: Enabled
           Peripheral Operation in HALT0 mode: Enabled */

        /* LPPC[6] ~ LPPC[7] are not used. */
        ME.LPPC[6].R = 0x00000000;
        /* Peripheral Operation in STANDBY0 mode: Disabled
           Peripheral Operation in STOP0 mode: Disabled
           Peripheral Operation in HALT0 mode: Disabled */

        ME.LPPC[7].R = 0x00000000;
        /* Peripheral Operation in STANDBY0 mode: Disabled
           Peripheral Operation in STOP0 mode: Disabled
           Peripheral Operation in HALT0 mode: Disabled */

        /******** step 4: Peripheral Mode Control Setup ********/
        /* DSPI_0 Control */
        ME.PCTL[4].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* DSPI_1 Control */
        ME.PCTL[5].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* DSPI_2 Control */
        ME.PCTL[6].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* DSPI_3 Control */
        ME.PCTL[7].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

#if defined(MPC5607B)
        /* DSPI_4 Control */
        ME.PCTL[8].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* DSPI_5 Control */
        ME.PCTL[9].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_8 Control */
        ME.PCTL[12].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_9 Control */
        ME.PCTL[13].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */
#endif /* #if defined(MPC5607B) */

        /* FlexCAN_0 Control */
        ME.PCTL[16].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* FlexCAN_1 Control */
        ME.PCTL[17].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* FlexCAN_2 Control */
        ME.PCTL[18].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* FlexCAN_3 Control */
        ME.PCTL[19].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* FlexCAN_4 Control */
        ME.PCTL[20].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* FlexCAN_5 Control */
        ME.PCTL[21].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* DMA_CH_MUX Control */
        ME.PCTL[23].R = 0x01;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_ACTIVE */

        /* ADC_0 Control */
        ME.PCTL[32].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

#if defined(MPC5607B)
        /* ADC_1 Control */
        ME.PCTL[33].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */
#endif /* #if defined(MPC5607B) */

        /* I2C Control */
        ME.PCTL[44].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_0 Control */
        ME.PCTL[48].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_1 Control */
        ME.PCTL[49].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_2 Control */
        ME.PCTL[50].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_3 Control */
        ME.PCTL[51].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

#if defined(MPC5607B)
        /* LINFlex_4 Control */
        ME.PCTL[52].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_5 Control */
        ME.PCTL[53].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_6 Control */
        ME.PCTL[54].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* LINFlex_7 Control */
        ME.PCTL[55].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */
#endif /* #if defined(MPC5607B) */

        /* CTU Control */
        ME.PCTL[57].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* CANSampler Control */
        ME.PCTL[60].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* SIUL Control */
        ME.PCTL[68].R = 0x2A;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_ACTIVE_ALL_LPM */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_ACTIVE_ALL */

        /* WKPU Control */
        ME.PCTL[69].R = 0x2A;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_ACTIVE_ALL_LPM */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_ACTIVE_ALL */

        /* eMIOS_0 Control */
        ME.PCTL[72].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* eMIOS_1 Control */
        ME.PCTL[73].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* RTC/API Control */
        ME.PCTL[91].R = 0x2A;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_ACTIVE_ALL_LPM */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_ACTIVE_ALL */

        /* PIT Control */
        ME.PCTL[92].R = 0x09;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_ACTIVE_HALT */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_ACTIVE */

        /* CMU Control */
        ME.PCTL[104].R = 0x00;
        /* Peripheral Operation in Debug mode: Disabled */
        /* Low-Power Mode Peripheral Configuration: MPC5_ME_LPPC_FROZEN */
        /* RUN Mode Peripheral Configuration: MPC5_ME_RUNPC_FROZEN */

        /* step 4: Interrupt Setup */
        ME.IM.R = 0x00000000;
        /* Invalid Mode Configuration Interrupt: Disabled */
        /* Invalid Mode Interrupt: Disabled */
        /* SAFE Mode Interrupt: Disabled */
        /* Mode Transition Complete Interrupt: Disabled */

        /******** step 5: setup peripheral clocks ********/
        CGM.SC_DC[0].R = 0x80;
        /* Peripheral Set 1 Clock Divider Status:
           Enabled Peripheral Set 1
           Clock Divider: 1 */

        CGM.SC_DC[1].R = 0x80;
        /* Peripheral Set 2 Clock Divider Status:
           Enabled Peripheral Set 2
           Clock Divider: 1 */

        CGM.SC_DC[2].R = 0x80;
        /* Peripheral Set 3 Clock Divider Status:
           Enabled Peripheral Set 3
           Clock Divider: 1 */

        /******** step 6: Activate the clock Configuration ********/
        ME.MCTL.R = ((xwu32_t)(ME.GS.B.S_CURRENTMODE << 28) | MPC5_ME_MCTL_KEY);
        ME.MCTL.R = ((xwu32_t)(ME.GS.B.S_CURRENTMODE << 28) | MPC5_ME_MCTL_KEY_INV);
}

__xwbsp_init_code
void soc_lowlevel_init(void)
{
        soc_sysclk_init();
        soc_rgm_init();
        soc_flash_init();
        soc_ram_init();
        soc_me_init();
}

#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
static __xwos_init_code
void soc_relocate_isrtable(void)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = xwos_ivt_lma_base;
        dst = xwos_ivt_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)xwos_ivt_vma_end - (xwsz_t)xwos_ivt_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
}
#endif /* !SOCCFG_RO_IVT */

__xwbsp_init_code
void soc_init(void)
{
#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
        soc_relocate_isrtable();
#endif /* !SOCCFG_RO_IVT */

        soc_reset_flags.fes = 0;
        soc_reset_flags.des = 0;

        soc_wkup_flags.nsr = 0;
        soc_wkup_flags.wisr = 0;

        /* clear reset flags */
        if (RGM.DES.R || RGM.FES.R) {
                /* reset event */
                soc_reset_flags.des = RGM.DES.R;
                soc_reset_flags.fes = RGM.FES.R;
                RGM.DES.R = 0x800F;
                RGM.FES.R = 0x81FF;
        }
        if (WKUP.NSR.R || WKUP.WISR.R) {
                /* wkup event */
                soc_wkup_flags.nsr = WKUP.NSR.R;
                soc_wkup_flags.wisr = WKUP.WISR.R;
                WKUP.IRER.R = 0;
                WKUP.WRER.R = 0;
                WKUP.NSR.R = 0xC0000000;
                WKUP.WISR.R = 0x000FFFFF;
        }

        soc_irqc_init();
        xwosplcb_skd_init_lc();
}
