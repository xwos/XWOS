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
#include <bm/stm32cube/standard.h>
#include <xwos/irq.h>
#include <soc_irq.h>
#include <soc_sched.h>
#include <soc_syshwt.h>
#include "xwac/xwds/stm32cube.h"
#include "xwac/xwds/soc.h"
#include "xwac/xwds/usart.h"
#include "cubemx/Core/Inc/stm32f4xx_it.h"
#include "cubemx/Core/Inc/main.h"

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
                [ARCH_IRQ_RSVN9 + ARCHCFG_IRQ_NUM] = arch_isr_nop,
                [ARCH_IRQ_RSVN8 + ARCHCFG_IRQ_NUM] = arch_isr_nop,
                [ARCH_IRQ_RSVN7 + ARCHCFG_IRQ_NUM] = arch_isr_nop,
                [ARCH_IRQ_RSVN6 + ARCHCFG_IRQ_NUM] = arch_isr_nop,
                [ARCH_IRQ_SVCALL + ARCHCFG_IRQ_NUM] = (xwisr_f)arch_isr_svc,
                [ARCH_IRQ_DBGMON + ARCHCFG_IRQ_NUM] = arch_isr_dbgmon,
                [ARCH_IRQ_RSVN3 + ARCHCFG_IRQ_NUM] = arch_isr_nop,
                [ARCH_IRQ_PENDSV + ARCHCFG_IRQ_NUM] = arch_scheduler_isr_swcx,
                [ARCH_IRQ_SYSTICK + ARCHCFG_IRQ_NUM] = arch_systick_isr,
        },
        .soc = {
                [WWDG_IRQn] = arch_isr_nop, /* isr0 */
                [PVD_IRQn] = stm32cube_exti_isr, /* isr1 */
                [TAMP_STAMP_IRQn] = stm32cube_exti_isr, /* isr2 */
                [RTC_WKUP_IRQn] = stm32cube_exti_isr, /* isr3 */
                [FLASH_IRQn] = arch_isr_nop, /* isr4 */
                [RCC_IRQn] = arch_isr_nop, /* isr5 */
                [EXTI0_IRQn] = stm32cube_exti_isr, /* isr6 */
                [EXTI1_IRQn] = stm32cube_exti_isr, /* isr7 */
                [EXTI2_IRQn] = stm32cube_exti_isr, /* isr8 */
                [EXTI3_IRQn] = stm32cube_exti_isr, /* isr9 */
                [EXTI4_IRQn] = stm32cube_exti_isr, /* isr10 */
                [DMA1_Stream0_IRQn] = stm32cube_dma1_stream0_isr, /* isr11 */
                [DMA1_Stream1_IRQn] = stm32cube_dma1_stream1_isr, /* isr12 */
                [DMA1_Stream2_IRQn] = stm32cube_dma1_stream2_isr, /* isr13 */
                [DMA1_Stream3_IRQn] = stm32cube_dma1_stream3_isr, /* isr14 */
                [DMA1_Stream4_IRQn] = stm32cube_dma1_stream4_isr, /* isr15 */
                [DMA1_Stream5_IRQn] = stm32cube_dma1_stream5_isr, /* isr16 */
                [DMA1_Stream6_IRQn] = stm32cube_dma1_stream6_isr, /* isr17 */
                [ADC_IRQn] = arch_isr_nop, /* isr18 */
                [CAN1_TX_IRQn] = arch_isr_nop, /* isr19 */
                [CAN1_RX0_IRQn] = arch_isr_nop, /* isr20 */
                [CAN1_RX1_IRQn] = arch_isr_nop, /* isr21 */
                [CAN1_SCE_IRQn] = arch_isr_nop, /* isr22 */
                [EXTI9_5_IRQn] = stm32cube_exti_isr, /* isr23 */
                [TIM1_BRK_TIM9_IRQn] = stm32cube_dmauartc1_timer_isr, /* isr24 */
                [TIM1_UP_TIM10_IRQn] = arch_isr_nop, /* isr25 */
                [TIM1_TRG_COM_TIM11_IRQn] = arch_isr_nop, /* isr26 */
                [TIM1_CC_IRQn] = arch_isr_nop, /* isr27 */
                [TIM2_IRQn] = arch_isr_nop, /* isr28 */
                [TIM3_IRQn] = arch_isr_nop, /* isr29 */
                [TIM4_IRQn] = arch_isr_nop, /* isr30 */
                [I2C1_EV_IRQn] = arch_isr_nop, /* isr31 */
                [I2C1_ER_IRQn] = arch_isr_nop, /* isr32 */
                [I2C2_EV_IRQn] = arch_isr_nop, /* isr33 */
                [I2C2_ER_IRQn] = arch_isr_nop, /* isr34 */
                [SPI1_IRQn] = arch_isr_nop, /* isr35 */
                [SPI2_IRQn] = arch_isr_nop, /* isr36 */
                [USART1_IRQn] = stm32cube_dmauartc1_isr, /* isr37 */
                [USART2_IRQn] = arch_isr_nop, /* isr38 */
                [USART3_IRQn] = arch_isr_nop, /* isr39 */
                [EXTI15_10_IRQn] = stm32cube_exti_isr, /* isr40 */
                [RTC_Alarm_IRQn] = stm32cube_exti_isr, /* isr41 */
                [OTG_FS_WKUP_IRQn] = stm32cube_exti_isr, /* isr42 */
                [TIM8_BRK_TIM12_IRQn] = arch_isr_nop, /* isr43 */
                [TIM8_UP_TIM13_IRQn] = arch_isr_nop, /* isr44 */
                [TIM8_TRG_COM_TIM14_IRQn] = arch_isr_nop, /* isr45 */
                [TIM8_CC_IRQn] = arch_isr_nop, /* isr46 */
                [DMA1_Stream7_IRQn] = stm32cube_dma1_stream7_isr, /* isr47 */
                [FMC_IRQn] = arch_isr_nop, /* isr48 */
                [SDIO_IRQn] = SDIO_IRQHandler, /* isr49 */
                [TIM5_IRQn] = arch_isr_nop, /* isr50 */
                [SPI3_IRQn] = arch_isr_nop, /* isr51 */
                [UART4_IRQn] = arch_isr_nop, /* isr52 */
                [UART5_IRQn] = arch_isr_nop, /* isr53 */
                [TIM6_DAC_IRQn] = arch_isr_nop, /* isr54 */
                [TIM7_IRQn] = arch_isr_nop, /* isr55 */
                [DMA2_Stream0_IRQn] = stm32cube_dma2_stream0_isr, /* isr56 */
                [DMA2_Stream1_IRQn] = stm32cube_dma2_stream1_isr, /* isr57 */
                [DMA2_Stream2_IRQn] = stm32cube_dma2_stream2_isr, /* isr58 */
                [DMA2_Stream3_IRQn] = DMA2_Stream3_IRQHandler, /* isr59 */
                [DMA2_Stream4_IRQn] = stm32cube_dma2_stream4_isr, /* isr60 */
                [ETH_IRQn] = arch_isr_nop, /* isr61 */
                [ETH_WKUP_IRQn] = stm32cube_exti_isr, /* isr62 */
                [CAN2_TX_IRQn] = arch_isr_nop, /* isr63 */
                [CAN2_RX0_IRQn] = arch_isr_nop, /* isr64 */
                [CAN2_RX1_IRQn] = arch_isr_nop, /* isr65 */
                [CAN2_SCE_IRQn] = arch_isr_nop, /* isr66 */
                [OTG_FS_IRQn] = arch_isr_nop, /* isr67 */
                [DMA2_Stream5_IRQn] = stm32cube_dma2_stream5_isr, /* isr68 */
                [DMA2_Stream6_IRQn] = DMA2_Stream6_IRQHandler, /* isr69 */
                [DMA2_Stream7_IRQn] = stm32cube_dma2_stream7_isr, /* isr70 */
                [USART6_IRQn] = arch_isr_nop, /* isr71 */
                [I2C3_EV_IRQn] = arch_isr_nop, /* isr72 */
                [I2C3_ER_IRQn] = arch_isr_nop, /* isr73 */
                [OTG_HS_EP1_OUT_IRQn] = arch_isr_nop, /* isr74 */
                [OTG_HS_EP1_IN_IRQn] = arch_isr_nop, /* isr75 */
                [OTG_HS_WKUP_IRQn] = stm32cube_exti_isr, /* isr76 */
                [OTG_HS_IRQn] = arch_isr_nop, /* isr77 */
                [DCMI_IRQn] = arch_isr_nop, /* isr78 */
                [XWOS_IRQ(79)] = arch_isr_nop, /* isr79 */
                [HASH_RNG_IRQn] = arch_isr_nop, /* isr80 */
                [FPU_IRQn] = arch_isr_nop, /* isr81 */
                [UART7_IRQn] = arch_isr_nop, /* isr82 */
                [UART8_IRQn] = arch_isr_nop, /* isr83 */
                [SPI4_IRQn] = arch_isr_nop, /* isr84 */
                [SPI5_IRQn] = arch_isr_nop, /* isr85 */
                [SPI6_IRQn] = arch_isr_nop, /* isr86 */
                [SAI1_IRQn] = arch_isr_nop, /* isr87 */
                [LTDC_IRQn] = arch_isr_nop, /* isr88 */
                [LTDC_ER_IRQn] = arch_isr_nop, /* isr89 */
                [DMA2D_IRQn] = arch_isr_nop, /* isr90 */

                [91 ... (SOCCFG_IRQ_NUM - 1)] = arch_isr_nop,
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
                [WWDG_IRQn] = NULL,
                [PVD_IRQn] = &stm32cube_soc_cb,
                [TAMP_STAMP_IRQn] = &stm32cube_soc_cb,
                [RTC_WKUP_IRQn] = &stm32cube_soc_cb,
                [FLASH_IRQn] = NULL,
                [RCC_IRQn] = NULL,
                [EXTI0_IRQn] = &stm32cube_soc_cb,
                [EXTI1_IRQn] = &stm32cube_soc_cb,
                [EXTI2_IRQn] = &stm32cube_soc_cb,
                [EXTI3_IRQn] = &stm32cube_soc_cb,
                [EXTI4_IRQn] = &stm32cube_soc_cb,
                [DMA1_Stream0_IRQn] = &stm32cube_soc_cb,
                [DMA1_Stream1_IRQn] = &stm32cube_soc_cb,
                [DMA1_Stream2_IRQn] = &stm32cube_soc_cb,
                [DMA1_Stream3_IRQn] = &stm32cube_soc_cb,
                [DMA1_Stream4_IRQn] = &stm32cube_soc_cb,
                [DMA1_Stream5_IRQn] = &stm32cube_soc_cb,
                [DMA1_Stream6_IRQn] = &stm32cube_soc_cb,
                [ADC_IRQn] = NULL,
                [CAN1_TX_IRQn] = NULL,
                [CAN1_RX0_IRQn] = NULL,
                [CAN1_RX1_IRQn] = NULL,
                [CAN1_SCE_IRQn] = NULL,
                [EXTI9_5_IRQn] = &stm32cube_soc_cb,
                [TIM1_BRK_TIM9_IRQn] = &stm32cube_dmauartc1_cb,
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
                [USART1_IRQn] = &stm32cube_dmauartc1_cb,
                [USART2_IRQn] = NULL,
                [USART3_IRQn] = NULL,
                [EXTI15_10_IRQn] = &stm32cube_soc_cb,
                [RTC_Alarm_IRQn] = &stm32cube_soc_cb,
                [OTG_FS_WKUP_IRQn] = &stm32cube_soc_cb,
                [TIM8_BRK_TIM12_IRQn] = NULL,
                [TIM8_UP_TIM13_IRQn] = NULL,
                [TIM8_TRG_COM_TIM14_IRQn] = NULL,
                [TIM8_CC_IRQn] = NULL,
                [DMA1_Stream7_IRQn] = &stm32cube_soc_cb,
                [FMC_IRQn] = NULL,
                [SDIO_IRQn] = NULL,
                [TIM5_IRQn] = NULL,
                [SPI3_IRQn] = NULL,
                [UART4_IRQn] = NULL,
                [UART5_IRQn] = NULL,
                [TIM6_DAC_IRQn] = NULL,
                [TIM7_IRQn] = NULL,
                [DMA2_Stream0_IRQn] = &stm32cube_soc_cb,
                [DMA2_Stream1_IRQn] = &stm32cube_soc_cb,
                [DMA2_Stream2_IRQn] = &stm32cube_soc_cb,
                [DMA2_Stream3_IRQn] = &stm32cube_soc_cb,
                [DMA2_Stream4_IRQn] = &stm32cube_soc_cb,
                [ETH_IRQn] = NULL,
                [ETH_WKUP_IRQn] = &stm32cube_soc_cb,
                [CAN2_TX_IRQn] = NULL,
                [CAN2_RX0_IRQn] = NULL,
                [CAN2_RX1_IRQn] = NULL,
                [CAN2_SCE_IRQn] = NULL,
                [OTG_FS_IRQn] = NULL,
                [DMA2_Stream5_IRQn] = &stm32cube_soc_cb,
                [DMA2_Stream6_IRQn] = &stm32cube_soc_cb,
                [DMA2_Stream7_IRQn] = &stm32cube_soc_cb,
                [USART6_IRQn] = NULL,
                [I2C3_EV_IRQn] = NULL,
                [I2C3_ER_IRQn] = NULL,
                [OTG_HS_EP1_OUT_IRQn] = NULL,
                [OTG_HS_EP1_IN_IRQn] = NULL,
                [OTG_HS_WKUP_IRQn] = &stm32cube_soc_cb,
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
        },
};
