/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.h
  * @brief   This file contains all the function prototypes for
  *          the quadspi.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * Copyright (c) 2023 xwos.tech.
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
#ifndef __QUADSPI_H__
#define __QUADSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern QSPI_HandleTypeDef hqspi;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_QUADSPI_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_W25Q_Init(void);
void MX_W25Q_DeInit(void);
xwer_t MX_W25Q_Write_Enable(void);
xwer_t MX_W25Q_Write_Disable(void);
xwer_t MX_W25Q_Read_UID(xwu64_t * uidbuf);
xwer_t MX_W25Q_Read_MID(xwu16_t * midbuf);
xwer_t MX_W25Q_Read_JID(xwu32_t * jidbuf);
xwer_t MX_W25Q_Set_BusMode(xwu32_t busmode);
xwer_t MX_W25Q_Set_AddrBits(xwu32_t addrbits);
xwer_t MX_W25Q_Mmap(void);
xwer_t MX_W25Q_Reset(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __QUADSPI_H__ */

