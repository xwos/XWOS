/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <bm/stm32cube/xwac/xwds/i2cm.h>

struct MX_I2C_MasterDriverData hi2c2_drvdata;

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x50404CFD;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();

    /* I2C2 DMA Init */
    /* I2C2_RX Init */
    hdma_i2c2_rx.Instance = DMA1_Stream2;
    hdma_i2c2_rx.Init.Request = DMA_REQUEST_I2C2_RX;
    hdma_i2c2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_i2c2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c2_rx.Init.Mode = DMA_NORMAL;
    hdma_i2c2_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_i2c2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i2cHandle,hdmarx,hdma_i2c2_rx);

    /* I2C2_TX Init */
    hdma_i2c2_tx.Instance = DMA1_Stream3;
    hdma_i2c2_tx.Init.Request = DMA_REQUEST_I2C2_TX;
    hdma_i2c2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2c2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c2_tx.Init.Mode = DMA_NORMAL;
    hdma_i2c2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_i2c2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i2cHandle,hdmatx,hdma_i2c2_tx);

    /* I2C2 interrupt Init */
    HAL_NVIC_SetPriority(I2C2_EV_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
    HAL_NVIC_SetPriority(I2C2_ER_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
  /* USER CODE BEGIN I2C2_MspInit 1 */
    hi2c2_drvdata.halhdl = &hi2c2;
    xwos_splk_init(&hi2c2_drvdata.splk);
    xwos_cond_init(&hi2c2_drvdata.cond);
    hi2c2_drvdata.msg = NULL;
    hi2c2_drvdata.rc = -ECANCELED;
    hi2c2_drvdata.size = 0;
  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */
    xwos_cond_fini(&hi2c2_drvdata.cond);

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_5);

    /* I2C2 DMA DeInit */
    HAL_DMA_DeInit(i2cHandle->hdmarx);
    HAL_DMA_DeInit(i2cHandle->hdmatx);

    /* I2C2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void MX_I2C2_DeInit(void)
{
  HAL_I2C_DeInit(&hi2c2);
}

void MX_I2C2_ReInit(xwu16_t addm)
{
  HAL_StatusTypeDef hrc;
  uint32_t AddressingMode;

  if (XWDS_I2C_F_10BITADDR == addm) {
    AddressingMode = I2C_ADDRESSINGMODE_10BIT;
  } else {
    AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  }

  HAL_I2C_DeInit(&hi2c2);

  hi2c2.Init.AddressingMode = AddressingMode;
  hrc = HAL_I2C_Init(&hi2c2);
  if (HAL_OK != hrc) {
    Error_Handler();
  }

  hrc = HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE);
  if (HAL_OK != hrc) {
    Error_Handler();
  }

  hrc = HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0);
  if (HAL_OK != hrc) {
    Error_Handler();
  }
}

xwu16_t MX_I2C2_GetAddressingMode(void)
{
  xwu16_t addm;

  if (hi2c2.Init.AddressingMode) {
    addm = XWDS_I2C_F_7BITADDR;
  } else {
    addm = XWDS_I2C_F_10BITADDR;
  }
  return addm;
}

xwer_t MX_I2C2_Xfer(struct xwds_i2c_msg * msg)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  if (NULL == msg) {
    hi2c2_drvdata.size = 0;
    hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c2, 0xFF, hi2c2_drvdata.mem, 0,
                                         I2C_FIRST_AND_LAST_FRAME);
  } else if ((NULL == msg->data) || (0 == msg->size)) {
    hi2c2_drvdata.size = 0;
    if (msg->flag & XWDS_I2C_F_RD) {
      hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c2, msg->addr, hi2c2_drvdata.mem, 0,
                                           I2C_FIRST_AND_LAST_FRAME);
    } else {
      hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c2, msg->addr, hi2c2_drvdata.mem, 0,
                                            I2C_FIRST_AND_LAST_FRAME);
    }
  } else {
    hi2c2_drvdata.size = msg->size;
    if (msg->flag & XWDS_I2C_F_RD) {
      if ((msg->flag & XWDS_I2C_F_START) && (msg->flag & XWDS_I2C_F_STOP)) {
        hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c2, msg->addr,
                                             hi2c2_drvdata.mem, hi2c2_drvdata.size,
                                             I2C_FIRST_AND_LAST_FRAME);
      } else if (msg->flag & XWDS_I2C_F_START) {
        hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c2, msg->addr,
                                             hi2c2_drvdata.mem, hi2c2_drvdata.size,
                                             I2C_FIRST_FRAME);
      } else if (msg->flag & XWDS_I2C_F_STOP) {
        hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c2, msg->addr,
                                             hi2c2_drvdata.mem, hi2c2_drvdata.size,
                                             I2C_OTHER_AND_LAST_FRAME);
      } else {
        hrc = HAL_I2C_Master_Seq_Receive_DMA(&hi2c2, msg->addr,
                                             hi2c2_drvdata.mem, hi2c2_drvdata.size,
                                             I2C_OTHER_FRAME);
      }
    } else {
      memcpy(hi2c2_drvdata.mem, msg->data, msg->size);
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
      SCB_CleanDCache_by_Addr((uint32_t *)hi2c2_drvdata.mem,
                              (int32_t)XWBOP_ALIGN(msg->size, CPUCFG_L1_CACHELINE_SIZE));
#endif /* STM32CUBECFG_DCACHE */
      if ((msg->flag & XWDS_I2C_F_START) && (msg->flag & XWDS_I2C_F_STOP)) {
        hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c2, msg->addr,
                                              hi2c2_drvdata.mem, hi2c2_drvdata.size,
                                              I2C_FIRST_AND_LAST_FRAME);
      } else if (msg->flag & XWDS_I2C_F_START) {
        hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c2, msg->addr,
                                              hi2c2_drvdata.mem, hi2c2_drvdata.size,
                                              I2C_FIRST_FRAME);
      } else if (msg->flag & XWDS_I2C_F_STOP) {
        hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c2, msg->addr,
                                              hi2c2_drvdata.mem, hi2c2_drvdata.size,
                                              I2C_OTHER_AND_LAST_FRAME);
      } else {
        hrc = HAL_I2C_Master_Seq_Transmit_DMA(&hi2c2, msg->addr,
                                              hi2c2_drvdata.mem, hi2c2_drvdata.size,
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

xwer_t MX_I2C2_Abort(xwu16_t addr)
{
  HAL_StatusTypeDef hrc;
  xwer_t rc;

  hrc = HAL_I2C_Master_Abort_IT(&hi2c2, addr);
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
void MX_I2C2_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c)
{
  stm32cube_i2c2m_cb_xfercplt(hi2c2_drvdata.i2cm, XWOK);
}

static
void MX_I2C2_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_InvalidateDCache_by_Addr((uint32_t *)hi2c2_drvdata.mem,
                               (int32_t)XWBOP_ALIGN(hi2c2_drvdata.size,
                                                    CPUCFG_L1_CACHELINE_SIZE));
#endif /* STM32CUBECFG_DCACHE */
  memcpy(hi2c2_drvdata.msg->data, hi2c2_drvdata.mem, hi2c2_drvdata.size);
  stm32cube_i2c2m_cb_xfercplt(hi2c2_drvdata.i2cm, XWOK);
}

static
void MX_I2C2_ErrorCallback(I2C_HandleTypeDef * hi2c)
{
  if (hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
    stm32cube_i2c2m_cb_xfercplt(hi2c2_drvdata.i2cm, -EADDRNOTAVAIL);
  } else if (hi2c->ErrorCode &
             (HAL_I2C_ERROR_BERR | HAL_I2C_ERROR_ARLO |
              HAL_I2C_ERROR_OVR | HAL_I2C_ERROR_DMA | HAL_I2C_ERROR_SIZE)) {
    stm32cube_i2c2m_cb_xfercplt(hi2c2_drvdata.i2cm, -EIO);
  } else if (hi2c->ErrorCode & HAL_I2C_ERROR_TIMEOUT) {
    stm32cube_i2c2m_cb_xfercplt(hi2c2_drvdata.i2cm, -ETIMEDOUT);
  } else {
    stm32cube_i2c2m_cb_xfercplt(hi2c2_drvdata.i2cm, -EBUG);
  }
}

static
void MX_I2C2_AbortCpltCallback(I2C_HandleTypeDef * hi2c)
{
  stm32cube_i2c2m_cb_xfercplt(hi2c2_drvdata.i2cm, -ECONNABORTED);
}

/* Redefine HAL weak callback */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c)
{
  if (&hi2c2 == hi2c) {
    MX_I2C2_MasterTxCpltCallback(hi2c);
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
  if (&hi2c2 == hi2c) {
    MX_I2C2_MasterRxCpltCallback(hi2c);
  }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef * hi2c)
{
  if (&hi2c2 == hi2c) {
    MX_I2C2_ErrorCallback(hi2c);
  }
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef * hi2c)
{
  if (&hi2c2 == hi2c) {
    MX_I2C2_AbortCpltCallback(hi2c);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
