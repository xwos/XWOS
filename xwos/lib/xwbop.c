/**
 * @file
 * @brief XWOS通用库：位操作
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (defined(XWLIBCFG_XWBOP_RBIT8)) && (1 == XWLIBCFG_XWBOP_RBIT8)
#if (!defined(ARCHCFG_LIB_XWBOP_RBIT8)) || (1 != ARCHCFG_LIB_XWBOP_RBIT8)
#define XWBOP_RBIT8_R2(n)  n,           n + 2*64,       n + 1*64,       n + 3*64
#define XWBOP_RBIT8_R4(n)  XWBOP_RBIT8_R2(n), XWBOP_RBIT8_R2(n + 2 * 16), \
                           XWBOP_RBIT8_R2(n + 1 * 16), XWBOP_RBIT8_R2(n + 3 * 16)
#define XWBOP_RBIT8_R6(n)  XWBOP_RBIT8_R4(n), XWBOP_RBIT8_R4(n + 2 * 4), \
                           XWBOP_RBIT8_R4(n + 1 * 4), XWBOP_RBIT8_R4(n + 3 * 4)
#endif /* !ARCHCFG_LIB_XWBOP_RBIT8 */
#endif /* XWLIBCFG_XWBOP_RBIT8 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (defined(XWLIBCFG_XWBOP_RBIT8)) && (1 == XWLIBCFG_XWBOP_RBIT8)
#if (!defined(ARCHCFG_LIB_XWBOP_RBIT8)) || (1 != ARCHCFG_LIB_XWBOP_RBIT8)
/**
 * @brief 位镜面翻转表
 */
__xwlib_rodata const xwu8_t xwbop_rbit8_table[256] = {
        XWBOP_RBIT8_R6(0), XWBOP_RBIT8_R6(2), XWBOP_RBIT8_R6(1), XWBOP_RBIT8_R6(3)
};
#endif /* !ARCHCFG_LIB_XWBOP_RBIT8 */
#endif /* XWLIBCFG_XWBOP_RBIT8 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (defined(XWLIBCFG_XWBOP_FFS8)) && (1 == XWLIBCFG_XWBOP_FFS8)
#if (!defined(ARCHCFG_LIB_XWBOP_FFS8)) || (1 != ARCHCFG_LIB_XWBOP_FFS8)
__xwlib_code
xwssq_t xwbop_ffs8(xwu8_t x)
{
#if defined(__GNUC__)
        if ((xwu8_t)0 == x) {
                return (xwssq_t)-1;
        }
        return (xwssq_t)__builtin_ffs((int)x) - (xwssq_t)1;
#else /* __GNUC__ */
        xwssq_t r = (xwssq_t)0;

        if (!x) {
                return (xwssq_t)-1;
        }
        if (!(x & (xwu8_t)0x0F)) {
                x >>= (xwu8_t)4;
                r += (xwssq_t)4;
        }
        if (!(x & (xwu8_t)0x03)) {
                x >>= (xwu8_t)2;
                r += (xwssq_t)2;
        }
        if (!(x & (xwu8_t)0x01)) {
                r += (xwssq_t)1;
        }
        return r;
#endif /* !__GNUC__ */
}
#endif /* !ARCHCFG_LIB_XWBOP_FFS8 */
#endif /* XWLIBCFG_XWBOP_FFS8 */

#if (defined(XWLIBCFG_XWBOP_FLS8) && (1 == XWLIBCFG_XWBOP_FLS8))
#if ((!defined(ARCHCFG_LIB_XWBOP_FLS8)) || (1 != ARCHCFG_LIB_XWBOP_FLS8))
__xwlib_code
xwssq_t xwbop_fls8(xwu8_t x)
{
#if defined(__GNUC__)
        if ((xwu8_t)0 == x) {
                return (xwssq_t)-1;
        }
        return (xwssq_t)7 - (xwssq_t)__builtin_clz((unsigned int)x);
#else /* __GNUC__ */
        xwssq_t r = (xwssq_t)7;

        if (!x) {
                return (xwssq_t)-1;
        }
        if (!(x & (xwu8_t)0xF0)) {
                x <<= (xwu8_t)4;
                r -= (xwssq_t)4;
        }
        if (!(x & (xwu8_t)0xC0)) {
                x <<= (xwu8_t)2;
                r -= (xwssq_t)2;
        }
        if (!(x & (xwu8_t)0x80)) {
                r -= (xwssq_t)1;
        }
        return r;
#endif /* !__GNUC__ */
}
#endif /* !ARCHCFG_LIB_XWBOP_FLS8 */
#endif /* XWLIBCFG_XWBOP_FLS8 */

#if (defined(XWLIBCFG_XWBOP_FFS16)) && (1 == XWLIBCFG_XWBOP_FFS16)
#if (!defined(ARCHCFG_LIB_XWBOP_FFS16)) || (1 != ARCHCFG_LIB_XWBOP_FFS16)
__xwlib_code
xwssq_t xwbop_ffs16(xwu16_t x)
{
#if defined(__GNUC__)
        if ((xwu16_t)0 == x) {
                return (xwssq_t)-1;
        }
        return (xwssq_t)__builtin_ffs((int)x) - (xwssq_t)1;
#else /* __GNUC__ */
        xwssq_t r = (xwssq_t)0;

        if (!x) {
                return (xwssq_t)-1;
        }
        if (!(x & (xwu16_t)0x00FF)) {
                x >>= (xwu16_t)8;
                r += (xwssq_t)8;
        }
        if (!(x & (xwu16_t)0x000F)) {
                x >>= (xwu16_t)4;
                r += (xwssq_t)4;
        }
        if (!(x & (xwu16_t)0x0003)) {
                x >>= (xwu16_t)2;
                r += (xwssq_t)2;
        }
        if (!(x & (xwu16_t)0x0001)) {
                r += (xwssq_t)1;
        }
        return r;
#endif /* !__GNUC__ */
}
#endif /* !ARCHCFG_LIB_XWBOP_FFS16 */
#endif /* XWLIBCFG_XWBOP_FFS16 */

#if (defined(XWLIBCFG_XWBOP_FLS16) && (1 == XWLIBCFG_XWBOP_FLS16))
#if ((!defined(ARCHCFG_LIB_XWBOP_FLS16)) || (1 != ARCHCFG_LIB_XWBOP_FLS16))
__xwlib_code
xwssq_t xwbop_fls16(xwu16_t x)
{
#if defined(__GNUC__)
        if ((xwu16_t)0 == x) {
                return (xwssq_t)-1;
        }
        return (xwssq_t)15 - (xwssq_t)__builtin_clz((unsigned int)x);
#else /* __GNUC__ */
        xwssq_t r = (xwssq_t)15;

        if (!x) {
                return (xwssq_t)-1;
        }
        if (!(x & (xwu16_t)0xFF00)) {
                x <<= (xwu16_t)8;
                r -= (xwssq_t)8;
        }
        if (!(x & (xwu16_t)0xF000)) {
                x <<= (xwu16_t)4;
                r -= (xwssq_t)4;
        }
        if (!(x & (xwu16_t)0xC000)) {
                x <<= (xwu16_t)2;
                r -= (xwssq_t)2;
        }
        if (!(x & (xwu16_t)0x8000)) {
                r -= (xwssq_t)1;
        }
        return r;
#endif /* !__GNUC__ */
}
#endif /* !ARCHCFG_LIB_XWBOP_FLS16 */
#endif /* XWLIBCFG_XWBOP_FLS16 */

#if (defined(XWLIBCFG_XWBOP_FFS32)) && (1 == XWLIBCFG_XWBOP_FFS32)
#if (!defined(ARCHCFG_LIB_XWBOP_FFS32)) || (1 != ARCHCFG_LIB_XWBOP_FFS32)
__xwlib_code
xwssq_t xwbop_ffs32(xwu32_t x)
{
#if defined(__GNUC__)
        if ((xwu32_t)0 == x) {
                return (xwssq_t)-1;
        }
        return (xwssq_t)__builtin_ffs((int)x) - (xwssq_t)1;
#else /* __GNUC__ */
        xwssq_t r = (xwssq_t)0;

        if (!x) {
                return -1;
        }
        if (!(x & (xwu32_t)0x0000FFFF)) {
                x >>= (xwu32_t)16;
                r += (xwssq_t)16;
        }
        if (!(x & (xwu32_t)0x000000FF)) {
                x >>= (xwu32_t)8;
                r += (xwssq_t)8;
        }
        if (!(x & (xwu32_t)0x0000000F)) {
                x >>= (xwu32_t)4;
                r += (xwssq_t)4;
        }
        if (!(x & (xwu32_t)0x00000003)) {
                x >>= (xwu32_t)2;
                r += (xwssq_t)2;
        }
        if (!(x & (xwu32_t)0x00000001)) {
                r += (xwssq_t)1;
        }
        return r;
#endif /* !__GNUC__ */
}
#endif /* !ARCHCFG_LIB_XWBOP_FFS32 */
#endif /* XWLIBCFG_XWBOP_FFS32 */

#if (defined(XWLIBCFG_XWBOP_FLS32) && (1 == XWLIBCFG_XWBOP_FLS32))
#if ((!defined(ARCHCFG_LIB_XWBOP_FLS32)) || (1 != ARCHCFG_LIB_XWBOP_FLS32))
__xwlib_code
xwssq_t xwbop_fls32(xwu32_t x)
{
#if defined(__GNUC__)
        if ((xwu32_t)0 == x) {
                return (xwssq_t)-1;
        }
        return (xwssq_t)31 - (xwssq_t)__builtin_clz((unsigned int)x);
#else /* __GNUC__ */
        xwssq_t r = (xwssq_t)31;

        if (!x) {
                return (xwssq_t)-1;
        }
        if (!(x & (xwu32_t)0xFFFF0000)) {
                x <<= (xwu32_t)16;
                r -= (xwssq_t)16;
        }
        if (!(x & (xwu32_t)0xFF000000)) {
                x <<= (xwu32_t)8;
                r -= (xwssq_t)8;
        }
        if (!(x & (xwu32_t)0xF0000000)) {
                x <<= (xwu32_t)4;
                r -= (xwssq_t)4;
        }
        if (!(x & (xwu32_t)0xC0000000)) {
                x <<= (xwu32_t)2;
                r -= (xwssq_t)2;
        }
        if (!(x & (xwu32_t)0x80000000)) {
                r -= (xwssq_t)1;
        }
        return r;
#endif /* !__GNUC__ */
}
#endif /* !ARCHCFG_LIB_XWBOP_FLS32 */
#endif /* XWLIBCFG_XWBOP_FLS32 */

#if (defined(XWLIBCFG_XWBOP_FFS64) && (1 == XWLIBCFG_XWBOP_FFS64))
#if ((!defined(ARCHCFG_LIB_XWBOP_FFS64)) || (1 != ARCHCFG_LIB_XWBOP_FFS64))
__xwlib_code
xwssq_t xwbop_ffs64(xwu64_t x)
{
#if defined(__GNUC__)
        if ((xwu64_t)0 == x) {
                return (xwssq_t)-1;
        }
        return (xwssq_t)__builtin_ffsll((long long)x) - (xwssq_t)1;
#else /* __GNUC__ */
        xwssq_t r = (xwssq_t)0;

        if (!x) {
                return (xwssq_t)-1;
        }
        if (!(x & (xwu64_t)0xFFFFFFFF)) {
                x >>= (xwu64_t)32;
                r += (xwssq_t)32;
        }
        if (!(x & (xwu64_t)0xFFFF)) {
                x >>= (xwu64_t)16;
                r += (xwssq_t)16;
        }
        if (!(x & (xwu64_t)0xFF)) {
                x >>= (xwu64_t)8;
                r += (xwssq_t)8;
        }
        if (!(x & (xwu64_t)0xF)) {
                x >>= (xwu64_t)4;
                r += (xwssq_t)4;
        }
        if (!(x & (xwu64_t)0x3)) {
                x >>= (xwu64_t)2;
                r += (xwssq_t)2;
        }
        if (!(x & (xwu64_t)0x1)) {
                r += (xwssq_t)1;
        }
        return r;
#endif /* !__GNUC__ */
}
#endif /* !ARCHCFG_LIB_XWBOP_FFS64 */
#endif /* XWLIBCFG_XWBOP_FFS64 */

#if (defined(XWLIBCFG_XWBOP_FLS64)) && (1 == XWLIBCFG_XWBOP_FLS64)
#if (!defined(ARCHCFG_LIB_XWBOP_FLS64)) || (1 != ARCHCFG_LIB_XWBOP_FLS64)
__xwlib_code
xwssq_t xwbop_fls64(xwu64_t x)
{
#if defined(__GNUC__)
        if (0 == x) {
                return (xwssq_t)-1;
        }
        return (xwssq_t)63 - (xwssq_t)__builtin_clzll((unsigned long long)x);
#else /* __GNUC__ */
        xwssq_t r = (xwssq_t)63;

        if (!x) {
                return (xwssq_t)-1;
        }

        if (!(x & (xwu64_t)0xFFFFFFFF00000000)) {
                r -= (xwssq_t)32;
        } else {
                x >>= (xwu64_t)32;
        }

        if (!(x & (xwu64_t)0xFFFF0000)) {
                x <<= (xwu64_t)16;
                r -= (xwssq_t)16;
        }
        if (!(x & (xwu64_t)0xFF000000)) {
                x <<= (xwu64_t)8;
                r -= (xwssq_t)8;
        }
        if (!(x & (xwu64_t)0xF0000000)) {
                x <<= (xwu64_t)4;
                r -= (xwssq_t)4;
        }
        if (!(x & (xwu64_t)0xC0000000)) {
                x <<= (xwu64_t)2;
                r -= (xwssq_t)2;
        }
        if (!(x & (xwu64_t)0x80000000)) {
                r -= (xwssq_t)1;
        }
        return r;
#endif /* !__GNUC__ */
}
#endif /* !ARCHCFG_LIB_XWBOP_FLS64 */
#endif /* XWLIBCFG_XWBOP_FLS64 */

/**
 * @brief 赋值操作数到位图
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 位图中总的位数
 */
__xwlib_code
void xwbmpop_assign(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num)
{
        xwsz_t n = BITS_TO_BMPS(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                bmp[i] = opd[i];
        }
}

/**
 * @brief 赋值操作数到位图
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 位图中总的位数
 * @return 比较结果
 * @retval 0: 相等
 * @retval <0: 小于
 * @retval >0: 大于
 */
__xwlib_code
xwssq_t xwbmpop_cmp(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num)
{
        xwsz_t n = BITS_TO_BMPS(num);
        xwsz_t i;
        xwssq_t ret;

        ret = 0;
        for (i = 0; i < n; i++) {
                if (bmp[i] < opd[i]) {
                        ret = -1;
                        break;
                } else if (bmp[i] > opd[i]) {
                        ret = 1;
                        break;
                }/* else {} */
        }
        return ret;
}

/**
 * @brief 将位图所有位设为1
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 */
__xwlib_code
void xwbmpop_s1all(xwbmp_t * bmp, xwsq_t num)
{
        xwsz_t n = BITS_TO_BMPS(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                xwbop_s1m(xwbmp_t, &bmp[i], (xwbmp_t)(~0));
        }
}

/**
 * @brief 清除位图中所有位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 */
__xwlib_code
void xwbmpop_c0all(xwbmp_t * bmp, xwsq_t num)
{
        xwsz_t n = BITS_TO_BMPS(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                xwbop_c0m(xwbmp_t, &bmp[i], (xwbmp_t)(~0));
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_S1I)) || (1 != ARCHCFG_LIB_XWBMPOP_S1I)
/**
 * @brief 将位图中某位置1
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 位的序号
 */
__xwlib_code
void xwbmpop_s1i(xwbmp_t * bmp, xwsq_t n)
{
        bmp = bmp + BIT_BMP(n);
        xwbop_s1m(xwbmp_t, bmp, BIT_BMP_MASK(n));
}
#endif /* !ARCHCFG_LIB_XWBMPOP_S1I */

/**
 * @brief 将位图中掩码位全部置1
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_BMPS(num);
        for (i = 0; i < size; i++) {
                xwbop_s1m(xwbmp_t, &bmp[i], msk[i]);
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_C0I)) || (1 != ARCHCFG_LIB_XWBMPOP_C0I)
/**
 * @brief 将位图中某位清0
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被清0的位的序号
 */
__xwlib_code
void xwbmpop_c0i(xwbmp_t * bmp, xwsq_t n)
{
        bmp = bmp + BIT_BMP(n);
        xwbop_c0m(xwbmp_t, bmp, BIT_BMP_MASK(n));
}
#endif /* !ARCHCFG_LIB_XWBMPOP_C0I */

/**
 * @brief 将位图中掩码位全部清0
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_BMPS(num);
        for (i = 0; i < size; i++) {
                xwbop_c0m(xwbmp_t, &bmp[i], msk[i]);
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_X1I)) || (1 != ARCHCFG_LIB_XWBMPOP_X1I)
/**
 * @brief 将位图中某位翻转
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被翻转的位的序号
 */
__xwlib_code
void xwbmpop_x1i(xwbmp_t * bmp, xwsq_t n)
{
        bmp = bmp + BIT_BMP(n);
        xwbop_x1m(xwbmp_t, bmp, BIT_BMP_MASK(n));
}
#endif /* !ARCHCFG_LIB_XWBMPOP_X1I */

/**
 * @brief 将位图中掩码位全部翻转
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_x1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_BMPS(num);
        for (i = 0; i < size; i++) {
                xwbop_x1m(xwbmp_t, &bmp[i], msk[i]);
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_T1I)) || (1 != ARCHCFG_LIB_XWBMPOP_T1I)
/**
 * @brief 测试位图中的某位是否为1
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被测试的位的序号
 * @return 布尔型结果
 * @retval true: 置位
 * @retval false: 复位
 */
__xwlib_code
bool xwbmpop_t1i(xwbmp_t * bmp, xwsq_t n)
{
        bmp = bmp + BIT_BMP(n);
        return (bool)(*bmp & BIT_BMP_MASK(n));
}
#endif /* !ARCHCFG_LIB_XWBMPOP_T1I */

/**
 * @brief 测试位图中掩码位是否全部为1
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔型结果
 * @retval true: 全部为1
 * @retval false: 至少一位为0
 */
__xwlib_code
bool xwbmpop_t1ma(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_BMPS(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!((bmp[i] & msk[i]) == msk[i]);
                if (!ret) {
                        break;
                }
        }
        return ret;
}

/**
 * @brief 测试位图中掩码位是否全部为1，如果是，就将掩码位全部清0。
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔型结果
 * @retval true: 全部为1
 * @retval false: 至少一位为0
 */
__xwlib_code
bool xwbmpop_t1ma_then_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_BMPS(num);
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

/**
 * @brief 测试位图中掩码位是否至少有一位为1
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔型结果
 * @retval true: 至少一位为1
 * @retval false: 全部为0
 */
__xwlib_code
bool xwbmpop_t1mo(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_BMPS(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!(bmp[i] & msk[i]);
                if (ret) {
                        break;
                }
        }
        return ret;
}

/**
 * @brief 测试位图中掩码位是否至少有一位为1，如果是，就将掩码位全部清0。
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔型结果
 * @retval true: 至少一位为1
 * @retval false: 全部为0
 */
__xwlib_code
bool xwbmpop_t1mo_then_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_BMPS(num);
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

/**
 * @brief 测试位图中掩码位是否全部为0
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔型结果
 * @retval true: 全部为0
 * @retval false: 至少一位为1
 */
__xwlib_code
bool xwbmpop_t0ma(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_BMPS(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !(bmp[i] & msk[i]);
                if (!ret) {
                        break;
                }
        }
        return ret;
}

/**
 * @brief 测试位图中掩码位是否全部为0，如果是，就将掩码位全部置1。
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔型结果
 * @retval true: 全部为0
 * @retval false: 至少一位为1
 */
__xwlib_code
bool xwbmpop_t0ma_then_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_BMPS(num);
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

/**
 * @brief 测试位图中掩码位是否至少有一位为0
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔型结果
 * @retval true: 至少一位为0
 * @retval false: 全部为1
 */
__xwlib_code
bool xwbmpop_t0mo(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_BMPS(num);
        ret = false;
        for (i = 0; i < size; i++) {
                ret = !!((bmp[i] & msk[i]) != msk[i]);
                if (ret) {
                        break;
                }
        }
        return ret;
}

/**
 * @brief 测试位图中掩码位是否至少有一位为0，如果是，就将掩码位全部置1。
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔型结果
 * @retval true: 至少一位为0
 * @retval false: 全部为1
 */
__xwlib_code
bool xwbmpop_t0mo_then_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;
        bool ret;

        size = BITS_TO_BMPS(num);
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

/**
 * @brief 将位图与操作数进行逐位“与”操作
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_and(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_BMPS(num);
        for (i = 0; i < size; i++) {
                bmp[i] &= opd[i];
        }
}

/**
 * @brief 将位图与操作数进行逐位“或”操作
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_or(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_BMPS(num);
        for (i = 0; i < size; i++) {
                bmp[i] |= opd[i];
        }
}

/**
 * @brief 将位图与操作数进行逐位“异或”操作
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_xor(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num)
{
        xwsq_t i;
        xwsz_t size;

        size = BITS_TO_BMPS(num);
        for (i = 0; i < size; i++) {
                bmp[i] ^= opd[i];
        }
}

#if (!defined(ARCHCFG_LIB_XWBMPOP_FFS)) || (1 != ARCHCFG_LIB_XWBMPOP_FFS)
/**
 * @brief 在位图中从最低位起查找第一个置1的位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
__xwlib_code
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m = BITS_TO_BMPS(num);
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p = -1;

        for (i = 0; i < m; i++) {
                if (i == (m - 1)) {
                        msk = (BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((xwbmp_t)0 == msk) {
                                msk = (xwbmp_t)(~(xwbmp_t)0);
                        }/* else {} */
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
        }/* else {} */
        return p;
}
#endif /* !ARCHCFG_LIB_XWBMPOP_FFS */

#if (!defined(ARCHCFG_LIB_XWBMPOP_FFZ)) || (1 != ARCHCFG_LIB_XWBMPOP_FFZ)
/**
 * @brief 在位图中从最低位起查找第一个清0的位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位被清0
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
__xwlib_code
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m = BITS_TO_BMPS(num);
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p = -1;

        for (i = 0; i < m; i++) {
                if (i == (m - 1)) {
                        msk = ~(BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((xwbmp_t)(~(xwbmp_t)0) == msk) {
                                msk = (xwbmp_t)0;
                        }/* else {} */
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
        }/* else {} */
        return p;
}
#endif /* !ARCHCFG_LIB_XWBMPOP_FFZ */

#if (!defined(ARCHCFG_LIB_XWBMPOP_FLS)) || (1 != ARCHCFG_LIB_XWBMPOP_FLS)
/**
 * @brief 在位图中从最高位起查找第一个为1的位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
__xwlib_code
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t i = BITS_TO_BMPS(num);
        xwbmp_t msk;
        xwssq_t p = -1;

        msk = (BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((xwbmp_t)0 == msk) {
                msk = (xwbmp_t)(~(xwbmp_t)0);
        }/* else {} */
        for (; i > 0;) {
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
#endif /* !ARCHCFG_LIB_XWBMPOP_FLS */

#if (!defined(ARCHCFG_LIB_XWBMPOP_FLZ)) || (1 != ARCHCFG_LIB_XWBMPOP_FLZ)
/**
 * @brief 在位图中从最高位起查找第一个为0的位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
__xwlib_code
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t i = BITS_TO_BMPS(num);
        xwbmp_t msk;
        xwssq_t p = -1;

        msk = ~(BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((xwbmp_t)(~(xwbmp_t)0) == msk) {
                msk = (xwbmp_t)0;
        }/* else {} */
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
#endif /* !ARCHCFG_LIB_XWBMPOP_FLZ */

#if (defined(XWLIBCFG_XWBOP_RE16)) && (1 == XWLIBCFG_XWBOP_RE16)
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
#endif /* !ARCHCFG_LIB_XWBOP_RE16 */
#endif /* XWLIBCFG_XWBOP_RE16 */

#if (defined(XWLIBCFG_XWBOP_RE32)) && (1 == XWLIBCFG_XWBOP_RE32)
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
#endif /* !ARCHCFG_LIB_XWBOP_RE32 */
#endif /* XWLIBCFG_XWBOP_RE32 */

#if (defined(XWLIBCFG_XWBOP_RE64)) && (1 == XWLIBCFG_XWBOP_RE64)
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
#endif /* !ARCHCFG_LIB_XWBOP_RE64 */
#endif /* XWLIBCFG_XWBOP_RE64 */

#if (defined(XWLIBCFG_XWBOP_RBIT8)) && (1 == XWLIBCFG_XWBOP_RBIT8)
#if (!defined(ARCHCFG_LIB_XWBOP_RBIT8)) || (1 != ARCHCFG_LIB_XWBOP_RBIT8)
__xwlib_code
xwu8_t xwbop_rbit8(xwu8_t x)
{
        return xwbop_rbit8_table[x];
}
#endif /* !ARCHCFG_LIB_XWBOP_RBIT8 */
#endif /* XWLIBCFG_XWBOP_RBIT8 */

#if (defined(XWLIBCFG_XWBOP_RBIT16)) && (1 == XWLIBCFG_XWBOP_RBIT16)
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
        } r, t;

        t.d16 = x;
        r.d8.h = xwbop_rbit8(t.d8.l);
        r.d8.l = xwbop_rbit8(t.d8.h);
        return r.d16;
}
#endif /* !ARCHCFG_LIB_XWBOP_RBIT16 */
#endif /* XWLIBCFG_XWBOP_RBIT16 */

#if (defined(XWLIBCFG_XWBOP_RBIT32)) && (1 == XWLIBCFG_XWBOP_RBIT32)
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
        } r, t;

        t.d32 = x;
        r.d8.byte0 = xwbop_rbit8(t.d8.byte3);
        r.d8.byte1 = xwbop_rbit8(t.d8.byte2);
        r.d8.byte2 = xwbop_rbit8(t.d8.byte1);
        r.d8.byte3 = xwbop_rbit8(t.d8.byte0);
        return r.d32;
}
#endif /* !ARCHCFG_LIB_XWBOP_RBIT32 */
#endif /* XWLIBCFG_XWBOP_RBIT32 */

#if defined(XWLIBCFG_XWBOP_RBIT64) && (1 == XWLIBCFG_XWBOP_RBIT64)
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
        } r, t;

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
#endif /* !ARCHCFG_LIB_XWBOP_RBIT64 */
#endif /* XWLIBCFG_XWBOP_RBIT64 */
