/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include <bm/stm32cube/xwac/xwds/spim.h>

struct MX_SPI_MasterDriverData hspi4_drvdata;
/* USER CODE END 0 */

SPI_HandleTypeDef hspi4;
DMA_HandleTypeDef hdma_spi4_tx;
DMA_HandleTypeDef hdma_spi4_rx;

/* SPI4 init function */
void MX_SPI4_Init(void)
{

  /* USER CODE BEGIN SPI4_Init 0 */

  /* USER CODE END SPI4_Init 0 */

  /* USER CODE BEGIN SPI4_Init 1 */

  /* USER CODE END SPI4_Init 1 */
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_1LINE;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 0x0;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi4.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi4.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi4.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi4.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi4.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi4.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI4_Init 2 */

  /* USER CODE END SPI4_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(spiHandle->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspInit 0 */

  /* USER CODE END SPI4_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI4;
    PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* SPI4 clock enable */
    __HAL_RCC_SPI4_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**SPI4 GPIO Configuration
    PE12     ------> SPI4_SCK
    PE14     ------> SPI4_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* SPI4 DMA Init */
    /* SPI4_TX Init */
    hdma_spi4_tx.Instance = DMA1_Stream2;
    hdma_spi4_tx.Init.Request = DMA_REQUEST_SPI4_TX;
    hdma_spi4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi4_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi4_tx.Init.Mode = DMA_NORMAL;
    hdma_spi4_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_spi4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi4_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmatx,hdma_spi4_tx);

    /* SPI4_RX Init */
    hdma_spi4_rx.Instance = DMA1_Stream3;
    hdma_spi4_rx.Init.Request = DMA_REQUEST_SPI4_RX;
    hdma_spi4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi4_rx.Init.Mode = DMA_NORMAL;
    hdma_spi4_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi4_rx);

    /* SPI4 interrupt Init */
    HAL_NVIC_SetPriority(SPI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI4_IRQn);
  /* USER CODE BEGIN SPI4_MspInit 1 */
    hspi4_drvdata.halhdl = &hspi4;
    xwos_splk_init(&hspi4_drvdata.splk);
    xwos_cond_init(&hspi4_drvdata.cond);
    hspi4_drvdata.txd = NULL;
    hspi4_drvdata.rxb = NULL;
    hspi4_drvdata.rc = -ECANCELED;
    hspi4_drvdata.size = 0;

  /* USER CODE END SPI4_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspDeInit 0 */
    xwos_cond_destroy(&hspi4_drvdata.cond);

  /* USER CODE END SPI4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI4_CLK_DISABLE();

    /**SPI4 GPIO Configuration
    PE12     ------> SPI4_SCK
    PE14     ------> SPI4_MOSI
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_12|GPIO_PIN_14);

    /* SPI4 DMA DeInit */
    HAL_DMA_DeInit(spiHandle->hdmatx);
    HAL_DMA_DeInit(spiHandle->hdmarx);

    /* SPI4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI4_IRQn);
  /* USER CODE BEGIN SPI4_MspDeInit 1 */

  /* USER CODE END SPI4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/* SPI4 */
void MX_SPI4_DeInit(void)
{
  HAL_SPI_DeInit(&hspi4);
}

xwer_t MX_SPI4_ReInit(const SPI_InitTypeDef * cfg)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  HAL_SPI_DeInit(&hspi4);
  hspi4.Init = *cfg;
  hrc = HAL_SPI_Init(&hspi4);
  if (HAL_OK != hrc) {
    rc = -EIO;
  } else {
    rc = XWOK;
  }
  return rc;
}

xwer_t MX_SPI4_Xfer(const xwu8_t txd[], xwu8_t * rxb, xwsz_t * size)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  hrc = HAL_OK;
  rc = XWOK;
  hspi4_drvdata.txd = txd;
  hspi4_drvdata.rxb = rxb;
  hspi4_drvdata.size = *size;
  if (hspi4_drvdata.size > MX_SPI_MEM_MAXSIZE) {
          hspi4_drvdata.size = MX_SPI_MEM_MAXSIZE;
  }
  if ((txd) && (rxb)) {
    memcpy(hspi4_drvdata.mem.tx, txd, hspi4_drvdata.size);
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_CleanDCache_by_Addr((uint32_t *)hspi4_drvdata.mem.tx, MX_SPI_MEM_MAXSIZE);
#endif /* STM32CUBECFG_DCACHE */
    hrc = HAL_SPI_TransmitReceive_DMA(&hspi4,
                                      hspi4_drvdata.mem.tx,
                                      hspi4_drvdata.mem.rx,
                                      hspi4_drvdata.size);
  } else if (txd) {
    memcpy(hspi4_drvdata.mem.tx, txd, hspi4_drvdata.size);
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_CleanDCache_by_Addr((uint32_t *)hspi4_drvdata.mem.tx, MX_SPI_MEM_MAXSIZE);
#endif /* STM32CUBECFG_DCACHE */
    hrc = HAL_SPI_Transmit_DMA(&hspi4, hspi4_drvdata.mem.tx, hspi4_drvdata.size);
  } else if (rxb) {
    hrc = HAL_SPI_Receive_DMA(&hspi4, hspi4_drvdata.mem.rx, hspi4_drvdata.size);
  } else {
    rc = -EINVAL;
  }
  if (HAL_BUSY == hrc) {
    rc = -EBUSY;
  } else if (HAL_ERROR == hrc) {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_SPI4_Abort(void)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  hrc = HAL_SPI_Abort_IT(&hspi4);
  if (HAL_OK == hrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_SPI4_TxCpltCallback(SPI_HandleTypeDef * hspi)
{
  stm32cube_spi4m_cb_xfercplt(hspi4_drvdata.spim, XWOK);
}

void MX_SPI4_RxCpltCallback(SPI_HandleTypeDef * hspi)
{
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_InvalidateDCache_by_Addr((uint32_t *)hspi4_drvdata.mem.rx, MX_SPI_MEM_MAXSIZE);
#endif /* STM32CUBECFG_DCACHE */
  memcpy(hspi4_drvdata.rxb, hspi4_drvdata.mem.rx, hspi4_drvdata.size);
  stm32cube_spi4m_cb_xfercplt(hspi4_drvdata.spim, XWOK);
}

void MX_SPI4_TxRxCpltCallback(SPI_HandleTypeDef * hspi)
{
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_InvalidateDCache_by_Addr((uint32_t *)hspi4_drvdata.mem.rx, MX_SPI_MEM_MAXSIZE);
#endif /* STM32CUBECFG_DCACHE */
  memcpy(hspi4_drvdata.rxb, hspi4_drvdata.mem.rx, hspi4_drvdata.size);
  stm32cube_spi4m_cb_xfercplt(hspi4_drvdata.spim, XWOK);
}

void MX_SPI4_TxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
}

void MX_SPI4_RxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
}

void MX_SPI4_TxRxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
}

void MX_SPI4_ErrorCallback(SPI_HandleTypeDef * hspi)
{
  if (hspi->ErrorCode & HAL_SPI_ERROR_TIMEOUT) {
    stm32cube_spi4m_cb_xfercplt(hspi4_drvdata.spim, -ETIMEDOUT);
  } else if (HAL_SPI_ERROR_NONE != hspi->ErrorCode) {
    stm32cube_spi4m_cb_xfercplt(hspi4_drvdata.spim, -EIO);
  } else {
    stm32cube_spi4m_cb_xfercplt(hspi4_drvdata.spim, -EBUG);
  }
}

void MX_SPI4_AbortCpltCallback(SPI_HandleTypeDef * hspi)
{
  stm32cube_spi4m_cb_xfercplt(hspi4_drvdata.spim, -ECONNABORTED);
}

/* Redefine HAL weak callback */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi4 == hspi) {
    MX_SPI4_TxCpltCallback(hspi);
  }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi4 == hspi) {
    MX_SPI4_RxCpltCallback(hspi);
  }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi4 == hspi) {
    MX_SPI4_TxRxCpltCallback(hspi);
  }
}

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi4 == hspi) {
    MX_SPI4_TxHalfCpltCallback(hspi);
  }
}

void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi4 == hspi) {
    MX_SPI4_RxHalfCpltCallback(hspi);
  }
}

void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi4 == hspi) {
    MX_SPI4_TxRxHalfCpltCallback(hspi);
  }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi4 == hspi) {
    MX_SPI4_ErrorCallback(hspi);
  }
}

void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi4 == hspi) {
    MX_SPI4_AbortCpltCallback(hspi);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
