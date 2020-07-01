/**
 * @file
 * @brief 汇编库：位操作
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <e200x_core.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_FFS8)) && (1 == ARCHCFG_LIB_XWBOP_FFS8)
__xwbsp_code
xwssq_t arch_xwbop_ffs8(xwu8_t x)
{
        return arch_xwbop_ffs32((xwu32_t)(x & 0xFFU));
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_FFS8)) && (1 == ARCHCFG_LIB_XWBOP_FFS8) */

#if (defined(ARCHCFG_LIB_XWBOP_FLS8)) && (1 == ARCHCFG_LIB_XWBOP_FLS8)
__xwbsp_code
xwssq_t arch_xwbop_fls8(xwu8_t x)
{
        return arch_xwbop_fls32((xwu32_t)(x & 0xFFU));
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_FLS8)) && (1 == ARCHCFG_LIB_XWBOP_FLS8) */

#if (defined(ARCHCFG_LIB_XWBOP_RE16)) && (1 == ARCHCFG_LIB_XWBOP_RE16)
__xwbsp_code
xwu16_t arch_xwbop_re16(xwu16_t a)
{
        xwu16_t r;

        r = lhbrx(&a);
        return r;
}
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS16)) && (1 == ARCHCFG_LIB_XWBOP_FFS16)
__xwbsp_code
xwssq_t arch_xwbop_ffs16(xwu16_t x)
{
        return arch_xwbop_ffs32((xwu32_t)(x & 0xFFFFU));
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_FFS16)) && (1 == ARCHCFG_LIB_XWBOP_FFS16) */

#if (defined(ARCHCFG_LIB_XWBOP_FLS16)) && (1 == ARCHCFG_LIB_XWBOP_FLS16)
__xwbsp_code
xwssq_t arch_xwbop_fls16(xwu16_t x)
{
        return arch_xwbop_fls32((xwu32_t)(x & 0xFFFFU));
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_FLS16)) && (1 == ARCHCFG_LIB_XWBOP_FLS16) */

#if (defined(ARCHCFG_LIB_XWBOP_RE32)) && (1 == ARCHCFG_LIB_XWBOP_RE32)
__xwbsp_code
xwu32_t arch_xwbop_re32(xwu32_t a)
{
        xwu32_t r;

        r = lwbrx(&a);
        return r;
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_RE32)) && (1 == ARCHCFG_LIB_XWBOP_RE32) */

#if (defined(ARCHCFG_LIB_XWBOP_RE64)) && (1 == ARCHCFG_LIB_XWBOP_RE64)
__xwbsp_code
xwu64_t arch_xwbop_re64(xwu64_t x)
{
        xwu32_t * addrl;
        xwu32_t * addrh;
        union {
                xwu64_t q;
                xwu32_t w[2];
        } r;

        addrl = (xwu32_t *)&x;
        addrh = addrl + 1;
        r.w[0] = lwbrx(addrh);
        r.w[1] = lwbrx(addrl);
        return r.q;
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBOP_RE64)) && (1 == ARCHCFG_LIB_XWBOP_RE64) */

#if (defined(ARCHCFG_LIB_XWBMPOP_FFS) && (1 == ARCHCFG_LIB_XWBMPOP_FFS))
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t len)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk, tmp;
        xwssq_t p;

        m = BITS_TO_BMPS(len);
        p = -1;
        for (i = 0; i < m; i++) {
                if (i == (m - 1)) {
                        msk = (BIT(len % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((xwbmp_t)0 == msk) {
                                msk = (~(xwbmp_t)0);
                        }/* else {} */
                } else {
                        msk = (~(xwbmp_t)0);
                }
                tmp = bmp[i] & msk;
                if (tmp) {
                        tmp = tmp & (~tmp + 1);
                        __asm__ volatile(
                        "       cntlzw          %[__p], %[__tmp]\n"
                        "       e_subfic        %[__p], %[__p], 31\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        : "memory"
                        );
                        p += (xwssq_t)(i << XWBMP_T_SHIFT);
                        break;
                }
        }
        return p;
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBMPOP_FFS) && (1 == ARCHCFG_LIB_XWBMPOP_FFS)) */

#if (defined(ARCHCFG_LIB_XWBMPOP_FFZ) && (1 == ARCHCFG_LIB_XWBMPOP_FFZ))
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t len)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        m = BITS_TO_BMPS(len);
        p = -1;
        for (i = 0; i < m; i++) {
                if (i == (m - 1)) {
                        msk = ~(BIT(len % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((~(xwbmp_t)0) == msk) {
                                msk = (xwbmp_t)0;
                        }/* else {} */
                } else {
                        msk = (xwbmp_t)0;
                }
                tmp = ~(bmp[i] | msk);
                if (tmp) {
                        tmp = tmp & (~tmp + 1);
                        __asm__ volatile(
                        "       cntlzw          %[__p], %[__tmp]\n"
                        "       e_subfic        %[__p], %[__p], 31\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        : "memory"
                        );
                        p += (xwssq_t)(i << XWBMP_T_SHIFT);
                        break;
                }
        }
        return p;
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBMPOP_FFZ) && (1 == ARCHCFG_LIB_XWBMPOP_FFZ)) */

#if (defined(ARCHCFG_LIB_XWBMPOP_FLS) && (1 == ARCHCFG_LIB_XWBMPOP_FLS))
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t len)
{
        xwsz_t i;
        xwbmp_t msk, tmp;
        xwssq_t p;

        i = BITS_TO_BMPS(len);
        msk = (BIT(len % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((xwbmp_t)0 == msk) {
                msk = (~(xwbmp_t)0);
        }/* else {} */
        p = -1;
        while (i > 0) {
                i--;
                tmp = bmp[i] & msk;
                if (tmp) {
                        __asm__ volatile(
                        "       cntlzw          %[__p], %[__tmp]\n"
                        "       e_subfic        %[__p], %[__p], 31\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        : "memory"
                        );
                        p += (xwssq_t)(i << XWBMP_T_SHIFT);
                        break;
                }/* else {} */
                msk = (~(xwbmp_t)0);
        }
        return p;
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBMPOP_FLS) && (1 == ARCHCFG_LIB_XWBMPOP_FLS)) */

#if (defined(ARCHCFG_LIB_XWBMPOP_FLZ) && (1 == ARCHCFG_LIB_XWBMPOP_FLZ))
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t len)
{
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        i = BITS_TO_BMPS(len);
        msk = ~(BIT(len % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((~(xwbmp_t)0) == msk) {
                msk = (xwbmp_t)0;
        }/* else {} */
        p = -1;
        while (i > 0) {
                i--;
                tmp = ~(bmp[i] | msk);
                if (tmp) {
                        __asm__ volatile(
                        "       cntlzw          %[__p], %[__tmp]\n"
                        "       e_subfic        %[__p], %[__p], 31\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        : "memory"
                        );
                        p += (xwssq_t)(i << XWBMP_T_SHIFT);
                        break;
                }/* else {} */
                msk = (xwbmp_t)0;
        }
        return p;
}
#endif
/* #if (defined(ARCHCFG_LIB_XWBMPOP_FLZ) && (1 == ARCHCFG_LIB_XWBMPOP_FLZ)) */
