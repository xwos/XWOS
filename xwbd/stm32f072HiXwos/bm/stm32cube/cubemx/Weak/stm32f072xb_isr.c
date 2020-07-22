/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Weak/STM32F072xB_isr.c
  * @brief   Weak Interrupt Service Routines.
  ******************************************************************************
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
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MX_NOOP_IRQHandler(void)
{
  while (true) {
  }
}

void WWDG_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void PVD_VDDIO2_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void RTC_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void FLASH_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void RCC_CRS_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void EXTI0_1_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void EXTI2_3_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void EXTI4_15_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TSC_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel1_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel2_3_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void DMA1_Channel4_5_6_7_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void ADC1_COMP_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM1_BRK_UP_TRG_COM_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM1_CC_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM2_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM3_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM6_DAC_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM7_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM14_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM15_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM16_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void TIM17_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void I2C1_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void I2C2_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void SPI1_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void SPI2_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void USART1_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void USART2_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void USART3_4_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void CEC_CAN_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");
void USB_IRQHandler(void) __weakalias("MX_NOOP_IRQHandler");

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
