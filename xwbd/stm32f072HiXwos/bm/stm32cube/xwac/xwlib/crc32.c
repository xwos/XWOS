/**
 * @file
 * @brief STM32CUBE：CRC32
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/crc32.h>
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_crc.h"

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwsz_t stm32_crc32_ls(xwu32_t * crc32, bool refin, const xwu8_t stream[], xwsz_t len);

static
xwsz_t stm32_crc32_rs(xwu32_t * crc32, bool refin, const xwu8_t stream[], xwsz_t len);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief Computes the 32-bit CRC of a given buffer of data stream (left-shift).
 * @param crc32: (I) buffer to give initial value
 *               (O) buffer to return the result
 * @param refin: (I) reverse bits of every byte (rbit8) if ture
 * @param stream: (I) pointer to the buffer containing the data to be computed
 * @param len: (I) length of the buffer to be computed
 * @return real length of data that be computed. Caller must computed the
 *         remaining data by self
 */
static
xwsz_t stm32_crc32_ls(xwu32_t * crc32, bool refin, const xwu8_t stream[], xwsz_t len)
{
        xwsz_t i;
        xwu32_t * p;
        xwu32_t tmp;

        if (__unlikely((*crc32 > 0) && (*crc32 < 0xFFFFFFFF))) {
                return 0;
        }

        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);
        LL_CRC_ResetCRCCalculationUnit(CRC);
        if (0x00000000 == *crc32) {
                LL_CRC_FeedData32(CRC, 0xFFFFFFFF);
        }
        i = 0;
        while (len >= sizeof(xwu32_t)) {
                p = (xwu32_t *)&stream[i];
                /* The CRC data register of stm32 is a 32-bit little-endian register,
                   When writing the register, the *(stream+3) is the most significant
                   byte and the CRC hardware computes the data from the most significant
                   byte. So it is necessary to reverse endian of the 32-bit data
                   before writing it to the register. */
                if (refin) {
                        tmp = xwbop_rbit(xwu32_t, *p);
                } else {
                        tmp = xwbop_re(xwu32_t, *p);
                }
                LL_CRC_FeedData32(CRC, tmp);
                len -= sizeof(xwu32_t);
                i += sizeof(xwu32_t);
        }
        *crc32 = LL_CRC_ReadData32(CRC);
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_CRC);
        return i;
}

/**
 * @brief Computes the 32-bit CRC of a given buffer of data stream (right-shift).
 * @param crc32: (I) buffer to give initial value
 *               (O) buffer to return the result
 * @param refin: (I) reverse bits of every byte (rbit8) if ture
 * @param stream: (I) pointer to the buffer containing the data to be computed
 * @param len: (I) length of the buffer to be computed
 * @return real length of data that be computed. Caller must computed the
 *         remaining data by self
 */
static
xwsz_t stm32_crc32_rs(xwu32_t * crc32, bool refin,
                      const xwu8_t stream[], xwsz_t len)
{
        xwsz_t i;
        xwu32_t * p;
        xwu32_t tmp;

        if (__unlikely((*crc32 > 0) && (*crc32 < 0xFFFFFFFF))) {
                return 0;
        }

        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);
        LL_CRC_ResetCRCCalculationUnit(CRC);
        if (0x00000000 == *crc32) {
                LL_CRC_FeedData32(CRC, 0xFFFFFFFF);
        }
        i = 0;
        while (len >= sizeof(xwu32_t)) {
                p = (xwu32_t *)&stream[i];
                /* The CRC data register of stm32 is a 32-bit little-endian register,
                   When writing the register, the *(stream+3) is the most significant
                   byte and the CRC hardware computes the data from the most significant
                   byte. So it is necessary to reverse endian of the 32-bit data
                   before writing it to the register. */
                if (refin) {
                        tmp = xwbop_re(xwu32_t, *p);
                } else {
                        tmp = xwbop_rbit(xwu32_t, *p);
                }
                LL_CRC_FeedData32(CRC, tmp);
                len -= sizeof(xwu32_t);
                i += sizeof(xwu32_t);
        }
        tmp = LL_CRC_ReadData32(CRC);
        LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_CRC);
        *crc32 = xwbop_rbit(xwu32_t, tmp);
        return i;
}

/**
 * @brief Computes the 32-bit CRC of a given buffer of data stream (SOC specific)
 * @param crc32: (I) buffer to give initial value
 *               (O) buffer to return the result
 * @param refin: (I) reverse bits of every byte (rbit8) if ture
 * @param polynomial: (I) polynomial
 * @param direction: (I) data stream shift direction: left or right
 * @param stream: (I) pointer to the data buffer
 * @param len: (I) data length
 * @return crc32 result of giving data to <b><em>crc32</em></b>
 * @retval real length of data that be computed. Caller must computed the
 *         remaining data by self
 */
xwsz_t soc_crc32_cal(xwu32_t * crc32, bool refin, int polynomial,
                     int direction, const xwu8_t stream[], xwsz_t len)
{
        xwsz_t real = 0;
        switch (polynomial) {
#if defined(XWLIBCFG_CRC32_0X04C11DB7) && (1 == XWLIBCFG_CRC32_0X04C11DB7)
        case XWLIB_CRC32_PLYNML_0X04C11DB7:
                if (XWLIB_CRC32_LEFT_SHIFT == direction) {
                        real = stm32_crc32_ls(crc32, refin, stream, len);
                }
                break;
#endif /* XWLIBCFG_CRC32_0X04C11DB7 */
#if defined(XWLIBCFG_CRC32_0XEDB88320) && (1 == XWLIBCFG_CRC32_0XEDB88320)
        case XWLIB_CRC32_PLYNML_0XEDB88320:
                if (XWLIB_CRC32_RIGHT_SHIFT == direction) {
                        real = stm32_crc32_rs(crc32, refin, stream, len);
                }
                break;
#endif /* XWLIBCFG_CRC32_0XEDB88320 */
        default:
                break;
        }
        return real;
}
