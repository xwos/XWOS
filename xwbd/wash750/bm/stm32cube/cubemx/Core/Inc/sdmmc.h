/**
  ******************************************************************************
  * @file    sdmmc.h
  * @brief   This file contains all the function prototypes for
  *          the sdmmc.c file
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
