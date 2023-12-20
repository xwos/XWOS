/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
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
    struct xwos_sem available; /**< 端口可用的信号量 */
    struct xwos_cond completion; /**< 发送完成的条件量 */
    struct xwos_splk splk; /**< 保证发送状态只被单一上下文访问的锁 */
    xwer_t rc; /**< 返回值 */
    xwu8_t mem[HAL_UART_TXMEM_MAXSIZE] __xwcc_alignl1cache; /**< 发送缓冲区 */
    xwu32_t size; /**< 待发送的数据大小 */
    xwds_uartc_eqcb_f eqcb; /**< 异步发送的回调函数 */
  } tx;
};

extern struct MX_UART_DriverData huart1_drvdata;
extern struct MX_UART_DriverData huart3_drvdata;

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_USART_UART_Init(xwu32_t hwid);
void MX_USART_UART_DeInit(xwu32_t hwid);
void MX_USART_Timer_Init(xwu32_t hwid);
void MX_USART_Timer_DeInit(xwu32_t hwid);
void MX_USART_Timer_Start(xwu32_t hwid);
void MX_USART_Timer_Stop(xwu32_t hwid);
xwer_t MX_USART_RXDMA_Start(xwu32_t hwid, xwu8_t * mem, xwsz_t size);
xwsq_t MX_USART_RXDMA_GetCounter(xwu32_t hwid);
void MX_USART_TXDMA_Prepare(xwu32_t hwid, const xwu8_t * mem, xwsz_t size);
xwer_t MX_USART_TXDMA_Start(xwu32_t hwid);
xwer_t MX_USART_Putc(xwu32_t hwid, xwu8_t byte);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
