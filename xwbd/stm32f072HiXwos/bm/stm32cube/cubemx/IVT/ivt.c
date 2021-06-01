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
#include <xwos/ospl/syshwt.h>
#include <xwos/ospl/skd.h>
#include "cubemx/IVT/isr.h"
#include "cubemx/Core/Inc/main.h"

extern xwu8_t xwos_stk_top[];

__xwos_ivt __xwos_ivt_qualifier struct soc_ivt xwospl_ivt = {
        .exc = {
                [SOC_SP_TOP + SOCCFG_EXC_NUM] = (xwisr_f)xwos_stk_top,
                [SOC_EXC_RESET + SOCCFG_EXC_NUM] = (xwisr_f)arch_isr_reset,
                [SOC_EXC_NMI + SOCCFG_EXC_NUM] = arch_isr_nmi,
                [SOC_EXC_HARDFAULT + SOCCFG_EXC_NUM] = arch_isr_hardfault,
                [SOC_EXC_MMFAULT + SOCCFG_EXC_NUM] = arch_isr_mm,
                [SOC_EXC_BUSFAULT + SOCCFG_EXC_NUM] = arch_isr_busfault,
                [SOC_EXC_USGFAULT + SOCCFG_EXC_NUM] = arch_isr_usagefault,
                [SOC_EXC_RSVN9 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_RSVN8 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_RSVN7 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_RSVN6 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_SVCALL + SOCCFG_EXC_NUM] = (xwisr_f)arch_isr_svc,
                [SOC_EXC_DBGMON + SOCCFG_EXC_NUM] = arch_isr_dbgmon,
                [SOC_EXC_RSVN3 + SOCCFG_EXC_NUM] = arch_isr_noop,
                [SOC_EXC_PENDSV + SOCCFG_EXC_NUM] = xwospl_skd_isr_swcx,
                [SOC_EXC_SYSTICK + SOCCFG_EXC_NUM] = xwospl_syshwt_isr,
        },
        .irq = {
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
        },
};

__xwos_ivt_qualifier struct soc_idvt xwospl_idvt = {
        .exc = {
                [SOC_SP_TOP + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RESET + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_NMI + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_HARDFAULT + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_MMFAULT + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_BUSFAULT + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_USGFAULT + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN9 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN8 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN7 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN6 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_SVCALL + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_DBGMON + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_RSVN3 + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_PENDSV + SOCCFG_EXC_NUM] = NULL,
                [SOC_EXC_SYSTICK + SOCCFG_EXC_NUM] = NULL,
        },
        .irq = {
                [0 ... (SOCCFG_IRQ_NUM - 1)] = NULL,
        },
};
