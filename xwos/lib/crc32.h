/**
 * @file
 * @brief XWOS通用库：CRC32
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_crc32_h__
#define __xwos_lib_crc32_h__

#include <xwos/standard.h>

/**
 * @defgroup xwos_lib_crc32 CRC32
 * @ingroup xwos_lib
 * @{
 */

extern const xwu32_t xwlib_crc32tbl_0x04c11db7[256];
extern const xwu32_t xwlib_crc32tbl_0xedb88320[256];

/**
 * @brief 计算CRC32的移位方向枚举
 */
enum xwlib_crc32_shift_direction_em {
        XWLIB_CRC32_LEFT_SHIFT, /**< 左移 */
        XWLIB_CRC32_RIGHT_SHIFT, /**< 右移 */
};

/**
 * @brief 计算一段数据的CRC32校验值
 * @param[in,out] crc32: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示初始值
 * + (O) 作为输出时，返回计算结果，当计算失败时，值不会发生改变
 * @param[in] xorout: 与结果异或的值
 * @param[in] refin: 是否按位镜像翻转输入的每个字节(xwbop_rbit8)
 * @param[in] refout: 是否按位镜像翻转输出(xwbop_rbit32)
 * @param[in] plynml: 多项式，取值：
 *   @arg 0x04C11DB7
 *   @arg 0xEDB88320
 * @param[in] direction: 数据移位的方向
 * @param[in] stream: 指向数据的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度，当返回不为0时，
 *       表示计算未完成，计算结果不会镜像翻转，也不会与xorout异或。
 * @return 错误码
 * @retval XWOK: 没有错误，计算结果有效
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持的多项式
 */
xwer_t xwlib_crc32_cal(xwu32_t * crc32, xwu32_t xorout,
                       bool refin, bool refout,
                       xwu32_t plynml, xwu32_t direction,
                       const xwu8_t stream[], xwsz_t * size);

/**
 * @brief 用主流CRC32参数模型计算一段数据的校验值
 * @param[in] stream: 指向数据的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度
 * @return CRC32
 * @note
 * + 主流CRC32参数模型：
 *   - 多项式：0xEDB88320
 *   - 初始值：0xFFFFFFFF
 *   - 数据移位方向：右移
 *   - 是否按位镜像翻转输入：否
 *   - 是否按位镜像翻转输出：否
 *   - 与结果进行异或计算：0xFFFFFFFF
 * + 主流CRC32参数的镜像模型：
 *   - 多项式：0x04C11DB7
 *   - 初始值：0xFFFFFFFF
 *   - 数据移位方向：左移
 *   - 是否按位镜像翻转输入：是
 *   - 是否按位镜像翻转输出：是
 *   - 与结果进行异或计算：0xFFFFFFFF
 * + 上面两个模型计算结果是一致的；
 * + 本算法优先使用硬件加速运算，但某些平台的硬件可能有字节数的要求
 *   （例如STM32要求字节数为4的倍数），多余的字节再采用软件计算；
 * + 本软件算法使用的是直驱表法。
 */
xwu32_t xwlib_crc32_calms(const xwu8_t stream[], xwsz_t * size);

/**
 * @} xwos_lib_crc32
 */

#endif /* xwos/lib/crc32.h */
