/**
 * @file
 * @brief 汇编库：位操作
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

#ifndef __asmlib_xwbop_h__
#define __asmlib_xwbop_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  macro functions & inline functions ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_C0M8) && (1 == ARCHCFG_LIB_XWBOP_C0M8))
#define arch_xwbop_c0m8(a, mask)                        \
        __asm__ volatile(                               \
        "       bic     %[__a], %[__mask]\n"            \
        : [__a] "+r" (*(a))                             \
        : [__mask] "rI" (mask)                          \
        : "memory"                                      \
        )
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_C0M8) && (1 == ARCHCFG_LIB_XWBOP_C0M8)) */

#if (defined(ARCHCFG_LIB_XWBOP_C0M16) && (1 == ARCHCFG_LIB_XWBOP_C0M16))
#define arch_xwbop_c0m16(a, mask)                       \
        __asm__ volatile(                               \
        "       bic     %[__a], %[__mask]\n"            \
        : [__a] "+r" (*(a))                             \
        : [__mask] "rI" (mask)                          \
        : "memory"                                      \
        )
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_C0M16) && (1 == ARCHCFG_LIB_XWBOP_C0M16)) */

#if (defined(ARCHCFG_LIB_XWBOP_C0M32) && (1 == ARCHCFG_LIB_XWBOP_C0M32))
#define arch_xwbop_c0m32(a, mask)                       \
        __asm__ volatile(                               \
        "       bic     %[__a], %[__mask]\n"            \
        : [__a] "+r" (*(a))                             \
        : [__mask] "rI" (mask)                          \
        : "memory"                                      \
        )
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_C0M32) && (1 == ARCHCFG_LIB_XWBOP_C0M32)) */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwlib_code
void arch_xwbop_c0m64(xwu64_t * a, xwu64_t mask);

__xwlib_code
xwu8_t arch_xwbop_rbit8(xwu8_t x);

__xwlib_code
xwu16_t arch_xwbop_rbit16(xwu16_t x);

__xwlib_code
xwu32_t arch_xwbop_rbit32(xwu32_t x);

__xwlib_code
xwu64_t arch_xwbop_rbit64(xwu64_t x);

__xwlib_code
xwu16_t arch_xwbop_re16(xwu16_t x);

__xwlib_code
xws32_t arch_xwbop_re16s32(xwu16_t x);

__xwlib_code
xwu32_t arch_xwbop_re32(xwu32_t x);

__xwlib_code
xws64_t arch_xwbop_re32s64(xwu32_t x);

__xwlib_code
xwu64_t arch_xwbop_re64(xwu64_t x);

__xwlib_code
xwssq_t arch_xwbop_ffs8(xwu8_t x);

__xwlib_code
xwssq_t arch_xwbop_fls8(xwu8_t x);

__xwlib_code
xwssq_t arch_xwbop_ffs16(xwu16_t x);

__xwlib_code
xwssq_t arch_xwbop_fls16(xwu16_t x);

__xwlib_code
xwssq_t arch_xwbop_ffs32(xwu32_t x);

__xwlib_code
xwssq_t arch_xwbop_fls32(xwu32_t x);

__xwlib_code
xwssq_t arch_xwbop_ffs64(xwu64_t x);

__xwlib_code
xwssq_t arch_xwbop_fls64(xwu64_t x);

#endif /* asmlib/xwbop.h */
