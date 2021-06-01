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
