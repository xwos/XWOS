/**
 * @file
 * @brief 玄武OS通用库：64位除法
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_div64_h__
#define __xwos_lib_div64_h__

#include <xwos/standard.h>

/**
 * @defgroup xwos_lib_div64 64位除法
 * @ingroup xwos_lib
 * @{
 */

/**
 * @brief long long类型除法
 * @param[in,out] n: 指向无符号64位整数缓冲区的指针，
 * + (I) 作为输入时，表示被除数
 * + (O) 作为输出时，返回商
 * @param[in] divisor: 除数
 * @return 余数
 */
#define xwdivll(n, divisor)     xwdiv64(n, divisor)

__xwlib_code
xwu32_t xwdiv64(xwu64_t * n, xwu32_t divisor);

/**
 * @} xwos_lib_div64
 */

#endif /* xwos/lib/div64.h */
