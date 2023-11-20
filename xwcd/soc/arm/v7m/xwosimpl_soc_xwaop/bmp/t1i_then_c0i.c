/**
 * @file
 * @brief 位图原子操作汇编库: xwbmpaop_t1i_then_c0i
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

__xwlib_code __xwcc_naked
xwssq_t xwbmpaop_t1i_then_c0i(__xwcc_unused atomic_xwbmp_t * bmp,
                              __xwcc_unused xwsq_t idx)
{
        __asm__ volatile("      lsr     r2, r1, #5");           /* r2 = n / 32; (BIC low 5 bits) */
        __asm__ volatile("      lsl     r2 ,r2, #2");           /* r2 <<= 2; */
        __asm__ volatile("      add     r0, r2");               /* bmp = (void *)((xwptr_t)bmp + r2); */
        __asm__ volatile("      and     r3, r1, #31");          /* r3 = n % 32; */
        __asm__ volatile("      mov     r1, #1");
        __asm__ volatile("      lsl     r1, r3");               /* r1 = bitmask; */
        __asm__ volatile("1:");
        __asm__ volatile("      ldrex   r2, [r0, #0]");         /* r2 = *(r0); */
        __asm__ volatile("      dmb");
        __asm__ volatile("      tst     r2, r1");               /* result = r2 & r1; update Z flag */
        __asm__ volatile("      ittee   ne\n"                   /* if (result is non-zero (Z != 1)) */
                         "      bicne   r2, r1\n"               /* r2 &= ~r1; */
                         "      strexne r3, r2, [r0, #0]\n"     /* *(r0) = r2; r3 = result; */
                         "      mvneq   r0, %[__rc]\n"
                         "      bxeq    lr" : : [__rc] "I" (EACCES - 1) :);
        __asm__ volatile("      teq     r3, #0");
        __asm__ volatile("      bne     1b");
        __asm__ volatile("      mov     r0, %[__rc]"  : : [__rc] "I" (XWOK) :);
        __asm__ volatile("      bx      lr");                   /* return XWOK; */
}
