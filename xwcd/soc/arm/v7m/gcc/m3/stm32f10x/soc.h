/**
 * @file
 * @brief STM32F10x SOC
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

#ifndef __soc_h__
#define __soc_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/* define macros for STM32 SPL and CubeMX */
#if defined(SOCCFG_CHIP_STM32F100x4) && (1 == SOCCFG_CHIP_STM32F100x4)
  #ifndef STM32F10X_LD_VL
    #define STM32F10X_LD_VL
  #endif
  #ifndef STM32F100x
    #define STM32F100x
  #endif
  #ifndef STM32F100xB
    #define STM32F100xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F100x6) && (1 == SOCCFG_CHIP_STM32F100x6)
  #ifndef STM32F10X_LD_VL
    #define STM32F10X_LD_VL
  #endif
  #ifndef STM32F100x
    #define STM32F100x
  #endif
  #ifndef STM32F100xB
    #define STM32F100xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F100x8) && (1 == SOCCFG_CHIP_STM32F100x8)
  #ifndef STM32F10X_MD_VL
    #define STM32F10X_MD_VL
  #endif
  #ifndef STM32F100x
    #define STM32F100x
  #endif
  #ifndef STM32F100xB
    #define STM32F100xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F100xB) && (1 == SOCCFG_CHIP_STM32F100xB)
  #ifndef STM32F10X_MD_VL
    #define STM32F10X_MD_VL
  #endif
  #ifndef STM32F100x
    #define STM32F100x
  #endif
  #ifndef STM32F100xB
    #define STM32F100xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F100xC) && (1 == SOCCFG_CHIP_STM32F100xC)
  #ifndef STM32F10X_HD_VL
    #define STM32F10X_HD_VL
  #endif
  #ifndef STM32F100x
    #define STM32F100x
  #endif
  #ifndef STM32F100xE
    #define STM32F100xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F100xD) && (1 == SOCCFG_CHIP_STM32F100xD)
  #ifndef STM32F10X_HD_VL
    #define STM32F10X_HD_VL
  #endif
  #ifndef STM32F100x
    #define STM32F100x
  #endif
  #ifndef STM32F100xE
    #define STM32F100xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F100xE) && (1 == SOCCFG_CHIP_STM32F100xE)
  #ifndef STM32F10X_HD_VL
    #define STM32F10X_HD_VL
  #endif
  #ifndef STM32F100x
    #define STM32F100x
  #endif
  #ifndef STM32F100xE
    #define STM32F100xE
  #endif

#elif defined(SOCCFG_CHIP_STM32F101x4) && (1 == SOCCFG_CHIP_STM32F101x4)
  #ifndef STM32F10X_LD
    #define STM32F10X_LD
  #endif
  #ifndef STM32F101x
    #define STM32F101x
  #endif
  #ifndef STM32F101x6
    #define STM32F101x6
  #endif
#elif defined(SOCCFG_CHIP_STM32F101x6) && (1 == SOCCFG_CHIP_STM32F101x6)
  #ifndef STM32F10X_LD
    #define STM32F10X_LD
  #endif
  #ifndef STM32F101x
    #define STM32F101x
  #endif
  #ifndef STM32F101x6
    #define STM32F101x6
  #endif
#elif defined(SOCCFG_CHIP_STM32F101x8) && (1 == SOCCFG_CHIP_STM32F101x8)
  #ifndef STM32F10X_MD
    #define STM32F10X_MD
  #endif
  #ifndef STM32F101x
    #define STM32F101x
  #endif
  #ifndef STM32F101xB
    #define STM32F101xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F101xB) && (1 == SOCCFG_CHIP_STM32F101xB)
  #ifndef STM32F10X_MD
    #define STM32F10X_MD
  #endif
  #ifndef STM32F101x
    #define STM32F101x
  #endif
  #ifndef STM32F101xB
    #define STM32F101xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F101xC) && (1 == SOCCFG_CHIP_STM32F101xC)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F101xE
    #define STM32F101xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F101xD) && (1 == SOCCFG_CHIP_STM32F101xD)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F101x
    #define STM32F101x
  #endif
  #ifndef STM32F101xE
    #define STM32F101xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F101xE) && (1 == SOCCFG_CHIP_STM32F101xE)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F101x
    #define STM32F101x
  #endif
  #ifndef STM32F101xE
    #define STM32F101xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F101xF) && (1 == SOCCFG_CHIP_STM32F101xF)
  #ifndef STM32F10X_XL
    #define STM32F10X_XL
  #endif
  #ifndef STM32F101x
    #define STM32F101x
  #endif
  #ifndef STM32F101xG
    #define STM32F101xG
  #endif
#elif defined(SOCCFG_CHIP_STM32F101xG) && (1 == SOCCFG_CHIP_STM32F101xG)
  #ifndef STM32F10X_XL
    #define STM32F10X_XL
  #endif
  #ifndef STM32F101x
    #define STM32F101x
  #endif
  #ifndef STM32F101xG
    #define STM32F101xG
  #endif

#elif defined(SOCCFG_CHIP_STM32F102x4) && (1 == SOCCFG_CHIP_STM32F102x4)
  #ifndef STM32F10X_LD
    #define STM32F10X_LD
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102x6
    #define STM32F102x6
  #endif
#elif defined(SOCCFG_CHIP_STM32F102x6) && (1 == SOCCFG_CHIP_STM32F102x6)
  #ifndef STM32F10X_LD
    #define STM32F10X_LD
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102x6
    #define STM32F102x6
  #endif
#elif defined(SOCCFG_CHIP_STM32F102x8) && (1 == SOCCFG_CHIP_STM32F102x8)
  #ifndef STM32F10X_MD
    #define STM32F10X_MD
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102xB
    #define STM32F102xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F102xB) && (1 == SOCCFG_CHIP_STM32F102xB)
  #ifndef STM32F10X_MD
    #define STM32F10X_MD
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102xB
    #define STM32F102xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F102xC) && (1 == SOCCFG_CHIP_STM32F102xC)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102xE
    #define STM32F102xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F102xD) && (1 == SOCCFG_CHIP_STM32F102xD)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102xE
    #define STM32F102xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F102xE) && (1 == SOCCFG_CHIP_STM32F102xE)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102xE
    #define STM32F102xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F102xF) && (1 == SOCCFG_CHIP_STM32F102xF)
  #ifndef STM32F10X_XL
    #define STM32F10X_XL
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102xG
    #define STM32F102xG
  #endif
#elif defined(SOCCFG_CHIP_STM32F102xG) && (1 == SOCCFG_CHIP_STM32F102xG)
  #ifndef STM32F10X_XL
    #define STM32F10X_XL
  #endif
  #ifndef STM32F102x
    #define STM32F102x
  #endif
  #ifndef STM32F102xG
    #define STM32F102xG
  #endif

#elif defined(SOCCFG_CHIP_STM32F103x4) && (1 == SOCCFG_CHIP_STM32F103x4)
  #ifndef STM32F10X_LD
    #define STM32F10X_LD
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103x6
    #define STM32F103x6
  #endif
#elif defined(SOCCFG_CHIP_STM32F103x6) && (1 == SOCCFG_CHIP_STM32F103x6)
  #ifndef STM32F10X_LD
    #define STM32F10X_LD
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103x6
    #define STM32F103x6
  #endif
#elif defined(SOCCFG_CHIP_STM32F103x8) && (1 == SOCCFG_CHIP_STM32F103x8)
  #ifndef STM32F10X_MD
    #define STM32F10X_MD
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103xB
    #define STM32F103xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F103xB) && (1 == SOCCFG_CHIP_STM32F103xB)
  #ifndef STM32F10X_MD
    #define STM32F10X_MD
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103xB
    #define STM32F103xB
  #endif
#elif defined(SOCCFG_CHIP_STM32F103xC) && (1 == SOCCFG_CHIP_STM32F103xC)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103xE
    #define STM32F103xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F103xD) && (1 == SOCCFG_CHIP_STM32F103xD)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103xE
    #define STM32F103xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F103xE) && (1 == SOCCFG_CHIP_STM32F103xE)
  #ifndef STM32F10X_HD
    #define STM32F10X_HD
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103xE
    #define STM32F103xE
  #endif
#elif defined(SOCCFG_CHIP_STM32F103xF) && (1 == SOCCFG_CHIP_STM32F103xF)
  #ifndef STM32F10X_XL
    #define STM32F10X_XL
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103xG
    #define STM32F103xG
  #endif
#elif defined(SOCCFG_CHIP_STM32F103xG) && (1 == SOCCFG_CHIP_STM32F103xG)
  #ifndef STM32F10X_XL
    #define STM32F10X_XL
  #endif
  #ifndef STM32F103x
    #define STM32F103x
  #endif
  #ifndef STM32F103xG
    #define STM32F103xG
  #endif
#elif defined(SOCCFG_CHIP_STM32F105x) && (1 == SOCCFG_CHIP_STM32F105x)
  #ifndef STM32F10X_CL
    #define STM32F10X_CL
  #endif
  #ifndef STM32F105xC
    #define STM32F105xC
  #endif
#elif defined(SOCCFG_CHIP_STM32F107x) && (1 == SOCCFG_CHIP_STM32F107x)
  #ifndef STM32F10X_CL
    #define STM32F10X_CL
  #endif
  #ifndef STM32F107xC
    #define STM32F107xC
  #endif
#else
  #error "Please select first the target STM32F10x device used in your application."
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     registers     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/* peripheral memory map */
#define SOC_FLASH_BASE          (0x08000000U) /**< FLASH base address in the alias region */
#define SOC_SRAM_BASE           (0x20000000U) /**< SRAM base address in the alias region */
#define SOC_PERIPH_BASE         (0x40000000U) /**< Peripheral base address in the alias region */
#define SOC_SRAM_BB_BASE        (0x22000000U) /**< SRAM base address in the bit-band region */
#define SOC_PERIPH_BB_BASE      (0x42000000U) /**< Peripheral base address in the bit-band region */
#define SOC_FSMC_R_BASE         (0xA0000000U) /**< FSMC registers base address */

/* Peripheral memory map */
#define SOC_APB1PERIPH_BASE     SOC_PERIPH_BASE
#define SOC_APB2PERIPH_BASE     (SOC_PERIPH_BASE + 0x10000)
#define SOC_AHBPERIPH_BASE      (SOC_PERIPH_BASE + 0x20000)

#define SOC_TIM2_BASE           (SOC_APB1PERIPH_BASE + 0x0000)
#define SOC_TIM3_BASE           (SOC_APB1PERIPH_BASE + 0x0400)
#define SOC_TIM4_BASE           (SOC_APB1PERIPH_BASE + 0x0800)
#define SOC_TIM5_BASE           (SOC_APB1PERIPH_BASE + 0x0C00)
#define SOC_TIM6_BASE           (SOC_APB1PERIPH_BASE + 0x1000)
#define SOC_TIM7_BASE           (SOC_APB1PERIPH_BASE + 0x1400)
#define SOC_TIM12_BASE          (SOC_APB1PERIPH_BASE + 0x1800)
#define SOC_TIM13_BASE          (SOC_APB1PERIPH_BASE + 0x1C00)
#define SOC_TIM14_BASE          (SOC_APB1PERIPH_BASE + 0x2000)
#define SOC_RTC_BASE            (SOC_APB1PERIPH_BASE + 0x2800)
#define SOC_WWDG_BASE           (SOC_APB1PERIPH_BASE + 0x2C00)
#define SOC_IWDG_BASE           (SOC_APB1PERIPH_BASE + 0x3000)
#define SOC_SPI2_BASE           (SOC_APB1PERIPH_BASE + 0x3800)
#define SOC_SPI3_BASE           (SOC_APB1PERIPH_BASE + 0x3C00)
#define SOC_USART2_BASE         (SOC_APB1PERIPH_BASE + 0x4400)
#define SOC_USART3_BASE         (SOC_APB1PERIPH_BASE + 0x4800)
#define SOC_UART4_BASE          (SOC_APB1PERIPH_BASE + 0x4C00)
#define SOC_UART5_BASE          (SOC_APB1PERIPH_BASE + 0x5000)
#define SOC_I2C1_BASE           (SOC_APB1PERIPH_BASE + 0x5400)
#define SOC_I2C2_BASE           (SOC_APB1PERIPH_BASE + 0x5800)
#define SOC_CANC1_BASE          (SOC_APB1PERIPH_BASE + 0x6400)
#define SOC_CANC2_BASE          (SOC_APB1PERIPH_BASE + 0x6800)
#define SOC_BKP_BASE            (SOC_APB1PERIPH_BASE + 0x6C00)
#define SOC_PWR_BASE            (SOC_APB1PERIPH_BASE + 0x7000)
#define SOC_DAC_BASE            (SOC_APB1PERIPH_BASE + 0x7400)
#define SOC_CEC_BASE            (SOC_APB1PERIPH_BASE + 0x7800)

#define SOC_AFIO_BASE           (SOC_APB2PERIPH_BASE + 0x0000)
#define SOC_EXTI_BASE           (SOC_APB2PERIPH_BASE + 0x0400)
#define SOC_GPIOA_BASE          (SOC_APB2PERIPH_BASE + 0x0800)
#define SOC_GPIOB_BASE          (SOC_APB2PERIPH_BASE + 0x0C00)
#define SOC_GPIOC_BASE          (SOC_APB2PERIPH_BASE + 0x1000)
#define SOC_GPIOD_BASE          (SOC_APB2PERIPH_BASE + 0x1400)
#define SOC_GPIOE_BASE          (SOC_APB2PERIPH_BASE + 0x1800)
#define SOC_GPIOF_BASE          (SOC_APB2PERIPH_BASE + 0x1C00)
#define SOC_GPIOG_BASE          (SOC_APB2PERIPH_BASE + 0x2000)
#define SOC_ADC1_BASE           (SOC_APB2PERIPH_BASE + 0x2400)
#define SOC_ADC2_BASE           (SOC_APB2PERIPH_BASE + 0x2800)
#define SOC_TIM1_BASE           (SOC_APB2PERIPH_BASE + 0x2C00)
#define SOC_SPI1_BASE           (SOC_APB2PERIPH_BASE + 0x3000)
#define SOC_TIM8_BASE           (SOC_APB2PERIPH_BASE + 0x3400)
#define SOC_USART1_BASE         (SOC_APB2PERIPH_BASE + 0x3800)
#define SOC_ADC3_BASE           (SOC_APB2PERIPH_BASE + 0x3C00)
#define SOC_TIM15_BASE          (SOC_APB2PERIPH_BASE + 0x4000)
#define SOC_TIM16_BASE          (SOC_APB2PERIPH_BASE + 0x4400)
#define SOC_TIM17_BASE          (SOC_APB2PERIPH_BASE + 0x4800)
#define SOC_TIM9_BASE           (SOC_APB2PERIPH_BASE + 0x4C00)
#define SOC_TIM10_BASE          (SOC_APB2PERIPH_BASE + 0x5000)
#define SOC_TIM11_BASE          (SOC_APB2PERIPH_BASE + 0x5400)

#define SOC_SDIO_BASE           (SOC_PERIPH_BASE + 0x18000)

#define SOC_DMA1_BASE           (SOC_AHBPERIPH_BASE + 0x0000)
#define SOC_DMA1_CHANNEL1_BASE  (SOC_AHBPERIPH_BASE + 0x0008)
#define SOC_DMA1_CHANNEL2_BASE  (SOC_AHBPERIPH_BASE + 0x001C)
#define SOC_DMA1_CHANNEL3_BASE  (SOC_AHBPERIPH_BASE + 0x0030)
#define SOC_DMA1_CHANNEL4_BASE  (SOC_AHBPERIPH_BASE + 0x0044)
#define SOC_DMA1_CHANNEL5_BASE  (SOC_AHBPERIPH_BASE + 0x0058)
#define SOC_DMA1_CHANNEL6_BASE  (SOC_AHBPERIPH_BASE + 0x006C)
#define SOC_DMA1_CHANNEL7_BASE  (SOC_AHBPERIPH_BASE + 0x0080)
#define SOC_DMA2_BASE           (SOC_AHBPERIPH_BASE + 0x0400)
#define SOC_DMA2_CHANNEL1_BASE  (SOC_AHBPERIPH_BASE + 0x0408)
#define SOC_DMA2_CHANNEL2_BASE  (SOC_AHBPERIPH_BASE + 0x041C)
#define SOC_DMA2_CHANNEL3_BASE  (SOC_AHBPERIPH_BASE + 0x0430)
#define SOC_DMA2_CHANNEL4_BASE  (SOC_AHBPERIPH_BASE + 0x0444)
#define SOC_DMA2_CHANNEL5_BASE  (SOC_AHBPERIPH_BASE + 0x0458)
#define SOC_RCC_BASE            (SOC_AHBPERIPH_BASE + 0x1000)
#define SOC_CRC_BASE            (SOC_AHBPERIPH_BASE + 0x3000)

#define SOC_FLASH_R_BASE        (SOC_AHBPERIPH_BASE + 0x2000) /**< Flash registers */
#define SOC_FLASHSIZE_BASE      (0x1FFFF7E0U) /**< FLASH Size register base address */
#define SOC_UID_BASE            (0x1FFFF7E8U) /**< Unique device ID register base address */
#define SOC_OB_BASE             (0x1FFFF800U) /**< Flash Option Bytes */

#define SOC_ETH_BASE            (SOC_AHBPERIPH_BASE + 0x8000)
#define SOC_ETH_MAC_BASE        (SOC_ETH_BASE)
#define SOC_ETH_MMC_BASE        (SOC_ETH_BASE + 0x0100)
#define SOC_ETH_PTP_BASE        (SOC_ETH_BASE + 0x0700)
#define SOC_ETH_DMA_BASE        (SOC_ETH_BASE + 0x1000)

#define SOC_FSMC_BANK1_R_BASE   (SOC_FSMC_R_BASE + 0x0000) /**< FSMC Bank1 registers */
#define SOC_FSMC_BANK1E_R_BASE  (SOC_FSMC_R_BASE + 0x0104) /**< FSMC Bank1E registers */
#define SOC_FSMC_Bank2_R_BASE   (SOC_FSMC_R_BASE + 0x0060) /**< FSMC Bank2 registers */
#define SOC_FSMC_BANK3_R_BASE   (SOC_FSMC_R_BASE + 0x0080) /**< FSMC Bank3 registers */
#define SOC_FSMC_BANK4_R_BASE   (SOC_FSMC_R_BASE + 0x00A0) /**< FSMC Bank4 registers */

#define SOC_DBGMCU_BASE         (0xE0042000U) /**< Debug MCU registers */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     registers     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* soc.h */
