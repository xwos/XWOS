/**
  ******************************************************************************
  * @file    sdio.c
  * @brief   This file provides code for the configuration
  *          of the SDIO instances.
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
#include "sdio.h"

/* USER CODE BEGIN 0 */
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>

#define MX_SDIO_SD_MAX_RETRY_TIMES 8

struct xwos_mtx hsd_xfer_mtx;
struct xwos_splk hsd_lock;
struct xwos_cond hsd_xfer_cond;
xwer_t hsd_xfer_rc = XWOK;

void MX_SDIO_SD_Construct(void)
{
  xwos_mtx_init(&hsd_xfer_mtx, XWOS_SKD_PRIORITY_RT_MAX);
  xwos_splk_init(&hsd_lock);
  xwos_cond_init(&hsd_xfer_cond);
}

void MX_SDIO_SD_Destruct(void)
{
  xwos_cond_fini(&hsd_xfer_cond);
  xwos_mtx_fini(&hsd_xfer_mtx);
}

/* USER CODE END 0 */

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio;

/* SDIO init function */

void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 6;
  if (HAL_SD_Init(&hsd) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */
    MX_SDIO_SD_Construct();

  /* USER CODE END SDIO_MspInit 0 */
    /* SDIO clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDIO DMA Init */
    /* SDIO Init */
    hdma_sdio.Instance = DMA2_Channel4;
    hdma_sdio.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio.Init.Mode = DMA_NORMAL;
    hdma_sdio.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_sdio) != HAL_OK)
    {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    /* Be sure to change transfer direction before calling
     HAL_SD_ReadBlocks_DMA or HAL_SD_WriteBlocks_DMA. */
    __HAL_LINKDMA(sdHandle,hdmarx,hdma_sdio);
    __HAL_LINKDMA(sdHandle,hdmatx,hdma_sdio);

    /* SDIO interrupt Init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();

    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(sdHandle->hdmarx);
    HAL_DMA_DeInit(sdHandle->hdmatx);

    /* SDIO interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspDeInit 1 */
    MX_SDIO_SD_Destruct();
  /* USER CODE END SDIO_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void MX_SDIO_SD_DeInit(void)
{
  HAL_SD_DeInit(&hsd);
}

void MX_SDIO_SD_ReInit(uint32_t clkdiv)
{
  HAL_StatusTypeDef ret;
  uint32_t cnt;

  HAL_Delay(1);
  MX_SDIO_SD_DeInit();
  HAL_Delay(1);
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = clkdiv;

  cnt = 0;
  do {
    ret = HAL_SD_Init(&hsd);
    if (HAL_OK == ret)
    {
      ret = HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B);
    }
    cnt++;
  } while ((ret != HAL_OK) && (cnt < 5));
  if (ret != HAL_OK)
  {
    Error_Handler();
  }
}

xwer_t MX_SDIO_SD_TrimClk(xwsq_t cnt)
{
  xwu8_t buf[512] __xwcc_aligned(4);
  xwer_t rc;
  xwsq_t i;
  xwtm_t time;

  rc = XWOK;
  for (i = 0; i < cnt; i++) {
    time = 1 * XWTM_MS;
    xwos_cthd_sleep(&time);
    rc = MX_SDIO_SD_Read(buf, i, 1);
    if (XWOK == rc) {
    } else if (-EIO == rc) {
      if (hsd.Init.ClockDiv <= 0xFD) {
        MX_SDIO_SD_ReInit(hsd.Init.ClockDiv + 2);
        rc = XWOK;
      } else {
        break;
      }
    } else {
      break;
    }
  }
  return rc;
}

xwer_t MX_SDIO_SD_GetState(void)
{
  xwer_t rc;
  xwsq_t cnt;
  HAL_SD_CardStateTypeDef cardst;

  cnt = 0;
  do {
    cardst = HAL_SD_GetCardState(&hsd);
    if ((0 == cardst) && (HAL_SD_ERROR_NONE != hsd.ErrorCode)) {
      rc = -EIO;
      MX_SDIO_SD_ReInit(hsd.Init.ClockDiv);
      cnt++;
    } else if (HAL_SD_CARD_TRANSFER == cardst) {
      rc = XWOK;
    } else {
      rc = -EBUSY;
    }
  } while ((-EIO == rc) && (cnt < MX_SDIO_SD_MAX_RETRY_TIMES));
  return rc;
}

xwer_t MX_SDIO_SD_Read(uint8_t * data, uint32_t blkaddr,
                       uint32_t nrblk)
{
  xwer_t rc;
  xwsq_t cnt;
  HAL_SD_CardStateTypeDef cardst;

  rc = xwos_mtx_lock(&hsd_xfer_mtx);
  if (XWOK == rc) {
    cnt = 0;
    do {
      if (rc < 0) {
        MX_SDIO_SD_ReInit(hsd.Init.ClockDiv);
        cnt++;
      }
      cardst = HAL_SD_GetCardState(&hsd);
      if ((0 == cardst) && (HAL_SD_ERROR_NONE != hsd.ErrorCode)) {
        rc = -EIO;
      } else if (HAL_SD_CARD_TRANSFER == cardst) {
        HAL_StatusTypeDef halrc;
        xwreg_t cpuirq;
        xwsq_t lkst;
        union xwos_ulock ulk;

        hsd_xfer_rc = -EINPROGRESS;
        ulk.osal.splk = &hsd_lock;
        halrc = HAL_SD_ReadBlocks_DMA(&hsd, data, blkaddr, nrblk);
        if (HAL_OK == halrc) {
          xwos_splk_lock_cpuirqsv(&hsd_lock, &cpuirq);
          if (-EINPROGRESS == hsd_xfer_rc) {
            rc = xwos_cond_wait(&hsd_xfer_cond,
                                ulk, XWOS_LK_SPLK,
                                NULL, &lkst);
            if (XWOK == rc) {
              rc = hsd_xfer_rc;
            } else {
              if (XWOS_LKST_UNLOCKED == lkst) {
                xwos_splk_lock(&hsd_lock);
              }
            }
          } else {
            rc = hsd_xfer_rc;
          }
          xwos_splk_unlock_cpuirqrs(&hsd_lock, cpuirq);
        } else if (HAL_BUSY == halrc) {
          rc = -EBUSY;
        } else {
          rc = -EIO;
        }
      } else {
        rc = -EBUSY;
      }
    } while ((-EIO == rc) && (cnt < MX_SDIO_SD_MAX_RETRY_TIMES));
    xwos_mtx_unlock(&hsd_xfer_mtx);
  }/* else { failed to lock mutex! } */
  return rc;
}

xwer_t MX_SDIO_SD_Write(uint8_t * data, uint32_t blkaddr,
                        uint32_t nrblk)
{
  xwer_t rc;
  xwsq_t cnt;
  HAL_SD_CardStateTypeDef cardst;

  rc = xwos_mtx_lock(&hsd_xfer_mtx);
  if (XWOK == rc) {
    cnt = 0;
    do {
      if (rc < 0) {
        MX_SDIO_SD_ReInit(hsd.Init.ClockDiv);
        cnt++;
      }
      cardst = HAL_SD_GetCardState(&hsd);
      if ((0 == cardst) && (HAL_SD_ERROR_NONE != hsd.ErrorCode)) {
        rc = -EIO;
      } else if (HAL_SD_CARD_TRANSFER == cardst) {
        HAL_StatusTypeDef halrc;
        xwreg_t cpuirq;
        xwsq_t lkst;
        union xwos_ulock ulk;

        hsd_xfer_rc = -EINPROGRESS;
        ulk.osal.splk = &hsd_lock;
        halrc = HAL_SD_WriteBlocks_DMA(&hsd, data, blkaddr, nrblk);
        if (HAL_OK == halrc) {
          xwos_splk_lock_cpuirqsv(&hsd_lock, &cpuirq);
          if (-EINPROGRESS == hsd_xfer_rc) {
            rc = xwos_cond_wait(&hsd_xfer_cond,
                                ulk, XWOS_LK_SPLK,
                                NULL, &lkst);
            if (XWOK == rc) {
              rc = hsd_xfer_rc;
            } else {
              if (XWOS_LKST_UNLOCKED == lkst) {
                xwos_splk_lock(&hsd_lock);
              }
            }
          } else {
            rc = hsd_xfer_rc;
          }
          xwos_splk_unlock_cpuirqrs(&hsd_lock, cpuirq);
        } else if (HAL_BUSY == halrc) {
          rc = -EBUSY;
        } else {
          rc = -EIO;
        }
      } else {
        rc = -EBUSY;
      }
    } while ((-EIO == rc) && (cnt < MX_SDIO_SD_MAX_RETRY_TIMES));
    xwos_mtx_unlock(&hsd_xfer_mtx);
  }/* else { failed to lock mutex! } */
  return rc;
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef * phsd)
{
  HAL_SD_CardStateTypeDef cardst;

  cardst = HAL_SD_GetCardState(phsd);
  if (cardst != HAL_SD_CARD_TRANSFER) {
    /* FIXME */
  }

  if ((SD_CONTEXT_NONE == phsd->Context) && (HAL_SD_STATE_READY == phsd->State)) {
    xwreg_t cpuirq;
    xwos_splk_lock_cpuirqsv(&hsd_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd_xfer_rc = -EIO;
    } else {
      hsd_xfer_rc = XWOK;
    }
    xwos_splk_unlock_cpuirqrs(&hsd_lock, cpuirq);
    xwos_cond_broadcast(&hsd_xfer_cond);
  }
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef * phsd)
{
  HAL_SD_CardStateTypeDef cardst;

  cardst = HAL_SD_GetCardState(phsd);
  if (cardst != HAL_SD_CARD_TRANSFER) {
    /* FIXME */
  }
  if ((SD_CONTEXT_NONE == phsd->Context) && (HAL_SD_STATE_READY == phsd->State)) {
    xwreg_t cpuirq;
    xwos_splk_lock_cpuirqsv(&hsd_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd_xfer_rc = -EIO;
    } else {
      hsd_xfer_rc = XWOK;
    }
    xwos_splk_unlock_cpuirqrs(&hsd_lock, cpuirq);
    xwos_cond_broadcast(&hsd_xfer_cond);
  }
}

void HAL_SD_ErrorCallback(SD_HandleTypeDef * phsd)
{
  if ((SD_CONTEXT_NONE == phsd->Context) && (HAL_SD_STATE_READY == phsd->State)) {
    xwreg_t cpuirq;
    xwos_splk_lock_cpuirqsv(&hsd_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd_xfer_rc = -EIO;
    } else {
      hsd_xfer_rc = XWOK;
    }
    xwos_splk_unlock_cpuirqrs(&hsd_lock, cpuirq);
    xwos_cond_broadcast(&hsd_xfer_cond);
  }
}

void HAL_SD_AbortCallback(SD_HandleTypeDef * phsd)
{
  if ((SD_CONTEXT_NONE == phsd->Context) && (HAL_SD_STATE_READY == phsd->State)) {
    xwreg_t cpuirq;
    xwos_splk_lock_cpuirqsv(&hsd_lock, &cpuirq);
    hsd_xfer_rc = -ECANCELED;
    xwos_splk_unlock_cpuirqrs(&hsd_lock, cpuirq);
    xwos_cond_broadcast(&hsd_xfer_cond);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
