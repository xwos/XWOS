/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "board/std.h"
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/i2c/master.h>
/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN Private defines */
#define MX_I2C_MEM_MAXSIZE     256U /**< 单次最大发送的字节数 */

struct MX_I2C_MasterDriverData {
  struct xwds_i2cm * i2cm;
  struct xwos_cond cond; /**< 条件量 */
  struct xwos_splk splk; /**< 保证发送状态只被单一上下文访问的锁 */
  struct xwds_i2c_msg * msg; /**< 正在传输的消息 */
  xwer_t rc; /**< 返回值 */
  xwu8_t mem[MX_I2C_MEM_MAXSIZE] __xwcc_alignl1cache; /**< 发送缓冲区 */
  xwu32_t size; /**< 待发送的数据大小 */
};

extern struct MX_I2C_MasterDriverData hi2c1_drvdata;

/* USER CODE END Private defines */

void MX_I2C1_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_I2C_Init(xwu32_t hwid);
void MX_I2C_DeInit(xwu32_t hwid);
void MX_I2C_ReInit(xwu32_t hwid, xwu16_t addm);
xwer_t MX_I2C_GetAddressingMode(xwu32_t hwid, xwu16_t * m);
xwer_t MX_I2C_Xfer(xwu32_t hwid, struct xwds_i2c_msg * msg);
xwer_t MX_I2C_Abort(xwu32_t hwid, xwu16_t addr);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */
