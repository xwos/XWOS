/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* TIM9 init function */
void MX_TIM9_Init(void)
{
  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);

  /* TIM9 interrupt Init */
  NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),2, 1));
  NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

  TIM_InitStruct.Prescaler = 16799;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 100;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM9, &TIM_InitStruct);
  LL_TIM_EnableARRPreload(TIM9);
  LL_TIM_SetClockSource(TIM9, LL_TIM_CLOCKSOURCE_INTERNAL);

}

/* USER CODE BEGIN 1 */
void MX_TIM9_Start(void)
{
  MX_TIM9_Init();
  LL_TIM_ClearFlag_UPDATE(TIM9);
  LL_TIM_EnableIT_UPDATE(TIM9);
  LL_TIM_EnableCounter(TIM9);
}

void MX_TIM9_Stop(void)
{
  LL_TIM_DisableCounter(TIM9);
  LL_TIM_DisableIT_UPDATE(TIM9);
  NVIC_DisableIRQ(TIM1_BRK_TIM9_IRQn);
  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM9);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
