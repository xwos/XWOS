/**
 * @file
 * @brief 架构描述层：System Call & Trap
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
#include <arch_sc_trap.h>

/**
 * @brief system call entry
 */
__xwbsp_code
xws64_t arch_systemcall(xwid_t sc_no, xwreg_t arg1, xwreg_t arg2)
{
        xws64_t rc;

        __asm__ volatile(
        "       se_mr           3, %[__sc_no]\n" /* system call number */
        "       se_mr           4, %[__arg1]\n" /* arg1 */
        "       se_mr           5, %[__arg2]\n" /* arg2 */
        "       se_sc\n"
        :
        : [__sc_no] "r" (sc_no),
          [__arg1] "r" (arg1),
          [__arg2] "r" (arg2)
        : "3", "4", "5", "lr"
        );

        __asm__ volatile(
        "       se_stw    3, 0 (%[__prc])\n" /* r3, r4 is result */
        "       se_stw    4, 4 (%[__prc])\n" /* r3, r4 is result */
        :
        : [__prc] "r" (&rc)
        : "3", "4", "memory"
        );
        return rc;
}
