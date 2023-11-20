/**
 * @file
 * @brief 位图操作汇编库：xwbmpop_ffs
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

__xwbsp_code
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk;
        xwbmp_t tmp;
        xwssq_t p;

        m = BITS_TO_XWBMP_T(num);
        p = -(xwssq_t)1;
        for (i = (xwsz_t)0; i < m; i++) {
                if (i == (m - (xwsz_t)1)) {
                        msk = XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1;
                        if ((xwbmp_t)0 == msk) {
                                msk = (~(xwbmp_t)0);
                        }
                        tmp = bmp[i] & msk;
                } else {
                        tmp = bmp[i];
                }
                if (tmp) {
                        __asm__ volatile(
                        "       rbit    %[__p], %[__tmp]\n"
                        "       clz     %[__p], %[__p]\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        :
                        );
                        p += ((xwssq_t)i << (xwssq_t)XWBMP_T_SHIFT);
                        break;
                }
        }
        return p;
}
