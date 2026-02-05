/**
 * @file
 * @brief 板级描述层：XWOS适配层：库：CRC
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#include "board/std.h"
#include <xwos/lib/xwbop.h>
#include <xwos/lib/crc32.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include "bm/Stm32Hal/mi.h"

struct xwos_splk xwlibac_crc_splk;
struct xwos_mtx xwlibac_crc_mtx;

void xwlibac_crc_init(void)
{
        xwos_splk_init(&xwlibac_crc_splk);
        xwos_mtx_init(&xwlibac_crc_mtx, XWOS_SKD_PRIORITY_RT_MAX);
}

xwer_t xwlibac_crc_lock(xwreg_t * cpuirq)
{
        xwsq_t ctx;
        xwer_t rc;

        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_THD == ctx) {
                rc = xwos_mtx_lock(&xwlibac_crc_mtx);
        } else if (XWOS_SKD_CONTEXT_ISR == ctx) {
                xwos_splk_lock_cpuirqsv(&xwlibac_crc_splk, cpuirq);
                rc = XWOK;
        } else if (XWOS_SKD_CONTEXT_BH == ctx) {
                xwos_splk_lock_cpuirq(&xwlibac_crc_splk);
                rc = XWOK;
        } else if (XWOS_SKD_CONTEXT_IDLE == ctx) {
                xwos_splk_lock_cpuirq(&xwlibac_crc_splk);
                rc = XWOK;
        } else {
                rc = XWOK;
        }
        return rc;
}

xwer_t xwlibac_crc_unlock(xwreg_t cpuirq)
{
        xwsq_t ctx;
        xwer_t rc;

        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_THD == ctx) {
                rc = xwos_mtx_unlock(&xwlibac_crc_mtx);
        } else if (XWOS_SKD_CONTEXT_ISR == ctx) {
                xwos_splk_unlock_cpuirqrs(&xwlibac_crc_splk, cpuirq);
                rc = XWOK;
        } else if (XWOS_SKD_CONTEXT_BH == ctx) {
                xwos_splk_unlock_cpuirq(&xwlibac_crc_splk);
                rc = XWOK;
        } else if (XWOS_SKD_CONTEXT_IDLE == ctx) {
                xwos_splk_unlock_cpuirq(&xwlibac_crc_splk);
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
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwlibac_crc_lock(&cpuirq);
        if (XWOK == rc) {
                rc = stm32hal_crc32_cal(crc32, refin, polynomial, direction,
                                        stream, size);
                xwlibac_crc_unlock(cpuirq);
        }
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
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwlibac_crc_lock(&cpuirq);
        if (XWOK == rc) {
                rc = stm32hal_crc8_cal(crc8, refin, polynomial, stream, size);
                xwlibac_crc_unlock(cpuirq);
        }
        return rc;
}
