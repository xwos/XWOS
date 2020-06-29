/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USART1 init function */

void MX_USART1_UART_Init(void)
{
  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**USART1 GPIO Configuration
  PA9   ------> USART1_TX
  PA10   ------> USART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9|LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART1 DMA Init */

  /* USART1_TX Init */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_7, LL_DMA_CHANNEL_4);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_7, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_7, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_7, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_7, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_7, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_7, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_7, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_7);

  /* USART1_RX Init */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_2, LL_DMA_CHANNEL_4);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_2, LL_DMA_PRIORITY_HIGH);

  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_2, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_2, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_2, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_2, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_2, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_2);

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART1_IRQn);

  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART1);
  LL_USART_Enable(USART1);

}

/* USER CODE BEGIN 1 */
void MX_USART1_UART_Deinit(void)
{
  LL_USART_Disable(USART1);
  NVIC_DisableIRQ(USART1_IRQn);
  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1);
}

void MX_USART1_UART_Setup_RXDMA(xwu8_t * mem, xwsz_t size)
{
  LL_DMA_EnableIT_HT(DMA2, LL_DMA_STREAM_2);
  LL_DMA_EnableIT_TE(DMA2, LL_DMA_STREAM_2);
  LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_2);
  LL_DMA_DisableDoubleBufferMode(DMA2, LL_DMA_STREAM_2);
  LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM0);
  LL_DMA_ConfigAddresses(DMA2, LL_DMA_STREAM_2,
                         LL_USART_DMA_GetRegAddr(USART1),
                         (uint32_t)mem,
                         LL_DMA_GetDataTransferDirection(DMA2, LL_DMA_STREAM_2));
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, size);
  LL_USART_EnableDMAReq_RX(USART1);
}

void MX_USART1_UART_Setup_TXDMA(const xwu8_t * mem, xwsz_t size)
{
  LL_DMA_EnableIT_TE(DMA2, LL_DMA_STREAM_7);
  LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_7);
  LL_DMA_DisableDoubleBufferMode(DMA2, LL_DMA_STREAM_7);
  LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_7, LL_DMA_CURRENTTARGETMEM0);
  LL_DMA_ConfigAddresses(DMA2, LL_DMA_STREAM_7,
                         (uint32_t)mem,
                         LL_USART_DMA_GetRegAddr(USART1),
                         LL_DMA_GetDataTransferDirection(DMA2, LL_DMA_STREAM_7));
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_7, size);
  LL_USART_EnableDMAReq_TX(USART1);
}

void MX_USART1_UART_Finish_TXDMA(void)
{
  LL_USART_DisableDMAReq_TX(USART1);
}

void MX_USART1_UART_Putc(xwu8_t byte)
{
  while (!LL_USART_IsActiveFlag_TXE(USART1)) {
  }
  LL_USART_TransmitData8(USART1, (uint8_t)byte);
}

void MX_USART1_UART_Isr(void)
{
  if (LL_USART_IsActiveFlag_PE(USART1)) {
    LL_USART_ClearFlag_PE(USART1);
  }
  if (LL_USART_IsActiveFlag_FE(USART1)) {
    LL_USART_ClearFlag_FE(USART1);
  }
  if (LL_USART_IsActiveFlag_NE(USART1)) {
    LL_USART_ClearFlag_NE(USART1);
  }
  if (LL_USART_IsActiveFlag_ORE(USART1)) {
    LL_USART_ClearFlag_ORE(USART1);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
