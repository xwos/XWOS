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
#include "stm32h743xx.h"
#include <bm/stm32cube/cubemx/Core/Inc/stm32h7xx_it.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>

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
                [WWDG_IRQn] = arch_isr_nop,
                [PVD_AVD_IRQn] = arch_isr_nop,
                [TAMP_STAMP_IRQn] = arch_isr_nop,
                [RTC_WKUP_IRQn] = arch_isr_nop,
                [FLASH_IRQn] = arch_isr_nop,
                [RCC_IRQn] = arch_isr_nop,
                [EXTI0_IRQn] = arch_isr_nop,
                [EXTI1_IRQn] = arch_isr_nop,
                [EXTI2_IRQn] = arch_isr_nop,
                [EXTI3_IRQn] = arch_isr_nop,
                [EXTI4_IRQn] = arch_isr_nop,
                [DMA1_Stream0_IRQn] = DMA1_Stream0_IRQHandler,
                [DMA1_Stream1_IRQn] = DMA1_Stream1_IRQHandler,
                [DMA1_Stream2_IRQn] = DMA1_Stream2_IRQHandler,
                [DMA1_Stream3_IRQn] = DMA1_Stream3_IRQHandler,
                [DMA1_Stream4_IRQn] = arch_isr_nop,
                [DMA1_Stream5_IRQn] = arch_isr_nop,
                [DMA1_Stream6_IRQn] = arch_isr_nop,
                [ADC_IRQn] = arch_isr_nop,
                [FDCAN1_IT0_IRQn] = arch_isr_nop,
                [FDCAN2_IT0_IRQn] = arch_isr_nop,
                [FDCAN1_IT1_IRQn] = arch_isr_nop,
                [FDCAN2_IT1_IRQn] = arch_isr_nop,
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
                [TIM8_BRK_TIM12_IRQn] = arch_isr_nop,
                [TIM8_UP_TIM13_IRQn] = arch_isr_nop,
                [TIM8_TRG_COM_TIM14_IRQn] = arch_isr_nop,
                [TIM8_CC_IRQn] = arch_isr_nop,
                [DMA1_Stream7_IRQn] = arch_isr_nop,
                [FMC_IRQn] = arch_isr_nop,
                [SDMMC1_IRQn] = arch_isr_nop,
                [TIM5_IRQn] = arch_isr_nop,
                [SPI3_IRQn] = arch_isr_nop,
                [UART4_IRQn] = arch_isr_nop,
                [UART5_IRQn] = arch_isr_nop,
                [TIM6_DAC_IRQn] = arch_isr_nop,
                [TIM7_IRQn] = arch_isr_nop,
                [DMA2_Stream0_IRQn] = arch_isr_nop,
                [DMA2_Stream1_IRQn] = arch_isr_nop,
                [DMA2_Stream2_IRQn] = arch_isr_nop,
                [DMA2_Stream3_IRQn] = arch_isr_nop,
                [DMA2_Stream4_IRQn] = arch_isr_nop,
                [ETH_IRQn] = arch_isr_nop,
                [ETH_WKUP_IRQn] = arch_isr_nop,
                [FDCAN_CAL_IRQn] = arch_isr_nop,
                [DMA2_Stream5_IRQn] = arch_isr_nop,
                [DMA2_Stream6_IRQn] = arch_isr_nop,
                [DMA2_Stream7_IRQn] = arch_isr_nop,
                [USART6_IRQn] = arch_isr_nop,
                [I2C3_EV_IRQn] = arch_isr_nop,
                [I2C3_ER_IRQn] = arch_isr_nop,
                [OTG_HS_EP1_OUT_IRQn] = arch_isr_nop,
                [OTG_HS_EP1_IN_IRQn] = arch_isr_nop,
                [OTG_HS_WKUP_IRQn] = arch_isr_nop,
                [OTG_HS_IRQn] = arch_isr_nop,
                [DCMI_IRQn] = arch_isr_nop,
                [RNG_IRQn] = arch_isr_nop,
                [FPU_IRQn] = arch_isr_nop,
                [UART7_IRQn] = arch_isr_nop,
                [UART8_IRQn] = arch_isr_nop,
                [SPI4_IRQn] = arch_isr_nop,
                [SPI5_IRQn] = arch_isr_nop,
                [SPI6_IRQn] = arch_isr_nop,
                [SAI1_IRQn] = arch_isr_nop,
                [LTDC_IRQn] = arch_isr_nop,
                [LTDC_ER_IRQn] = arch_isr_nop,
                [DMA2D_IRQn] = arch_isr_nop,
                [SAI2_IRQn] = arch_isr_nop,
                [QUADSPI_IRQn] = arch_isr_nop,
                [LPTIM1_IRQn] = arch_isr_nop,
                [CEC_IRQn] = arch_isr_nop,
                [I2C4_EV_IRQn] = arch_isr_nop,
                [I2C4_ER_IRQn] = arch_isr_nop,
                [SPDIF_RX_IRQn] = arch_isr_nop,
                [OTG_FS_EP1_OUT_IRQn] = arch_isr_nop,
                [OTG_FS_EP1_IN_IRQn] = arch_isr_nop,
                [OTG_FS_WKUP_IRQn] = arch_isr_nop,
                [OTG_FS_IRQn] = arch_isr_nop,
                [DMAMUX1_OVR_IRQn] = arch_isr_nop,
                [HRTIM1_Master_IRQn] = arch_isr_nop,
                [HRTIM1_TIMA_IRQn] = arch_isr_nop,
                [HRTIM1_TIMB_IRQn] = arch_isr_nop,
                [HRTIM1_TIMC_IRQn] = arch_isr_nop,
                [HRTIM1_TIMD_IRQn] = arch_isr_nop,
                [HRTIM1_TIME_IRQn] = arch_isr_nop,
                [HRTIM1_FLT_IRQn] = arch_isr_nop,
                [DFSDM1_FLT0_IRQn] = arch_isr_nop,
                [DFSDM1_FLT1_IRQn] = arch_isr_nop,
                [DFSDM1_FLT2_IRQn] = arch_isr_nop,
                [DFSDM1_FLT3_IRQn] = arch_isr_nop,
                [SAI3_IRQn] = arch_isr_nop,
                [SWPMI1_IRQn] = arch_isr_nop,
                [TIM15_IRQn] = arch_isr_nop,
                [TIM16_IRQn] = arch_isr_nop,
                [TIM17_IRQn] = arch_isr_nop,
                [MDIOS_WKUP_IRQn] = arch_isr_nop,
                [MDIOS_IRQn] = arch_isr_nop,
                [JPEG_IRQn] = arch_isr_nop,
                [MDMA_IRQn] = arch_isr_nop,
                [SDMMC2_IRQn] = arch_isr_nop,
                [HSEM1_IRQn] = arch_isr_nop,
                [ADC3_IRQn] = arch_isr_nop,
                [DMAMUX2_OVR_IRQn] = arch_isr_nop,
                [BDMA_Channel0_IRQn] = arch_isr_nop,
                [BDMA_Channel1_IRQn] = arch_isr_nop,
                [BDMA_Channel2_IRQn] = arch_isr_nop,
                [BDMA_Channel3_IRQn] = arch_isr_nop,
                [BDMA_Channel4_IRQn] = arch_isr_nop,
                [BDMA_Channel5_IRQn] = arch_isr_nop,
                [BDMA_Channel6_IRQn] = arch_isr_nop,
                [BDMA_Channel7_IRQn] = arch_isr_nop,
                [COMP_IRQn] = arch_isr_nop,
                [LPTIM2_IRQn] = arch_isr_nop,
                [LPTIM3_IRQn] = arch_isr_nop,
                [LPTIM4_IRQn] = arch_isr_nop,
                [LPTIM5_IRQn] = arch_isr_nop,
                [LPUART1_IRQn] = arch_isr_nop,
                [CRS_IRQn] = arch_isr_nop,
                [ECC_IRQn] = arch_isr_nop,
                [SAI4_IRQn] = arch_isr_nop,
                [WAKEUP_PIN_IRQn] = arch_isr_nop,

                [150 ... (SOCCFG_IRQ_NUM - 1)] = arch_isr_nop,
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
                [PVD_AVD_IRQn] = NULL,
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
                [FDCAN1_IT0_IRQn] = NULL,
                [FDCAN2_IT0_IRQn] = NULL,
                [FDCAN1_IT1_IRQn] = NULL,
                [FDCAN2_IT1_IRQn] = NULL,
                [EXTI9_5_IRQn] = NULL,
                [TIM1_BRK_IRQn] = NULL,
                [TIM1_UP_IRQn] = NULL,
                [TIM1_TRG_COM_IRQn] = NULL,
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
                [TIM8_BRK_TIM12_IRQn] = NULL,
                [TIM8_UP_TIM13_IRQn] = NULL,
                [TIM8_TRG_COM_TIM14_IRQn] = NULL,
                [TIM8_CC_IRQn] = NULL,
                [DMA1_Stream7_IRQn] = NULL,
                [FMC_IRQn] = NULL,
                [SDMMC1_IRQn] = NULL,
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
                [FDCAN_CAL_IRQn] = NULL,
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
                [RNG_IRQn] = NULL,
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
                [SAI2_IRQn] = NULL,
                [QUADSPI_IRQn] = NULL,
                [LPTIM1_IRQn] = NULL,
                [CEC_IRQn] = NULL,
                [I2C4_EV_IRQn] = NULL,
                [I2C4_ER_IRQn] = NULL,
                [SPDIF_RX_IRQn] = NULL,
                [OTG_FS_EP1_OUT_IRQn] = NULL,
                [OTG_FS_EP1_IN_IRQn] = NULL,
                [OTG_FS_WKUP_IRQn] = NULL,
                [OTG_FS_IRQn] = NULL,
                [DMAMUX1_OVR_IRQn] = NULL,
                [HRTIM1_Master_IRQn] = NULL,
                [HRTIM1_TIMA_IRQn] = NULL,
                [HRTIM1_TIMB_IRQn] = NULL,
                [HRTIM1_TIMC_IRQn] = NULL,
                [HRTIM1_TIMD_IRQn] = NULL,
                [HRTIM1_TIME_IRQn] = NULL,
                [HRTIM1_FLT_IRQn] = NULL,
                [DFSDM1_FLT0_IRQn] = NULL,
                [DFSDM1_FLT1_IRQn] = NULL,
                [DFSDM1_FLT2_IRQn] = NULL,
                [DFSDM1_FLT3_IRQn] = NULL,
                [SAI3_IRQn] = NULL,
                [SWPMI1_IRQn] = NULL,
                [TIM15_IRQn] = NULL,
                [TIM16_IRQn] = NULL,
                [TIM17_IRQn] = NULL,
                [MDIOS_WKUP_IRQn] = NULL,
                [MDIOS_IRQn] = NULL,
                [JPEG_IRQn] = NULL,
                [MDMA_IRQn] = NULL,
                [SDMMC2_IRQn] = NULL,
                [HSEM1_IRQn] = NULL,
                [ADC3_IRQn] = NULL,
                [DMAMUX2_OVR_IRQn] = NULL,
                [BDMA_Channel0_IRQn] = NULL,
                [BDMA_Channel1_IRQn] = NULL,
                [BDMA_Channel2_IRQn] = NULL,
                [BDMA_Channel3_IRQn] = NULL,
                [BDMA_Channel4_IRQn] = NULL,
                [BDMA_Channel5_IRQn] = NULL,
                [BDMA_Channel6_IRQn] = NULL,
                [BDMA_Channel7_IRQn] = NULL,
                [COMP_IRQn] = NULL,
                [LPTIM2_IRQn] = NULL,
                [LPTIM3_IRQn] = NULL,
                [LPTIM4_IRQn] = NULL,
                [LPTIM5_IRQn] = NULL,
                [LPUART1_IRQn] = NULL,
                [CRS_IRQn] = NULL,
                [ECC_IRQn] = NULL,
                [SAI4_IRQn] = NULL,
                [WAKEUP_PIN_IRQn] = NULL,

                [150 ... (SOCCFG_IRQ_NUM - 1)] = NULL,
        },
};