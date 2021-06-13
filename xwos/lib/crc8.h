/**
 * @file
 * @brief XWOS通用库：CRC8
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_crc8_h__
#define __xwos_lib_crc8_h__

#include <xwos/standard.h>

/**
 * @defgroup CRC8 CRC8
 * @{
 */

xwer_t xwlib_crc8_cal(xwu8_t * crc8,
                      xwu8_t xorout, bool refin, bool refout, xwu8_t plynml,
                      const xwu8_t stream[], xwsz_t * size);
xwu8_t xwlib_crc8_calms(const xwu8_t stream[], xwsz_t * size);

/**
 * @} CRC8
 */

#endif /* xwos/lib/crc8.h */
