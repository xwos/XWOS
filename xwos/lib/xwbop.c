/**
 * @file
 * @brief XWOS通用库：位操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

#if (!defined(ARCHCFG_LIB_XWBOP_RBIT8)) || (1 != ARCHCFG_LIB_XWBOP_RBIT8)
#  define XWBOP_RBIT8_R2(n)  (n), ((n) + 2U * 64U), ((n) + 1U * 64U), ((n) + 3U * 64U)
#  define XWBOP_RBIT8_R4(n)  XWBOP_RBIT8_R2(n), \
                             XWBOP_RBIT8_R2((n) + 2U * 16U), \
                             XWBOP_RBIT8_R2((n) + 1U * 16U), \
                             XWBOP_RBIT8_R2((n) + 3U * 16U)
#  define XWBOP_RBIT8_R6(n)  XWBOP_RBIT8_R4(n), \
                             XWBOP_RBIT8_R4((n) + 2U * 4U), \
                             XWBOP_RBIT8_R4((n) + 1U * 4U), \
                             XWBOP_RBIT8_R4((n) + 3U * 4U)
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_RBIT8)) || (1 != ARCHCFG_LIB_XWBOP_RBIT8)
/**
 * @brief 位镜面翻转表
 */
__xwlib_rodata const xwu8_t xwbop_rbit8_table[256] = {
        // cppcheck-suppress [misra-c2012-2.2, misra-c2012-12.1]
        XWBOP_RBIT8_R6(0U), XWBOP_RBIT8_R6(2U), XWBOP_RBIT8_R6(1U), XWBOP_RBIT8_R6(3U),
};
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_FFS8)) || (1 != ARCHCFG_LIB_XWBOP_FFS8)
__xwlib_code
xwssq_t xwbop_ffs8(xwu8_t x)
{
        xwssq_t r;
#  if defined(__GNUC__)
        if ((xwu8_t)0 == x) {
                r = (xwssq_t)-1;
        } else {
                r = (xwssq_t)__builtin_ffs((int)x) - (xwssq_t)1;
        }
#  else
        r = (xwssq_t)0;
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                if (0 == (x & (xwu8_t)0x0F)) {
                        x >>= (xwu8_t)4;
                        r += (xwssq_t)4;
                }
                if (0 == (x & (xwu8_t)0x03)) {
                        x >>= (xwu8_t)2;
                        r += (xwssq_t)2;
                }
                if (0 == (x & (xwu8_t)0x01)) {
                        r += (xwssq_t)1;
                }
        }
#  endif
        return r;
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FLS8)) || (1 != ARCHCFG_LIB_XWBOP_FLS8))
__xwlib_code
xwssq_t xwbop_fls8(xwu8_t x)
{
        xwssq_t r;
#  if defined(__GNUC__)
        if ((xwu8_t)0 == x) {
                r = (xwssq_t)-1;
        } else {
                r = (xwssq_t)7 - (xwssq_t)__builtin_clz((unsigned int)x);
        }
#  else
        r = (xwssq_t)7;
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                if (0 == (x & (xwu8_t)0xF0)) {
                        x <<= (xwu8_t)4;
                        r -= (xwssq_t)4;
                }
                if (0 == (x & (xwu8_t)0xC0)) {
                        x <<= (xwu8_t)2;
                        r -= (xwssq_t)2;
                }
                if (0 == (x & (xwu8_t)0x80)) {
                        r -= (xwssq_t)1;
                }
        }
#  endif
        return r;
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FFZ8)) || (1 != ARCHCFG_LIB_XWBOP_FFZ8))
__xwlib_code
xwssq_t xwbop_ffz8(xwu8_t x)
{
        return xwbop_ffs8((xwu8_t)(~x));
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_FLZ8)) || (1 != ARCHCFG_LIB_XWBOP_FLZ8)
__xwlib_code
xwssq_t xwbop_flz8(xwu8_t x)
{
        return xwbop_fls8((xwu8_t)(~x));
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_RBIT8)) || (1 != ARCHCFG_LIB_XWBOP_RBIT8)
__xwlib_code
xwu8_t xwbop_rbit8(xwu8_t x)
{
        return xwbop_rbit8_table[x];
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_WEIGHT8)) || (1 != ARCHCFG_LIB_XWBOP_WEIGHT8)
__xwlib_code
xwsz_t xwbop_weight8(xwu8_t x)
{
        xwu8_t res = x - ((x >> (xwu8_t)1) & (xwu8_t)0x55);
        res = (res & (xwu8_t)0x33) + ((res >> (xwu8_t)2) & (xwu8_t)0x33);
        return (res + (res >> (xwu8_t)4)) & (xwu8_t)0x0F;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_FFS16)) || (1 != ARCHCFG_LIB_XWBOP_FFS16)
__xwlib_code
xwssq_t xwbop_ffs16(xwu16_t x)
{
        xwssq_t r;
#  if defined(__GNUC__)
        if ((xwu16_t)0 == x) {
                r = (xwssq_t)-1;
        } else {
                r = (xwssq_t)__builtin_ffs((int)x) - (xwssq_t)1;
        }
#  else
        r = (xwssq_t)0;
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                if (0 == (x & (xwu16_t)0x00FF)) {
                        x >>= (xwu16_t)8;
                        r += (xwssq_t)8;
                }
                if (0 == (x & (xwu16_t)0x000F)) {
                        x >>= (xwu16_t)4;
                        r += (xwssq_t)4;
                }
                if (0 == (x & (xwu16_t)0x0003)) {
                        x >>= (xwu16_t)2;
                        r += (xwssq_t)2;
                }
                if (0 == (x & (xwu16_t)0x0001)) {
                        r += (xwssq_t)1;
                }
        }
#  endif
        return r;
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FLS16)) || (1 != ARCHCFG_LIB_XWBOP_FLS16))
__xwlib_code
xwssq_t xwbop_fls16(xwu16_t x)
{
        xwssq_t r;
#  if defined(__GNUC__)
        if ((xwu16_t)0 == x) {
                r = (xwssq_t)-1;
        } else {
                r = (xwssq_t)15 - (xwssq_t)__builtin_clz((unsigned int)x);
        }
#  else
        r = (xwssq_t)15;
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                if (0 == (x & (xwu16_t)0xFF00)) {
                        x <<= (xwu16_t)8;
                        r -= (xwssq_t)8;
                }
                if (0 == (x & (xwu16_t)0xF000)) {
                        x <<= (xwu16_t)4;
                        r -= (xwssq_t)4;
                }
                if (0 == (x & (xwu16_t)0xC000)) {
                        x <<= (xwu16_t)2;
                        r -= (xwssq_t)2;
                }
                if (0 == (x & (xwu16_t)0x8000)) {
                        r -= (xwssq_t)1;
                }
        }
#  endif
        return r;
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FFZ16)) || (1 != ARCHCFG_LIB_XWBOP_FFZ16))
__xwlib_code
xwssq_t xwbop_ffz16(xwu16_t x)
{
        return xwbop_ffs16((xwu16_t)(~x));
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FLZ16)) || (1 != ARCHCFG_LIB_XWBOP_FLZ16))
__xwlib_code
xwssq_t xwbop_flz16(xwu16_t x)
{
        return xwbop_fls16((xwu16_t)(~x));
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_RBIT16)) || (1 != ARCHCFG_LIB_XWBOP_RBIT16)
__xwlib_code
xwu16_t xwbop_rbit16(xwu16_t x)
{
        union {
                xwu16_t d16;
                struct {
                        xwu8_t h;
                        xwu8_t l;
                } d8;
        } r, t; // cppcheck-suppress [misra-c2012-12.3]

        t.d16 = x;
        r.d8.h = xwbop_rbit8(t.d8.l);
        r.d8.l = xwbop_rbit8(t.d8.h);
        return r.d16;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_RE16)) || (1 != ARCHCFG_LIB_XWBOP_RE16)
__xwlib_code
xwu16_t xwbop_re16(xwu16_t x)
{
        union {
                xwu16_t d16;
                struct {
                        xwu8_t byte0;
                        xwu8_t byte1;
                } d8;
        } r;

        r.d8.byte0 = (xwu8_t)(x >> (xwu16_t)8);
        r.d8.byte1 = (xwu8_t)(x >> (xwu16_t)0);
        return r.d16;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_WEIGHT16)) || (1 != ARCHCFG_LIB_XWBOP_WEIGHT16)
__xwlib_code
xwsz_t xwbop_weight16(xwu16_t x)
{
        xwu16_t res = x - ((x >> (xwu16_t)1) & (xwu16_t)0x5555);
        res = (res & (xwu16_t)0x3333) + ((res >> (xwu16_t)2) & (xwu16_t)0x3333);
        res = (res + (res >> (xwu16_t)4)) & (xwu16_t)0x0F0F;
        return (res + (res >> (xwu16_t)8)) & (xwu16_t)0x00FF;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_FFS32)) || (1 != ARCHCFG_LIB_XWBOP_FFS32)
__xwlib_code
xwssq_t xwbop_ffs32(xwu32_t x)
{
        xwssq_t r;
#  if defined(__GNUC__)
        if ((xwu32_t)0 == x) {
                r = (xwssq_t)-1;
        } else {
                r = (xwssq_t)__builtin_ffs((int)x) - (xwssq_t)1;
        }
#  else
        r = (xwssq_t)0;
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                if (0 == (x & (xwu32_t)0x0000FFFF)) {
                        x >>= (xwu32_t)16;
                        r += (xwssq_t)16;
                }
                if (0 == (x & (xwu32_t)0x000000FF)) {
                        x >>= (xwu32_t)8;
                        r += (xwssq_t)8;
                }
                if (0 == (x & (xwu32_t)0x0000000F)) {
                        x >>= (xwu32_t)4;
                        r += (xwssq_t)4;
                }
                if (0 == (x & (xwu32_t)0x00000003)) {
                        x >>= (xwu32_t)2;
                        r += (xwssq_t)2;
                }
                if (0 == (x & (xwu32_t)0x00000001)) {
                        r += (xwssq_t)1;
                }
        }
#  endif
        return r;
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FLS32)) || (1 != ARCHCFG_LIB_XWBOP_FLS32))
__xwlib_code
xwssq_t xwbop_fls32(xwu32_t x)
{
        xwssq_t r;
#  if defined(__GNUC__)
        if ((xwu32_t)0 == x) {
                r = (xwssq_t)-1;
        } else {
                r = (xwssq_t)31 - (xwssq_t)__builtin_clz((unsigned int)x);
        }
#  else
        r = (xwssq_t)31;
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                if (0 == (x & (xwu32_t)0xFFFF0000)) {
                        x <<= (xwu32_t)16;
                        r -= (xwssq_t)16;
                }
                if (0 == (x & (xwu32_t)0xFF000000)) {
                        x <<= (xwu32_t)8;
                        r -= (xwssq_t)8;
                }
                if (0 == (x & (xwu32_t)0xF0000000)) {
                        x <<= (xwu32_t)4;
                        r -= (xwssq_t)4;
                }
                if (0 == (x & (xwu32_t)0xC0000000)) {
                        x <<= (xwu32_t)2;
                        r -= (xwssq_t)2;
                }
                if (0 == (x & (xwu32_t)0x80000000)) {
                        r -= (xwssq_t)1;
                }
        }
#  endif
        return r;
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FFZ32)) || (1 != ARCHCFG_LIB_XWBOP_FFZ32))
__xwlib_code
xwssq_t xwbop_ffz32(xwu32_t x)
{
        return xwbop_ffs32(~x);
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FLZ32)) || (1 != ARCHCFG_LIB_XWBOP_FLZ32))
__xwlib_code
xwssq_t xwbop_flz32(xwu32_t x)
{
        return xwbop_fls32(~x);
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_RBIT32)) || (1 != ARCHCFG_LIB_XWBOP_RBIT32)
__xwlib_code
xwu32_t xwbop_rbit32(xwu32_t x)
{
        union {
                xwu32_t d32;
                struct {
                        xwu8_t byte0;
                        xwu8_t byte1;
                        xwu8_t byte2;
                        xwu8_t byte3;
                } d8;
        } r, t; // cppcheck-suppress [misra-c2012-12.3]

        t.d32 = x;
        r.d8.byte0 = xwbop_rbit8(t.d8.byte3);
        r.d8.byte1 = xwbop_rbit8(t.d8.byte2);
        r.d8.byte2 = xwbop_rbit8(t.d8.byte1);
        r.d8.byte3 = xwbop_rbit8(t.d8.byte0);
        return r.d32;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_RE32)) || (1 != ARCHCFG_LIB_XWBOP_RE32)
__xwlib_code
xwu32_t xwbop_re32(xwu32_t x)
{
        union {
                xwu32_t d32;
                struct {
                        xwu8_t byte0;
                        xwu8_t byte1;
                        xwu8_t byte2;
                        xwu8_t byte3;
                } d8;
        } r;

        r.d8.byte0 = (xwu8_t)(x >> (xwu32_t)24);
        r.d8.byte1 = (xwu8_t)(x >> (xwu32_t)16);
        r.d8.byte2 = (xwu8_t)(x >> (xwu32_t)8);
        r.d8.byte3 = (xwu8_t)(x >> (xwu32_t)0);
        return r.d32;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_WEIGHT32)) || (1 != ARCHCFG_LIB_XWBOP_WEIGHT32)
__xwlib_code
xwsz_t xwbop_weight32(xwu32_t x)
{
        xwu32_t res = x - ((x >> (xwu32_t)1) & (xwu32_t)0x55555555);
        res = (res & (xwu32_t)0x33333333) + ((res >> (xwu32_t)2) & (xwu32_t)0x33333333);
        res = (res + (res >> (xwu32_t)4)) & (xwu32_t)0x0F0F0F0F;
        res = res + (res >> (xwu32_t)8);
        return (res + (res >> (xwu32_t)16)) & (xwu32_t)0x000000FF;
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FFS64)) || (1 != ARCHCFG_LIB_XWBOP_FFS64))
__xwlib_code
xwssq_t xwbop_ffs64(xwu64_t x)
{
        xwssq_t r;
#  if defined(__GNUC__)
        if ((xwu64_t)0 == x) {
                r = (xwssq_t)-1;
        } else {
                r = (xwssq_t)__builtin_ffsll((long long)x) - (xwssq_t)1;
        }
#  else
        r = (xwssq_t)0;
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                if (0 == (x & (xwu64_t)0xFFFFFFFF)) {
                        x >>= (xwu64_t)32;
                        r += (xwssq_t)32;
                }
                if (0 == (x & (xwu64_t)0xFFFF)) {
                        x >>= (xwu64_t)16;
                        r += (xwssq_t)16;
                }
                if (0 == (x & (xwu64_t)0xFF)) {
                        x >>= (xwu64_t)8;
                        r += (xwssq_t)8;
                }
                if (0 == (x & (xwu64_t)0xF)) {
                        x >>= (xwu64_t)4;
                        r += (xwssq_t)4;
                }
                if (0 == (x & (xwu64_t)0x3)) {
                        x >>= (xwu64_t)2;
                        r += (xwssq_t)2;
                }
                if (0 == (x & (xwu64_t)0x1)) {
                        r += (xwssq_t)1;
                }
        }
#  endif
        return r;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_FLS64)) || (1 != ARCHCFG_LIB_XWBOP_FLS64)
__xwlib_code
xwssq_t xwbop_fls64(xwu64_t x)
{
        xwssq_t r;
#  if defined(__GNUC__)
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                r = (xwssq_t)63 - (xwssq_t)__builtin_clzll((unsigned long long)x);
        }
#  else
        r = (xwssq_t)63;
        if (0 == x) {
                r = (xwssq_t)-1;
        } else {
                if (0 == (x & (xwu64_t)0xFFFFFFFF00000000)) {
                        r -= (xwssq_t)32;
                } else {
                        x >>= (xwu64_t)32;
                }
                if (0 == (x & (xwu64_t)0xFFFF0000)) {
                        x <<= (xwu64_t)16;
                        r -= (xwssq_t)16;
                }
                if (0 == (x & (xwu64_t)0xFF000000)) {
                        x <<= (xwu64_t)8;
                        r -= (xwssq_t)8;
                }
                if (0 == (x & (xwu64_t)0xF0000000)) {
                        x <<= (xwu64_t)4;
                        r -= (xwssq_t)4;
                }
                if (0 == (x & (xwu64_t)0xC0000000)) {
                        x <<= (xwu64_t)2;
                        r -= (xwssq_t)2;
                }
                if (0 == (x & (xwu64_t)0x80000000)) {
                        r -= (xwssq_t)1;
                }
        }
#  endif
        return r;
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FFZ64)) || (1 != ARCHCFG_LIB_XWBOP_FFZ64))
__xwlib_code
xwssq_t xwbop_ffz64(xwu64_t x)
{
        return xwbop_ffs64(~x);
}
#endif

#if ((!defined(ARCHCFG_LIB_XWBOP_FLZ64)) || (1 != ARCHCFG_LIB_XWBOP_FLZ64))
__xwlib_code
xwssq_t xwbop_flz64(xwu64_t x)
{
        return xwbop_ffs64(~x);
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_RBIT64)) || (1 != ARCHCFG_LIB_XWBOP_RBIT64)
__xwlib_code
xwu64_t xwbop_rbit64(xwu64_t x)
{
        union {
                xwu64_t d64;
                struct {
                        xwu8_t byte0;
                        xwu8_t byte1;
                        xwu8_t byte2;
                        xwu8_t byte3;
                        xwu8_t byte4;
                        xwu8_t byte5;
                        xwu8_t byte6;
                        xwu8_t byte7;
                } d8;
        } r, t; // cppcheck-suppress [misra-c2012-12.3]

        t.d64 = x;
        r.d8.byte0 = xwbop_rbit8(t.d8.byte7);
        r.d8.byte1 = xwbop_rbit8(t.d8.byte6);
        r.d8.byte2 = xwbop_rbit8(t.d8.byte5);
        r.d8.byte3 = xwbop_rbit8(t.d8.byte4);
        r.d8.byte4 = xwbop_rbit8(t.d8.byte3);
        r.d8.byte5 = xwbop_rbit8(t.d8.byte2);
        r.d8.byte6 = xwbop_rbit8(t.d8.byte1);
        r.d8.byte7 = xwbop_rbit8(t.d8.byte0);
        return r.d64;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_RE64)) || (1 != ARCHCFG_LIB_XWBOP_RE64)
__xwlib_code
xwu64_t xwbop_re64(xwu64_t x)
{
        union {
                xwu64_t d64;
                struct {
                        xwu8_t byte0;
                        xwu8_t byte1;
                        xwu8_t byte2;
                        xwu8_t byte3;
                        xwu8_t byte4;
                        xwu8_t byte5;
                        xwu8_t byte6;
                        xwu8_t byte7;
                } d8;
        } r;

        r.d8.byte0 = (xwu8_t)(x >> (xwu64_t)56);
        r.d8.byte1 = (xwu8_t)(x >> (xwu64_t)48);
        r.d8.byte2 = (xwu8_t)(x >> (xwu64_t)40);
        r.d8.byte3 = (xwu8_t)(x >> (xwu64_t)32);
        r.d8.byte4 = (xwu8_t)(x >> (xwu64_t)24);
        r.d8.byte5 = (xwu8_t)(x >> (xwu64_t)16);
        r.d8.byte6 = (xwu8_t)(x >> (xwu64_t)8);
        r.d8.byte7 = (xwu8_t)(x >> (xwu64_t)0);
        return r.d64;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBOP_WEIGHT64)) || (1 != ARCHCFG_LIB_XWBOP_WEIGHT64)
__xwlib_code
xwsz_t xwbop_weight64(xwu64_t x)
{
        xwu64_t res = x - ((x >> (xwu64_t)1) & (xwu64_t)0x5555555555555555);
        res = (res & (xwu64_t)0x3333333333333333) +
              ((res >> (xwu64_t)2) & (xwu64_t)0x3333333333333333);
        res = (res + (res >> (xwu64_t)4)) & (xwu64_t)0x0F0F0F0F0F0F0F0F;
        res = res + (res >> (xwu64_t)8);
        res = res + (res >> (xwu64_t)16);
        return (res + (res >> (xwu64_t)32)) & (xwu64_t)0x00000000000000FF;
}
#endif

__xwlib_code
void xwbmpop_assign(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num)
{
        xwsz_t n = BITS_TO_XWBMP_T(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                bmp[i] = opd[i];
        }
}

__xwlib_code
xwssq_t xwbmpop_cmp(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num)
{
        xwsz_t n = BITS_TO_XWBMP_T(num);
        xwsz_t i;
        xwsz_t j;
        xwssq_t ret;

        ret = 0;
        for (j = n; j > (xwsz_t)0; j--) { // cppcheck-suppress [misra-c2012-15.4]
                i = j - (xwsz_t)1;
                if (bmp[i] < opd[i]) {
                        ret = -1;
                        break;
                } else if (bmp[i] > opd[i]) {
                        ret = 1;
                        break;
                } else {}
        }
        return ret;
}

__xwlib_code
void xwbmpop_s1all(xwbmp_t * bmp, xwsq_t num)
{
        xwsz_t n = BITS_TO_XWBMP_T(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                xwbop_s1m(xwbmp_t, &bmp[i], (~(xwbmp_t)0));
        }
}

__xwlib_code
void xwbmpop_c0all(xwbmp_t * bmp, xwsq_t num)
{
        xwsz_t n = BITS_TO_XWBMP_T(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                xwbop_c0m(xwbmp_t, &bmp[i], (~(xwbmp_t)0));
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_S1I)) || (1 != ARCHCFG_LIB_XWBMPOP_S1I)
__xwlib_code
void xwbmpop_s1i(xwbmp_t * bmp, xwsq_t n)
{
        bmp = bmp + XWBOP_BMP(n);
        xwbop_s1m(xwbmp_t, bmp, XWBOP_BMP_MASK(n));
}
#endif

__xwlib_code
void xwbmpop_s1m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_XWBMP_T(num);
        for (i = 0; i < size; i++) {
                xwbop_s1m(xwbmp_t, &bmp[i], msk[i]);
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_C0I)) || (1 != ARCHCFG_LIB_XWBMPOP_C0I)
__xwlib_code
void xwbmpop_c0i(xwbmp_t * bmp, xwsq_t n)
{
        bmp = bmp + XWBOP_BMP(n);
        xwbop_c0m(xwbmp_t, bmp, XWBOP_BMP_MASK(n));
}
#endif

__xwlib_code
void xwbmpop_c0m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_XWBMP_T(num);
        for (i = 0; i < size; i++) {
                xwbop_c0m(xwbmp_t, &bmp[i], msk[i]);
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_X1I)) || (1 != ARCHCFG_LIB_XWBMPOP_X1I)
__xwlib_code
void xwbmpop_x1i(xwbmp_t * bmp, xwsq_t n)
{
        bmp = bmp + XWBOP_BMP(n);
        xwbop_x1m(xwbmp_t, bmp, XWBOP_BMP_MASK(n));
}
#endif

__xwlib_code
void xwbmpop_x1m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_XWBMP_T(num);
        for (i = 0; i < size; i++) {
                xwbop_x1m(xwbmp_t, &bmp[i], msk[i]);
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_T1I)) || (1 != ARCHCFG_LIB_XWBMPOP_T1I)
__xwlib_code
bool xwbmpop_t1i(xwbmp_t * bmp, xwsq_t n)
{
        bmp = bmp + XWBOP_BMP(n);
        return (bool)(*bmp & XWBOP_BMP_MASK(n));
}
#endif

__xwlib_code
bool xwbmpop_t1ma(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_XWBMP_T(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!((bmp[i] & msk[i]) == msk[i]);
                if (!ret) {
                        break;
                }
        }
        return ret;
}

__xwlib_code
bool xwbmpop_t1ma_then_c0m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_XWBMP_T(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!((bmp[i] & msk[i]) == msk[i]);
                if (!ret) {
                        break;
                }
        }
        if (ret) {
                for (i = 0; i < size; i++) {
                        xwbop_c0m(xwbmp_t, &bmp[i], msk[i]);
                }
        }
        return ret;
}

__xwlib_code
bool xwbmpop_t1mo(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_XWBMP_T(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!(bmp[i] & msk[i]);
                if (ret) {
                        break;
                }
        }
        return ret;
}

__xwlib_code
bool xwbmpop_t1mo_then_c0m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_XWBMP_T(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!(bmp[i] & msk[i]);
                if (ret) {
                        break;
                }
        }
        if (ret) {
                for (i = 0; i < size; i++) {
                        xwbop_c0m(xwbmp_t, &bmp[i], msk[i]);
                }
        }
        return ret;
}

__xwlib_code
bool xwbmpop_t0ma(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_XWBMP_T(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !(bmp[i] & msk[i]);
                if (!ret) {
                        break;
                }
        }
        return ret;
}

__xwlib_code
bool xwbmpop_t0ma_then_s1m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_XWBMP_T(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !(bmp[i] & msk[i]);
                if (!ret) {
                        break;
                }
        }
        if (ret) {
                for (i = 0; i < size; i++) {
                        xwbop_s1m(xwbmp_t, &bmp[i], msk[i]);
                }
        }
        return ret;
}

__xwlib_code
bool xwbmpop_t0mo(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_XWBMP_T(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!((bmp[i] & msk[i]) != msk[i]);
                if (ret) {
                        break;
                }
        }
        return ret;
}

__xwlib_code
bool xwbmpop_t0mo_then_s1m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_XWBMP_T(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!((bmp[i] & msk[i]) != msk[i]);
                if (ret) {
                        break;
                }
        }
        if (ret) {
                for (i = 0; i < size; i++) {
                        xwbop_s1m(xwbmp_t, &bmp[i], msk[i]);
                }
        }
        return ret;
}

__xwlib_code
void xwbmpop_not(xwbmp_t * bmp, xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_XWBMP_T(num);
        for (i = 0; i < size; i++) {
                bmp[i] = ~bmp[i];
        }
}

__xwlib_code
void xwbmpop_and(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_XWBMP_T(num);
        for (i = 0; i < size; i++) {
                bmp[i] &= opd[i];
        }
}

__xwlib_code
void xwbmpop_or(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_XWBMP_T(num);
        for (i = 0; i < size; i++) {
                bmp[i] |= opd[i];
        }
}

__xwlib_code
void xwbmpop_xor(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_XWBMP_T(num);
        for (i = 0; i < size; i++) {
                bmp[i] ^= opd[i];
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_FFS)) || (1 != ARCHCFG_LIB_XWBMPOP_FFS)
__xwlib_code
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m = BITS_TO_XWBMP_T(num);
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p = -1;

        for (i = 0; i < m; i++) {
                if (i == (m - 1)) {
                        msk = (XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((xwbmp_t)0 == msk) {
                                msk = (xwbmp_t)(~(xwbmp_t)0);
                        }
                } else {
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                }
                p = xwbop_ffs(xwbmp_t, bmp[i] & msk);
                if (p >= 0) {
                        break;
                }
        }
        if (p >= 0) {
                p += ((xwssq_t)i << XWBMP_T_SHIFT); /* p = p + i * BITS_PER_XWBMP_T; */
        }
        return p;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBMPOP_FFZ)) || (1 != ARCHCFG_LIB_XWBMPOP_FFZ)
__xwlib_code
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m = BITS_TO_XWBMP_T(num);
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p = -1;

        for (i = 0; i < m; i++) {
                if (i == (m - 1)) {
                        msk = ~(XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((xwbmp_t)(~(xwbmp_t)0) == msk) {
                                msk = (xwbmp_t)0;
                        }
                } else {
                        msk = (xwbmp_t)0;
                }
                p = xwbop_ffz(xwbmp_t, bmp[i] | msk);
                if (p >= 0) {
                        break;
                }
        }
        if (p >= 0) {
                p += ((xwssq_t)i << XWBMP_T_SHIFT); /* p = p + i * BITS_PER_XWBMP_T; */
        }
        return p;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBMPOP_FLS)) || (1 != ARCHCFG_LIB_XWBMPOP_FLS)
__xwlib_code
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t i = BITS_TO_XWBMP_T(num);
        xwbmp_t msk;
        xwssq_t p = -1;

        msk = (XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((xwbmp_t)0 == msk) {
                msk = (xwbmp_t)(~(xwbmp_t)0);
        }
        while (i > 0) {
                i--;
                p = xwbop_fls(xwbmp_t, bmp[i] & msk);
                if (p >= 0) {
                        break;
                }
                msk = (xwbmp_t)(~(xwbmp_t)0);
        }
        p += ((xwssq_t)i << XWBMP_T_SHIFT); /* p = p + i * BITS_PER_XWBMP_T; */
        return p;
}
#endif

#if (!defined(ARCHCFG_LIB_XWBMPOP_FLZ)) || (1 != ARCHCFG_LIB_XWBMPOP_FLZ)
__xwlib_code
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t i = BITS_TO_XWBMP_T(num);
        xwbmp_t msk;
        xwssq_t p = -1;

        msk = ~(XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((xwbmp_t)(~(xwbmp_t)0) == msk) {
                msk = (xwbmp_t)0;
        }
        while (i > 0) {
                i--;
                p = xwbop_flz(xwbmp_t, bmp[i] | msk);
                if (p >= 0) {
                        break;
                }
                msk = (xwbmp_t)0;
        }
        p += ((xwssq_t)i << XWBMP_T_SHIFT); /* p = p + i * BITS_PER_XWBMP_T; */
        return p;
}
#endif

__xwlib_code
xwsz_t xwbmpop_weight(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t i;
        xwsz_t n;
        xwsz_t res;

        res = 0;
        n = BITS_TO_XWBMP_T(num);
        for (i = 0; i < n; i++) {
                res += xwbop_weight(xwbmp_t, bmp[i]);
        }
        return res;
}
