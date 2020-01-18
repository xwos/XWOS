/**
 * @file
 * @brief 位操作汇编库：arch_xwbop_re32s64
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <asmlib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code
xws64_t arch_xwbop_re32s64(xwu32_t x)
{
        union {
                xws64_t q;
                xwu32_t w[2];
        } r;

        __asm__ volatile(
        "       rev     %[__r], %[__x]\n"
        : [__r] "=&r" (r.w[0])
        : [__x] "r" (x)
        : "memory"
        );
        if (r.w[0] & 0x80000000UL) {
                r.w[1] = (xwu32_t)(~0UL);
        }/* else {} */
        return r.q;
}
