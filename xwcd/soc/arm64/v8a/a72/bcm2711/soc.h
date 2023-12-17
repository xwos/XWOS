/**
 * @file
 * @brief SOC描述层：BCM2711寄存器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#ifndef __xwcd_soc_arm_v8a_a72_bcm2711_soc_h__
#define __xwcd_soc_arm_v8a_a72_bcm2711_soc_h__

#include <xwos/standard.h>

#define SOC_LEGACY_PERI_BASE 0x7C000000UL
#if defined(SOCCFG_PERI_HIGH) && (1 == SOCCFG_PERI_HIGH)
#  define SOC_MAIN_PERI_BASE 0x47C000000UL
#  define SOC_ARM_LOCAL_PERI_BASE 0x4C0000000UL
#else
#  define SOC_MAIN_PERI_BASE 0xFC000000UL
#  define SOC_ARM_LOCAL_PERI_BASE 0xFF800000UL
#endif

#define SOC_AUX_REGBASE (SOC_MAIN_PERI_BASE + 0x02215000UL)

#define SOC_BSC0_REGBASE (SOC_MAIN_PERI_BASE + 0x02205000UL)
#define SOC_BSC1_REGBASE (SOC_MAIN_PERI_BASE + 0x02804000UL)
#define SOC_BSC3_REGBASE (SOC_MAIN_PERI_BASE + 0x02205600UL)
#define SOC_BSC4_REGBASE (SOC_MAIN_PERI_BASE + 0x02205800UL)
#define SOC_BSC5_REGBASE (SOC_MAIN_PERI_BASE + 0x02205A80UL)
#define SOC_BSC6_REGBASE (SOC_MAIN_PERI_BASE + 0x02205C00UL)

#define SOC_DMA_REGBASE (SOC_MAIN_PERI_BASE + 0x02007000UL)
#define SOC_DMA0_REGBASE (SOC_DMA_REGBASE + 0x000)
#define SOC_DMA1_REGBASE (SOC_DMA_REGBASE + 0x100)
#define SOC_DMA2_REGBASE (SOC_DMA_REGBASE + 0x200)
#define SOC_DMA3_REGBASE (SOC_DMA_REGBASE + 0x300)
#define SOC_DMA4_REGBASE (SOC_DMA_REGBASE + 0x400)
#define SOC_DMA5_REGBASE (SOC_DMA_REGBASE + 0x500)
#define SOC_DMA6_REGBASE (SOC_DMA_REGBASE + 0x600)
#define SOC_DMA7_REGBASE (SOC_DMA_REGBASE + 0x700)
#define SOC_DMA8_REGBASE (SOC_DMA_REGBASE + 0x800)
#define SOC_DMA9_REGBASE (SOC_DMA_REGBASE + 0x900)
#define SOC_DMA10_REGBASE (SOC_DMA_REGBASE + 0xA00)
#define SOC_DMA11_REGBASE (SOC_DMA_REGBASE + 0xB00)
#define SOC_DMA12_REGBASE (SOC_DMA_REGBASE + 0xC00)
#define SOC_DMA13_REGBASE (SOC_DMA_REGBASE + 0xD00)
#define SOC_DMA14_REGBASE (SOC_DMA_REGBASE + 0xE00)
#define SOC_DMA15_REGBASE (SOC_DMA_REGBASE + 0xF00)

#define SOC_GPIOCM_REGBASE (SOC_MAIN_PERI_BASE + 0x02101000UL)
#define SOC_GPIO_REGBASE (SOC_MAIN_PERI_BASE + 0x02200000UL)

#define SOC_GIC2_REGBASE (SOC_ARM_LOCAL_PERI_BASE + 0x00040000UL)
#define SOC_GIC2_D_OFFSET (0x1000UL)
#define SOC_GIC2_C_OFFSET (0x2000UL)
#define SOC_GIC2_H_OFFSET (0x4000UL)
#define SOC_GIC2_V_OFFSET (0x6000UL)

#define SOC_GIC2_IRQn_OFFSET_ARMC (64UL)
#define SOC_GIC2_IRQn_OFFSET_VC (96UL)
#define SOC_GIC2_IRQn_OFFSET_PCIeL2 (160UL)

#define SOC_ARM_CORE_REGBASE (SOC_MAIN_PERI_BASE + 0x0200B000UL)

#define SOC_PCM_REGBASE (SOC_MAIN_PERI_BASE + 0x02203000UL)

#define SOC_PWM0_REGBASE (SOC_MAIN_PERI_BASE + 0x0220C000UL)
#define SOC_PWM1_REGBASE (SOC_MAIN_PERI_BASE + 0x0220C800UL)

#define SOC_SPI0_REGBASE (SOC_MAIN_PERI_BASE + 0x02204000UL)
#define SOC_SPI3_REGBASE (SOC_MAIN_PERI_BASE + 0x02204600UL)
#define SOC_SPI4_REGBASE (SOC_MAIN_PERI_BASE + 0x02204800UL)
#define SOC_SPI5_REGBASE (SOC_MAIN_PERI_BASE + 0x02204A00UL)
#define SOC_SPI6_REGBASE (SOC_MAIN_PERI_BASE + 0x02204C00UL)

#define SOC_SYST_REGBASE (SOC_MAIN_PERI_BASE + 0x02003000UL)

#define SOC_UART0_REGBASE (SOC_MAIN_PERI_BASE + 0x02201000UL)
#define SOC_UART2_REGBASE (SOC_MAIN_PERI_BASE + 0x02201400UL)
#define SOC_UART3_REGBASE (SOC_MAIN_PERI_BASE + 0x02201600UL)
#define SOC_UART4_REGBASE (SOC_MAIN_PERI_BASE + 0x02201800UL)
#define SOC_UART5_REGBASE (SOC_MAIN_PERI_BASE + 0x02201A00UL)

#define SOC_TIMER_REGBASE (SOC_MAIN_PERI_BASE + 0x0200B000UL)

struct soc_armlocal_regs {
        xwu32_t arm_control; /**< 0x0*/
        xwu32_t reserved0; /**< 0x4 */
        xwu32_t local_prescaler; /**< 0x8 */
        xwu32_t irq_control; /**< 0xC */
};
#define soc_armlocal (*((volatile struct soc_armlocal_regs *)SOC_ARM_LOCAL_PERI_BASE))

struct soc_armc_regs {
        xwu32_t reserved0[128]; /**< 0x0:0x1FC */
        struct {
                xwu32_t pending0; /**< 0x200 */
                xwu32_t pending1; /**< 0x204 */
                xwu32_t pending2; /**< 0x208 */
                xwu32_t reserved0;
                xwu32_t set_en0; /**< 0x210 */
                xwu32_t set_en1; /**< 0x214 */
                xwu32_t set_en2; /**< 0x218 */
                xwu32_t reserved1;
                xwu32_t clr_en0; /**< 0x220 */
                xwu32_t clr_en1; /**< 0x224 */
                xwu32_t clr_en2; /**< 0x228 */
                xwu32_t reserved2;
                xwu32_t status0; /**< 0x230 */
                xwu32_t status1; /**< 0x234 */
                xwu32_t status2; /**< 0x238 */
                xwu32_t reserved3;
        } irq0;
        struct {
                xwu32_t pending0; /**< 0x240 */
                xwu32_t pending1; /**< 0x244 */
                xwu32_t pending2; /**< 0x248 */
                xwu32_t reserved0;
                xwu32_t set_en0; /**< 0x250 */
                xwu32_t set_en1; /**< 0x254 */
                xwu32_t set_en2; /**< 0x258 */
                xwu32_t reserved1;
                xwu32_t clr_en0; /**< 0x260 */
                xwu32_t clr_en1; /**< 0x264 */
                xwu32_t clr_en2; /**< 0x268 */
                xwu32_t reserved2;
                xwu32_t status0; /**< 0x270 */
                xwu32_t status1; /**< 0x274 */
                xwu32_t status2; /**< 0x278 */
                xwu32_t reserved3;
        } irq1;
        struct {
                xwu32_t pending0; /**< 0x280 */
                xwu32_t pending1; /**< 0x284 */
                xwu32_t pending2; /**< 0x288 */
                xwu32_t reserved0;
                xwu32_t set_en0; /**< 0x290 */
                xwu32_t set_en1; /**< 0x294 */
                xwu32_t set_en2; /**< 0x298 */
                xwu32_t reserved1;
                xwu32_t clr_en0; /**< 0x2A0 */
                xwu32_t clr_en1; /**< 0x2A4 */
                xwu32_t clr_en2; /**< 0x2A8 */
                xwu32_t reserved2;
                xwu32_t status0; /**< 0x2B0 */
                xwu32_t status1; /**< 0x2B4 */
                xwu32_t status2; /**< 0x2B8 */
                xwu32_t reserved3;
        } irq2;
        struct {
                xwu32_t pending0; /**< 0x2C0 */
                xwu32_t pending1; /**< 0x2C4 */
                xwu32_t pending2; /**< 0x2C8 */
                xwu32_t reserved0;
                xwu32_t set_en0; /**< 0x2D0 */
                xwu32_t set_en1; /**< 0x2D4 */
                xwu32_t set_en2; /**< 0x2D8 */
                xwu32_t reserved1;
                xwu32_t clr_en0; /**< 0x2E0 */
                xwu32_t clr_en1; /**< 0x2E4 */
                xwu32_t clr_en2; /**< 0x2E8 */
                xwu32_t reserved2;
                xwu32_t status0; /**< 0x2F0 */
                xwu32_t status1; /**< 0x2F4 */
                xwu32_t status2; /**< 0x2F8 */
                xwu32_t reserved3;
        } irq3;
};
#define soc_armc (*((volatile struct soc_armc_regs *)SOC_ARM_CORE_REGBASE))

#endif /* xwcd/soc/arm64/v8a/a72/bcm2711/soc.h */
