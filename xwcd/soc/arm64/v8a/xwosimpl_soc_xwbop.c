/**
 * @file
 * @brief XWOS移植实现层：SOC：位操作
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <xwos/lib/xwbop.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>

__xwbsp_code
xwssq_t xwbop_ffs8(xwu8_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x000000FF;
        return xwbop_ffs32(tmp);
}

__xwbsp_code
xwssq_t xwbop_fls8(xwu8_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x000000FF;
        return xwbop_fls32(tmp);
}

__xwbsp_code
xwssq_t xwbop_ffz8(xwu8_t x)
{
        xwu32_t tmp = x | (xwu32_t)0xFFFFFF00;
        return xwbop_ffz32(tmp);
}

__xwbsp_code
xwssq_t xwbop_flz8(xwu8_t x)
{
        xwu32_t tmp = x | (xwu32_t)0xFFFFFF00;
        return xwbop_flz32(tmp);
}

__xwbsp_code
xwu8_t xwbop_rbit8(xwu8_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x000000FF;
        __asm__ volatile(
        "       rbit    %w[__tmp], %w[__tmp]\n"
        : [__tmp] "+r" (tmp)
        :
        :
        );
        return (xwu8_t)(tmp >> (xwu32_t)24);
}

__xwbsp_code
xwssq_t xwbop_ffs16(xwu16_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x0000FFFF;
        return xwbop_ffs32(tmp);
}

__xwbsp_code
xwssq_t xwbop_fls16(xwu16_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x0000FFFF;
        return xwbop_fls32(tmp);
}

__xwbsp_code
xwssq_t xwbop_ffz16(xwu16_t x)
{
        xwu32_t tmp = x | (xwu32_t)0xFFFF0000;
        return xwbop_ffz32(tmp);
}

__xwbsp_code
xwssq_t xwbop_flz16(xwu16_t x)
{
        xwu32_t tmp = x | (xwu32_t)0xFFFF0000;
        return xwbop_flz32(tmp);
}

__xwbsp_code
xwu16_t xwbop_rbit16(xwu16_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x0000FFFF;
        __asm__ volatile(
        "       rbit    %w[__tmp], %w[__tmp]\n"
        : [__tmp] "+r" (tmp)
        :
        :
        );
        return (xwu16_t)(tmp >> (xwu32_t)16);
}

__xwbsp_code
xwu16_t xwbop_re16(xwu16_t x)
{
        xwu32_t res;
        xwu32_t tmp = x & (xwu32_t)0x0000FFFF;
        __asm__ volatile(
        "       rev16   %w[__res], %w[__tmp]\n"
        : [__res] "=&r" (res)
        : [__tmp] "r" (tmp)
        :
        );
        return (xwu16_t)res;
}

__xwbsp_code
xwssq_t xwbop_ffs32(xwu32_t x)
{
        register xws32_t pos;
        __asm__ volatile(
        "       rbit    %w[__pos], %w[__x]\n"
        "       clz     %w[__pos], %w[__pos]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        if (pos >= (xws32_t)32) {
                pos = (-(xws32_t)1);
        }
        return (xwssq_t)pos;
}

__xwbsp_code
xwssq_t xwbop_fls32(xwu32_t x)
{
        xws32_t pos;
        __asm__ volatile(
        "       clz     %w[__pos], %w[__x]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        pos = (xws32_t)31 - pos;
        return (xwssq_t)pos;
}

__xwbsp_code
xwssq_t xwbop_ffz32(xwu32_t x)
{
        register xws32_t pos;
        __asm__ volatile(
        "       rbit    %w[__pos], %w[__x]\n"
        "       cls     %w[__pos], %w[__pos]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        if (pos >= (xws32_t)32) {
                pos = (-(xws32_t)1);
        }
        return (xwssq_t)pos;
}

__xwbsp_code
xwssq_t xwbop_flz32(xwu32_t x)
{
        xws32_t pos;
        __asm__ volatile(
        "       cls     %w[__pos], %w[__x]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        pos = (xws32_t)31 - pos;
        return (xwssq_t)pos;
}

__xwbsp_code
xwu32_t xwbop_rbit32(xwu32_t x)
{
        __asm__ volatile(
        "       rbit    %w[__x], %w[__x]\n"
        : [__x] "+r" (x)
        :
        :
        );
        return x;
}

__xwbsp_code
xwu32_t xwbop_re32(xwu32_t x)
{
        xwu32_t res;
        __asm__ volatile(
        "       rev     %w[__res], %w[__x]\n"
        : [__res] "=&r" (res)
        : [__x] "r" (x)
        :
        );
        return res;
}

__xwbsp_code
xwssq_t xwbop_ffs64(xwu64_t x)
{
        xws64_t pos;
        __asm__ volatile(
        "       rbit    %[__pos], %[__x]\n"
        "       clz     %[__pos], %[__pos]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        if (pos >= (xws64_t)64) {
                pos = -(xws64_t)1;
        }
        return (xwssq_t)pos;
}

__xwbsp_code
xwssq_t xwbop_fls64(xwu64_t x)
{
        xws64_t pos;
        __asm__ volatile(
        "       clz     %[__pos], %[__x]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        pos = (xws64_t)63 - pos;
        return (xwssq_t)pos;
}

__xwbsp_code
xwssq_t xwbop_ffz64(xwu64_t x)
{
        xws64_t pos;
        __asm__ volatile(
        "       rbit    %[__pos], %[__x]\n"
        "       cls     %[__pos], %[__pos]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        if (pos >= (xws64_t)64) {
                pos = -(xws64_t)1;
        }
        return (xwssq_t)pos;
}

__xwbsp_code
xwssq_t xwbop_flz64(xwu64_t x)
{
        xws64_t pos;
        __asm__ volatile(
        "       cls     %[__pos], %[__x]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        pos = (xws64_t)63 - pos;
        return (xwssq_t)pos;
}

__xwbsp_code
xwu64_t xwbop_rbit64(xwu64_t x)
{
        __asm__ volatile(
        "       rbit    %[__x], %[__x]\n"
        : [__x] "+r" (x)
        :
        :
        );
        return x;
}

__xwbsp_code
xwu64_t xwbop_re64(xwu64_t x)
{
        xwu64_t res;
        __asm__ volatile(
        "       rev     %[__res], %[__x]\n"
        : [__res] "=&r" (res)
        : [__x] "r" (x)
        :
        );
        return res;
}

__xwbsp_code
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk;
        xwbmp_t tmp;
        xwssq_t p;

        m = BITS_TO_XWBMP_T(num);
        p = -(xwssq_t)1;
        for (i = (xwsz_t)0; i < m; i++) {
                if (i == (m - (xwsz_t)1)) {
                        msk = (XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((xwbmp_t)0 == msk) {
                                msk = (~(xwbmp_t)0);
                        }
                        tmp = bmp[i] & msk;
                } else {
                        tmp = bmp[i];
                }
                if (tmp) {
                        __asm__ volatile(
                        "       rbit    %[__p], %[__tmp]\n"
                        "       clz     %[__p], %[__p]\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        :
                        );
                        p += ((xwssq_t)i << (xwssq_t)XWBMP_T_SHIFT);
                        break;
                }
        }
        return p;
}

__xwbsp_code
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        m = BITS_TO_XWBMP_T(num);
        p = -(xwssq_t)1;
        for (i = (xwsz_t)0; i < m; i++) {
                if (i == (m - (xwsz_t)1)) {
                        msk = ~(XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((~(xwbmp_t)0) == msk) {
                                msk = (xwbmp_t)0;
                        }
                } else {
                        msk = (xwbmp_t)0;
                }
                tmp = ~(bmp[i] | msk);
                if (tmp) {
                        __asm__ volatile(
                        "       rbit    %[__p], %[__tmp]\n"
                        "       clz     %[__p], %[__p]\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        :
                        );
                        p += ((xwssq_t)i << (xwssq_t)XWBMP_T_SHIFT);
                        break;
                }
        }
        return p;
}

__xwbsp_code
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t i;
        xwbmp_t msk;
        xwbmp_t tmp;
        xwssq_t p;

        tmp = 0;
        i = BITS_TO_XWBMP_T(num);
        msk = (XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((xwbmp_t)0 == msk) {
                msk = (~(xwbmp_t)0);
        }
        while (i > (xwsz_t)0) {
                i--;
                tmp = bmp[i] & msk;
                if (tmp) {
                        break;
                }
                msk = (~(xwbmp_t)0);
        }
        __asm__ volatile(
        "       clz     %[__p], %[__tmp]\n"
        : [__p] "=&r" (p)
        : [__tmp] "r" (tmp)
        :
        );
        p = ((xwssq_t)BITS_PER_XWBMP_T - (xwssq_t)1) - p;
        if (p >= (xwssq_t)0) {
                p += ((xwssq_t)i << (xwssq_t)XWBMP_T_SHIFT); /* p += i * BITS_PER_XWBMP_T; */
        }
        return p;
}

__xwbsp_code
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        tmp = 0;
        i = BITS_TO_XWBMP_T(num);
        msk = ~(XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((~(xwbmp_t)0) == msk) {
                msk = (xwbmp_t)0;
        }
        while (i > 0) {
                i--;
                tmp = ~(bmp[i] | msk);
                if (tmp) {
                        break;
                }
                msk = (xwbmp_t)0;
        }
        __asm__ volatile(
        "       clz     %[__p], %[__tmp]\n"
        : [__p] "=&r" (p)
        : [__tmp] "r" (tmp)
        :
        );
        p = ((xwssq_t)BITS_PER_XWBMP_T - (xwssq_t)1) - p;
        if (p >= 0) {
                p += ((xwssq_t)i << (xwssq_t)XWBMP_T_SHIFT);
        }
        return p;
}
