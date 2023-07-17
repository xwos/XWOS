/**
 * @file
 * @brief STM32CUBE模块：Interrupt Service Routines
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

#ifndef __stm32cube_cubemx_IVT_isr_h__
#define __stm32cube_cubemx_IVT_isr_h__

#include "standard.h"
#include "cubemx/Core/Inc/main.h"

struct soc_ivt {
        xwisr_f exc[SOCCFG_EXC_NUM];
        xwisr_f irq[SOCCFG_IRQ_NUM];
};

extern struct soc_ivt stm32_ivt;

void WWDG_IRQHandler(void);
void PVD_AVD_IRQHandler(void);
void TAMP_STAMP_IRQHandler(void);
void RTC_WKUP_IRQHandler(void);
void FLASH_IRQHandler(void);
void RCC_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void DMA1_Stream0_IRQHandler(void);
void DMA1_Stream1_IRQHandler(void);
void DMA1_Stream2_IRQHandler(void);
void DMA1_Stream3_IRQHandler(void);
void DMA1_Stream4_IRQHandler(void);
void DMA1_Stream5_IRQHandler(void);
void DMA1_Stream6_IRQHandler(void);
void ADC_IRQHandler(void);
void FDCAN1_IT0_IRQHandler(void);
void FDCAN2_IT0_IRQHandler(void);
void FDCAN1_IT1_IRQHandler(void);
void FDCAN2_IT1_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void TIM1_BRK_IRQHandler(void);
void TIM1_UP_IRQHandler(void);
void TIM1_TRG_COM_IRQHandler(void);
void TIM1_CC_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);
void I2C2_EV_IRQHandler(void);
void I2C2_ER_IRQHandler(void);
void SPI1_IRQHandler(void);
void SPI2_IRQHandler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void RTC_Alarm_IRQHandler(void);
void TIM8_BRK_TIM12_IRQHandler(void);
void TIM8_UP_TIM13_IRQHandler(void);
void TIM8_TRG_COM_TIM14_IRQHandler(void);
void TIM8_CC_IRQHandler(void);
void DMA1_Stream7_IRQHandler(void);
void FMC_IRQHandler(void);
void SDMMC1_IRQHandler(void);
void TIM5_IRQHandler(void);
void SPI3_IRQHandler(void);
void UART4_IRQHandler(void);
void UART5_IRQHandler(void);
void TIM6_DAC_IRQHandler(void);
void TIM7_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void);
void DMA2_Stream1_IRQHandler(void);
void DMA2_Stream2_IRQHandler(void);
void DMA2_Stream3_IRQHandler(void);
void DMA2_Stream4_IRQHandler(void);
void ETH_IRQHandler(void);
void ETH_WKUP_IRQHandler(void);
void FDCAN_CAL_IRQHandler(void);
void DMA2_Stream5_IRQHandler(void);
void DMA2_Stream6_IRQHandler(void);
void DMA2_Stream7_IRQHandler(void);
void USART6_IRQHandler(void);
void I2C3_EV_IRQHandler(void);
void I2C3_ER_IRQHandler(void);
void OTG_HS_EP1_OUT_IRQHandler(void);
void OTG_HS_EP1_IN_IRQHandler(void);
void OTG_HS_WKUP_IRQHandler(void);
void OTG_HS_IRQHandler(void);
void DCMI_IRQHandler(void);
void RNG_IRQHandler(void);
void FPU_IRQHandler(void);
void UART7_IRQHandler(void);
void UART8_IRQHandler(void);
void SPI4_IRQHandler(void);
void SPI5_IRQHandler(void);
void SPI6_IRQHandler(void);
void SAI1_IRQHandler(void);
void LTDC_IRQHandler(void);
void LTDC_ER_IRQHandler(void);
void DMA2D_IRQHandler(void);
void SAI2_IRQHandler(void);
void QUADSPI_IRQHandler(void);
void LPTIM1_IRQHandler(void);
void CEC_IRQHandler(void);
void I2C4_EV_IRQHandler(void);
void I2C4_ER_IRQHandler(void);
void SPDIF_RX_IRQHandler(void);
void OTG_FS_EP1_OUT_IRQHandler(void);
void OTG_FS_EP1_IN_IRQHandler(void);
void OTG_FS_WKUP_IRQHandler(void);
void OTG_FS_IRQHandler(void);
void DMAMUX1_OVR_IRQHandler(void);
void HRTIM1_Master_IRQHandler(void);
void HRTIM1_TIMA_IRQHandler(void);
void HRTIM1_TIMB_IRQHandler(void);
void HRTIM1_TIMC_IRQHandler(void);
void HRTIM1_TIMD_IRQHandler(void);
void HRTIM1_TIME_IRQHandler(void);
void HRTIM1_FLT_IRQHandler(void);
void DFSDM1_FLT0_IRQHandler(void);
void DFSDM1_FLT1_IRQHandler(void);
void DFSDM1_FLT2_IRQHandler(void);
void DFSDM1_FLT3_IRQHandler(void);
void SAI3_IRQHandler(void);
void SWPMI1_IRQHandler(void);
void TIM15_IRQHandler(void);
void TIM16_IRQHandler(void);
void TIM17_IRQHandler(void);
void MDIOS_WKUP_IRQHandler(void);
void MDIOS_IRQHandler(void);
void JPEG_IRQHandler(void);
void MDMA_IRQHandler(void);
void SDMMC2_IRQHandler(void);
void HSEM1_IRQHandler(void);
void ADC3_IRQHandler(void);
void DMAMUX2_OVR_IRQHandler(void);
void BDMA_Channel0_IRQHandler(void);
void BDMA_Channel1_IRQHandler(void);
void BDMA_Channel2_IRQHandler(void);
void BDMA_Channel3_IRQHandler(void);
void BDMA_Channel4_IRQHandler(void);
void BDMA_Channel5_IRQHandler(void);
void BDMA_Channel6_IRQHandler(void);
void BDMA_Channel7_IRQHandler(void);
void COMP_IRQHandler(void);
void LPTIM2_IRQHandler(void);
void LPTIM3_IRQHandler(void);
void LPTIM4_IRQHandler(void);
void LPTIM5_IRQHandler(void);
void LPUART1_IRQHandler(void);
void CRS_IRQHandler(void);
void ECC_IRQHandler(void);
void SAI4_IRQHandler(void);
void WAKEUP_PIN_IRQHandler(void);

#endif /* stm32cube/cubemx/IVT/isr.h */
