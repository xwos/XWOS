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
#include "cubemx/Core/Inc/tim.h"
#include "xwac/xwds/uart.h"

struct MX_UART_DriverData huart1_drvdata;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

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
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */
    huart1_drvdata.halhdl = &huart1;
    xwos_splk_init(&huart1_drvdata.tx.splk);
    xwos_cond_init(&huart1_drvdata.tx.cond);
    huart1_drvdata.tx.rc = -ECANCELED;

  /* USER CODE END USART1_MspInit 1 */
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
  HAL_UART_DMAStop(&huart1);
  HAL_UART_DeInit(&huart1);
}

void MX_USART1_Timer_Init(void)
{
  MX_TIM7_Init();
}

void MX_USART1_Timer_DeInit(void)
{
  MX_TIM7_DeInit();
}

void MX_USART1_Timer_Start(void)
{
  MX_TIM7_Start();
}

void MX_USART1_Timer_Stop(void)
{
  MX_TIM7_Stop();
}

void MX_USART1_Timer_Callback(void)
{
  stm32cube_usart1_cb_rxdma_timer(huart1_drvdata.dmauartc);
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
    stm32cube_usart1_cb_rxdma_halfcplt(huart1_drvdata.dmauartc);
  } else {
  }
}

void MX_USART1_RxCpltCallback(UART_HandleTypeDef * huart)
{
  DMA_HandleTypeDef * hdma;

  hdma = huart->hdmarx;
  if (HAL_DMA_ERROR_NONE == hdma->ErrorCode) {
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

xwer_t MX_USART1_TXDMA_Start(xwu8_t * mem, xwsz_t size)
{
  xwer_t rc;
  HAL_StatusTypeDef ret;

  ret = HAL_UART_Transmit_DMA(&huart1, (uint8_t *)mem, (uint16_t)size);
  if (HAL_OK == ret) {
    rc = XWOK;
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
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  stm32cube_usart1_cb_txdma_cplt(huart1_drvdata.dmauartc, rc);
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
  }
  if (0 != (huart->ErrorCode & HAL_UART_ERROR_ORE)) {
    huart->ErrorCode &= ~(HAL_UART_ERROR_ORE);
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
  if (HAL_UART_STATE_READY == huart->RxState) {
    stm32cube_usart1_cb_rxdma_restart(huart1_drvdata.dmauartc);
  }
}

/* Redefine HAL weak callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart1) {
    MX_USART1_RxCpltCallback(huart);
  }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart1) {
    MX_USART1_RxHalfCpltCallback(huart);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart1) {
    MX_USART1_TxCpltCallback(huart);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart1) {
    MX_USART1_ErrorCallback(huart);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
