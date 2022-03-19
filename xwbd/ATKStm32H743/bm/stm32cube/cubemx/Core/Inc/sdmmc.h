/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sdmmc.h
  * @brief   This file contains all the function prototypes for
  *          the sdmmc.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDMMC_H__
#define __SDMMC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SD_HandleTypeDef hsd1;

/* USER CODE BEGIN Private defines */
#define MX_SD_SECTOR_SIZE BLOCKSIZE

/* USER CODE END Private defines */

void MX_SDMMC1_SD_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_SDMMC1_SD_DeInit(void);
void MX_SDMMC1_SD_ReInit(uint32_t clkdiv);
xwer_t MX_SDMMC1_SD_TrimClk(xwsq_t cnt);
xwer_t MX_SDMMC1_SD_GetState(void);
xwer_t MX_SDMMC1_SD_Read(uint8_t * buf, uint32_t blkaddr,
                         uint32_t nrblk);
xwer_t MX_SDMMC1_SD_Write(uint8_t * data, uint32_t blkaddr,
                          uint32_t nrblk);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SDMMC_H__ */

