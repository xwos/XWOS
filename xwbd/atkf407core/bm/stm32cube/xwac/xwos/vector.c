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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <bm/stm32cube/standard.h>
#include <xwos/ospl/irq.h>
#include <arch_skd.h>
#include <arch_systick.h>
#include <bm/stm32cube/cubemx/Core/Inc/isr.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240)
  #error "SOCCFG_IRQ_NUM must be between 8 and 240 inclusive"
#endif /* #if (SOCCFG_IRQ_NUM < 8) || (SOCCFG_IRQ_NUM > 240) */

extern xwu8_t xwos_stk_top[];

/**
 * @brief 中断向量表
 */
__soc_isr_table_qualifier struct soc_isr_table soc_isr_table __xwos_vctbl = {
        .arch = {
                (xwisr_f)xwos_stk_top,
                (xwisr_f)arch_isr_reset,
                arch_isr_nmi,
                arch_isr_hardfault,
                arch_isr_mm,
                arch_isr_busfault,
                arch_isr_usagefault,
                arch_isr_noop,
                arch_isr_noop,
                arch_isr_noop,
                arch_isr_noop,
                (xwisr_f)arch_isr_svc,
                arch_isr_dbgmon,
                arch_isr_noop,
                arch_isr_pendsv,
                arch_isr_systick,
        },
        .soc = {
                [WWDG_IRQn] = WWDG_IRQHandler,
                [PVD_IRQn] = PVD_IRQHandler,
                [TAMP_STAMP_IRQn] = TAMP_STAMP_IRQHandler,
                [RTC_WKUP_IRQn] = RTC_WKUP_IRQHandler,
                [FLASH_IRQn] = FLASH_IRQHandler,
                [RCC_IRQn] = RCC_IRQHandler,
                [EXTI0_IRQn] = EXTI0_IRQHandler,
                [EXTI1_IRQn] = EXTI1_IRQHandler,
                [EXTI2_IRQn] = EXTI2_IRQHandler,
                [EXTI3_IRQn] = EXTI3_IRQHandler,
                [EXTI4_IRQn] = EXTI4_IRQHandler,
                [DMA1_Stream0_IRQn] = DMA1_Stream0_IRQHandler,
                [DMA1_Stream1_IRQn] = DMA1_Stream1_IRQHandler,
                [DMA1_Stream2_IRQn] = DMA1_Stream2_IRQHandler,
                [DMA1_Stream3_IRQn] = DMA1_Stream3_IRQHandler,
                [DMA1_Stream4_IRQn] = DMA1_Stream4_IRQHandler,
                [DMA1_Stream5_IRQn] = DMA1_Stream5_IRQHandler,
                [DMA1_Stream6_IRQn] = DMA1_Stream6_IRQHandler,
                [ADC_IRQn] = ADC_IRQHandler,
                [CAN1_TX_IRQn] = CAN1_TX_IRQHandler,
                [CAN1_RX0_IRQn] = CAN1_RX0_IRQHandler,
                [CAN1_RX1_IRQn] = CAN1_RX1_IRQHandler,
                [CAN1_SCE_IRQn] = CAN1_SCE_IRQHandler,
                [EXTI9_5_IRQn] = EXTI9_5_IRQHandler,
                [TIM1_BRK_TIM9_IRQn] = TIM1_BRK_TIM9_IRQHandler,
                [TIM1_UP_TIM10_IRQn] = TIM1_UP_TIM10_IRQHandler,
                [TIM1_TRG_COM_TIM11_IRQn] = TIM1_TRG_COM_TIM11_IRQHandler,
                [TIM1_CC_IRQn] = TIM1_CC_IRQHandler,
                [TIM2_IRQn] = TIM2_IRQHandler,
                [TIM3_IRQn] = TIM3_IRQHandler,
                [TIM4_IRQn] = TIM4_IRQHandler,
                [I2C1_EV_IRQn] = I2C1_EV_IRQHandler,
                [I2C1_ER_IRQn] = I2C1_ER_IRQHandler,
                [I2C2_EV_IRQn] = I2C2_EV_IRQHandler,
                [I2C2_ER_IRQn] = I2C2_ER_IRQHandler,
                [SPI1_IRQn] = SPI1_IRQHandler,
                [SPI2_IRQn] = SPI2_IRQHandler,
                [USART1_IRQn] = USART1_IRQHandler,
                [USART2_IRQn] = USART2_IRQHandler,
                [USART3_IRQn] = USART3_IRQHandler,
                [EXTI15_10_IRQn] = EXTI15_10_IRQHandler,
                [RTC_Alarm_IRQn] = RTC_Alarm_IRQHandler,
                [OTG_FS_WKUP_IRQn] = OTG_FS_WKUP_IRQHandler,
                [TIM8_BRK_TIM12_IRQn] = TIM8_BRK_TIM12_IRQHandler,
                [TIM8_UP_TIM13_IRQn] = TIM8_UP_TIM13_IRQHandler,
                [TIM8_TRG_COM_TIM14_IRQn] = TIM8_TRG_COM_TIM14_IRQHandler,
                [TIM8_CC_IRQn] = TIM8_CC_IRQHandler,
                [DMA1_Stream7_IRQn] = DMA1_Stream7_IRQHandler,
                [FSMC_IRQn] = FSMC_IRQHandler,
                [SDIO_IRQn] = SDIO_IRQHandler,
                [TIM5_IRQn] = TIM5_IRQHandler,
                [SPI3_IRQn] = SPI3_IRQHandler,
                [UART4_IRQn] = UART4_IRQHandler,
                [UART5_IRQn] = UART5_IRQHandler,
                [TIM6_DAC_IRQn] = TIM6_DAC_IRQHandler,
                [TIM7_IRQn] = TIM7_IRQHandler,
                [DMA2_Stream0_IRQn] = DMA2_Stream0_IRQHandler,
                [DMA2_Stream1_IRQn] = DMA2_Stream1_IRQHandler,
                [DMA2_Stream2_IRQn] = DMA2_Stream2_IRQHandler,
                [DMA2_Stream3_IRQn] = DMA2_Stream3_IRQHandler,
                [DMA2_Stream4_IRQn] = DMA2_Stream4_IRQHandler,
                [ETH_IRQn] = ETH_IRQHandler,
                [ETH_WKUP_IRQn] = ETH_WKUP_IRQHandler,
                [CAN2_TX_IRQn] = CAN2_TX_IRQHandler,
                [CAN2_RX0_IRQn] = CAN2_RX0_IRQHandler,
                [CAN2_RX1_IRQn] = CAN2_RX1_IRQHandler,
                [CAN2_SCE_IRQn] = CAN2_SCE_IRQHandler,
                [OTG_FS_IRQn] = OTG_FS_IRQHandler,
                [DMA2_Stream5_IRQn] = DMA2_Stream5_IRQHandler,
                [DMA2_Stream6_IRQn] = DMA2_Stream6_IRQHandler,
                [DMA2_Stream7_IRQn] = DMA2_Stream7_IRQHandler,
                [USART6_IRQn] = USART6_IRQHandler,
                [I2C3_EV_IRQn] = I2C3_EV_IRQHandler,
                [I2C3_ER_IRQn] = I2C3_ER_IRQHandler,
                [OTG_HS_EP1_OUT_IRQn] = OTG_HS_EP1_OUT_IRQHandler,
                [OTG_HS_EP1_IN_IRQn] = OTG_HS_EP1_IN_IRQHandler,
                [OTG_HS_WKUP_IRQn] = OTG_HS_WKUP_IRQHandler,
                [OTG_HS_IRQn] = OTG_HS_IRQHandler,
                [DCMI_IRQn] = DCMI_IRQHandler,
                [RNG_IRQn] = RNG_IRQHandler,
                [FPU_IRQn] = FPU_IRQHandler,

                [82 ... (SOCCFG_IRQ_NUM - 1)] = arch_isr_noop,
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
                [0 ... (SOCCFG_IRQ_NUM - 1)] = NULL,
        },
};
