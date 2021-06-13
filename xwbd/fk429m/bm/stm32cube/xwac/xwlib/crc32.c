/**
 * @file
 * @brief STM32CUBE模块：CRC32
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
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/crc32.h>
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_crc.h"

static
xwer_t stm32_crc32_ls(xwu32_t * crc32, bool refin, bool refout,
                      const xwu8_t stream[], xwsz_t * size);

static
xwer_t stm32_crc32_ls(xwu32_t * crc32, bool refin, bool refout,
                      const xwu8_t stream[], xwsz_t * size)
{
        xwsz_t rest;
        xwsz_t pos;
        xwu32_t seg;
        xwu32_t * p;
        xwer_t rc;

        if ((*crc32 > 0) && (*crc32 < 0xFFFFFFFF)) {
                /* STM32F4不支持任意CRC32初始值 */
                rc = -EOPNOTSUPP;
        } else {
                LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);
                LL_CRC_ResetCRCCalculationUnit(CRC);
                if (0x00000000 == *crc32) {
                        LL_CRC_FeedData32(CRC, 0xFFFFFFFF);
                }
                rest = *size;
                pos = 0;
                if ((xwptr_t)stream & (sizeof(xwu32_t) - 1)) {
                        /* 数据地址没有对齐到4的边界上，不能直接按32位访问数据内存，
                           否则会出busfault */
                        while (rest >= sizeof(xwu32_t)) {
                                seg = (xwu32_t)stream[pos] << 24U;
                                seg |= (xwu32_t)stream[pos + 1] << 16U;
                                seg |= (xwu32_t)stream[pos + 2] << 8U;
                                seg |= (xwu32_t)stream[pos + 3];
                                if (refin) {
                                        seg = xwbop_rbit32(seg);
                                        seg = xwbop_re32(seg);
                                }
                                LL_CRC_FeedData32(CRC, seg);
                                rest -= sizeof(xwu32_t);
                                pos += sizeof(xwu32_t);
                        }
                } else {
                        /* 当数据地址对齐到4的边界上时可提高效率 */
                        while (rest >= sizeof(xwu32_t)) {
                                /* STM32是小端模式处理器，CRC硬件单元是从32位的
                                   数据寄存器的最高有效字节开始处理数据的。
                                   stream[pos + 0]在小端模式是最低有效字节，
                                   stream[pos + 3]是最高有效字节，
                                   因此需要做大小端转换。*/
                                p = (xwu32_t *)&stream[pos];
                                if (refin) {
                                        seg = xwbop_rbit32(*p);
                                } else {
                                        seg = xwbop_re32(*p);
                                }
                                LL_CRC_FeedData32(CRC, seg);
                                rest -= sizeof(xwu32_t);
                                pos += sizeof(xwu32_t);
                        }
                }
                if (pos > 0) {
                        seg = LL_CRC_ReadData32(CRC);
                        if (refout) {
                                seg = xwbop_rbit32(seg);
                        }
                        rc = XWOK;
                } else {
                        seg = *crc32;
                        rc = -EOPNOTSUPP;
                }
                LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_CRC);
                *crc32 = seg;
                *size = rest;
        }
        return rc;
}

/**
 * @brief 使用STM32的CRC硬件单元计算数据的CRC32校验值
 * @param[in,out] crc32: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示初始值
 * + (O) 作为输出时，返回计算结果
 * @param[in] refin: 是否颠倒每个字节中的位的顺序
 * @param[in] stream: 数据缓冲区
 * @param[in] polynomial: 多项式
 * @param[in] direction: 数据移位的方向
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度
 * @return 错误码
 * @retval XWOK: 计算结果有效
 * @retval -EOPNOTSUPP: 计算失败
 */
xwer_t soc_crc32_cal(xwu32_t * crc32, bool refin,
                     xwu32_t polynomial, xwu32_t direction,
                     const xwu8_t stream[], xwsz_t * size)
{
        bool refout;
        xwer_t rc;

        /* STM32的CRC单元是左移模式，右移模式需要对参数进行镜像转换 */
        if (XWLIB_CRC32_RIGHT_SHIFT == direction) {
                polynomial = xwbop_rbit32(polynomial);
                refin = !refin;
                refout = true;
        } else {
                refout = false;
        }
        if (0x04C11DB7 == polynomial) {
                rc = stm32_crc32_ls(crc32, refin, refout, stream, size);
        } else {
                rc = -EOPNOTSUPP;
        }
        return rc;
}
