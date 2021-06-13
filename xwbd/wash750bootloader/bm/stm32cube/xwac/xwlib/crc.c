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
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <bm/stm32cube/cubemx/Core/Inc/crc.h>

struct xwos_splk stm32cube_crc_splk;
struct xwos_mtx stm32cube_crc_mtx;

void stm32cube_crc_init(void)
{
        xwos_splk_init(&stm32cube_crc_splk);
        xwos_mtx_init(&stm32cube_crc_mtx, XWOS_SKD_PRIORITY_RT_MAX);
}

xwer_t stm32cube_crc_lock(xwreg_t * cpuirq)
{
        xwsq_t ctx;
        xwer_t rc;

        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_THD == ctx) {
                rc = xwos_mtx_lock(&stm32cube_crc_mtx);
        } else if (XWOS_SKD_CONTEXT_ISR == ctx) {
                xwos_splk_lock_cpuirqsv(&stm32cube_crc_splk, cpuirq);
                rc = XWOK;
        } else if (XWOS_SKD_CONTEXT_BH == ctx) {
                xwos_splk_lock_cpuirq(&stm32cube_crc_splk);
                rc = XWOK;
        } else if (XWOS_SKD_CONTEXT_IDLE == ctx) {
                xwos_splk_lock_cpuirq(&stm32cube_crc_splk);
                rc = XWOK;
        } else {
                rc = XWOK;
        }
        return rc;
}

xwer_t stm32cube_crc_unlock(xwreg_t cpuirq)
{
        xwsq_t ctx;
        xwer_t rc;

        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_THD == ctx) {
                rc = xwos_mtx_unlock(&stm32cube_crc_mtx);
        } else if (XWOS_SKD_CONTEXT_ISR == ctx) {
                xwos_splk_unlock_cpuirqrs(&stm32cube_crc_splk, cpuirq);
                rc = XWOK;
        } else if (XWOS_SKD_CONTEXT_BH == ctx) {
                xwos_splk_unlock_cpuirq(&stm32cube_crc_splk);
                rc = XWOK;
        } else if (XWOS_SKD_CONTEXT_IDLE == ctx) {
                xwos_splk_unlock_cpuirq(&stm32cube_crc_splk);
                rc = XWOK;
        } else {
                rc = XWOK;
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
xwer_t soc_crc32_cal(xwu32_t * crc32,
                     bool refin, xwu32_t polynomial, xwu32_t direction,
                     const xwu8_t stream[], xwsz_t * size)
{
        bool refout;
        xwsz_t rest;
        xwsz_t pos;
        xwu32_t seg;
        xwu32_t * p;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = stm32cube_crc_lock(&cpuirq);
        if (rc < 0) {
                goto err_crc_lock;
        }

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

        stm32cube_crc_unlock(cpuirq);
err_crc_lock:
        return rc;
}

/**
 * @brief 使用STM32的CRC硬件单元计算数据的CRC8校验值
 * @param[in,out] crc8: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示初始值
 * + (O) 作为输出时，返回计算结果
 * @param[in] refin: 是否颠倒每个字节中的位的顺序
 * @param[in] stream: 数据缓冲区
 * @param[in] polynomial: 多项式
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度
 * @return 错误码
 * @retval XWOK: 计算结果有效
 * @retval -EOPNOTSUPP: 计算失败
 */
xwer_t soc_crc8_cal(xwu8_t * crc8,
                    bool refin, xwu8_t polynomial,
                    const xwu8_t stream[], xwsz_t * size)
{
        xwsz_t rest;
        xwsz_t pos;
        xwu8_t seg;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = stm32cube_crc_lock(&cpuirq);
        if (rc < 0) {
                goto err_crc_lock;
        }

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

        stm32cube_crc_unlock(cpuirq);
err_crc_lock:
        return rc;
}
