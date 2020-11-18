/**
 * @file
 * @brief STM32CUBE模块：中断向量表
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
#include <arch_skd.h>
#include <arch_systick.h>
#include <bm/stm32cube/cubemx/Core/Inc/isr.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>

#if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240)
  #error "SOCCFG_IRQ_NUM must be between 8 and 240 inclusive"
#endif /* #if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240) */

extern xwu8_t xwos_stk_top[];

/**
 * @brief ISR table
 */
__soc_isr_table_qualifier struct soc_isr_table soc_isr_table __xwos_vctbl = {
        .arch = {
                [ARCH_SP_TOP + ARCHCFG_IRQ_NUM] = (xwisr_f)xwos_stk_top,
                [ARCH_IRQ_RESET + ARCHCFG_IRQ_NUM] = (xwisr_f)arch_isr_reset,
                [ARCH_IRQ_NMI + ARCHCFG_IRQ_NUM] = arch_isr_nmi,
                [ARCH_IRQ_HARDFAULT + ARCHCFG_IRQ_NUM] = arch_isr_hardfault,
                [ARCH_IRQ_MMFAULT + ARCHCFG_IRQ_NUM] = arch_isr_mm,
                [ARCH_IRQ_BUSFAULT + ARCHCFG_IRQ_NUM] = arch_isr_busfault,
                [ARCH_IRQ_USGFAULT + ARCHCFG_IRQ_NUM] = arch_isr_usagefault,
                [ARCH_IRQ_RSVN9 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_RSVN8 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_RSVN7 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_RSVN6 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_SVCALL + ARCHCFG_IRQ_NUM] = (xwisr_f)arch_isr_svc,
                [ARCH_IRQ_DBGMON + ARCHCFG_IRQ_NUM] = arch_isr_dbgmon,
                [ARCH_IRQ_RSVN3 + ARCHCFG_IRQ_NUM] = arch_isr_noop,
                [ARCH_IRQ_PENDSV + ARCHCFG_IRQ_NUM] = arch_isr_pendsv,
                [ARCH_IRQ_SYSTICK + ARCHCFG_IRQ_NUM] = arch_isr_systick,
        },
        .soc = {
                [WWDG_IRQn] = WWDG_IRQHandler, /* isr0 */
                [PVD_IRQn] = PVD_IRQHandler, /* isr1 */
                [TAMP_STAMP_IRQn] = TAMP_STAMP_IRQHandler, /* isr2 */
                [RTC_WKUP_IRQn] = RTC_WKUP_IRQHandler, /* isr3 */
                [FLASH_IRQn] = FLASH_IRQHandler, /* isr4 */
                [RCC_IRQn] = RCC_IRQHandler, /* isr5 */
                [EXTI0_IRQn] = EXTI0_IRQHandler, /* isr6 */
                [EXTI1_IRQn] = EXTI1_IRQHandler, /* isr7 */
                [EXTI2_IRQn] = EXTI2_IRQHandler, /* isr8 */
                [EXTI3_IRQn] = EXTI3_IRQHandler, /* isr9 */
                [EXTI4_IRQn] = EXTI4_IRQHandler, /* isr10 */
                [DMA1_Stream0_IRQn] = DMA1_Stream0_IRQHandler, /* isr11 */
                [DMA1_Stream1_IRQn] = DMA1_Stream1_IRQHandler, /* isr12 */
                [DMA1_Stream2_IRQn] = DMA1_Stream2_IRQHandler, /* isr13 */
                [DMA1_Stream3_IRQn] = DMA1_Stream3_IRQHandler, /* isr14 */
                [DMA1_Stream4_IRQn] = DMA1_Stream4_IRQHandler, /* isr15 */
                [DMA1_Stream5_IRQn] = DMA1_Stream5_IRQHandler, /* isr16 */
                [DMA1_Stream6_IRQn] = DMA1_Stream6_IRQHandler, /* isr17 */
                [ADC_IRQn] = ADC_IRQHandler, /* isr18 */
                [CAN1_TX_IRQn] = CAN1_TX_IRQHandler, /* isr19 */
                [CAN1_RX0_IRQn] = CAN1_RX0_IRQHandler, /* isr20 */
                [CAN1_RX1_IRQn] = CAN1_RX1_IRQHandler, /* isr21 */
                [CAN1_SCE_IRQn] = CAN1_SCE_IRQHandler, /* isr22 */
                [EXTI9_5_IRQn] = EXTI9_5_IRQHandler, /* isr23 */
                [TIM1_BRK_TIM9_IRQn] = TIM1_BRK_TIM9_IRQHandler, /* isr24 */
                [TIM1_UP_TIM10_IRQn] = TIM1_UP_TIM10_IRQHandler, /* isr25 */
                [TIM1_TRG_COM_TIM11_IRQn] = TIM1_TRG_COM_TIM11_IRQHandler, /* isr26 */
                [TIM1_CC_IRQn] = TIM1_CC_IRQHandler, /* isr27 */
                [TIM2_IRQn] = TIM2_IRQHandler, /* isr28 */
                [TIM3_IRQn] = TIM3_IRQHandler, /* isr29 */
                [TIM4_IRQn] = TIM4_IRQHandler, /* isr30 */
                [I2C1_EV_IRQn] = I2C1_EV_IRQHandler, /* isr31 */
                [I2C1_ER_IRQn] = I2C1_ER_IRQHandler, /* isr32 */
                [I2C2_EV_IRQn] = I2C2_EV_IRQHandler, /* isr33 */
                [I2C2_ER_IRQn] = I2C2_ER_IRQHandler, /* isr34 */
                [SPI1_IRQn] = SPI1_IRQHandler, /* isr35 */
                [SPI2_IRQn] = SPI2_IRQHandler, /* isr36 */
                [USART1_IRQn] = USART1_IRQHandler, /* isr37 */
                [USART2_IRQn] = USART2_IRQHandler, /* isr38 */
                [USART3_IRQn] = USART3_IRQHandler, /* isr39 */
                [EXTI15_10_IRQn] = EXTI15_10_IRQHandler, /* isr40 */
                [RTC_Alarm_IRQn] = RTC_Alarm_IRQHandler, /* isr41 */
                [OTG_FS_WKUP_IRQn] = OTG_FS_WKUP_IRQHandler, /* isr42 */
                [TIM8_BRK_TIM12_IRQn] = TIM8_BRK_TIM12_IRQHandler, /* isr43 */
                [TIM8_UP_TIM13_IRQn] = TIM8_UP_TIM13_IRQHandler, /* isr44 */
                [TIM8_TRG_COM_TIM14_IRQn] = TIM8_TRG_COM_TIM14_IRQHandler, /* isr45 */
                [TIM8_CC_IRQn] = TIM8_CC_IRQHandler, /* isr46 */
                [DMA1_Stream7_IRQn] = DMA1_Stream7_IRQHandler, /* isr47 */
                [FMC_IRQn] = FMC_IRQHandler, /* isr48 */
                [SDIO_IRQn] = SDIO_IRQHandler, /* isr49 */
                [TIM5_IRQn] = TIM5_IRQHandler, /* isr50 */
                [SPI3_IRQn] = SPI3_IRQHandler, /* isr51 */
                [UART4_IRQn] = UART4_IRQHandler, /* isr52 */
                [UART5_IRQn] = UART5_IRQHandler, /* isr53 */
                [TIM6_DAC_IRQn] = TIM6_DAC_IRQHandler, /* isr54 */
                [TIM7_IRQn] = TIM7_IRQHandler, /* isr55 */
                [DMA2_Stream0_IRQn] = DMA2_Stream0_IRQHandler, /* isr56 */
                [DMA2_Stream1_IRQn] = DMA2_Stream1_IRQHandler, /* isr57 */
                [DMA2_Stream2_IRQn] = DMA2_Stream2_IRQHandler, /* isr58 */
                [DMA2_Stream3_IRQn] = DMA2_Stream3_IRQHandler, /* isr59 */
                [DMA2_Stream4_IRQn] = DMA2_Stream4_IRQHandler, /* isr60 */
                [ETH_IRQn] = ETH_IRQHandler, /* isr61 */
                [ETH_WKUP_IRQn] = ETH_WKUP_IRQHandler, /* isr62 */
                [CAN2_TX_IRQn] = CAN2_TX_IRQHandler, /* isr63 */
                [CAN2_RX0_IRQn] = CAN2_RX0_IRQHandler, /* isr64 */
                [CAN2_RX1_IRQn] = CAN2_RX1_IRQHandler, /* isr65 */
                [CAN2_SCE_IRQn] = CAN2_SCE_IRQHandler, /* isr66 */
                [OTG_FS_IRQn] = OTG_FS_IRQHandler, /* isr67 */
                [DMA2_Stream5_IRQn] = DMA2_Stream5_IRQHandler, /* isr68 */
                [DMA2_Stream6_IRQn] = DMA2_Stream6_IRQHandler, /* isr69 */
                [DMA2_Stream7_IRQn] = DMA2_Stream7_IRQHandler, /* isr70 */
                [USART6_IRQn] = USART6_IRQHandler, /* isr71 */
                [I2C3_EV_IRQn] = I2C3_EV_IRQHandler, /* isr72 */
                [I2C3_ER_IRQn] = I2C3_ER_IRQHandler, /* isr73 */
                [OTG_HS_EP1_OUT_IRQn] = OTG_HS_EP1_OUT_IRQHandler, /* isr74 */
                [OTG_HS_EP1_IN_IRQn] = OTG_HS_EP1_IN_IRQHandler, /* isr75 */
                [OTG_HS_WKUP_IRQn] = OTG_HS_WKUP_IRQHandler, /* isr76 */
                [OTG_HS_IRQn] = OTG_HS_IRQHandler, /* isr77 */
                [DCMI_IRQn] = DCMI_IRQHandler, /* isr78 */
                [XWOS_IRQ(79)] = NULL, /* isr79 */
                [HASH_RNG_IRQn] = HASH_RNG_IRQHandler, /* isr80 */
                [FPU_IRQn] = FPU_IRQHandler, /* isr81 */
                [UART7_IRQn] = UART7_IRQHandler, /* isr82 */
                [UART8_IRQn] = UART8_IRQHandler, /* isr83 */
                [SPI4_IRQn] = SPI4_IRQHandler, /* isr84 */
                [SPI5_IRQn] = SPI5_IRQHandler, /* isr85 */
                [SPI6_IRQn] = SPI6_IRQHandler, /* isr86 */
                [SAI1_IRQn] = SAI1_IRQHandler, /* isr87 */
                [LTDC_IRQn] = LTDC_IRQHandler, /* isr88 */
                [LTDC_ER_IRQn] = LTDC_ER_IRQHandler, /* isr89 */
                [DMA2D_IRQn] = DMA2D_IRQHandler, /* isr90 */

                [91 ... (SOCCFG_IRQ_NUM - 1)] = arch_isr_noop,
        },
};

__soc_isr_table_qualifier struct soc_isr_data_table soc_isr_data_table = {
        .arch = {
                [ARCH_SP_TOP + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RESET + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_NMI + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_HARDFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_MMFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_BUSFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_USGFAULT + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN9 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN8 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN7 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN6 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_SVCALL + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_DBGMON + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_RSVN3 + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_PENDSV + ARCHCFG_IRQ_NUM] = NULL,
                [ARCH_IRQ_SYSTICK + ARCHCFG_IRQ_NUM] = NULL,
        },
        .soc = {
                [WWDG_IRQn] = NULL,
                [PVD_IRQn] = NULL,
                [TAMP_STAMP_IRQn] = NULL,
                [RTC_WKUP_IRQn] = NULL,
                [FLASH_IRQn] = NULL,
                [RCC_IRQn] = NULL,
                [EXTI0_IRQn] = NULL,
                [EXTI1_IRQn] = NULL,
                [EXTI2_IRQn] = NULL,
                [EXTI3_IRQn] = NULL,
                [EXTI4_IRQn] = NULL,
                [DMA1_Stream0_IRQn] = NULL,
                [DMA1_Stream1_IRQn] = NULL,
                [DMA1_Stream2_IRQn] = NULL,
                [DMA1_Stream3_IRQn] = NULL,
                [DMA1_Stream4_IRQn] = NULL,
                [DMA1_Stream5_IRQn] = NULL,
                [DMA1_Stream6_IRQn] = NULL,
                [ADC_IRQn] = NULL,
                [CAN1_TX_IRQn] = NULL,
                [CAN1_RX0_IRQn] = NULL,
                [CAN1_RX1_IRQn] = NULL,
                [CAN1_SCE_IRQn] = NULL,
                [EXTI9_5_IRQn] = NULL,
                [TIM1_BRK_TIM9_IRQn] = NULL,
                [TIM1_UP_TIM10_IRQn] = NULL,
                [TIM1_TRG_COM_TIM11_IRQn] = NULL,
                [TIM1_CC_IRQn] = NULL,
                [TIM2_IRQn] = NULL,
                [TIM3_IRQn] = NULL,
                [TIM4_IRQn] = NULL,
                [I2C1_EV_IRQn] = NULL,
                [I2C1_ER_IRQn] = NULL,
                [I2C2_EV_IRQn] = NULL,
                [I2C2_ER_IRQn] = NULL,
                [SPI1_IRQn] = NULL,
                [SPI2_IRQn] = NULL,
                [USART1_IRQn] = NULL,
                [USART2_IRQn] = NULL,
                [USART3_IRQn] = NULL,
                [EXTI15_10_IRQn] = NULL,
                [RTC_Alarm_IRQn] = NULL,
                [OTG_FS_WKUP_IRQn] = NULL,
                [TIM8_BRK_TIM12_IRQn] = NULL,
                [TIM8_UP_TIM13_IRQn] = NULL,
                [TIM8_TRG_COM_TIM14_IRQn] = NULL,
                [TIM8_CC_IRQn] = NULL,
                [DMA1_Stream7_IRQn] = NULL,
                [FMC_IRQn] = NULL,
                [SDIO_IRQn] = NULL,
                [TIM5_IRQn] = NULL,
                [SPI3_IRQn] = NULL,
                [UART4_IRQn] = NULL,
                [UART5_IRQn] = NULL,
                [TIM6_DAC_IRQn] = NULL,
                [TIM7_IRQn] = NULL,
                [DMA2_Stream0_IRQn] = NULL,
                [DMA2_Stream1_IRQn] = NULL,
                [DMA2_Stream2_IRQn] = NULL,
                [DMA2_Stream3_IRQn] = NULL,
                [DMA2_Stream4_IRQn] = NULL,
                [ETH_IRQn] = NULL,
                [ETH_WKUP_IRQn] = NULL,
                [CAN2_TX_IRQn] = NULL,
                [CAN2_RX0_IRQn] = NULL,
                [CAN2_RX1_IRQn] = NULL,
                [CAN2_SCE_IRQn] = NULL,
                [OTG_FS_IRQn] = NULL,
                [DMA2_Stream5_IRQn] = NULL,
                [DMA2_Stream6_IRQn] = NULL,
                [DMA2_Stream7_IRQn] = NULL,
                [USART6_IRQn] = NULL,
                [I2C3_EV_IRQn] = NULL,
                [I2C3_ER_IRQn] = NULL,
                [OTG_HS_EP1_OUT_IRQn] = NULL,
                [OTG_HS_EP1_IN_IRQn] = NULL,
                [OTG_HS_WKUP_IRQn] = NULL,
                [OTG_HS_IRQn] = NULL,
                [DCMI_IRQn] = NULL,
                [HASH_RNG_IRQn] = NULL,
                [FPU_IRQn] = NULL,
                [UART7_IRQn] = NULL,
                [UART8_IRQn] = NULL,
                [SPI4_IRQn] = NULL,
                [SPI5_IRQn] = NULL,
                [SPI6_IRQn] = NULL,
                [SAI1_IRQn] = NULL,
                [LTDC_IRQn] = NULL,
                [LTDC_ER_IRQn] = NULL,
                [DMA2D_IRQn] = NULL,

                [91 ... (SOCCFG_IRQ_NUM - 1)] = NULL,
        },
};
