/**
  ******************************************************************************
  * File Name          : SDIO.h
  * Description        : This file provides code for the configuration
  *                      of the SDIO instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2020
  * 隐星魂 (Roy.Sun) https://xwos.tech
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __sdio_H
#define __sdio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SD_HandleTypeDef hsd;

/* USER CODE BEGIN Private defines */
#define MX_SD_SECTOR_SIZE 512U

/* USER CODE END Private defines */

void MX_SDIO_SD_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_SDIO_SD_Construct(void);
void MX_SDIO_SD_Destruct(void);
void MX_SDIO_SD_Deinit(void);
xwer_t MX_SDIO_SD_TrimClk(xwsq_t cnt);
xwer_t MX_SDIO_SD_GetState(void);
xwer_t MX_SDIO_SD_Read(uint8_t * data, uint32_t blkaddr,
                       uint32_t nrblk);
xwer_t MX_SDIO_SD_Write(uint8_t * data, uint32_t blkaddr,
                        uint32_t nrblk);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ sdio_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
