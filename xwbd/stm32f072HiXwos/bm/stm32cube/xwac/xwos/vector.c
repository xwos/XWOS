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
                [PVD_VDDIO2_IRQn] = PVD_VDDIO2_IRQHandler,
                [RTC_IRQn] = RTC_IRQHandler,
                [FLASH_IRQn] = FLASH_IRQHandler,
                [RCC_CRS_IRQn] = RCC_CRS_IRQHandler,
                [EXTI0_1_IRQn] = EXTI0_1_IRQHandler,
                [EXTI2_3_IRQn] = EXTI2_3_IRQHandler,
                [EXTI4_15_IRQn] = EXTI4_15_IRQHandler,
                [TSC_IRQn] = TSC_IRQHandler,
                [DMA1_Channel1_IRQn] = DMA1_Channel1_IRQHandler,
                [DMA1_Channel2_3_IRQn] = DMA1_Channel2_3_IRQHandler,
                [DMA1_Channel4_5_6_7_IRQn] = DMA1_Channel4_5_6_7_IRQHandler,
                [ADC1_COMP_IRQn] = ADC1_COMP_IRQHandler,
                [TIM1_BRK_UP_TRG_COM_IRQn] = TIM1_BRK_UP_TRG_COM_IRQHandler,
                [TIM1_CC_IRQn] = TIM1_CC_IRQHandler,
                [TIM2_IRQn] = TIM2_IRQHandler,
                [TIM3_IRQn] = TIM3_IRQHandler,
                [TIM6_DAC_IRQn] = TIM6_DAC_IRQHandler,
                [TIM7_IRQn] = TIM7_IRQHandler,
                [TIM14_IRQn] = TIM14_IRQHandler,
                [TIM15_IRQn] = TIM15_IRQHandler,
                [TIM16_IRQn] = TIM16_IRQHandler,
                [TIM17_IRQn] = TIM17_IRQHandler,
                [I2C1_IRQn] = I2C1_IRQHandler,
                [I2C2_IRQn] = I2C2_IRQHandler,
                [SPI1_IRQn] = SPI1_IRQHandler,
                [SPI2_IRQn] = SPI2_IRQHandler,
                [USART1_IRQn] = USART1_IRQHandler,
                [USART2_IRQn] = USART2_IRQHandler,
                [USART3_4_IRQn] = USART3_4_IRQHandler,
                [CEC_CAN_IRQn] = CEC_CAN_IRQHandler,
                [USB_IRQn] = USB_IRQHandler,

                [32 ... (SOCCFG_IRQ_NUM - 1)] = arch_isr_noop,
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
