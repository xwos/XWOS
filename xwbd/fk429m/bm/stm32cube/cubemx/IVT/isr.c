/**
 * @file
 * @brief STM32CUBE模块：Weak Interrupt Service Routines
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

#include "standard.h"
#include "cubemx/Core/Inc/main.h"

void MX_NOOP_IRQHandler(void)
{
  while (true) {
  }
}

void WWDG_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void PVD_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
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
void CAN1_TX_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CAN1_RX0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CAN1_RX1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CAN1_SCE_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI9_5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM1_BRK_TIM9_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM1_UP_TIM10_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM1_TRG_COM_TIM11_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
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
void OTG_FS_WKUP_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM8_BRK_TIM12_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM8_UP_TIM13_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM8_TRG_COM_TIM14_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void TIM8_CC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Stream7_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FMC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void SDIO_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
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
void CAN2_TX_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CAN2_RX0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CAN2_RX1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CAN2_SCE_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void OTG_FS_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
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
