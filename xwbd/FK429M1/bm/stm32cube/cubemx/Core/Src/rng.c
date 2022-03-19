/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rng.c
  * @brief   This file provides code for the configuration
  *          of the RNG instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * Copyright (c) 2022 xwos.tech.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rng.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* RNG init function */
void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* Peripheral clock enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG);

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  LL_RNG_Enable(RNG);
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
