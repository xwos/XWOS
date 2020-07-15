/**
 * @file
 * @brief STM32CUBE：中断向量表
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
#include <xwos/standard.h>
#include <xwos/irq.h>
#include <soc_irq.h>
#include <soc_sched.h>
#include <soc_syshwt.h>
#include "stm32f1xx.h"
#include <bm/stm32cube/cubemx/Core/Inc/stm32f1xx_it.h>

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
                arch_isr_nop,
                arch_isr_nop,
                arch_isr_nop,
                arch_isr_nop,
                (xwisr_f)arch_isr_svc,
                arch_isr_dbgmon,
                arch_isr_nop,
                arch_scheduler_isr_swcx,
                arch_systick_isr,
        },
        .soc = {
                [WWDG_IRQn] = arch_isr_nop,
                [PVD_IRQn] = arch_isr_nop,
                [TAMPER_IRQn] = arch_isr_nop,
                [RTC_IRQn] = arch_isr_nop,
                [FLASH_IRQn] = arch_isr_nop,
                [RCC_IRQn] = arch_isr_nop,
                [EXTI0_IRQn] = arch_isr_nop,
                [EXTI1_IRQn] = arch_isr_nop,
                [EXTI2_IRQn] = arch_isr_nop,
                [EXTI3_IRQn] = arch_isr_nop,
                [EXTI4_IRQn] = arch_isr_nop,
                [DMA1_Channel1_IRQn] = arch_isr_nop,
                [DMA1_Channel2_IRQn] = arch_isr_nop,
                [DMA1_Channel3_IRQn] = arch_isr_nop,
                [DMA1_Channel4_IRQn] = DMA1_Channel4_IRQHandler,
                [DMA1_Channel5_IRQn] = DMA1_Channel5_IRQHandler,
                [DMA1_Channel6_IRQn] = arch_isr_nop,
                [DMA1_Channel7_IRQn] = arch_isr_nop,
                [ADC1_2_IRQn] = arch_isr_nop,
                [USB_HP_CAN1_TX_IRQn] = arch_isr_nop,
                [USB_LP_CAN1_RX0_IRQn] = arch_isr_nop,
                [CAN1_RX1_IRQn] = arch_isr_nop,
                [CAN1_SCE_IRQn] = arch_isr_nop,
                [EXTI9_5_IRQn] = arch_isr_nop,
                [TIM1_BRK_IRQn] = arch_isr_nop,
                [TIM1_UP_IRQn] = arch_isr_nop,
                [TIM1_TRG_COM_IRQn] = arch_isr_nop,
                [TIM1_CC_IRQn] = arch_isr_nop,
                [TIM2_IRQn] = arch_isr_nop,
                [TIM3_IRQn] = arch_isr_nop,
                [TIM4_IRQn] = arch_isr_nop,
                [I2C1_EV_IRQn] = arch_isr_nop,
                [I2C1_ER_IRQn] = arch_isr_nop,
                [I2C2_EV_IRQn] = arch_isr_nop,
                [I2C2_ER_IRQn] = arch_isr_nop,
                [SPI1_IRQn] = arch_isr_nop,
                [SPI2_IRQn] = arch_isr_nop,
                [USART1_IRQn] = USART1_IRQHandler,
                [USART2_IRQn] = arch_isr_nop,
                [USART3_IRQn] = arch_isr_nop,
                [EXTI15_10_IRQn] = arch_isr_nop,
                [RTC_Alarm_IRQn] = arch_isr_nop,
                [USBWakeUp_IRQn] = arch_isr_nop,
                [TIM8_BRK_IRQn] = arch_isr_nop,
                [TIM8_UP_IRQn] = arch_isr_nop,
                [TIM8_TRG_COM_IRQn] = arch_isr_nop,
                [TIM8_CC_IRQn] = arch_isr_nop,
                [ADC3_IRQn] = arch_isr_nop,
                [FSMC_IRQn] = arch_isr_nop,
                [SDIO_IRQn] = arch_isr_nop,
                [TIM5_IRQn] = arch_isr_nop,
                [SPI3_IRQn] = arch_isr_nop,
                [UART4_IRQn] = arch_isr_nop,
                [UART5_IRQn] = arch_isr_nop,
                [TIM6_IRQn] = arch_isr_nop,
                [TIM7_IRQn] = TIM7_IRQHandler,
                [DMA2_Channel1_IRQn] = arch_isr_nop,
                [DMA2_Channel2_IRQn] = arch_isr_nop,
                [DMA2_Channel3_IRQn] = arch_isr_nop,
                [DMA2_Channel4_5_IRQn] = arch_isr_nop,
                [60 ... (SOCCFG_IRQ_NUM - 1)] = arch_isr_nop,
        },
};

__soc_isr_table_qualifier struct soc_irq_data_table soc_irq_data_table = {
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
