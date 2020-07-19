/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32h7xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization
  *                      and de-Initialization codes.
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
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);

  /* System interrupt init*/
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 1);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 7, 1);

  /* Peripheral interrupt init */
  /* PVD_AVD_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PVD_AVD_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(PVD_AVD_IRQn);
  /* WAKEUP_PIN_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(WAKEUP_PIN_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(WAKEUP_PIN_IRQn);

  /* USER CODE BEGIN MspInit 1 */
  HAL_EnableDBGSleepMode();
  HAL_EnableDBGStopMode();
  HAL_EnableDBGStandbyMode();

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
