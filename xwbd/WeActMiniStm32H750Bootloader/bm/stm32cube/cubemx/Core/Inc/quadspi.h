/**
  ******************************************************************************
  * @file    quadspi.h
  * @brief   This file contains all the function prototypes for
  *          the quadspi.c file
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
