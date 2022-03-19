/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <bm/stm32cube/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/uart/dma.h>

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
struct MX_UART_DriverData {
  UART_HandleTypeDef * halhdl;
  struct xwds_dmauartc * dmauartc;
  struct {
    struct xwos_cond cond; /**< 条件量 */
    struct xwos_splk splk; /**< 保证发送状态只被单一上下文访问的锁 */
    xwer_t rc;
  } tx;
};

extern struct MX_UART_DriverData huart1_drvdata;

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_USART1_UART_DeInit(void);
void MX_USART1_Timer_Init(void);
void MX_USART1_Timer_DeInit(void);
void MX_USART1_Timer_Start(void);
void MX_USART1_Timer_Stop(void);
void MX_USART1_Timer_Callback(void);
xwer_t MX_USART1_RXDMA_Start(xwu8_t * mem, xwsz_t size);
xwsq_t MX_USART1_RXDMA_GetCounter(void);
xwer_t MX_USART1_TXDMA_Start(xwu8_t * mem, xwsz_t size);
xwer_t MX_USART1_Putc(xwu8_t byte);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

