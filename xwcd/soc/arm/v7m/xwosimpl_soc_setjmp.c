/**
 * @file
 * @brief 玄武OS移植实现层：SOC setjmp/longjmp
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
#include <armv7m_isa.h>
#include <xwos/lib/setjmp.h>

__xwlib_code __xwcc_naked
int xwlib_setjmp(__xwcc_unused xwjmpbuf_b ctx)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      mov     ip, sp");
        __asm__ volatile("      stmia   r0!, {r4-r12, lr}");
#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
        __asm__ volatile("      vstmia  r0!, {s16-s31}");
#endif
        __asm__ volatile("      mov     r0, #0");
        __asm__ volatile("      bx      lr");
}

__xwlib_code __xwcc_naked
void xwlib_longjmp(__xwcc_unused xwjmpbuf_b ctx, __xwcc_unused int val)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      ldmia   r0!, {r4-r12, lr}");
        __asm__ volatile("      mov     sp, ip");
#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
        __asm__ volatile("      vldmia  r0!, {s16-s31}");
#endif
        __asm__ volatile("      movs	r0, r1");
        __asm__ volatile("      it	eq\n"
                         "      moveq	r0, #1");
        __asm__ volatile("      bx	lr");
}
