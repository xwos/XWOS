/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <bm/stm32cube/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/spi/master.h>

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

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
extern struct MX_SPI_MasterDriverData hspi2_drvdata;

/* USER CODE END Private defines */

void MX_SPI1_Init(void);
void MX_SPI2_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_SPI1_DeInit(void);
xwer_t MX_SPI1_ReInit(const SPI_InitTypeDef * cfg);
xwer_t MX_SPI1_Xfer(const xwu8_t txd[], xwu8_t * rxb, xwsz_t * size);
xwer_t MX_SPI1_Abort(void);

void MX_SPI2_DeInit(void);
xwer_t MX_SPI2_ReInit(const SPI_InitTypeDef * cfg);
xwer_t MX_SPI2_Xfer(const xwu8_t txd[], xwu8_t * rxb, xwsz_t * size);
xwer_t MX_SPI2_Abort(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
