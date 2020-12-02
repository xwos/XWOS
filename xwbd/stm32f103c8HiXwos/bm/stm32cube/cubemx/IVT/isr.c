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
void TAMPER_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void RTC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void FLASH_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void RCC_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void EXTI4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel3_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel4_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel5_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel6_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel7_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void ADC1_2_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void USB_HP_CAN1_TX_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void USB_LP_CAN1_RX0_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CAN1_RX1_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
void CAN1_SCE_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
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
void USBWakeUp_IRQHandler(void) __xwcc_weakalias("MX_NOOP_IRQHandler");
