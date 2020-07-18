/**
  ******************************************************************************
  * File Name          : SDMMC.c
  * Description        : This file provides code for the configuration
  *                      of the SDMMC instances.
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
#include "sdmmc.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/lock/mutex.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/condition.h>
#include <xwos/mm/common.h>
#include <xwos/mm/mempool/allocator.h>
#include <bdl/axisram.h>

#define MX_SDMMC1_SD_MAX_RETRY_TIMES 8

extern xwsz_t axisram_mr_origin[];
extern xwsz_t axisram_mr_size[];

extern xwsz_t sdram_mr_origin[];
extern xwsz_t sdram_mr_size[];

extern struct xwmm_mempool * sdram_mempool;

struct xwosal_mtx hsd1_xfer_mtx;
xwid_t hsd1_xfer_mtx_id;
struct xwosal_splk hsd1_lock;
struct xwosal_cdt hsd1_xfer_cdt;
xwid_t hsd1_xfer_cdt_id;
xwer_t hsd1_xfer_rc = XWOK;

static
xwer_t MX_SDMMC1_SD_Construct(void)
{
  xwer_t rc;

  xwosal_splk_init(&hsd1_lock);
  rc = xwosal_mtx_init(&hsd1_xfer_mtx, XWOSAL_SD_PRIORITY_RT_MAX);
  if (rc < 0) {
    goto err_mtx_init;
  }
  hsd1_xfer_mtx_id = xwosal_mtx_get_id(&hsd1_xfer_mtx);

  rc = xwosal_cdt_init(&hsd1_xfer_cdt);
  if (rc < 0) {
    goto err_cdt_init;
  }
  hsd1_xfer_cdt_id = xwosal_cdt_get_id(&hsd1_xfer_cdt);
  return XWOK;

err_cdt_init:
  xwosal_mtx_destroy(&hsd1_xfer_mtx);
err_mtx_init:
  return rc;
}

void MX_SDMMC1_SD_Destruct(void)
{
  xwosal_cdt_destroy(&hsd1_xfer_cdt);
  xwosal_mtx_destroy(&hsd1_xfer_mtx);
}

/* USER CODE END 0 */

SD_HandleTypeDef hsd1;

/* SDMMC1 init function */

void MX_SDMMC1_SD_Init(void)
{

  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 4;
  hsd1.Init.TranceiverPresent = SDMMC_TRANSCEIVER_NOT_PRESENT;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(sdHandle->Instance==SDMMC1)
  {
  /* USER CODE BEGIN SDMMC1_MspInit 0 */
    xwer_t rc;
    rc = MX_SDMMC1_SD_Construct();
    if (rc < 0)
    {
      Error_Handler();
    }

  /* USER CODE END SDMMC1_MspInit 0 */
    /* SDMMC1 clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDMMC1 interrupt Init */
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
  /* USER CODE BEGIN SDMMC1_MspInit 1 */

  /* USER CODE END SDMMC1_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDMMC1)
  {
  /* USER CODE BEGIN SDMMC1_MspDeInit 0 */

  /* USER CODE END SDMMC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC1_CLK_DISABLE();

    /**SDMMC1 GPIO Configuration
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDMMC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);
  /* USER CODE BEGIN SDMMC1_MspDeInit 1 */
    MX_SDMMC1_SD_Destruct();
  /* USER CODE END SDMMC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void MX_SDMMC1_SD_Deinit(void)
{
  HAL_SD_DeInit(&hsd1);
}

void MX_SDMMC1_SD_Reinit(uint32_t clkdiv)
{
  HAL_Delay(1);
  MX_SDMMC1_SD_Deinit();
  HAL_Delay(1);

  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = clkdiv;
  hsd1.Init.TranceiverPresent = SDMMC_TRANSCEIVER_NOT_PRESENT;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }
}

xwer_t MX_SDMMC1_SD_TrimClk(xwsq_t cnt)
{
  __aligned_l1cacheline xwu8_t buf[512];
  xwer_t rc;
  xwsq_t i;
  xwtm_t time;

  memset(buf, 0xFF, 512);
  for (i = 0; i < cnt; i++) {
    time = 1 * XWTM_MS;
    xwosal_cthrd_sleep(&time);
    rc = MX_SDMMC1_SD_Read(buf, i, 1);
    if (XWOK == rc) {
    } else if (-EIO == rc) {
      if (hsd1.Init.ClockDiv <= 0xFD) {
        MX_SDMMC1_SD_Reinit(hsd1.Init.ClockDiv + 1);
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

xwer_t MX_SDMMC1_SD_GetState(void)
{
  xwer_t rc;
  xwsq_t cnt;
  HAL_SD_CardStateTypeDef cardst;

  cnt = 0;
  do {
    cardst = HAL_SD_GetCardState(&hsd1);
    if ((0 == cardst) && (HAL_SD_ERROR_NONE != hsd1.ErrorCode)) {
      rc = -EIO;
      MX_SDMMC1_SD_Reinit(hsd1.Init.ClockDiv);
      cnt++;
    } else if (HAL_SD_CARD_TRANSFER == cardst) {
      rc = XWOK;
    } else {
      rc = -EBUSY;
    }
  } while ((-EIO == rc) && (cnt < MX_SDMMC1_SD_MAX_RETRY_TIMES));
  return rc;
}

xwer_t MX_SDMMC1_SD_Read(uint8_t * buf, uint32_t blkaddr,
                         uint32_t nrblk)
{
  xwer_t rc;
  xwsq_t cnt;
  HAL_SD_CardStateTypeDef cardst;
  void * mem = NULL;

  if (((!xwmm_in_zone(buf, (xwptr_t)axisram_mr_origin, (xwsz_t)axisram_mr_size)) &&
       (!xwmm_in_zone(buf, (xwptr_t)sdram_mr_origin, (xwsz_t)sdram_mr_size))) ||
      (((xwu32_t)buf) & (CPUCFG_L1_CACHELINE_SIZE - 1))) {
    rc = axisram_alloc(nrblk * BLOCKSIZE, &mem);
    if (rc < 0) {
      mem = NULL;
      rc = xwmm_mempool_malloc(sdram_mempool, nrblk * BLOCKSIZE, &mem);
      if (rc < 0) {
        goto err_nomem;
      }
    }
  } else {
    mem = buf;
  }

  rc = xwosal_mtx_lock(hsd1_xfer_mtx_id);
  if (rc < 0) {
    goto err_mtx_lock;
  }

  cnt = 0;
  do {
    if (rc < 0) {
      MX_SDMMC1_SD_Reinit(hsd1.Init.ClockDiv);
      cnt++;
    }
    cardst = HAL_SD_GetCardState(&hsd1);
    if ((0 == cardst) && (HAL_SD_ERROR_NONE != hsd1.ErrorCode)) {
      rc = -EIO;
    } else if (HAL_SD_CARD_TRANSFER == cardst) {
      HAL_StatusTypeDef halrc;
      xwreg_t cpuirq;
      xwsq_t lkst;
      union xwlk_ulock ulk;

      hsd1_xfer_rc = -EINPROGRESS;
      ulk.osal.splk = &hsd1_lock;
      halrc = HAL_SD_ReadBlocks_DMA(&hsd1, mem, blkaddr, nrblk);
      if (HAL_OK == halrc) {
        xwosal_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
        if (-EINPROGRESS == hsd1_xfer_rc) {
          rc = xwosal_cdt_wait(hsd1_xfer_cdt_id,
                               ulk, XWLK_TYPE_SPLK,
                               NULL, &lkst);
          if (XWOK == rc) {
            rc = hsd1_xfer_rc;
          } else {
            if (XWLK_STATE_UNLOCKED == lkst) {
              xwosal_splk_lock(&hsd1_lock);
            }
          }
        } else {
          rc = hsd1_xfer_rc;
        }
        xwosal_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
      } else if (HAL_BUSY == halrc) {
        rc = -EBUSY;
      } else {
        rc = -EIO;
      }
    } else {
      rc = -EBUSY;
    }
  } while ((-EIO == rc) && (cnt < MX_SDMMC1_SD_MAX_RETRY_TIMES));
  xwosal_mtx_unlock(hsd1_xfer_mtx_id);
  if (XWOK == rc) {
    SCB_InvalidateDCache_by_Addr((uint32_t *)mem, (int32_t)(nrblk * BLOCKSIZE));
    if (mem != buf) {
      memcpy(buf, mem, nrblk * BLOCKSIZE);
    }
  }

err_mtx_lock:
  if (mem != buf) {
    if (xwmm_in_zone(mem, (xwptr_t)axisram_mr_origin, (xwsz_t)axisram_mr_size)) {
      axisram_free(mem);
    } else if (xwmm_in_zone(mem, (xwptr_t)sdram_mr_origin, (xwsz_t)sdram_mr_size)) {
      xwmm_mempool_free(sdram_mempool, mem);
    }
  }
err_nomem:
  return rc;
}

xwer_t MX_SDMMC1_SD_Write(uint8_t * data, uint32_t blkaddr,
                          uint32_t nrblk)
{
  xwer_t rc;
  xwsq_t cnt;
  HAL_SD_CardStateTypeDef cardst;
  void * mem = NULL;

  if (((!xwmm_in_zone(data, (xwptr_t)axisram_mr_origin, (xwsz_t)axisram_mr_size)) &&
       (!xwmm_in_zone(data, (xwptr_t)sdram_mr_origin, (xwsz_t)sdram_mr_size))) ||
      (((xwu32_t)data) & (CPUCFG_L1_CACHELINE_SIZE - 1))) {
    rc = axisram_alloc(nrblk * BLOCKSIZE, &mem);
    if (rc < 0) {
      mem = NULL;
      rc = xwmm_mempool_malloc(sdram_mempool, nrblk * BLOCKSIZE, &mem);
      if (rc < 0) {
        goto err_nomem;
      }
      memcpy(mem, data, nrblk * BLOCKSIZE);
    }
  } else {
    mem = data;
  }

  SCB_CleanDCache_by_Addr((uint32_t *)mem, (int32_t)(nrblk * BLOCKSIZE));
  rc = xwosal_mtx_lock(hsd1_xfer_mtx_id);
  if (rc < 0) {
    goto err_mtx_lock;
  }

  cnt = 0;
  do {
    if (rc < 0) {
      MX_SDMMC1_SD_Reinit(hsd1.Init.ClockDiv);
      cnt++;
    }
    cardst = HAL_SD_GetCardState(&hsd1);
    if ((0 == cardst) && (HAL_SD_ERROR_NONE != hsd1.ErrorCode)) {
      rc = -EIO;
    } else if (HAL_SD_CARD_TRANSFER == cardst) {
      HAL_StatusTypeDef halrc;
      xwreg_t cpuirq;
      xwsq_t lkst;
      union xwlk_ulock ulk;

      hsd1_xfer_rc = -EINPROGRESS;
      ulk.osal.splk = &hsd1_lock;
      halrc = HAL_SD_WriteBlocks_DMA(&hsd1, mem, blkaddr, nrblk);
      if (HAL_OK == halrc) {
        xwosal_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
        if (-EINPROGRESS == hsd1_xfer_rc) {
          rc = xwosal_cdt_wait(hsd1_xfer_cdt_id,
                               ulk, XWLK_TYPE_SPLK,
                               NULL, &lkst);
          if (XWOK == rc) {
            rc = hsd1_xfer_rc;
          } else {
            if (XWLK_STATE_UNLOCKED == lkst) {
              xwosal_splk_lock(&hsd1_lock);
            }
          }
        } else {
          rc = hsd1_xfer_rc;
        }
        xwosal_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
      } else if (HAL_BUSY == halrc) {
        rc = -EBUSY;
      } else {
        rc = -EIO;
      }
    } else {
      rc = -EBUSY;
    }
  } while ((-EIO == rc) && (cnt < MX_SDMMC1_SD_MAX_RETRY_TIMES));
  xwosal_mtx_unlock(hsd1_xfer_mtx_id);

err_mtx_lock:
  if (mem != data) {
    if (xwmm_in_zone(mem, (xwptr_t)axisram_mr_origin, (xwsz_t)axisram_mr_size)) {
      axisram_free(mem);
    } else if (xwmm_in_zone(mem, (xwptr_t)sdram_mr_origin, (xwsz_t)sdram_mr_size)) {
      xwmm_mempool_free(sdram_mempool, mem);
    }
  }
err_nomem:
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
    xwosal_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd1_xfer_rc = -EIO;
    } else {
      hsd1_xfer_rc = XWOK;
    }
    xwosal_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
    xwosal_cdt_broadcast(hsd1_xfer_cdt_id);
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
    xwosal_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd1_xfer_rc = -EIO;
    } else {
      hsd1_xfer_rc = XWOK;
    }
    xwosal_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
    xwosal_cdt_broadcast(hsd1_xfer_cdt_id);
  }
}

void HAL_SD_ErrorCallback(SD_HandleTypeDef * phsd)
{
  if ((SD_CONTEXT_NONE == phsd->Context) && (HAL_SD_STATE_READY == phsd->State)) {
    xwreg_t cpuirq;
    xwosal_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd1_xfer_rc = -EIO;
    } else {
      hsd1_xfer_rc = XWOK;
    }
    xwosal_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
    xwosal_cdt_broadcast(hsd1_xfer_cdt_id);
  }
}

void HAL_SD_AbortCallback(SD_HandleTypeDef * phsd)
{
  if ((SD_CONTEXT_NONE == phsd->Context) && (HAL_SD_STATE_READY == phsd->State)) {
    xwreg_t cpuirq;
    xwosal_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
    hsd1_xfer_rc = -ECANCELED;
    xwosal_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
    xwosal_cdt_broadcast(hsd1_xfer_cdt_id);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
