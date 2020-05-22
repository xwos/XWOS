/**
 * @file
 * @brief 位图操作汇编库：xwbmpop_x1i
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
__xwbsp_code __naked
void xwbmpop_x1i(__maybe_unused xwbmp_t * addr, __maybe_unused xwsq_t n)
{
        __asm__ volatile("      lsr     r2, r1, #5"); /* r2 = nr / 32; */
        __asm__ volatile("      and     r3, r1, #31"); /* r3 = nr % 32; */
        __asm__ volatile("      mov     r1, #1");
        __asm__ volatile("      lsl     r1, r3"); /* r1 = bitmask; */
        __asm__ volatile("      ldr     r3, [r0, r2, lsl #2]"); /* r3 = *(addr+r2); */
        __asm__ volatile("      eor     r3, r1"); /* r3 ^= r1; */
        __asm__ volatile("      str     r3, [r0, r2, lsl #2]"); /* *(addr+r2) = r3; */
        __asm__ volatile("      bx      lr"); /* return; */
}
