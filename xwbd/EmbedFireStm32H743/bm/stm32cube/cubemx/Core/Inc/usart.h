/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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

extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define HAL_UART_TXMEM_MAXSIZE    512U /**< 单次最大发送的字节数 */

struct MX_UART_DriverData {
  UART_HandleTypeDef * halhdl;
  struct xwds_dmauartc * dmauartc;
  struct {
    struct xwos_cond cond; /**< 条件量 */
    struct xwos_splk splk; /**< 保证发送状态只被单一上下文访问的锁 */
    xwer_t rc; /**< 返回值 */
    xwu8_t mem[HAL_UART_TXMEM_MAXSIZE] __xwcc_alignl1cache; /**< 发送缓冲区 */
    xwu32_t size; /**< 待发送的数据大小 */
  } tx;
};

extern struct MX_UART_DriverData huart5_drvdata;
extern struct MX_UART_DriverData huart1_drvdata;

/* USER CODE END Private defines */

void MX_UART5_Init(void);
void MX_USART1_UART_Init(void);

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

void MX_UART5_DeInit(void);
void MX_UART5_Timer_Init(void);
void MX_UART5_Timer_DeInit(void);
void MX_UART5_Timer_Start(void);
void MX_UART5_Timer_Stop(void);
xwer_t MX_UART5_RXDMA_Start(xwu8_t * mem, xwsz_t size);
xwsq_t MX_UART5_RXDMA_GetCounter(void);
void MX_UART5_TXDMA_Prepare(const xwu8_t * mem, xwsz_t size);
xwer_t MX_UART5_TXDMA_Start(void);
xwer_t MX_UART5_Putc(xwu8_t byte);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
