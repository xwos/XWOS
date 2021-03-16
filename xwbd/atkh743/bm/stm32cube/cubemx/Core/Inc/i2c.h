/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <bm/stm32cube/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/i2c/master.h>
/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN Private defines */
#define MX_I2C_MEM_MAXSIZE     256U /**< 单次最大发送的字节数 */

struct MX_I2C_MasterDriverData {
  I2C_HandleTypeDef * halhdl;
  struct xwds_i2cm * i2cm;
  struct xwos_cond cond; /**< 条件量 */
  struct xwos_splk splk; /**< 保证发送状态只被单一上下文访问的锁 */
  struct xwds_i2c_msg * msg; /**< 正在传输的消息 */
  xwer_t rc; /**< 返回值 */
  xwu8_t mem[MX_I2C_MEM_MAXSIZE] __xwcc_alignl1cache; /**< 发送缓冲区 */
  xwu32_t size; /**< 待发送的数据大小 */
};

extern struct MX_I2C_MasterDriverData hi2c2_drvdata;

/* USER CODE END Private defines */

void MX_I2C2_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_I2C2_DeInit(void);
void MX_I2C2_ReInit(xwu16_t addm);
xwu16_t MX_I2C2_GetAddressingMode(void);
xwer_t MX_I2C2_Xfer(struct xwds_i2c_msg * msg);
xwer_t MX_I2C2_Abort(xwu16_t addr);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
