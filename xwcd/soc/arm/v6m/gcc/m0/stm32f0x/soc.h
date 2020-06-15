/**
 * @file
 * @brief SOC
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (defined(SOCCFG_CHIP_STM32F030x6) && (1 == SOCCFG_CHIP_STM32F030x6))
  #ifndef STM32F030x6
    #define STM32F030x6
  #endif
#elif (defined(SOCCFG_CHIP_STM32F030x8) && (1 == SOCCFG_CHIP_STM32F030x8))
  #ifndef STM32F030x8
    #define STM32F030x8
  #endif
#elif (defined(SOCCFG_CHIP_STM32F031x6) && (1 == SOCCFG_CHIP_STM32F031x6))
  #ifndef STM32F031x6
    #define STM32F031x6
  #endif
#elif (defined(SOCCFG_CHIP_STM32F038xx) && (1 == SOCCFG_CHIP_STM32F038xx))
  #ifndef STM32F038xx
    #define STM32F038xx
  #endif
#elif (defined(SOCCFG_CHIP_STM32F042x6) && (1 == SOCCFG_CHIP_STM32F042x6))
  #ifndef STM32F042x6
    #define STM32F042x6
  #endif
#elif (defined(SOCCFG_CHIP_STM32F048x6) && (1 == SOCCFG_CHIP_STM32F048x6))
  #ifndef STM32F048x6
    #define STM32F048x6
  #endif
#elif (defined(SOCCFG_CHIP_STM32F051x8) && (1 == SOCCFG_CHIP_STM32F051x8))
  #ifndef STM32F051x8
    #define STM32F051x8
  #endif
#elif (defined(SOCCFG_CHIP_STM32F058xx) && (1 == SOCCFG_CHIP_STM32F058xx))
  #ifndef STM32F058xx
    #define STM32F058xx
  #endif
#elif (defined(SOCCFG_CHIP_STM32F070x6) && (1 == SOCCFG_CHIP_STM32F070x6))
  #ifndef STM32F070x6
    #define STM32F070x6
  #endif
#elif (defined(SOCCFG_CHIP_STM32F071xB) && (1 == SOCCFG_CHIP_STM32F071xB))
  #ifndef STM32F071xB
    #define STM32F071xB
  #endif
#elif (defined(SOCCFG_CHIP_STM32F072xB) && (1 == SOCCFG_CHIP_STM32F072xB))
  #ifndef STM32F072xB
    #define STM32F072xB
  #endif
#elif (defined(SOCCFG_CHIP_STM32F078xx) && (1 == SOCCFG_CHIP_STM32F078xx))
  #ifndef STM32F078xx
    #define STM32F078xx
  #endif
#elif (defined(SOCCFG_CHIP_STM32F030xC) && (1 == SOCCFG_CHIP_STM32F030xC))
  #ifndef STM32F030xC
    #define STM32F030xC
  #endif
#elif (defined(SOCCFG_CHIP_STM32F091xC) && (1 == SOCCFG_CHIP_STM32F091xC))
  #ifndef STM32F091xC
    #define STM32F091xC
  #endif
#elif (defined(SOCCFG_CHIP_STM32F098xx) && (1 == SOCCFG_CHIP_STM32F098xx))
  #ifndef STM32F098xx
    #define STM32F098xx
  #endif
#else
 #error "unsupport SOC!"
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   register maps   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SOC_FLASH_BASE          ((xwu32_t)0x08000000) /**< FLASH base address in the alias region */
#define SOC_SRAM_BASE           ((xwu32_t)0x20000000) /**< SRAM base address in the alias region */
#define SOC_PERIPH_BASE         ((xwu32_t)0x40000000) /**< Peripheral base address in the alias region */

/*!< Peripheral memory map */
#define SOC_APBPERIPH_BASE      SOC_PERIPH_BASE
#define SOC_AHBPERIPH_BASE      (SOC_PERIPH_BASE + 0x00020000)
#define SOC_AHB2PERIPH_BASE     (SOC_PERIPH_BASE + 0x08000000)

#define SOC_TIM2_BASE           (SOC_APBPERIPH_BASE + 0x00000000)
#define SOC_TIM3_BASE           (SOC_APBPERIPH_BASE + 0x00000400)
#define SOC_TIM6_BASE           (SOC_APBPERIPH_BASE + 0x00001000)
#define SOC_TIM7_BASE           (SOC_APBPERIPH_BASE + 0x00001400)
#define SOC_TIM14_BASE          (SOC_APBPERIPH_BASE + 0x00002000)
#define SOC_RTC_BASE            (SOC_APBPERIPH_BASE + 0x00002800)
#define SOC_WWDG_BASE           (SOC_APBPERIPH_BASE + 0x00002C00)
#define SOC_IWDG_BASE           (SOC_APBPERIPH_BASE + 0x00003000)
#define SOC_SPI2_BASE           (SOC_APBPERIPH_BASE + 0x00003800)
#define SOC_USART2_BASE         (SOC_APBPERIPH_BASE + 0x00004400)
#define SOC_USART3_BASE         (SOC_APBPERIPH_BASE + 0x00004800)
#define SOC_USART4_BASE         (SOC_APBPERIPH_BASE + 0x00004C00)
#define SOC_USART5_BASE         (SOC_APBPERIPH_BASE + 0x00005000)
#define SOC_I2C1_BASE           (SOC_APBPERIPH_BASE + 0x00005400)
#define SOC_I2C2_BASE           (SOC_APBPERIPH_BASE + 0x00005800)
#define SOC_CAN_BASE            (SOC_APBPERIPH_BASE + 0x00006400)
#define SOC_CRS_BASE            (SOC_APBPERIPH_BASE + 0x00006C00)
#define SOC_PWR_BASE            (SOC_APBPERIPH_BASE + 0x00007000)
#define SOC_DAC_BASE            (SOC_APBPERIPH_BASE + 0x00007400)
#define SOC_CEC_BASE            (SOC_APBPERIPH_BASE + 0x00007800)

#define SOC_SYSCFG_BASE         (SOC_APBPERIPH_BASE + 0x00010000)
#define SOC_COMP_BASE           (SOC_APBPERIPH_BASE + 0x0001001C)
#define SOC_EXTI_BASE           (SOC_APBPERIPH_BASE + 0x00010400)
#define SOC_USART6_BASE         (SOC_APBPERIPH_BASE + 0x00011400)
#define SOC_USART7_BASE         (SOC_APBPERIPH_BASE + 0x00011800)
#define SOC_USART8_BASE         (SOC_APBPERIPH_BASE + 0x00011C00)
#define SOC_ADC1_BASE           (SOC_APBPERIPH_BASE + 0x00012400)
#define SOC_ADC_BASE            (SOC_APBPERIPH_BASE + 0x00012708)
#define SOC_TIM1_BASE           (SOC_APBPERIPH_BASE + 0x00012C00)
#define SOC_SPI1_BASE           (SOC_APBPERIPH_BASE + 0x00013000)
#define SOC_USART1_BASE         (SOC_APBPERIPH_BASE + 0x00013800)
#define SOC_TIM15_BASE          (SOC_APBPERIPH_BASE + 0x00014000)
#define SOC_TIM16_BASE          (SOC_APBPERIPH_BASE + 0x00014400)
#define SOC_TIM17_BASE          (SOC_APBPERIPH_BASE + 0x00014800)
#define SOC_DBGMCU_BASE         (SOC_APBPERIPH_BASE + 0x00015800)

#define SOC_DMA1_BASE           (SOC_AHBPERIPH_BASE + 0x00000000)
#define SOC_DMA1_CHANNEL1_BASE  (SOC_DMA1_BASE + 0x00000008)
#define SOC_DMA1_CHANNEL2_BASE  (SOC_DMA1_BASE + 0x0000001C)
#define SOC_DMA1_CHANNEL3_BASE  (SOC_DMA1_BASE + 0x00000030)
#define SOC_DMA1_CHANNEL4_BASE  (SOC_DMA1_BASE + 0x00000044)
#define SOC_DMA1_CHANNEL5_BASE  (SOC_DMA1_BASE + 0x00000058)
#define SOC_DMA1_CHANNEL6_BASE  (SOC_DMA1_BASE + 0x0000006C)
#define SOC_DMA1_CHANNEL7_BASE  (SOC_DMA1_BASE + 0x00000080)
#define SOC_DMA2_BASE           (SOC_AHBPERIPH_BASE + 0x00000400)
#define SOC_DMA2_Channel1_BASE  (SOC_DMA2_BASE + 0x00000008)
#define SOC_DMA2_Channel2_BASE  (SOC_DMA2_BASE + 0x0000001C)
#define SOC_DMA2_Channel3_BASE  (SOC_DMA2_BASE + 0x00000030)
#define SOC_DMA2_Channel4_BASE  (SOC_DMA2_BASE + 0x00000044)
#define SOC_DMA2_Channel5_BASE  (SOC_DMA2_BASE + 0x00000058)

#define SOC_RCC_BASE            (SOC_AHBPERIPH_BASE + 0x00001000)
#define SOC_FLASH_R_BASE        (SOC_AHBPERIPH_BASE + 0x00002000) /**< FLASH registers base address */
#define SOC_OB_BASE             ((xwu32_t)0x1FFFF800) /**< FLASH Option Bytes base address */
#define SOC_FLASHSIZE_BASE      ((xwu32_t)0x1FFFF7CCU) /**< FLASH Size register base address */
#define SOC_UID_BASE            ((xwu32_t)0x1FFFF7ACU) /**< Unique device ID register base address */
#define SOC_CRC_BASE            (SOC_AHBPERIPH_BASE + 0x00003000)
#define SOC_TSC_BASE            (SOC_AHBPERIPH_BASE + 0x00004000)

#define SOC_GPIOA_BASE          (SOC_AHB2PERIPH_BASE + 0x00000000)
#define SOC_GPIOB_BASE          (SOC_AHB2PERIPH_BASE + 0x00000400)
#define SOC_GPIOC_BASE          (SOC_AHB2PERIPH_BASE + 0x00000800)
#define SOC_GPIOD_BASE          (SOC_AHB2PERIPH_BASE + 0x00000C00)
#define SOC_GPIOE_BASE          (SOC_AHB2PERIPH_BASE + 0x00001000)
#define SOC_GPIOF_BASE          (SOC_AHB2PERIPH_BASE + 0x00001400)

/**
 * @defgroup SOC configure mode
 */
#define SOC_CFG_I2C_PA10_FMPLUS         BIT(23)
#define SOC_CFG_I2C_PA9_FMPLUS          BIT(22)
#define SOC_CFG_I2C1_FMPLUS             BIT(20)
#define SOC_CFG_I2C_PB9_FMPLUS          BIT(19)
#define SOC_CFG_I2C_PB8_FMPLUS          BIT(18)
#define SOC_CFG_I2C_PB7_FMPLUS          BIT(17)
#define SOC_CFG_I2C_PB6_FMPLUS          BIT(16)

#define SOC_CFG_TIM17_DMA_RMP_CH1       0
#define SOC_CFG_TIM17_DMA_RMP_CH2       BIT(12)
#define SOC_CFG_TIM16_DMA_RMP_CH3       0
#define SOC_CFG_TIM16_DMA_RMP_CH4       BIT(11)
#define SOC_CFG_USART1_RX_DMA_RMP_CH3   0
#define SOC_CFG_USART1_RX_DMA_RMP_CH5   BIT(10)
#define SOC_CFG_USART1_TX_DMA_RMP_CH2   0
#define SOC_CFG_USART1_TX_DMA_RMP_CH4   BIT(9)
#define SOC_CFG_ADC_DMA_RMP_CH1         0
#define SOC_CFG_ADC_DMA_RMP_CH2         BIT(9)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     registers     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/


#endif /* soc.h */
