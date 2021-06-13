/**
 * @file
 * @brief XWOS通用库：CRC32
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_crc32_h__
#define __xwos_lib_crc32_h__

#include <xwos/standard.h>

/**
 * @defgroup CRC32 CRC32
 * @{
 */

/**
 * @brief 计算CRC32的移位方向枚举
 */
enum xwlib_crc32_shift_direction_em {
        XWLIB_CRC32_LEFT_SHIFT, /**< 左移 */
        XWLIB_CRC32_RIGHT_SHIFT, /**< 右移 */
};

xwer_t xwlib_crc32_cal(xwu32_t * crc32, xwu32_t xorout,
                       bool refin, bool refout,
                       xwu32_t plynml, xwu32_t direction,
                       const xwu8_t stream[], xwsz_t * size);
xwu32_t xwlib_crc32_calms(const xwu8_t stream[], xwsz_t * size);

/**
 * @} CRC32
 */

#endif /* xwos/lib/crc32.h */
