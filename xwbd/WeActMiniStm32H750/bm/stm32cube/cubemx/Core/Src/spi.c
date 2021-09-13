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

struct MX_SPI_MasterDriverData hspi1_drvdata;
struct MX_SPI_MasterDriverData hspi4_drvdata;
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi4;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi4_tx;
DMA_HandleTypeDef hdma_spi4_rx;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}
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
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.PLL2.PLL2M = 5;
    PeriphClkInitStruct.PLL2.PLL2N = 80;
    PeriphClkInitStruct.PLL2.PLL2P = 2;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 2;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PD7     ------> SPI1_MOSI
    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
    PB4 (NJTRST)     ------> SPI1_MISO
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI1 DMA Init */
    /* SPI1_RX Init */
    hdma_spi1_rx.Instance = DMA2_Stream2;
    hdma_spi1_rx.Init.Request = DMA_REQUEST_SPI1_RX;
    hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_rx.Init.Mode = DMA_NORMAL;
    hdma_spi1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi1_rx);

    /* SPI1_TX Init */
    hdma_spi1_tx.Instance = DMA2_Stream3;
    hdma_spi1_tx.Init.Request = DMA_REQUEST_SPI1_TX;
    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_tx.Init.Mode = DMA_NORMAL;
    hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmatx,hdma_spi1_tx);

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
  /* USER CODE BEGIN SPI1_MspInit 1 */
    hspi1_drvdata.halhdl = &hspi1;
    xwos_splk_init(&hspi1_drvdata.splk);
    xwos_cond_init(&hspi1_drvdata.cond);
    hspi1_drvdata.txd = NULL;
    hspi1_drvdata.rxb = NULL;
    hspi1_drvdata.rc = -ECANCELED;
    hspi1_drvdata.size = 0;

  /* USER CODE END SPI1_MspInit 1 */
  }
  else if(spiHandle->Instance==SPI4)
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
    HAL_NVIC_SetPriority(SPI4_IRQn, 2, 0);
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

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */
    xwos_cond_fini(&hspi1_drvdata.cond);

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PD7     ------> SPI1_MOSI
    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
    PB4 (NJTRST)     ------> SPI1_MISO
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4);

    /* SPI1 DMA DeInit */
    HAL_DMA_DeInit(spiHandle->hdmarx);
    HAL_DMA_DeInit(spiHandle->hdmatx);

    /* SPI1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  else if(spiHandle->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspDeInit 0 */
    xwos_cond_fini(&hspi4_drvdata.cond);

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
/* SPI1 */
void MX_SPI1_DeInit(void)
{
  HAL_SPI_DeInit(&hspi1);
}

xwer_t MX_SPI1_ReInit(const SPI_InitTypeDef * cfg)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  HAL_SPI_DeInit(&hspi1);
  hspi1.Init = *cfg;
  hrc = HAL_SPI_Init(&hspi1);
  if (HAL_OK != hrc) {
    rc = -EIO;
  } else {
    rc = XWOK;
  }
  return rc;
}

xwer_t MX_SPI1_Xfer(const xwu8_t txd[], xwu8_t * rxb, xwsz_t * size)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  hrc = HAL_OK;
  rc = XWOK;
  hspi1_drvdata.txd = txd;
  hspi1_drvdata.rxb = rxb;
  hspi1_drvdata.size = *size;
  if (hspi1_drvdata.size > MX_SPI_MEM_MAXSIZE) {
          hspi1_drvdata.size = MX_SPI_MEM_MAXSIZE;
  }
  if ((txd) && (rxb)) {
    memcpy(hspi1_drvdata.mem.tx, txd, hspi1_drvdata.size);
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_CleanDCache_by_Addr((uint32_t *)hspi1_drvdata.mem.tx, MX_SPI_MEM_MAXSIZE);
#endif
    hrc = HAL_SPI_TransmitReceive_DMA(&hspi1,
                                      hspi1_drvdata.mem.tx,
                                      hspi1_drvdata.mem.rx,
                                      hspi1_drvdata.size);
  } else if (txd) {
    memcpy(hspi1_drvdata.mem.tx, txd, hspi1_drvdata.size);
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_CleanDCache_by_Addr((uint32_t *)hspi1_drvdata.mem.tx, MX_SPI_MEM_MAXSIZE);
#endif
    hrc = HAL_SPI_Transmit_DMA(&hspi1, hspi1_drvdata.mem.tx, hspi1_drvdata.size);
  } else if (rxb) {
    hrc = HAL_SPI_Receive_DMA(&hspi1, hspi1_drvdata.mem.rx, hspi1_drvdata.size);
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

xwer_t MX_SPI1_Abort(void)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  hrc = HAL_SPI_Abort_IT(&hspi1);
  if (HAL_OK == hrc) {
    rc = XWOK;
  } else {
    rc = -EIO;
  }
  return rc;
}

void MX_SPI1_TxCpltCallback(SPI_HandleTypeDef * hspi)
{
  stm32cube_spi1m_cb_xfercplt(hspi1_drvdata.spim, XWOK);
}

void MX_SPI1_RxCpltCallback(SPI_HandleTypeDef * hspi)
{
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_InvalidateDCache_by_Addr((uint32_t *)hspi1_drvdata.mem.rx, MX_SPI_MEM_MAXSIZE);
#endif
  memcpy(hspi1_drvdata.rxb, hspi1_drvdata.mem.rx, hspi1_drvdata.size);
  stm32cube_spi1m_cb_xfercplt(hspi1_drvdata.spim, XWOK);
}

void MX_SPI1_TxRxCpltCallback(SPI_HandleTypeDef * hspi)
{
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_InvalidateDCache_by_Addr((uint32_t *)hspi1_drvdata.mem.rx, MX_SPI_MEM_MAXSIZE);
#endif
  memcpy(hspi1_drvdata.rxb, hspi1_drvdata.mem.rx, hspi1_drvdata.size);
  stm32cube_spi1m_cb_xfercplt(hspi1_drvdata.spim, XWOK);
}

void MX_SPI1_TxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
}

void MX_SPI1_RxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
}

void MX_SPI1_TxRxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
}

void MX_SPI1_ErrorCallback(SPI_HandleTypeDef * hspi)
{
  if (hspi->ErrorCode & HAL_SPI_ERROR_TIMEOUT) {
    stm32cube_spi1m_cb_xfercplt(hspi1_drvdata.spim, -ETIMEDOUT);
  } else if (HAL_SPI_ERROR_NONE != hspi->ErrorCode) {
    stm32cube_spi1m_cb_xfercplt(hspi1_drvdata.spim, -EIO);
  } else {
    stm32cube_spi1m_cb_xfercplt(hspi1_drvdata.spim, -EBUG);
  }
}

void MX_SPI1_AbortCpltCallback(SPI_HandleTypeDef * hspi)
{
  stm32cube_spi1m_cb_xfercplt(hspi1_drvdata.spim, -ECONNABORTED);
}

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
#endif
    hrc = HAL_SPI_TransmitReceive_DMA(&hspi4,
                                      hspi4_drvdata.mem.tx,
                                      hspi4_drvdata.mem.rx,
                                      hspi4_drvdata.size);
  } else if (txd) {
    memcpy(hspi4_drvdata.mem.tx, txd, hspi4_drvdata.size);
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_CleanDCache_by_Addr((uint32_t *)hspi4_drvdata.mem.tx, MX_SPI_MEM_MAXSIZE);
#endif
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
#endif
  memcpy(hspi4_drvdata.rxb, hspi4_drvdata.mem.rx, hspi4_drvdata.size);
  stm32cube_spi4m_cb_xfercplt(hspi4_drvdata.spim, XWOK);
}

void MX_SPI4_TxRxCpltCallback(SPI_HandleTypeDef * hspi)
{
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_InvalidateDCache_by_Addr((uint32_t *)hspi4_drvdata.mem.rx, MX_SPI_MEM_MAXSIZE);
#endif
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
  if (&hspi1 == hspi) {
    MX_SPI1_TxCpltCallback(hspi);
  } else if (&hspi4 == hspi) {
    MX_SPI4_TxCpltCallback(hspi);
  }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi1 == hspi) {
    MX_SPI1_RxCpltCallback(hspi);
  } else if (&hspi4 == hspi) {
    MX_SPI4_RxCpltCallback(hspi);
  }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi1 == hspi) {
    MX_SPI1_TxRxCpltCallback(hspi);
  } else if (&hspi4 == hspi) {
    MX_SPI4_TxRxCpltCallback(hspi);
  }
}

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi1 == hspi) {
    MX_SPI1_TxHalfCpltCallback(hspi);
  } else if (&hspi4 == hspi) {
    MX_SPI4_TxHalfCpltCallback(hspi);
  }
}

void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi1 == hspi) {
    MX_SPI1_RxHalfCpltCallback(hspi);
  } else if (&hspi4 == hspi) {
    MX_SPI4_RxHalfCpltCallback(hspi);
  }
}

void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi1 == hspi) {
    MX_SPI1_TxRxHalfCpltCallback(hspi);
  } else if (&hspi4 == hspi) {
    MX_SPI4_TxRxHalfCpltCallback(hspi);
  }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi1 == hspi) {
    MX_SPI1_ErrorCallback(hspi);
  } else if (&hspi4 == hspi) {
    MX_SPI4_ErrorCallback(hspi);
  }
}

void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef * hspi)
{
  if (&hspi1 == hspi) {
    MX_SPI1_AbortCpltCallback(hspi);
  } else if (&hspi4 == hspi) {
    MX_SPI4_AbortCpltCallback(hspi);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
