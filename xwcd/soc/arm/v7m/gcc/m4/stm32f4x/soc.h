/**
 * @file
 * @brief STM32F4x SOC
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
#if defined(SOCCFG_CHIP_STM32F405xx) && (1 == SOCCFG_CHIP_STM32F405xx)
  #ifndef STM32F405xx
    #define STM32F405xx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F415xx) && (1 == SOCCFG_CHIP_STM32F415xx)
  #ifndef STM32F415xx
    #define STM32F415xx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F407xx) && (1 == SOCCFG_CHIP_STM32F407xx)
  #ifndef STM32F407xx
    #define STM32F407xx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F417xx) && (1 == SOCCFG_CHIP_STM32F417xx)
  #ifndef STM32F417xx
    #define STM32F417xx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F427xx) && (1 == SOCCFG_CHIP_STM32F427xx)
  #ifndef STM32F427xx
    #define STM32F427xx
  #endif
  #ifndef STM32F427_437xx
    #define STM32F427_437xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F437xx) && (1 == SOCCFG_CHIP_STM32F437xx)
  #ifndef STM32F437xx
    #define STM32F437xx
  #endif
  #ifndef STM32F427_437xx
    #define STM32F427_437xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F429xx) && (1 == SOCCFG_CHIP_STM32F429xx)
  #ifndef STM32F429xx
    #define STM32F429xx
  #endif
  #ifndef STM32F429_439xx
    #define STM32F429_439xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F439xx) && (1 == SOCCFG_CHIP_STM32F439xx)
  #ifndef STM32F439xx
    #define STM32F439xx
  #endif
  #ifndef STM32F429_439xx
    #define STM32F429_439xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F401xC) && (1 == SOCCFG_CHIP_STM32F401xC)
  #ifndef STM32F401xC
    #define STM32F401xC
  #endif
  #ifndef STM32F401xx
    #define STM32F401xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F401xE) && (1 == SOCCFG_CHIP_STM32F401xE)
  #ifndef STM32F401xE
    #define STM32F401xE
  #endif
  #ifndef STM32F401xx
    #define STM32F401xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F410Tx) && (1 == SOCCFG_CHIP_STM32F410Tx)
  #ifndef STM32F410Tx
    #define STM32F410Tx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F410Cx) && (1 == SOCCFG_CHIP_STM32F410Cx)
  #ifndef STM32F410Cx
    #define STM32F410Cx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F410Rx) && (1 == SOCCFG_CHIP_STM32F410Rx)
  #ifndef STM32F410Rx
    #define STM32F410Rx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F411xE) && (1 == SOCCFG_CHIP_STM32F411xE)
  #ifndef STM32F411xE
    #define STM32F411xE
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F446xx) && (1 == SOCCFG_CHIP_STM32F446xx)
  #ifndef STM32F446xx
    #define STM32F446xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F469xx) && (1 == SOCCFG_CHIP_STM32F469xx)
  #ifndef STM32F469xx
    #define STM32F469xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F479xx) && (1 == SOCCFG_CHIP_STM32F479xx)
  #ifndef STM32F479xx
    #define STM32F479xx
  #endif
#elif defined(SOCCFG_CHIP_STM32F412Cx) && (1 == SOCCFG_CHIP_STM32F412Cx)
  #ifndef STM32F412Cx
    #define STM32F412Cx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F412Zx) && (1 == SOCCFG_CHIP_STM32F412Zx)
  #ifndef STM32F412Zx
    #define STM32F412Zx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F412Vx) && (1 == SOCCFG_CHIP_STM32F412Vx)
  #ifndef STM32F412Vx
    #define STM32F412Vx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F412Rx) && (1 == SOCCFG_CHIP_STM32F412Rx)
  #ifndef STM32F412Rx
    #define STM32F412Rx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F413xx) && (1 == SOCCFG_CHIP_STM32F413xx)
  #ifndef STM32F413xx
    #define STM32F413xx
  #endif
  #ifndef STM32F40_41xxx
    #define STM32F40_41xxx
  #endif
#elif defined(SOCCFG_CHIP_STM32F423xx) && (1 == SOCCFG_CHIP_STM32F423xx)
  #ifndef STM32F423xx
    #define STM32F423xx
  #endif
  #ifndef STM32F427_437xx
    #define STM32F427_437xx
  #endif
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   register maps   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/* peripheral memory map */
#define SOC_FLASH_BASE          (0x08000000UL) /**< FLASH base address in the alias region */
#define SOC_CCMRAM_BASE         (0x10000000UL) /**< CCM(core coupled memory) RAM base address in the alias region */
#define SOC_SRAM1_BASE          (0x20000000UL) /**< SRAM1(112 KB) base address in the alias region */
#define SOC_SRAM2_BASE          (0x2001C000UL) /**< SRAM2(16 KB) base address in the alias region */
#define SOC_SRAM3_BASE          (0x20030000UL) /**< SRAM3(128 KB) base address in the alias region */
#define SOC_PERIPH_BASE         (0x40000000UL) /**< Peripheral base address in the alias region */
#define SOC_BKPSRAM_BASE        (0x40024000UL) /**< Backup SRAM(4 KB) base address in the alias region */
#define SOC_FSMC_R_BASE         (0xA0000000UL) /**< FSMC registers base address */
#define SOC_FMC_R_BASE          (0xA0000000UL) /**< FMC registers base address */
#define SOC_QSPI_R_BASE         (0xA0001000UL) /**< QuadSPI registers base address */
#define SOC_SRAM1_BB_BASE       (0x22000000UL) /**< SRAM1(112 KB) base address in the bit-band region */
#define SOC_SRAM2_BB_BASE       (0x22500000UL) /**< SRAM2(16 KB) base address in the bit-band region */
#define SOC_SRAM3_BB_BASE       (0x22600000UL) /**< SRAM3(64 KB) base address in the bit-band region */
#define SOC_PERIPH_BB_BASE      (0x42000000UL) /**< Peripheral base address in the bit-band region */
#define SOC_BKPSRAM_BB_BASE     (0x42480000UL) /**< Backup SRAM(4 KB) base address in the bit-band region */
#define SOC_FLASH_END           (0x081FFFFFUL) /**< FLASH end address */
#define SOC_FLASH_OTP_BASE      (0x1FFF7800UL) /**< Base address of: (up to 528 Bytes) embedded FLASH OTP Area */
#define SOC_FLASH_OTP_END       (0x1FFF7A0FUL) /**< End address of: (up to 528 Bytes) embedded FLASH OTP Area */
#define SOC_CCMRAM_END          (0x1000FFFFUL) /**< CCM RAM end address */

/* Legacy defines */
#define SOC_SRAM_BASE           SOC_SRAM1_BASE
#define SOC_SRAM_BB_BASE        SOC_SRAM1_BB_BASE

/* Peripheral memory map */
#define SOC_APB1PERIPH_BASE     SOC_PERIPH_BASE
#define SOC_APB2PERIPH_BASE     (SOC_PERIPH_BASE + 0x00010000)
#define SOC_AHB1PERIPH_BASE     (SOC_PERIPH_BASE + 0x00020000)
#define SOC_AHB2PERIPH_BASE     (SOC_PERIPH_BASE + 0x10000000)

/* APB1 peripherals */
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
#define SOC_I2S2EXT_BASE        (SOC_APB1PERIPH_BASE + 0x3400)
#define SOC_SPI2_BASE           (SOC_APB1PERIPH_BASE + 0x3800)
#define SOC_SPI3_BASE           (SOC_APB1PERIPH_BASE + 0x3C00)
#define SOC_SPDIFRX_BASE        (SOC_APB1PERIPH_BASE + 0x4000)
#define SOC_I2S3EXT_BASE        (SOC_APB1PERIPH_BASE + 0x4000)
#define SOC_USART2_BASE         (SOC_APB1PERIPH_BASE + 0x4400)
#define SOC_USART3_BASE         (SOC_APB1PERIPH_BASE + 0x4800)
#define SOC_UART4_BASE          (SOC_APB1PERIPH_BASE + 0x4C00)
#define SOC_UART5_BASE          (SOC_APB1PERIPH_BASE + 0x5000)
#define SOC_I2C1_BASE           (SOC_APB1PERIPH_BASE + 0x5400)
#define SOC_I2C2_BASE           (SOC_APB1PERIPH_BASE + 0x5800)
#define SOC_I2C3_BASE           (SOC_APB1PERIPH_BASE + 0x5C00)
#define SOC_FMPI2C1_BASE        (SOC_APB1PERIPH_BASE + 0x6000)
#define SOC_CANC1_BASE          (SOC_APB1PERIPH_BASE + 0x6400)
#define SOC_CANC2_BASE          (SOC_APB1PERIPH_BASE + 0x6800)
#define SOC_CEC_BASE            (SOC_APB1PERIPH_BASE + 0x6C00)
#define SOC_PWR_BASE            (SOC_APB1PERIPH_BASE + 0x7000)
#define SOC_DAC_BASE            (SOC_APB1PERIPH_BASE + 0x7400)
#define SOC_UART7_BASE          (SOC_APB1PERIPH_BASE + 0x7800)
#define SOC_UART8_BASE          (SOC_APB1PERIPH_BASE + 0x7C00)

/* APB2 peripherals */
#define SOC_TIM1_BASE           (SOC_APB2PERIPH_BASE + 0x0000)
#define SOC_TIM8_BASE           (SOC_APB2PERIPH_BASE + 0x0400)
#define SOC_USART1_BASE         (SOC_APB2PERIPH_BASE + 0x1000)
#define SOC_USART6_BASE         (SOC_APB2PERIPH_BASE + 0x1400)
#define SOC_ADC1_BASE           (SOC_APB2PERIPH_BASE + 0x2000)
#define SOC_ADC2_BASE           (SOC_APB2PERIPH_BASE + 0x2100)
#define SOC_ADC3_BASE           (SOC_APB2PERIPH_BASE + 0x2200)
#define SOC_ADC_COMMON_BASE     (SOC_APB2PERIPH_BASE + 0x2300)
#define SOC_SDIO_BASE           (SOC_APB2PERIPH_BASE + 0x2C00)
#define SOC_SPI1_BASE           (SOC_APB2PERIPH_BASE + 0x3000)
#define SOC_SPI4_BASE           (SOC_APB2PERIPH_BASE + 0x3400)
#define SOC_SYSCFG_BASE         (SOC_APB2PERIPH_BASE + 0x3800)
#define SOC_EXTI_BASE           (SOC_APB2PERIPH_BASE + 0x3C00)
#define SOC_TIM9_BASE           (SOC_APB2PERIPH_BASE + 0x4000)
#define SOC_TIM10_BASE          (SOC_APB2PERIPH_BASE + 0x4400)
#define SOC_TIM11_BASE          (SOC_APB2PERIPH_BASE + 0x4800)
#define SOC_SPI5_BASE           (SOC_APB2PERIPH_BASE + 0x5000)
#define SOC_SPI6_BASE           (SOC_APB2PERIPH_BASE + 0x5400)
#define SOC_SAI1_BASE           (SOC_APB2PERIPH_BASE + 0x5800)
#define SOC_SAI1_BLOCK_A_BASE   (SOC_SAI1_BASE + 0x004)
#define SOC_SAI1_BLOCK_B_BASE   (SOC_SAI1_BASE + 0x024)
#define SOC_SAI2_BASE           (SOC_APB2PERIPH_BASE + 0x5C00)
#define SOC_SAI2_BLOCK_A_BASE   (SOC_SAI2_BASE + 0x004)
#define SOC_SAI2_BLOCK_B_BASE   (SOC_SAI2_BASE + 0x024)
#define SOC_LTDC_BASE           (SOC_APB2PERIPH_BASE + 0x6800)
#define SOC_LTDC_LAYER1_BASE    (SOC_LTDC_BASE + 0x84)
#define SOC_LTDC_LAYER2_BASE    (SOC_LTDC_BASE + 0x104)
#define SOC_DSI_BASE            (SOC_APB2PERIPH_BASE + 0x6C00U)

/* AHB1 peripherals */
#define SOC_GPIOA_BASE          (SOC_AHB1PERIPH_BASE + 0x0000)
#define SOC_GPIOB_BASE          (SOC_AHB1PERIPH_BASE + 0x0400)
#define SOC_GPIOC_BASE          (SOC_AHB1PERIPH_BASE + 0x0800)
#define SOC_GPIOD_BASE          (SOC_AHB1PERIPH_BASE + 0x0C00)
#define SOC_GPIOE_BASE          (SOC_AHB1PERIPH_BASE + 0x1000)
#define SOC_GPIOF_BASE          (SOC_AHB1PERIPH_BASE + 0x1400)
#define SOC_GPIOG_BASE          (SOC_AHB1PERIPH_BASE + 0x1800)
#define SOC_GPIOH_BASE          (SOC_AHB1PERIPH_BASE + 0x1C00)
#define SOC_GPIOI_BASE          (SOC_AHB1PERIPH_BASE + 0x2000)
#define SOC_GPIOJ_BASE          (SOC_AHB1PERIPH_BASE + 0x2400)
#define SOC_GPIOK_BASE          (SOC_AHB1PERIPH_BASE + 0x2800)
#define SOC_CRC_BASE            (SOC_AHB1PERIPH_BASE + 0x3000)
#define SOC_RCC_BASE            (SOC_AHB1PERIPH_BASE + 0x3800)
#define SOC_FLASH_R_BASE        (SOC_AHB1PERIPH_BASE + 0x3C00)
#define SOC_DMA1_BASE           (SOC_AHB1PERIPH_BASE + 0x6000)
#define SOC_DMA1_STREAM0_BASE   (SOC_DMA1_BASE + 0x010)
#define SOC_DMA1_STREAM1_BASE   (SOC_DMA1_BASE + 0x028)
#define SOC_DMA1_STREAM2_BASE   (SOC_DMA1_BASE + 0x040)
#define SOC_DMA1_STREAM3_BASE   (SOC_DMA1_BASE + 0x058)
#define SOC_DMA1_STREAM4_BASE   (SOC_DMA1_BASE + 0x070)
#define SOC_DMA1_STREAM5_BASE   (SOC_DMA1_BASE + 0x088)
#define SOC_DMA1_STREAM6_BASE   (SOC_DMA1_BASE + 0x0A0)
#define SOC_DMA1_STREAM7_BASE   (SOC_DMA1_BASE + 0x0B8)
#define SOC_DMA2_BASE           (SOC_AHB1PERIPH_BASE + 0x6400)
#define SOC_DMA2_STREAM0_BASE   (SOC_DMA2_BASE + 0x010)
#define SOC_DMA2_STREAM1_BASE   (SOC_DMA2_BASE + 0x028)
#define SOC_DMA2_STREAM2_BASE   (SOC_DMA2_BASE + 0x040)
#define SOC_DMA2_STREAM3_BASE   (SOC_DMA2_BASE + 0x058)
#define SOC_DMA2_STREAM4_BASE   (SOC_DMA2_BASE + 0x070)
#define SOC_DMA2_STREAM5_BASE   (SOC_DMA2_BASE + 0x088)
#define SOC_DMA2_STREAM6_BASE   (SOC_DMA2_BASE + 0x0A0)
#define SOC_DMA2_STREAM7_BASE   (SOC_DMA2_BASE + 0x0B8)
#define SOC_ETH_BASE            (SOC_AHB1PERIPH_BASE + 0x8000)
#define SOC_ETH_MAC_BASE        (SOC_ETH_BASE)
#define SOC_ETH_MMC_BASE        (SOC_ETH_BASE + 0x0100)
#define SOC_ETH_PTP_BASE        (SOC_ETH_BASE + 0x0700)
#define SOC_ETH_DMA_BASE        (SOC_ETH_BASE + 0x1000)
#define SOC_DMA2D_BASE          (SOC_AHB1PERIPH_BASE + 0xB000)

/* AHB2 peripherals */
#define SOC_DCMI_BASE           (SOC_AHB2PERIPH_BASE + 0x50000)
#define SOC_CRYP_BASE           (SOC_AHB2PERIPH_BASE + 0x60000)
#define SOC_HASH_BASE           (SOC_AHB2PERIPH_BASE + 0x60400)
#define SOC_HASH_DIGEST_BASE    (SOC_AHB2PERIPH_BASE + 0x60710)
#define SOC_RNG_BASE            (SOC_AHB2PERIPH_BASE + 0x60800)

/* FSMC Bankx registers base address */
#define SOC_FSMC_BANK1_R_BASE   (SOC_FSMC_R_BASE + 0x0000)
#define SOC_FSMC_BANK1E_R_BASE  (SOC_FSMC_R_BASE + 0x0104)
#define SOC_FSMC_BANK2_R_BASE   (SOC_FSMC_R_BASE + 0x0060)
#define SOC_FSMC_BANK3_R_BASE   (SOC_FSMC_R_BASE + 0x0080)
#define SOC_FSMC_BANK4_R_BASE   (SOC_FSMC_R_BASE + 0x00A0)

#define SOC_FMC_BANK1_R_BASE    (SOC_FMC_R_BASE + 0x0000)
#define SOC_FMC_BANK1E_R_BASE   (SOC_FMC_R_BASE + 0x0104)
#define SOC_FMC_BANK2_R_BASE    (SOC_FMC_R_BASE + 0x0060)
#define SOC_FMC_BANK3_R_BASE    (SOC_FMC_R_BASE + 0x0080)
#define SOC_FMC_BANK4_R_BASE    (SOC_FMC_R_BASE + 0x00A0)
#define SOC_FMC_BANK5_6_R_BASE  (SOC_FMC_R_BASE + 0x0140)

/* Debug MCU registers base address */
#define SOC_DBGMCU_BASE         (0xE0042000UL)

/* USB registers base address */
#define SOC_USB_OTG_HS_PERIPH_BASE      (0x40040000UL)
#define SOC_USB_OTG_FS_PERIPH_BASE      (0x50000000UL)

#define SOC_USB_OTG_GLOBAL_BASE         0x000U
#define SOC_USB_OTG_DEVICE_BASE         0x800U
#define SOC_USB_OTG_IN_ENDPOINT_BASE    0x900U
#define SOC_USB_OTG_OUT_ENDPOINT_BASE   0xB00U
#define SOC_USB_OTG_EP_REG_SIZE         0x20U
#define SOC_USB_OTG_HOST_BASE           0x400U
#define SOC_USB_OTG_HOST_PORT_BASE      0x440U
#define SOC_USB_OTG_HOST_CHANNEL_BASE   0x500U
#define SOC_USB_OTG_HOST_CHANNEL_SIZE   0x20U
#define SOC_USB_OTG_PCGCCTL_BASE        0xE00U
#define SOC_USB_OTG_FIFO_BASE           0x1000U
#define SOC_USB_OTG_FIFO_SIZE           0x1000U

/* Chip spec registers base address */
#define SOC_UID_BASE            (0x1FFF7A10UL) /**< Unique device ID register base address */
#define SOC_FLASHSIZE_BASE      (0x1FFF7A22UL) /**< FLASH Size register base address */
#define SOC_PACKAGE_BASE        (0x1FFF7BF0UL) /**< Package size register base address */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     registers     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* soc.h */
