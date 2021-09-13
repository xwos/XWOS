/**
 * @file
 * @brief 位图操作汇编库：xwbmpop_flz
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

__xwbsp_code
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        i = BITS_TO_XWBMP_T(num);
        msk = ~(XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((~(xwbmp_t)0) == msk) {
                msk = (xwbmp_t)0;
        }
        do {
                i--;
                tmp = ~(bmp[i] | msk);
                if (tmp) {
                        break;
                }
                msk = (xwbmp_t)0;
         } while (i > 0);
        __asm__ volatile(
        "       clz             %[__p], %[__tmp]\n"
        : [__p] "=&r" (p)
        : [__tmp] "r" (tmp)
        :
        );
        p = (BITS_PER_XWBMP_T - 1) - p;
        if (p >= 0) {
                p += (xwssq_t)(i << XWBMP_T_SHIFT);
        }/* else {} */
        return p;
}
