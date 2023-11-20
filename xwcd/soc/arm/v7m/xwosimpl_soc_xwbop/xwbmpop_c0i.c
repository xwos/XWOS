/**
 * @file
 * @brief 位图操作汇编库：xwbmpop_c0i
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

__xwbsp_code __xwcc_naked
void xwbmpop_c0i(__xwcc_unused xwbmp_t * bmp, __xwcc_unused xwsq_t n)
{
        __asm__ volatile("      lsr     r2, r1, #5"); /* r2 = nr / 32; */
        __asm__ volatile("      and     r3, r1, #31");/* r3 = nr % 32; */
        __asm__ volatile("      mov     r1, #1");
        __asm__ volatile("      lsl     r1, r3"); /* r1 = bitmask; */
        __asm__ volatile("      ldr     r3, [r0, r2, lsl #2]"); /* r3 = *(bmp+r2); */
        __asm__ volatile("      bic     r3, r1"); /* r3 &= (~r1); */
        __asm__ volatile("      str     r3, [r0, r2, lsl #2]"); /* *(bmp+r2) = r3; */
        __asm__ volatile("      bx      lr"); /* return; */
}
