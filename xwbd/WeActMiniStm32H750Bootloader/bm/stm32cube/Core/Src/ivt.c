/**
 * @file
 * @brief STM32CUBE模块：中断向量表
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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
#include "bm/stm32cube/Core/Inc/main.h"

struct soc_ivt {
        xwisr_f exc[SOCCFG_EXC_NUM];
        xwisr_f irq[SOCCFG_IRQ_NUM];
};

void MX_NOOP_IRQHandler(void)
{
        while (true) {
        }
}

extern xwu8_t armv7m_isr_stack_top[];
void WWDG_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void PVD_AVD_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TAMP_STAMP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void RTC_WKUP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FLASH_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void RCC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream6_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void ADC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FDCAN1_IT0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FDCAN2_IT0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FDCAN1_IT1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FDCAN2_IT1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI9_5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM1_BRK_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM1_UP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM1_TRG_COM_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM1_CC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void I2C1_EV_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void I2C1_ER_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void I2C2_EV_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void I2C2_ER_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SPI1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SPI2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void USART1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void USART2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void USART3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI15_10_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void RTC_Alarm_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM8_BRK_TIM12_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM8_UP_TIM13_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM8_TRG_COM_TIM14_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM8_CC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream7_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FMC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SDMMC1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SPI3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void UART4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void UART5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM6_DAC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM7_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2_Stream0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2_Stream1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2_Stream2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2_Stream3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2_Stream4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void ETH_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void ETH_WKUP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FDCAN_CAL_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2_Stream5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2_Stream6_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2_Stream7_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void USART6_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void I2C3_EV_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void I2C3_ER_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_HS_EP1_OUT_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_HS_EP1_IN_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_HS_WKUP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_HS_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DCMI_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CRYP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HASH_RNG_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FPU_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void UART7_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void UART8_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SPI4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SPI5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SPI6_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SAI1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void LTDC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void LTDC_ER_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA2D_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SAI2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void QUADSPI_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void LPTIM1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CEC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void I2C4_EV_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void I2C4_ER_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SPDIF_RX_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_FS_EP1_OUT_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_FS_EP1_IN_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_FS_WKUP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_FS_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMAMUX1_OVR_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HRTIM1_Master_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HRTIM1_TIMA_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HRTIM1_TIMB_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HRTIM1_TIMC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HRTIM1_TIMD_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HRTIM1_TIME_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HRTIM1_FLT_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DFSDM1_FLT0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DFSDM1_FLT1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DFSDM1_FLT2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DFSDM1_FLT3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SAI3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SWPMI1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM15_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM16_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM17_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void MDIOS_WKUP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void MDIOS_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void JPEG_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void MDMA_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SDMMC2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void HSEM1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void ADC3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMAMUX2_OVR_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void BDMA_Channel0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void BDMA_Channel1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void BDMA_Channel2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void BDMA_Channel3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void BDMA_Channel4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void BDMA_Channel5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void BDMA_Channel6_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void BDMA_Channel7_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void COMP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void LPTIM2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void LPTIM3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void LPTIM4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void LPTIM5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void LPUART1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CRS_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void ECC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SAI4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void WAKEUP_PIN_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");

__xwcc_section(".armv7m.ivt") struct soc_ivt stm32_ivt = {
        .exc = {
                [SOC_SP_TOP + SOCCFG_EXC_NUM] = (xwisr_f)armv7m_isr_stack_top,
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
                [PVD_AVD_IRQn] = PVD_AVD_IRQHandler,
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
                [FDCAN1_IT0_IRQn] = FDCAN1_IT0_IRQHandler,
                [FDCAN2_IT0_IRQn] = FDCAN2_IT0_IRQHandler,
                [FDCAN1_IT1_IRQn] = FDCAN1_IT1_IRQHandler,
                [FDCAN2_IT1_IRQn] = FDCAN2_IT1_IRQHandler,
                [EXTI9_5_IRQn] = EXTI9_5_IRQHandler,
                [TIM1_BRK_IRQn] = TIM1_BRK_IRQHandler,
                [TIM1_UP_IRQn] = TIM1_UP_IRQHandler,
                [TIM1_TRG_COM_IRQn] = TIM1_TRG_COM_IRQHandler,
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
                [TIM8_BRK_TIM12_IRQn] = TIM8_BRK_TIM12_IRQHandler,
                [TIM8_UP_TIM13_IRQn] = TIM8_UP_TIM13_IRQHandler,
                [TIM8_TRG_COM_TIM14_IRQn] = TIM8_TRG_COM_TIM14_IRQHandler,
                [TIM8_CC_IRQn] = TIM8_CC_IRQHandler,
                [DMA1_Stream7_IRQn] = DMA1_Stream7_IRQHandler,
                [FMC_IRQn] = FMC_IRQHandler,
                [SDMMC1_IRQn] = SDMMC1_IRQHandler,
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
                [FDCAN_CAL_IRQn] = FDCAN_CAL_IRQHandler,
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
                [CRYP_IRQn] = CRYP_IRQHandler,
                [HASH_RNG_IRQn] = HASH_RNG_IRQHandler,
                [FPU_IRQn] = FPU_IRQHandler,
                [UART7_IRQn] = UART7_IRQHandler,
                [UART8_IRQn] = UART8_IRQHandler,
                [SPI4_IRQn] = SPI4_IRQHandler,
                [SPI5_IRQn] = SPI5_IRQHandler,
                [SPI6_IRQn] = SPI6_IRQHandler,
                [SAI1_IRQn] = SAI1_IRQHandler,
                [LTDC_IRQn] = LTDC_IRQHandler,
                [LTDC_ER_IRQn] = LTDC_ER_IRQHandler,
                [DMA2D_IRQn] = DMA2D_IRQHandler,
                [SAI2_IRQn] = SAI2_IRQHandler,
                [QUADSPI_IRQn] = QUADSPI_IRQHandler,
                [LPTIM1_IRQn] = LPTIM1_IRQHandler,
                [CEC_IRQn] = CEC_IRQHandler,
                [I2C4_EV_IRQn] = I2C4_EV_IRQHandler,
                [I2C4_ER_IRQn] = I2C4_ER_IRQHandler,
                [SPDIF_RX_IRQn] = SPDIF_RX_IRQHandler,
                [OTG_FS_EP1_OUT_IRQn] = OTG_FS_EP1_OUT_IRQHandler,
                [OTG_FS_EP1_IN_IRQn] = OTG_FS_EP1_IN_IRQHandler,
                [OTG_FS_WKUP_IRQn] = OTG_FS_WKUP_IRQHandler,
                [OTG_FS_IRQn] = OTG_FS_IRQHandler,
                [DMAMUX1_OVR_IRQn] = DMAMUX1_OVR_IRQHandler,
                [HRTIM1_Master_IRQn] = HRTIM1_Master_IRQHandler,
                [HRTIM1_TIMA_IRQn] = HRTIM1_TIMA_IRQHandler,
                [HRTIM1_TIMB_IRQn] = HRTIM1_TIMB_IRQHandler,
                [HRTIM1_TIMC_IRQn] = HRTIM1_TIMC_IRQHandler,
                [HRTIM1_TIMD_IRQn] = HRTIM1_TIMD_IRQHandler,
                [HRTIM1_TIME_IRQn] = HRTIM1_TIME_IRQHandler,
                [HRTIM1_FLT_IRQn] = HRTIM1_FLT_IRQHandler,
                [DFSDM1_FLT0_IRQn] = DFSDM1_FLT0_IRQHandler,
                [DFSDM1_FLT1_IRQn] = DFSDM1_FLT1_IRQHandler,
                [DFSDM1_FLT2_IRQn] = DFSDM1_FLT2_IRQHandler,
                [DFSDM1_FLT3_IRQn] = DFSDM1_FLT3_IRQHandler,
                [SAI3_IRQn] = SAI3_IRQHandler,
                [SWPMI1_IRQn] = SWPMI1_IRQHandler,
                [TIM15_IRQn] = TIM15_IRQHandler,
                [TIM16_IRQn] = TIM16_IRQHandler,
                [TIM17_IRQn] = TIM17_IRQHandler,
                [MDIOS_WKUP_IRQn] = MDIOS_WKUP_IRQHandler,
                [MDIOS_IRQn] = MDIOS_IRQHandler,
                [JPEG_IRQn] = JPEG_IRQHandler,
                [MDMA_IRQn] = MDMA_IRQHandler,
                [SDMMC2_IRQn] = SDMMC2_IRQHandler,
                [HSEM1_IRQn] = HSEM1_IRQHandler,
                [ADC3_IRQn] = ADC3_IRQHandler,
                [DMAMUX2_OVR_IRQn] = DMAMUX2_OVR_IRQHandler,
                [BDMA_Channel0_IRQn] = BDMA_Channel0_IRQHandler,
                [BDMA_Channel1_IRQn] = BDMA_Channel1_IRQHandler,
                [BDMA_Channel2_IRQn] = BDMA_Channel2_IRQHandler,
                [BDMA_Channel3_IRQn] = BDMA_Channel3_IRQHandler,
                [BDMA_Channel4_IRQn] = BDMA_Channel4_IRQHandler,
                [BDMA_Channel5_IRQn] = BDMA_Channel5_IRQHandler,
                [BDMA_Channel6_IRQn] = BDMA_Channel6_IRQHandler,
                [BDMA_Channel7_IRQn] = BDMA_Channel7_IRQHandler,
                [COMP_IRQn] = COMP_IRQHandler,
                [LPTIM2_IRQn] = LPTIM2_IRQHandler,
                [LPTIM3_IRQn] = LPTIM3_IRQHandler,
                [LPTIM4_IRQn] = LPTIM4_IRQHandler,
                [LPTIM5_IRQn] = LPTIM5_IRQHandler,
                [LPUART1_IRQn] = LPUART1_IRQHandler,
                [CRS_IRQn] = CRS_IRQHandler,
                [ECC_IRQn] = ECC_IRQHandler,
                [SAI4_IRQn] = SAI4_IRQHandler,
                [WAKEUP_PIN_IRQn] = WAKEUP_PIN_IRQHandler,
        },
};

void stm32cube_init_vtor(void)
{
        SCB->VTOR = (xwu32_t)&stm32_ivt;
}
