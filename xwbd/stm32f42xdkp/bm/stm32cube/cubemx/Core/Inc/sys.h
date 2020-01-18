/**
  ******************************************************************************
  * File Name          : SYS.h
  * Description        : This file provides code for the configuration
  *                      of the SYS instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#ifndef __sys_H
#define __sys_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
enum stm32cube_soc_dma_rc {
  STM32CUBE_SOC_DMA_RC_FE = BIT(0), /**< FIFO error */
  STM32CUBE_SOC_DMA_RC_DME = BIT(2), /**< Direct Mode error */
  STM32CUBE_SOC_DMA_RC_TE = BIT(3), /**< Transfer error */
  STM32CUBE_SOC_DMA_RC_HT = BIT(4), /**< Half transfer */
  STM32CUBE_SOC_DMA_RC_TC = BIT(5), /**< Transfer complete */
};

/* USER CODE END Private defines */

void MX_SYS_Init(void);

/* USER CODE BEGIN Prototypes */
void stm32cube_exti_isr(void);

void stm32cube_dma1_stream0_isr(void);

void stm32cube_dma1_stream1_isr(void);

void stm32cube_dma1_stream2_isr(void);

void stm32cube_dma1_stream3_isr(void);

void stm32cube_dma1_stream4_isr(void);

void stm32cube_dma1_stream5_isr(void);

void stm32cube_dma1_stream6_isr(void);

void stm32cube_dma1_stream7_isr(void);

void stm32cube_dma2_stream0_isr(void);

void stm32cube_dma2_stream1_isr(void);

void stm32cube_dma2_stream2_isr(void);

void stm32cube_dma2_stream3_isr(void);

void stm32cube_dma2_stream4_isr(void);

void stm32cube_dma2_stream5_isr(void);

void stm32cube_dma2_stream6_isr(void);

void stm32cube_dma2_stream7_isr(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ sys_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
