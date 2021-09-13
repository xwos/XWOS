/**
  ******************************************************************************
  * @file    sdmmc.c
  * @brief   This file provides code for the configuration
  *          of the SDMMC instances.
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
#include <xwos/lib/xwbop.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/mm/common.h>
#include <xwos/mm/mempool/allocator.h>
#include <bdl/axisram.h>

#define MX_SDMMC1_SD_MAX_RETRY_TIMES 8

extern xwsz_t axisram_mr_origin[];
extern xwsz_t axisram_mr_size[];

extern xwsz_t sdram_mr_origin[];
extern xwsz_t sdram_mr_size[];

extern struct xwmm_mempool * sdram_mempool;

struct xwos_mtx hsd1_xfer_mtx;
struct xwos_splk hsd1_lock;
struct xwos_cond hsd1_xfer_cond;
xwer_t hsd1_xfer_rc = XWOK;

static
xwer_t MX_SDMMC1_SD_Construct(void)
{
  xwer_t rc;

  xwos_splk_init(&hsd1_lock);
  rc = xwos_mtx_init(&hsd1_xfer_mtx, XWOS_SKD_PRIORITY_RT_MAX);
  if (rc < 0) {
    goto err_mtx_init;
  }

  rc = xwos_cond_init(&hsd1_xfer_cond);
  if (rc < 0) {
    goto err_cond_init;
  }

  return XWOK;

err_cond_init:
  xwos_mtx_fini(&hsd1_xfer_mtx);
err_mtx_init:
  return rc;
}

void MX_SDMMC1_SD_Destruct(void)
{
  xwos_cond_fini(&hsd1_xfer_cond);
  xwos_mtx_fini(&hsd1_xfer_mtx);
}

/* USER CODE END 0 */

SD_HandleTypeDef hsd1;

/* SDMMC1 init function */

void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 7;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
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
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDMMC;
    PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* SDMMC1 clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_8
                          |GPIO_PIN_9;
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
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 1, 0);
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
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_8
                          |GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDMMC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);
  /* USER CODE BEGIN SDMMC1_MspDeInit 1 */
    MX_SDMMC1_SD_Destruct();
  /* USER CODE END SDMMC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void MX_SDMMC1_SD_DeInit(void)
{
  HAL_SD_DeInit(&hsd1);
}

void MX_SDMMC1_SD_ReInit(uint32_t clkdiv)
{
  HAL_Delay(1);
  MX_SDMMC1_SD_DeInit();
  HAL_Delay(1);

  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = clkdiv;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }
}

xwer_t MX_SDMMC1_SD_TrimClk(xwsq_t cnt)
{
  xwu8_t __xwcc_alignl1cache buf[512];
  xwer_t rc;
  xwsq_t i;
  xwtm_t time;

  rc = XWOK;
  memset(buf, 0xFF, 512);
  for (i = 0; i < cnt; i++) {
    time = 1 * XWTM_MS;
    xwos_cthd_sleep(&time);
    rc = MX_SDMMC1_SD_Read(buf, i, 1);
    if (XWOK == rc) {
    } else if (-EIO == rc) {
      if (hsd1.Init.ClockDiv <= 0xFD) {
        MX_SDMMC1_SD_ReInit(hsd1.Init.ClockDiv + 1);
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
      MX_SDMMC1_SD_ReInit(hsd1.Init.ClockDiv);
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

  rc = xwos_mtx_lock(&hsd1_xfer_mtx);
  if (rc < 0) {
    goto err_mtx_lock;
  }

  cnt = 0;
  do {
    if (rc < 0) {
      MX_SDMMC1_SD_ReInit(hsd1.Init.ClockDiv);
      cnt++;
    }
    cardst = HAL_SD_GetCardState(&hsd1);
    if ((0 == cardst) && (HAL_SD_ERROR_NONE != hsd1.ErrorCode)) {
      rc = -EIO;
    } else if (HAL_SD_CARD_TRANSFER == cardst) {
      HAL_StatusTypeDef halrc;
      xwreg_t cpuirq;
      xwsq_t lkst;
      union xwos_ulock ulk;

      hsd1_xfer_rc = -EINPROGRESS;
      ulk.osal.splk = &hsd1_lock;
      halrc = HAL_SD_ReadBlocks_DMA(&hsd1, mem, blkaddr, nrblk);
      if (HAL_OK == halrc) {
        xwos_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
        if (-EINPROGRESS == hsd1_xfer_rc) {
          rc = xwos_cond_wait(&hsd1_xfer_cond,
                              ulk, XWOS_LK_SPLK,
                              NULL, &lkst);
          if (XWOK == rc) {
            rc = hsd1_xfer_rc;
          } else {
            if (XWOS_LKST_UNLOCKED == lkst) {
              xwos_splk_lock(&hsd1_lock);
            }
          }
        } else {
          rc = hsd1_xfer_rc;
        }
        xwos_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
      } else if (HAL_BUSY == halrc) {
        rc = -EBUSY;
      } else {
        rc = -EIO;
      }
    } else {
      rc = -EBUSY;
    }
  } while ((-EIO == rc) && (cnt < MX_SDMMC1_SD_MAX_RETRY_TIMES));
  xwos_mtx_unlock(&hsd1_xfer_mtx);
  if (XWOK == rc) {
#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
    SCB_InvalidateDCache_by_Addr((uint32_t *)mem,
                                 (int32_t)XWBOP_ALIGN((nrblk * BLOCKSIZE),
                                                      CPUCFG_L1_CACHELINE_SIZE));
#endif
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
    }
    memcpy(mem, data, nrblk * BLOCKSIZE);
  } else {
    mem = data;
  }

#if defined(STM32CUBECFG_DCACHE) && (1 == STM32CUBECFG_DCACHE)
  SCB_CleanDCache_by_Addr((uint32_t *)mem,
                          (int32_t)XWBOP_ALIGN((nrblk * BLOCKSIZE),
                                               CPUCFG_L1_CACHELINE_SIZE));
#endif
  rc = xwos_mtx_lock(&hsd1_xfer_mtx);
  if (rc < 0) {
    goto err_mtx_lock;
  }

  cnt = 0;
  do {
    if (rc < 0) {
      MX_SDMMC1_SD_ReInit(hsd1.Init.ClockDiv);
      cnt++;
    }
    cardst = HAL_SD_GetCardState(&hsd1);
    if ((0 == cardst) && (HAL_SD_ERROR_NONE != hsd1.ErrorCode)) {
      rc = -EIO;
    } else if (HAL_SD_CARD_TRANSFER == cardst) {
      HAL_StatusTypeDef halrc;
      xwreg_t cpuirq;
      xwsq_t lkst;
      union xwos_ulock ulk;

      hsd1_xfer_rc = -EINPROGRESS;
      ulk.osal.splk = &hsd1_lock;
      halrc = HAL_SD_WriteBlocks_DMA(&hsd1, mem, blkaddr, nrblk);
      if (HAL_OK == halrc) {
        xwos_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
        if (-EINPROGRESS == hsd1_xfer_rc) {
          rc = xwos_cond_wait(&hsd1_xfer_cond,
                              ulk, XWOS_LK_SPLK,
                              NULL, &lkst);
          if (XWOK == rc) {
            rc = hsd1_xfer_rc;
          } else {
            if (XWOS_LKST_UNLOCKED == lkst) {
              xwos_splk_lock(&hsd1_lock);
            }
          }
        } else {
          rc = hsd1_xfer_rc;
        }
        xwos_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
      } else if (HAL_BUSY == halrc) {
        rc = -EBUSY;
      } else {
        rc = -EIO;
      }
    } else {
      rc = -EBUSY;
    }
  } while ((-EIO == rc) && (cnt < MX_SDMMC1_SD_MAX_RETRY_TIMES));
  xwos_mtx_unlock(&hsd1_xfer_mtx);

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
    xwos_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd1_xfer_rc = -EIO;
    } else {
      hsd1_xfer_rc = XWOK;
    }
    xwos_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
    xwos_cond_broadcast(&hsd1_xfer_cond);
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
    xwos_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd1_xfer_rc = -EIO;
    } else {
      hsd1_xfer_rc = XWOK;
    }
    xwos_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
    xwos_cond_broadcast(&hsd1_xfer_cond);
  }
}

void HAL_SD_ErrorCallback(SD_HandleTypeDef * phsd)
{
  if ((SD_CONTEXT_NONE == phsd->Context) && (HAL_SD_STATE_READY == phsd->State)) {
    xwreg_t cpuirq;
    xwos_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
    if (phsd->ErrorCode != HAL_SD_ERROR_NONE) {
      hsd1_xfer_rc = -EIO;
    } else {
      hsd1_xfer_rc = XWOK;
    }
    xwos_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
    xwos_cond_broadcast(&hsd1_xfer_cond);
  }
}

void HAL_SD_AbortCallback(SD_HandleTypeDef * phsd)
{
  if ((SD_CONTEXT_NONE == phsd->Context) && (HAL_SD_STATE_READY == phsd->State)) {
    xwreg_t cpuirq;
    xwos_splk_lock_cpuirqsv(&hsd1_lock, &cpuirq);
    hsd1_xfer_rc = -ECANCELED;
    xwos_splk_unlock_cpuirqrs(&hsd1_lock, cpuirq);
    xwos_cond_broadcast(&hsd1_xfer_cond);
  }
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
