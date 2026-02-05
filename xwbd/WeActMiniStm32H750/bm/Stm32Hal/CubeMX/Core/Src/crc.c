/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    crc.c
  * @brief   This file provides code for the configuration
  *          of the CRC instances.
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
#include "crc.h"

/* USER CODE BEGIN 0 */
#include <board/std.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/crc32.h>

/* USER CODE END 0 */

/* CRC init function */
void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* Peripheral clock enable */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_CRC);

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_NONE);
  LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_NONE);
  LL_CRC_SetPolynomialCoef(CRC, LL_CRC_DEFAULT_CRC32_POLY);
  LL_CRC_SetPolynomialSize(CRC, LL_CRC_POLYLENGTH_32B);
  LL_CRC_SetInitialData(CRC, LL_CRC_DEFAULT_CRC_INITVALUE);
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/* USER CODE BEGIN 1 */
xwer_t MX_CRC32_Cal(xwu32_t * crc32,
                    bool refin, xwu32_t polynomial, xwu32_t direction,
                    const xwu8_t stream[], xwsz_t * size)
{
  bool refout;
  xwsz_t rest;
  xwsz_t pos;
  xwu32_t seg;
  xwu32_t * p;
  xwer_t rc;

  rest = *size;
  pos = 0;
  seg = *crc32;
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_CRC);
  LL_CRC_SetPolynomialSize(CRC, LL_CRC_POLYLENGTH_32B);
  LL_CRC_SetInitialData(CRC, seg);
  /* STM32H7的CRC单元是左移模式，右移模式需要对参数进行镜像转换 */
  if (XWLIB_CRC32_RIGHT_SHIFT == direction) {
    polynomial = xwbop_rbit32(polynomial);
    refin = !refin;
    refout = true;
  } else {
    refout = false;
  }
  if (refin) {
    LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_BYTE);
  } else {
    LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_NONE);
  }
  if (refout) {
    LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_BIT);
  } else {
    LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_NONE);
  }
  LL_CRC_SetPolynomialCoef(CRC, polynomial);
  LL_CRC_ResetCRCCalculationUnit(CRC);
  if ((xwptr_t)stream & (sizeof(xwu32_t) - 1)) {
    /* 数据地址没有对齐到4的边界上，不能直接按32位访问数据内存，
       否则会出busfault */
    while (rest >= sizeof(xwu32_t)) {
      seg = (xwu32_t)stream[pos] << 24U;
      seg |= (xwu32_t)stream[pos + 1] << 16U;
      seg |= (xwu32_t)stream[pos + 2] << 8U;
      seg |= (xwu32_t)stream[pos + 3];
      LL_CRC_FeedData32(CRC, seg);
      rest -= sizeof(xwu32_t);
      pos += sizeof(xwu32_t);
    }
  } else {
    /* 当数据地址对齐到4的边界上时可提高效率 */
    while (rest >= sizeof(xwu32_t)) {
      /* STM32是小端模式处理器，CRC硬件单元是从32位的数据寄存器的
         最高有效字节开始处理数据的。stream[pos + 0]在小端模式是最低
         有效字节，stream[pos + 3]是最高有效字节，
         因此需要做大小端转换。*/
      p = (xwu32_t *)&stream[pos];
      seg = xwbop_re32(*p);
      LL_CRC_FeedData32(CRC, seg);
      rest -= sizeof(xwu32_t);
      pos += sizeof(xwu32_t);
    }
  }
  if (pos > 0) {
    seg = LL_CRC_ReadData32(CRC);
    rc = XWOK;
  } else {
    seg = *crc32;
    rc = -EOPNOTSUPP;
  }
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_CRC);
  *size = rest;
  *crc32 = seg;
  return rc;
}

xwer_t MX_CRC8_Cal(xwu8_t * crc8,
                   bool refin, xwu8_t polynomial,
                   const xwu8_t stream[], xwsz_t * size)
{
  xwsz_t rest;
  xwsz_t pos;
  xwu8_t seg;
  xwer_t rc;

  rest = *size;
  pos = 0;
  seg = *crc8;
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_CRC);
  LL_CRC_SetPolynomialSize(CRC, LL_CRC_POLYLENGTH_8B);
  LL_CRC_SetInitialData(CRC, (xwu32_t)seg);
  if (refin) {
    LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_BYTE);
  } else {
    LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_NONE);
  }
  LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_NONE);
  LL_CRC_SetPolynomialCoef(CRC, (xwu32_t)polynomial);
  LL_CRC_ResetCRCCalculationUnit(CRC);
  while (rest >= sizeof(xwu8_t)) {
    LL_CRC_FeedData8(CRC, stream[pos]);
    rest -= sizeof(xwu8_t);
    pos += sizeof(xwu8_t);
  }
  if (pos > 0) {
    seg = LL_CRC_ReadData8(CRC);
    rc = XWOK;
  } else {
    rc = -EOPNOTSUPP;
  }
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_CRC);
  *size = rest;
  *crc8 = seg;
  return rc;
}
/* USER CODE END 1 */
