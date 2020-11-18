/**
 * @file
 * @brief 位操作汇编库：arch_c0m64
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
#include <asmlib/xwbop.h>

__xwbsp_code __xwcc_naked
void arch_xwbop_c0m64(__xwcc_unused xwu64_t * a, __xwcc_unused xwu64_t mask)
{
        __asm__ volatile("      ldr     r1, [r0, #0]");
        __asm__ volatile("      bic     r2, r1, r2"); /* r2 is low 32-bit of mask */
        __asm__ volatile("      ldr     r1, [r0, #4]");
        __asm__ volatile("      bic     r3, r1, r3"); /* r3 is high 32-bit of mask */
        __asm__ volatile("      strd    r2, r3, [r0, #0]");
        __asm__ volatile("      bx      lr");
}
