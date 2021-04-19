/**
 * @file
 * @brief 位操作汇编库：xwbop_re64
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

__xwbsp_code
xwu64_t xwbop_re64(xwu64_t x)
{
        xwu32_t * addrl;
        xwu32_t * addrh;
        union {
                xwu64_t q;
                xwu32_t w[2];
        } r;

        addrl = (xwu32_t *)&x;
        addrh = addrl + 1;
        __asm__ volatile(
        "       rev     %[__r_l32], %[__x_h32]\n"
        "       rev     %[__r_h32], %[__x_l32]\n"
        : [__r_l32] "=&r" (r.w[0]),
          [__r_h32] "=&r" (r.w[1])
        : [__x_l32] "r" (*addrl),
          [__x_h32] "r" (*addrh)
        : "memory"
        );
        return r.q;
}
