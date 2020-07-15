/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#include <soc.h>
#include <xwos/lib/string.h>
#include "xwac/xwds/uart.h"

struct HAL_UART_Xwds_driver_data husart1_xwds_drvdata;

/* USER CODE END 0 */

UART_HandleTypeDef husart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  husart1.Instance = USART1;
  husart1.Init.BaudRate = 1000000;
  husart1.Init.WordLength = UART_WORDLENGTH_8B;
  husart1.Init.StopBits = UART_STOPBITS_1;
  husart1.Init.Parity = UART_PARITY_NONE;
  husart1.Init.Mode = UART_MODE_TX_RX;
  husart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husart1.Init.OverSampling = UART_OVERSAMPLING_16;
  husart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  husart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  husart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&husart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&husart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&husart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&husart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
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
    HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */
    husart1_xwds_drvdata.halhdl = &husart1;
    xwosal_splk_init(&husart1_xwds_drvdata.tx.splk);
    xwosal_cdt_init(&husart1_xwds_drvdata.tx.cdt);
    husart1_xwds_drvdata.tx.rc = -ECANCELED;
    husart1_xwds_drvdata.tx.size = 0;
  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */
    xwosal_cdt_destroy(&husart1_xwds_drvdata.tx.cdt);

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void MX_USART1_UART_DeInit(void)
{
  HAL_UART_DMAStop(&husart1);
  HAL_UART_DeInit(&husart1);
}

void MX_USART1_Timer_Init(void)
{
  HAL_UART_ReceiverTimeout_Config(&husart1, 30000);
  HAL_UART_EnableReceiverTimeout(&husart1);
}

void MX_USART1_Timer_DeInit(void)
{
  HAL_UART_DisableReceiverTimeout(&husart1);
  HAL_UART_ReceiverTimeout_Config(&husart1, 0);
}

void MX_USART1_Timer_Start(void)
{
  __HAL_UART_ENABLE_IT(&husart1, UART_IT_RTO);
}

void MX_USART1_Timer_Stop(void)
{
  __HAL_UART_DISABLE_IT(&husart1, UART_IT_RTO);
}

xwer_t MX_USART1_RXDMA_Start(xwu8_t * mem, xwsz_t size)
{
  xwer_t rc;
  HAL_StatusTypeDef ret;

  ret = HAL_UART_Receive_DMA(&husart1, (uint8_t *)mem, (uint16_t)size);
  if (HAL_OK == ret) {
    rc = OK;
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
    SCB_InvalidateDCache_by_Addr((uint32_t *)husart1_xwds_drvdata.dmauartc->rxq.mem,
                                 sizeof(husart1_xwds_drvdata.dmauartc->rxq.mem));
    stm32cube_usart1_cb_rxdma_halfcplt(husart1_xwds_drvdata.dmauartc);
  } else {
  }
}

void MX_USART1_RxCpltCallback(UART_HandleTypeDef * huart)
{
  DMA_HandleTypeDef * hdma;

  hdma = huart->hdmarx;
  if (HAL_DMA_ERROR_NONE == hdma->ErrorCode) {
    SCB_InvalidateDCache_by_Addr((uint32_t *)husart1_xwds_drvdata.dmauartc->rxq.mem,
                                 sizeof(husart1_xwds_drvdata.dmauartc->rxq.mem));
    stm32cube_usart1_cb_rxdma_cplt(husart1_xwds_drvdata.dmauartc);
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
  memcpy(husart1_xwds_drvdata.tx.mem, mem, cpsz);
  husart1_xwds_drvdata.tx.size = cpsz;
  SCB_CleanDCache_by_Addr((uint32_t *)husart1_xwds_drvdata.tx.mem, (int32_t)cpsz);
}

xwer_t MX_USART1_TXDMA_Start(void)
{
  xwer_t rc;
  HAL_StatusTypeDef ret;

  ret = HAL_UART_Transmit_DMA(&husart1, (uint8_t *)husart1_xwds_drvdata.tx.mem,
                              (uint16_t)husart1_xwds_drvdata.tx.size);
  if (HAL_OK == ret) {
    rc = OK;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_USART1_TxCpltCallback(UART_HandleTypeDef * huart)
{
  DMA_HandleTypeDef * hdma;
  xwer_t rc;

  hdma = huart->hdmatx;
  if (HAL_DMA_ERROR_NONE == hdma->ErrorCode) {
    rc = OK;
  } else {
    rc = -EIO;
  }
  stm32cube_usart1_cb_txdma_cplt(husart1_xwds_drvdata.dmauartc, rc);
}

xwer_t MX_USART1_Putc(xwu8_t byte)
{
  HAL_StatusTypeDef ret;
  xwer_t rc;

  ret = HAL_UART_Transmit(&husart1, &byte, 1, 10000);
  if (HAL_OK == ret) {
    rc = OK;
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
      stm32cube_usart1_cb_rxdma_restart(husart1_xwds_drvdata.dmauartc);
    }
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_ORE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_ORE);
    if (HAL_UART_STATE_READY == huart->RxState) {
      stm32cube_usart1_cb_rxdma_restart(husart1_xwds_drvdata.dmauartc);
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
    SCB_InvalidateDCache_by_Addr((uint32_t *)husart1_xwds_drvdata.dmauartc->rxq.mem,
                                 sizeof(husart1_xwds_drvdata.dmauartc->rxq.mem));
    stm32cube_usart1_cb_rxdma_timer(husart1_xwds_drvdata.dmauartc);
  }
}

/* Redefine HAL weak callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &husart1) {
    MX_USART1_RxCpltCallback(huart);
  }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &husart1) {
    MX_USART1_RxHalfCpltCallback(huart);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &husart1) {
    MX_USART1_TxCpltCallback(huart);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
  if (huart == &husart1) {
    MX_USART1_ErrorCallback(huart);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
