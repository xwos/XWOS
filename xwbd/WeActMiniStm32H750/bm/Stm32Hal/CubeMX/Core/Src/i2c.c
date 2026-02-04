/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include "board/std.h"
#include <string.h>
#include <xwos/lib/xwbop.h>
#include "bm/Stm32Hal/xwds/i2cm.h"

/* Buffer of hi2c1 is in SRAM. Both SRAM & DMA1 are in D2 Domain.
   Data path in AHB bus matrix is shorter. */
struct MX_I2C_MasterDriverData hi2c1_drvdata;

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00B03FDB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = I2C1_SCL_Pin|I2C1_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* I2C1 DMA Init */
    /* I2C1_RX Init */
    hdma_i2c1_rx.Instance = DMA1_Stream4;
    hdma_i2c1_rx.Init.Request = DMA_REQUEST_I2C1_RX;
    hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i2cHandle,hdmarx,hdma_i2c1_rx);

    /* I2C1_TX Init */
    hdma_i2c1_tx.Instance = DMA1_Stream5;
    hdma_i2c1_tx.Init.Request = DMA_REQUEST_I2C1_TX;
    hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_i2c1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i2cHandle,hdmatx,hdma_i2c1_tx);

    /* I2C1 interrupt Init */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspInit 1 */
    hi2c1_drvdata.halhdl = &hi2c1;
    xwos_splk_init(&hi2c1_drvdata.splk);
    xwos_cond_init(&hi2c1_drvdata.cond);
    hi2c1_drvdata.msg = NULL;
    hi2c1_drvdata.rc = -ECANCELED;
    hi2c1_drvdata.size = 0;
  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */
    xwos_cond_fini(&hi2c1_drvdata.cond);

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(I2C1_SCL_GPIO_Port, I2C1_SCL_Pin);

    HAL_GPIO_DeInit(I2C1_SDA_GPIO_Port, I2C1_SDA_Pin);

    /* I2C1 DMA DeInit */
    HAL_DMA_DeInit(i2cHandle->hdmarx);
    HAL_DMA_DeInit(i2cHandle->hdmatx);

    /* I2C1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void MX_I2C1_DeInit(void)
{
  HAL_I2C_DeInit(&hi2c1);
}

void MX_I2C1_ReInit(xwu16_t addm)
{
  HAL_StatusTypeDef hrc;
  uint32_t AddressingMode;

  if (XWDS_I2C_F_10BITADDR == addm) {
    AddressingMode = I2C_ADDRESSINGMODE_10BIT;
  } else {
    AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  }

  HAL_I2C_DeInit(&hi2c1);

  hi2c1.Init.AddressingMode = AddressingMode;
  hrc = HAL_I2C_Init(&hi2c1);
  if (HAL_OK != hrc) {
    Error_Handler();
  }

  hrc = HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE);
  if (HAL_OK != hrc) {
    Error_Handler();
  }

  hrc = HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0);
  if (HAL_OK != hrc) {
    Error_Handler();
  }
}

xwu16_t MX_I2C1_GetAddressingMode(void)
{
  xwu16_t addm;

  if (hi2c1.Init.AddressingMode) {
    addm = 0U;
  } else {
    addm = XWDS_I2C_F_10BITADDR;
  }
  return addm;
}

xwer_t MX_I2C1_Xfer(struct xwds_i2c_msg * msg)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  if (NULL == msg) {
    hi2c1_drvdata.size = 0;
    hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c1, 0xFF, hi2c1_drvdata.mem, 0,
                                         I2C_FIRST_AND_LAST_FRAME);
  } else if ((NULL == msg->data) || (0 == msg->size)) {
    hi2c1_drvdata.size = 0;
    if (msg->flag & XWDS_I2C_F_RD) {
      hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c1, msg->addr, hi2c1_drvdata.mem, 0,
                                           I2C_FIRST_AND_LAST_FRAME);
    } else {
      hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, msg->addr, hi2c1_drvdata.mem, 0,
                                            I2C_FIRST_AND_LAST_FRAME);
    }
  } else {
    hi2c1_drvdata.size = msg->size;
    if (msg->flag & XWDS_I2C_F_RD) {
      if ((msg->flag & XWDS_I2C_F_START) && (msg->flag & XWDS_I2C_F_STOP)) {
        hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c1, msg->addr,
                                             hi2c1_drvdata.mem, hi2c1_drvdata.size,
                                             I2C_FIRST_AND_LAST_FRAME);
      } else if (msg->flag & XWDS_I2C_F_START) {
        hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c1, msg->addr,
                                             hi2c1_drvdata.mem, hi2c1_drvdata.size,
                                             I2C_FIRST_FRAME);
      } else if (msg->flag & XWDS_I2C_F_STOP) {
        hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c1, msg->addr,
                                             hi2c1_drvdata.mem, hi2c1_drvdata.size,
                                             I2C_OTHER_AND_LAST_FRAME);
      } else {
        hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c1, msg->addr,
                                             hi2c1_drvdata.mem, hi2c1_drvdata.size,
                                             I2C_OTHER_FRAME);
      }
    } else {
      memcpy(hi2c1_drvdata.mem, msg->data, msg->size);
#if defined(BRDCFG_DCACHE) && (1 == BRDCFG_DCACHE)
      SCB_CleanDCache_by_Addr((uint32_t *)hi2c1_drvdata.mem,
                              (int32_t)XWBOP_ALIGN(msg->size, CPUCFG_L1_CACHELINE_SIZE));
#endif /* BRDCFG_DCACHE */
      if ((msg->flag & XWDS_I2C_F_START) && (msg->flag & XWDS_I2C_F_STOP)) {
        hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, msg->addr,
                                              hi2c1_drvdata.mem, hi2c1_drvdata.size,
                                              I2C_FIRST_AND_LAST_FRAME);
      } else if (msg->flag & XWDS_I2C_F_START) {
        hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, msg->addr,
                                              hi2c1_drvdata.mem, hi2c1_drvdata.size,
                                              I2C_FIRST_FRAME);
      } else if (msg->flag & XWDS_I2C_F_STOP) {
        hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, msg->addr,
                                              hi2c1_drvdata.mem, hi2c1_drvdata.size,
                                              I2C_OTHER_AND_LAST_FRAME);
      } else {
        hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, msg->addr,
                                              hi2c1_drvdata.mem, hi2c1_drvdata.size,
                                              I2C_OTHER_FRAME);
      }
    }
  }
  if (HAL_OK == hrc) {
    rc = XWOK;
  } else if (HAL_BUSY == hrc) {
    rc = -EBUSY;
  } else {
    rc = -EIO;
  }
  return rc;
}

xwer_t MX_I2C1_Abort(xwu16_t addr)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  hrc = HAL_I2C_Master_Abort_IT(&hi2c1, addr);
  if (HAL_OK == hrc) {
    rc = XWOK;
  } else if (HAL_BUSY == hrc) {
    rc = -EBUSY;
  } else {
    rc = -EIO;
  }
  return rc;
}

static
void MX_I2C1_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c)
{
  stm32xwds_i2c1m_cb_xfercplt(hi2c1_drvdata.i2cm, XWOK);
}

static
void MX_I2C1_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
#if defined(BRDCFG_DCACHE) && (1 == BRDCFG_DCACHE)
  SCB_InvalidateDCache_by_Addr((uint32_t *)hi2c1_drvdata.mem,
                               (int32_t)XWBOP_ALIGN(hi2c1_drvdata.size,
                                                    CPUCFG_L1_CACHELINE_SIZE));
#endif /* BRDCFG_DCACHE */
  memcpy(hi2c1_drvdata.msg->data, hi2c1_drvdata.mem, hi2c1_drvdata.size);
  stm32xwds_i2c1m_cb_xfercplt(hi2c1_drvdata.i2cm, XWOK);
}

static
void MX_I2C1_ErrorCallback(I2C_HandleTypeDef * hi2c)
{
  if (hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
    stm32xwds_i2c1m_cb_xfercplt(hi2c1_drvdata.i2cm, -EADDRNOTAVAIL);
  } else if (hi2c->ErrorCode &
             (HAL_I2C_ERROR_BERR | HAL_I2C_ERROR_ARLO |
              HAL_I2C_ERROR_OVR | HAL_I2C_ERROR_DMA | HAL_I2C_ERROR_SIZE)) {
    stm32xwds_i2c1m_cb_xfercplt(hi2c1_drvdata.i2cm, -EIO);
  } else if (hi2c->ErrorCode & HAL_I2C_ERROR_TIMEOUT) {
    stm32xwds_i2c1m_cb_xfercplt(hi2c1_drvdata.i2cm, -ETIMEDOUT);
  } else {
    stm32xwds_i2c1m_cb_xfercplt(hi2c1_drvdata.i2cm, -EBUG);
  }
}

static
void MX_I2C1_AbortCpltCallback(I2C_HandleTypeDef * hi2c)
{
  stm32xwds_i2c1m_cb_xfercplt(hi2c1_drvdata.i2cm, -ECONNABORTED);
}

/* Redefine HAL weak callback */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c)
{
  if (&hi2c1 == hi2c) {
    MX_I2C1_MasterTxCpltCallback(hi2c);
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
  if (&hi2c1 == hi2c) {
    MX_I2C1_MasterRxCpltCallback(hi2c);
  }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef * hi2c)
{
  if (&hi2c1 == hi2c) {
    MX_I2C1_ErrorCallback(hi2c);
  }
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef * hi2c)
{
  if (&hi2c1 == hi2c) {
    MX_I2C1_AbortCpltCallback(hi2c);
  }
}

/* USER CODE END 1 */
