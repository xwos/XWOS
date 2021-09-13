/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <bm/stm32cube/xwac/xwds/uart.h>

struct MX_UART_DriverData huart1_drvdata;
struct MX_UART_DriverData huart3_drvdata;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 1000000;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB14     ------> USART1_TX
    PB15     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Stream0;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_USART1_RX;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Stream1;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_USART1_TX;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */
    huart1_drvdata.halhdl = &huart1;
    xwos_splk_init(&huart1_drvdata.tx.splk);
    xwos_cond_init(&huart1_drvdata.tx.cond);
    huart1_drvdata.tx.rc = -ECANCELED;
    huart1_drvdata.tx.size = 0;
  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_RX Init */
    hdma_usart3_rx.Instance = DMA2_Stream0;
    hdma_usart3_rx.Init.Request = DMA_REQUEST_USART3_RX;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);

    /* USART3_TX Init */
    hdma_usart3_tx.Instance = DMA2_Stream1;
    hdma_usart3_tx.Init.Request = DMA_REQUEST_USART3_TX;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart3_tx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */
    huart3_drvdata.halhdl = &huart3;
    xwos_splk_init(&huart3_drvdata.tx.splk);
    xwos_cond_init(&huart3_drvdata.tx.cond);
    huart3_drvdata.tx.rc = -ECANCELED;
    huart3_drvdata.tx.size = 0;
  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */
    xwos_cond_fini(&huart1_drvdata.tx.cond);
  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PB14     ------> USART1_TX
    PB15     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14|GPIO_PIN_15);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */
    xwos_cond_fini(&huart3_drvdata.tx.cond);
  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/* USART1 */
void MX_USART1_UART_DeInit(void)
{
  HAL_UART_DMAStop(&huart1);
  HAL_UART_DeInit(&huart1);
}

void MX_USART1_Timer_Init(void)
{
  HAL_UART_ReceiverTimeout_Config(&huart1, 20000);
  HAL_UART_EnableReceiverTimeout(&huart1);
}

void MX_USART1_Timer_DeInit(void)
{
  HAL_UART_DisableReceiverTimeout(&huart1);
  HAL_UART_ReceiverTimeout_Config(&huart1, 0);
}

void MX_USART1_Timer_Start(void)
{
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RTO);
}

void MX_USART1_Timer_Stop(void)
{
  __HAL_UART_DISABLE_IT(&huart1, UART_IT_RTO);
}

xwer_t MX_USART1_RXDMA_Start(xwu8_t * mem, xwsz_t size)
{
  xwer_t rc;
  HAL_StatusTypeDef ret;

  ret = HAL_UART_Receive_DMA(&huart1, (uint8_t *)mem, (uint16_t)size);
  if (HAL_OK == ret) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_USART1_RxHalfCpltCallback(UART_HandleTypeDef * huart)
{
  DMA_HandleTypeDef * hdma;

  hdma = huart->hdmarx;
  if (HAL_DMA_ERROR_NONE == hdma->ErrorCode) {
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_InvalidateDCache_by_Addr((uint32_t *)huart1_drvdata.dmauartc->rxq.mem,
                                 XWBOP_ALIGN(sizeof(huart1_drvdata.dmauartc->rxq.mem),
                                             CPUCFG_L1_CACHELINE_SIZE));
#endif
    stm32cube_usart1_cb_rxdma_halfcplt(huart1_drvdata.dmauartc);
  } else {
  }
}

void MX_USART1_RxCpltCallback(UART_HandleTypeDef * huart)
{
  DMA_HandleTypeDef * hdma;

  hdma = huart->hdmarx;
  if (HAL_DMA_ERROR_NONE == hdma->ErrorCode) {
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_InvalidateDCache_by_Addr((uint32_t *)huart1_drvdata.dmauartc->rxq.mem,
                                 XWBOP_ALIGN(sizeof(huart1_drvdata.dmauartc->rxq.mem),
                                             CPUCFG_L1_CACHELINE_SIZE));
#endif
    stm32cube_usart1_cb_rxdma_cplt(huart1_drvdata.dmauartc);
  } else {
  }
}

xwsq_t MX_USART1_RXDMA_GetCounter(void)
{
  xwsq_t cnt;

  cnt = (xwsq_t)__HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
  return cnt;
}

void MX_USART1_TXDMA_Prepare(const xwu8_t * mem, xwsz_t size)
{
  xwsz_t cpsz;

  cpsz = size > HAL_UART_TXMEM_MAXSIZE ? HAL_UART_TXMEM_MAXSIZE : size;
  memcpy(huart1_drvdata.tx.mem, mem, cpsz);
  huart1_drvdata.tx.size = cpsz;
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_CleanDCache_by_Addr((uint32_t *)huart1_drvdata.tx.mem,
                          (int32_t)XWBOP_ALIGN(cpsz, CPUCFG_L1_CACHELINE_SIZE));
#endif
}

xwer_t MX_USART1_TXDMA_Start(void)
{
  xwer_t rc;
  HAL_StatusTypeDef ret;

  ret = HAL_UART_Transmit_DMA(&huart1, (uint8_t *)huart1_drvdata.tx.mem,
                              (uint16_t)huart1_drvdata.tx.size);
  if (HAL_OK == ret) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_USART1_TxCpltCallback(UART_HandleTypeDef * huart)
{
  stm32cube_usart1_cb_txdma_cplt(huart1_drvdata.dmauartc, XWOK);
}

xwer_t MX_USART1_Putc(xwu8_t byte)
{
  HAL_StatusTypeDef ret;
  xwer_t rc;

  ret = HAL_UART_Transmit(&huart1, &byte, 1, 10000);
  if (HAL_OK == ret) {
    rc = XWOK;
  } else if (HAL_TIMEOUT == ret) {
    rc = -ETIMEDOUT;
  } else if (HAL_BUSY == ret) {
    rc = -EBUSY;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_USART1_ErrorCallback(UART_HandleTypeDef * huart)
{
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_DMA)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_DMA);
    if (HAL_UART_STATE_READY == huart->RxState) {
      stm32cube_usart1_cb_rxdma_restart(huart1_drvdata.dmauartc);
    }
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_ORE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_ORE);
    if (HAL_UART_STATE_READY == huart->RxState) {
      stm32cube_usart1_cb_rxdma_restart(huart1_drvdata.dmauartc);
    }
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_FE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_FE);
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_NE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_NE);
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_PE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_PE);
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_RTO)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_RTO);
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_InvalidateDCache_by_Addr((uint32_t *)huart1_drvdata.dmauartc->rxq.mem,
                                 XWBOP_ALIGN(sizeof(huart1_drvdata.dmauartc->rxq.mem),
                                             CPUCFG_L1_CACHELINE_SIZE));
#endif
    stm32cube_usart1_cb_rxdma_timer(huart1_drvdata.dmauartc);
  }
}

/* USART3 */
void MX_USART3_UART_DeInit(void)
{
  HAL_UART_DMAStop(&huart3);
  HAL_UART_DeInit(&huart3);
}

void MX_USART3_Timer_Init(void)
{
  HAL_UART_ReceiverTimeout_Config(&huart3, 20000);
  HAL_UART_EnableReceiverTimeout(&huart3);
}

void MX_USART3_Timer_DeInit(void)
{
  HAL_UART_DisableReceiverTimeout(&huart3);
  HAL_UART_ReceiverTimeout_Config(&huart3, 0);
}

void MX_USART3_Timer_Start(void)
{
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_RTO);
}

void MX_USART3_Timer_Stop(void)
{
  __HAL_UART_DISABLE_IT(&huart3, UART_IT_RTO);
}

xwer_t MX_USART3_RXDMA_Start(xwu8_t * mem, xwsz_t size)
{
  xwer_t rc;
  HAL_StatusTypeDef ret;

  ret = HAL_UART_Receive_DMA(&huart3, (uint8_t *)mem, (uint16_t)size);
  if (HAL_OK == ret) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_USART3_RxHalfCpltCallback(UART_HandleTypeDef * huart)
{
  DMA_HandleTypeDef * hdma;

  hdma = huart->hdmarx;
  if (HAL_DMA_ERROR_NONE == hdma->ErrorCode) {
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_InvalidateDCache_by_Addr((uint32_t *)huart3_drvdata.dmauartc->rxq.mem,
                                 XWBOP_ALIGN(sizeof(huart3_drvdata.dmauartc->rxq.mem),
                                             CPUCFG_L1_CACHELINE_SIZE));
#endif
    stm32cube_usart3_cb_rxdma_halfcplt(huart3_drvdata.dmauartc);
  } else {
  }
}

void MX_USART3_RxCpltCallback(UART_HandleTypeDef * huart)
{
  DMA_HandleTypeDef * hdma;

  hdma = huart->hdmarx;
  if (HAL_DMA_ERROR_NONE == hdma->ErrorCode) {
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_InvalidateDCache_by_Addr((uint32_t *)huart3_drvdata.dmauartc->rxq.mem,
                                 XWBOP_ALIGN(sizeof(huart3_drvdata.dmauartc->rxq.mem),
                                             CPUCFG_L1_CACHELINE_SIZE));
#endif
    stm32cube_usart3_cb_rxdma_cplt(huart3_drvdata.dmauartc);
  } else {
  }
}

xwsq_t MX_USART3_RXDMA_GetCounter(void)
{
  xwsq_t cnt;

  cnt = (xwsq_t)__HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
  return cnt;
}

void MX_USART3_TXDMA_Prepare(const xwu8_t * mem, xwsz_t size)
{
  xwsz_t cpsz;

  cpsz = size > HAL_UART_TXMEM_MAXSIZE ? HAL_UART_TXMEM_MAXSIZE : size;
  memcpy(huart3_drvdata.tx.mem, mem, cpsz);
  huart3_drvdata.tx.size = cpsz;
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_CleanDCache_by_Addr((uint32_t *)huart3_drvdata.tx.mem,
                          (int32_t)XWBOP_ALIGN(cpsz, CPUCFG_L1_CACHELINE_SIZE));
#endif
}

xwer_t MX_USART3_TXDMA_Start(void)
{
  xwer_t rc;
  HAL_StatusTypeDef ret;

  ret = HAL_UART_Transmit_DMA(&huart3, (uint8_t *)huart3_drvdata.tx.mem,
                              (uint16_t)huart3_drvdata.tx.size);
  if (HAL_OK == ret) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_USART3_TxCpltCallback(UART_HandleTypeDef * huart)
{
  stm32cube_usart3_cb_txdma_cplt(huart3_drvdata.dmauartc, XWOK);
}

xwer_t MX_USART3_Putc(xwu8_t byte)
{
  HAL_StatusTypeDef ret;
  xwer_t rc;

  ret = HAL_UART_Transmit(&huart3, &byte, 1, 10000);
  if (HAL_OK == ret) {
    rc = XWOK;
  } else if (HAL_TIMEOUT == ret) {
    rc = -ETIMEDOUT;
  } else if (HAL_BUSY == ret) {
    rc = -EBUSY;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_USART3_ErrorCallback(UART_HandleTypeDef * huart)
{
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_DMA)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_DMA);
    if (HAL_UART_STATE_READY == huart->RxState) {
      stm32cube_usart3_cb_rxdma_restart(huart3_drvdata.dmauartc);
    }
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_ORE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_ORE);
    if (HAL_UART_STATE_READY == huart->RxState) {
      stm32cube_usart3_cb_rxdma_restart(huart3_drvdata.dmauartc);
    }
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_FE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_FE);
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_NE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_NE);
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_PE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_PE);
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_RTO)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_RTO);
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_InvalidateDCache_by_Addr((uint32_t *)huart3_drvdata.dmauartc->rxq.mem,
                                 XWBOP_ALIGN(sizeof(huart3_drvdata.dmauartc->rxq.mem),
                                             CPUCFG_L1_CACHELINE_SIZE));
#endif
    stm32cube_usart3_cb_rxdma_timer(huart3_drvdata.dmauartc);
  }
}

/* Redefine HAL weak callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart1) {
    MX_USART1_RxCpltCallback(huart);
  } else if (huart == &huart3) {
    MX_USART3_RxCpltCallback(huart);
  }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart1) {
    MX_USART1_RxHalfCpltCallback(huart);
  } else if (huart == &huart3) {
    MX_USART3_RxHalfCpltCallback(huart);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart1) {
    MX_USART1_TxCpltCallback(huart);
  } else if (huart == &huart3) {
    MX_USART3_TxCpltCallback(huart);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart1) {
    MX_USART1_ErrorCallback(huart);
  } else if (huart == &huart3) {
    MX_USART3_ErrorCallback(huart);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
