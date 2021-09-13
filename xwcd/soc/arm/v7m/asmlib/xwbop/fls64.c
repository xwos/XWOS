/**
 * @file
 * @brief 位操作汇编库：xwbop_fls64
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

__xwbsp_code __xwcc_naked
xwssq_t xwbop_fls64(__xwcc_unused xwu64_t x)
{
        __asm__ volatile("      cmp     r1, #0");       /* if (high_word(x) != 0) */
        __asm__ volatile("      ittt    ne\n"           /* {                      */
                         "      clzne   r0, r1\n"       /*      r0 = clz(r1);     */
                         "      rsbne   r0, r0, #63\n"  /*      r0 = 63 - r0;     */
                         "      bxne    lr");           /*      return r0;        */
                                                        /* }                      */
        __asm__ volatile("      clz     r0, r0");       /* r0 = clz(r0);          */
        __asm__ volatile("      rsb     r0, r0, #31");  /* r0 = 31 - r0;          */
        __asm__ volatile("      bx      lr");           /* return r0;             */
}
