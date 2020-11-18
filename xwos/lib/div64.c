/**
 * @file
 * @brief XWOS通用库：64位除法
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/div64.h>

static __xwlib_code
xwu32_t xwdiv64_32(xwu64_t * dividend, xwu32_t divisor);

/**
 * @brief 64位除法运算
 * @param n: (I) 被除数
 *           (O) 商
 * @param divisor: (I) 除数
 * @return 余数
 */
__xwlib_code
xwu32_t xwdiv64(xwu64_t * n, xwu32_t divisor)
{
        xwu32_t rem;
        if (((*n) >> 32) == 0) {
                rem = (xwu32_t)(*n) % divisor;
                *n = (xwu32_t)(*n) / divisor;
        } else {
                rem = xwdiv64_32(n, divisor);
        }
        return rem;
}

/**
 * @brief 用一个32位数除以一个64位数
 * @param n: (I) 被除数
 *           (O) 商
 * @param divisor: (I) 除数
 * @return 余数
 */
static __xwlib_code
xwu32_t xwdiv64_32(xwu64_t * n, xwu32_t divisor)
{
        xwu64_t rem = *n;
        xwu64_t b = divisor;
        xwu64_t res, d = 1;
        xwu32_t high = (xwu32_t)(rem >> 32);

        res = 0;
        if (high >= divisor) {
                high /= divisor;
                res = (xwu64_t) high << 32;
                rem -= (xwu64_t) (high * divisor) << 32;
        }

        while ((xws64_t)b > 0 && b < rem) {
                b = b + b;
                d = d + d;
        }

        do {
                if (rem >= b) {
                        rem -= b;
                        res += d;
                }
                b >>= 1;
                d >>= 1;
        } while (d);

        *n = res;
        return (xwu32_t)rem;
}
