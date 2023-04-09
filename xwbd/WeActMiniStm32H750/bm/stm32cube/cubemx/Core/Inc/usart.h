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
#include <xwcd/ds/uart/controller.h>

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
#define HAL_UART_TXMEM_MAXSIZE    512U /**< 单次最大发送的字节数 */

struct MX_UART_DriverData {
  UART_HandleTypeDef * halhdl;
  struct xwds_uartc * uartc;
  struct {
    struct xwos_cond cond; /**< 条件量 */
    struct xwos_splk splk; /**< 保证发送状态只被单一上下文访问的锁 */
    xwer_t rc; /**< 返回值 */
    xwu8_t mem[HAL_UART_TXMEM_MAXSIZE] __xwcc_alignl1cache; /**< 发送缓冲区 */
    xwu32_t size; /**< 待发送的数据大小 */
  } tx;
};

extern struct MX_UART_DriverData huart1_drvdata;
extern struct MX_UART_DriverData huart3_drvdata;

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_USART1_UART_DeInit(void);
void MX_USART1_Timer_Init(void);
void MX_USART1_Timer_DeInit(void);
void MX_USART1_Timer_Start(void);
void MX_USART1_Timer_Stop(void);
xwer_t MX_USART1_RXDMA_Start(xwu8_t * mem, xwsz_t size);
xwsq_t MX_USART1_RXDMA_GetCounter(void);
void MX_USART1_TXDMA_Prepare(const xwu8_t * mem, xwsz_t size);
xwer_t MX_USART1_TXDMA_Start(void);
xwer_t MX_USART1_Putc(xwu8_t byte);

void MX_USART3_UART_DeInit(void);
void MX_USART3_Timer_Init(void);
void MX_USART3_Timer_DeInit(void);
void MX_USART3_Timer_Start(void);
void MX_USART3_Timer_Stop(void);
xwer_t MX_USART3_RXDMA_Start(xwu8_t * mem, xwsz_t size);
xwsq_t MX_USART3_RXDMA_GetCounter(void);
void MX_USART3_TXDMA_Prepare(const xwu8_t * mem, xwsz_t size);
xwer_t MX_USART3_TXDMA_Start(void);
xwer_t MX_USART3_Putc(xwu8_t byte);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
