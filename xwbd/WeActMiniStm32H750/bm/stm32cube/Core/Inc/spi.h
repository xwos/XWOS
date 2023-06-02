/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/spi/master.h>

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

extern SPI_HandleTypeDef hspi4;

/* USER CODE BEGIN Private defines */
#define MX_SPI_MEM_MAXSIZE     128U /**< 单次最大发送的字节数 */

struct MX_SPI_MasterDriverData {
  SPI_HandleTypeDef * halhdl;
  struct xwds_spim * spim;
  struct xwos_cond cond; /**< 条件量 */
  struct xwos_splk splk; /**< 保证发送状态只被单一上下文访问的锁 */
  const xwu8_t * txd; /**< 用户发送数据 */
  xwu8_t * rxb; /**< 用户接受缓冲区 */
  xwu32_t size; /**< 待传输的数据大小 */
  xwer_t rc; /**< 返回值 */
  struct {
    xwu8_t tx[MX_SPI_MEM_MAXSIZE] __xwcc_alignl1cache; /**< 发送缓冲区 */
    xwu8_t rx[MX_SPI_MEM_MAXSIZE] __xwcc_alignl1cache; /**< 发送缓冲区 */
  } mem; /**< 缓冲区 */
};

extern struct MX_SPI_MasterDriverData hspi1_drvdata;
extern struct MX_SPI_MasterDriverData hspi4_drvdata;

/* USER CODE END Private defines */

void MX_SPI1_Init(void);
void MX_SPI4_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_SPI1_DeInit(void);
xwer_t MX_SPI1_ReInit(const SPI_InitTypeDef * cfg);
xwer_t MX_SPI1_Xfer(const xwu8_t txd[], xwu8_t * rxb, xwsz_t * size);
xwer_t MX_SPI1_Abort(void);

void MX_SPI4_DeInit(void);
xwer_t MX_SPI4_ReInit(const SPI_InitTypeDef * cfg);
xwer_t MX_SPI4_Xfer(const xwu8_t txd[], xwu8_t * rxb, xwsz_t * size);
xwer_t MX_SPI4_Abort(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

