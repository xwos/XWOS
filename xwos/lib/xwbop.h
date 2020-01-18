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

#ifndef __xwos_lib_xwbop_h__
#define __xwos_lib_xwbop_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <asmlib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         macros & functions          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BIT(n)                  (1UL << (n))
#define BIT_ULL(n)              (1ULL << (n))
#define BIT_MASK(n)             (1UL << ((n) % BITS_PER_LONG))
#define BIT_WORD(n)             ((n) / BITS_PER_LONG)
#define BIT_ULL_MASK(n)         (1ULL << ((n) % BITS_PER_LONGLONG))
#define BIT_ULL_WORD(n)         ((n) / BITS_PER_LONGLONG)
#define BIT_BMP_MASK(n)         ((xwbmp_t)1 << (xwbmp_t)((n) % BITS_PER_XWBMP_T))
#define BIT_BMP(n)              ((n) / BITS_PER_XWBMP_T)
#define BITS_PER_BYTE           8
#define DIV_ROUND(n, d)         ((n) / (d))
#define DIV_ROUND_UP(n, d)      (((n) + (d) - 1U) / (d))
#define SHIFT_ROUND(n, s)       ((n) >> (s))
#define SHIFT_ROUND_UP(n, s)    (((n) + (1U << (s)) - 1U) >> (s))
#define BITS_TO_BYTES(n)        DIV_ROUND_UP(n, BITS_PER_BYTE)
#define BITS_TO_LONGS(n)        DIV_ROUND_UP(n, BITS_PER_BYTE * sizeof(long))
#define BITS_TO_BMPS(n)         DIV_ROUND_UP(n, BITS_PER_BYTE * sizeof(xwbmp_t))
#define ROUND(x, n)             ((x) & (~((n) - 1U)))
#define ALIGN(x, n)             (((x) + ((n) - 1U)) & (~((n) - 1U)))

/**
 * @brief 声明一个位图
 * @param name: (I) 符号名
 * @param bits: (I) 位图中的位数
 */
#define DECLARE_BITMAP(name, bits)  xwbmp_t name[BITS_TO_BMPS(bits)]

/**
 * @brief 得到一条位带掩码
 * @param s: (I) 起始位
 * @param bitwidth: (I) 位带宽度
 */
#define BITSTRIP_MASK(s, bitwidth)              \
        (((1UL << (bitwidth)) - 1U) << ((unsigned long)s))

/**
 * @brief 取得数值中的位带部分
 * @param s: (I) 起始位
 * @param v: (I) 位带数值
 * @param bitwidth: (I) 位带宽度
 */
#define BITSTRIP(s, v, bitwidth)                \
        ((((unsigned long)v) & ((1UL << (bitwidth)) - 1U)) << (s))

/**
 * @brief 得到一条unsigned long long类型的位带掩码
 * @param s: (I) 起始位
 * @param bitwidth: (I) 位带宽度
 */
#define BITSTRIP_MASK_ULL(s, bitwidth)          \
        (((1ULL << (bitwidth)) - 1) << ((unsigned long long)s))

/**
 * @brief 取得unsigned long long类型的数值中的位带部分
 * @param s: (I) 起始位
 * @param v: (I) 位带数值
 * @param bitwidth: (I) 位带宽度
 */
#define BITSTRIP_ULL(s, v, bitwidth)            \
        ((((unsigned long long)(v)) & ((1ULL << (bitwidth)) - 1U)) << (s))

/**
 * @brief 取得数据中的位
 * @param x: (I) 数据
 * @param bit: (I) 位的序号
 */
#define GETBIT(x, bit)      (((x) >> (bit)) & 1U)

/******** ******** bit-operations APIs ******** ********/
/**
 * @brief XWOS BOPLIB：调用位操作函数
 * @param type: (I) 数据类型
 * @param op: (I) 位操作函数
 * @param ...: (I) 函数的参数
 */
#define xwbop(type, op, ...)            xwbop_##op##__##type(__VA_ARGS__)

/**
 * @brief XWOS BOPLIB：掩码位置1
 * @param type: (I) 数据类型
 * @param addr: (I) 数据的地址指针
 * @param mask: (I) 置1位的掩码
 */
#define xwbop_s1m(type, addr, mask)     xwbop(type, s1m, (addr), (mask))

/**
 * @brief XWOS BOPLIB：掩码位清0
 * @param type: (I) 数据类型
 * @param addr: (I) 数据的地址指针
 * @param mask: (I) 清1位的掩码
 */
#define xwbop_c0m(type, addr, mask)     xwbop(type, c0m, (addr), (mask))

/**
 * @brief XWOS BOPLIB：掩码位反转
 * @param type: (I) 数据类型
 * @param addr: (I) 数据的地址指针
 * @param mask: (I) 置1位的掩码
 */
#define xwbop_x1m(type, addr, mask)     xwbop(type, x1m, (addr), (mask))

/**
 * @brief XWOS BOPLIB：位镜面翻转
 * @param type: (I) 数据类型
 * @param data: (I) 数据（注：非指针）
 * @return 镜面翻转后的结果
 */
#define xwbop_rbit(type, data)          xwbop(type, rbit, (data))

/**
 * @brief XWOS BOPLIB：调用大小端翻转函数
 * @param type: (I) 数据类型
 * @param data: (I) 数据（注：非指针）
 * @return 大小端翻转后的结果
 */
#define xwbop_re(type, data)            xwbop(type, re, (data))

/**
 * @brief XWOS BOPLIB：在数据中从最低位起查找第一个为1的位
 * @param type: (I) 数据类型
 * @param x: (I) 数据（注：非指针）
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
#define xwbop_ffs(type, data)           xwbop(type, ffs, (data))

/**
 * @brief XWOS BOPLIB：在数据中从最高位起查找第一个为1的位
 * @param type: (I) 数据类型
 * @param x: (I) 数据（注：非指针）
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_fls(type, data)           xwbop(type, fls, (data))

/**
 * @brief XWOS BOPLIB：在数据中从最低位起查找第一个为0的位
 * @param type: (I) 数据类型
 * @param x: (I) 数据（注：非指针）
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_ffz(type, data)           xwbop(type, flz, (data))

/**
 * @brief XWOS BOPLIB：在数据中从最高位起查找第一个为0的位
 * @param type: (I) 数据类型
 * @param x: (I) 数据（注：非指针）
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_flz(type, data)           xwbop(type, flz, (data))

/******** ******** 8-bit data bit-operations ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_S1M8) && (1 == ARCHCFG_LIB_XWBOP_S1M8))
#define xwbop_s1m8(a, mask)     arch_xwbop_s1m8(a, mask)
#else
#define xwbop_s1m8(a, mask)     *(a) |= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_C0M8) && (1 == ARCHCFG_LIB_XWBOP_C0M8))
#define xwbop_c0m8(a, mask)     arch_xwbop_c0m8(a, mask)
#else
#define xwbop_c0m8(a, mask)     *(a) &= (~(mask))
#endif

#if (defined(ARCHCFG_LIB_XWBOP_X1M8) && (1 == ARCHCFG_LIB_XWBOP_X1M8))
#define xwbop_x1m8(a, mask)     arch_xwbop_x1m8(a, mask)
#else
#define xwbop_x1m8(a, mask)     *(a) ^= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS8) && (1 == ARCHCFG_LIB_XWBOP_FFS8))
#define xwbop_ffs8(x)           arch_xwbop_ffs8(x)
#else
__xwlib_code
xwssq_t xwbop_ffs8(xwu8_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FLS8) && (1 == ARCHCFG_LIB_XWBOP_FLS8))
#define xwbop_fls8(x)           arch_xwbop_fls8(x)
#else
__xwlib_code
xwssq_t xwbop_fls8(xwu8_t x);
#endif

static __xwlib_inline
xwssq_t xwbop_ffz8(xwu8_t x)
{
        return xwbop_ffs8((xwu8_t)(~x));
}

static __xwlib_inline
xwssq_t xwbop_flz8(xwu8_t x)
{
        return xwbop_fls8((xwu8_t)(~x));
}

#if (defined(ARCHCFG_LIB_XWBOP_RBIT8) && (1 == ARCHCFG_LIB_XWBOP_RBIT8))
  #define xwbop_rbit8(x)        arch_xwbop_rbit8(x)
#else
__xwlib_code
xwu8_t xwbop_rbit8(xwu8_t x);
#endif

/**
 * @brief 翻转8位数据的大小端
 * @param x: (I) 8位数据
 * @return 结果
 * @note
 * - 这个函数不做任何操作，是为了位操作模板的统一接口而实现的dummy函数。
 *   因为大小端是以字节为单位的。
 */
static __xwlib_inline
xwu8_t xwbop_re8(xwu8_t x)
{
        return x;
}

/******** ******** 16-bit data bit operations ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_S1M16) && (1 == ARCHCFG_LIB_XWBOP_S1M16))
#define xwbop_s1m16(a, mask)    arch_s1m16(a, mask)
#else
#define xwbop_s1m16(a, mask)    *(a) |= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_C0M16) && (1 == ARCHCFG_LIB_XWBOP_C0M16))
#define xwbop_c0m16(a, mask)    arch_xwbop_c0m16(a, mask)
#else
#define xwbop_c0m16(a, mask)    *(a) &= (~(mask))
#endif

#if (defined(ARCHCFG_LIB_XWBOP_X1M16) && (1 == ARCHCFG_LIB_XWBOP_X1M16))
#define xwbop_x1m16(a, mask)    arch_xwbop_x1m16(a, mask)
#else
#define xwbop_x1m16(a, mask)    *(a) ^= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS16) && (1 == ARCHCFG_LIB_XWBOP_FFS16))
#define xwbop_ffs16(x)          arch_xwbop_ffs16(x)
#else
__xwlib_code
xwssq_t xwbop_ffs16(xwu16_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FLS16) && (1 == ARCHCFG_LIB_XWBOP_FLS16))
#define xwbop_fls16(x)          arch_xwbop_fls16(x)
#else
__xwlib_code
xwssq_t xwbop_fls16(xwu16_t x);
#endif

static __xwlib_inline
xwssq_t xwbop_ffz16(xwu16_t x)
{
        return xwbop_ffs16((xwu16_t)(~x));
}

static __xwlib_inline
xwssq_t xwbop_flz16(xwu16_t x)
{
        return xwbop_fls16((xwu16_t)(~x));
}

#if (defined(ARCHCFG_LIB_XWBOP_RBIT16) && (1 == ARCHCFG_LIB_XWBOP_RBIT16))
#define xwbop_rbit16(x)         arch_xwbop_rbit16(x)
#else
__xwlib_code
xwu16_t xwbop_rbit16(xwu16_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_RE16) && (1 == ARCHCFG_LIB_XWBOP_RE16))
#define xwbop_re16(x)           arch_xwbop_re16(x)
#else
__xwlib_code
xwu16_t xwbop_re16(xwu16_t a);
#endif

/**
 * @brief 将16位数据的大小端翻转，并将符号位扩展到32位
 * @param x: (I) 数据（注：非指针）
 * @return 结果
 */
#if (defined(ARCHCFG_LIB_XWBOP_RE16S32) && (1 == ARCHCFG_LIB_XWBOP_RE16S32))
#define xwbop_re16s32(x)        arch_xwbop_re16s32(x)
#else
__xwlib_code
xws32_t xwbop_re16s32(xwu16_t x);
#endif

/******** ******** 32-bit data bit operations ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_S1M32) && (1 == ARCHCFG_LIB_XWBOP_S1M32))
#define xwbop_s1m32(a, mask)    arch_xwbop_s1m32(a, mask)
#else
#define xwbop_s1m32(a, mask)    *(a) |= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_C0M32) && (1 == ARCHCFG_LIB_XWBOP_C0M32))
#define xwbop_c0m32(a, mask)    arch_xwbop_c0m32(a, mask)
#else
#define xwbop_c0m32(a, mask)    *(a) &= (~(mask))
#endif

#if (defined(ARCHCFG_LIB_XWBOP_X1M32) && (1 == ARCHCFG_LIB_XWBOP_X1M32))
#define xwbop_x1m32(a, mask)    arch_xwbop_x1m32(a, mask)
#else
#define xwbop_x1m32(a, mask)    *(a) ^= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS32) && (1 == ARCHCFG_LIB_XWBOP_FFS32))
#define xwbop_ffs32(x)                  arch_xwbop_ffs32(x)
#else
__xwlib_code
xwssq_t xwbop_ffs32(xwu32_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FLS32) && (1 == ARCHCFG_LIB_XWBOP_FLS32))
#define xwbop_fls32(x)                  arch_xwbop_fls32(x)
#else
__xwlib_code
xwssq_t xwbop_fls32(xwu32_t x);
#endif

static __xwlib_inline
xwssq_t xwbop_ffz32(xwu32_t x)
{
        return xwbop_ffs32(~x);
}

static __xwlib_inline
xwssq_t xwbop_flz32(xwu32_t x)
{
        return xwbop_fls32(~x);
}

#if (defined(ARCHCFG_LIB_XWBOP_RBIT32) && (1 == ARCHCFG_LIB_XWBOP_RBIT32))
#define xwbop_rbit32(x)         arch_xwbop_rbit32(x)
#else
__xwlib_code
xwu32_t xwbop_rbit32(xwu32_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_RE32) && (1 == ARCHCFG_LIB_XWBOP_RE32))
#define xwbop_re32(x)           arch_xwbop_re32(x)
#else
__xwlib_code
xwu32_t xwbop_re32(xwu32_t x);
#endif

/**
 * @brief 将32位数据的大小端翻转，并将符号位扩展到64位
 * @param x: (I) 数据（注：非指针）
 * @return 结果
 */
#if (defined(ARCHCFG_LIB_XWBOP_RE32S64) && (1 == ARCHCFG_LIB_XWBOP_RE32S64))
#define xwbop_re32s64(x)        arch_xwbop_re32s64(x)
#else
__xwlib_code
xws64_t xwbop_re32s64(xwu32_t x);
#endif

/******** ******** 64-bit data bit operations ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_S1M64) && (1 == ARCHCFG_LIB_XWBOP_S1M64))
#define xwbop_s1m64(a64, mask64)        arch_xwbop_s1m64(a64, mask64)
#else
#define xwbop_s1m64(a64, mask64)        *((xwu64_t *)a64) |= (mask64)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_C0M64) && (1 == ARCHCFG_LIB_XWBOP_C0M64))
#define xwbop_c0m64(a64, mask64)        arch_xwbop_c0m64(a64, mask64)
#else
#define xwbop_c0m64(a64, mask64)        *((xwu64_t *)a64) &= (~(mask64))
#endif

#if (defined(ARCHCFG_LIB_XWBOP_X1M64) && (1 == ARCHCFG_LIB_XWBOP_X1M64))
#define xwbop_x1m64(a64, mask64)        arch_xwbop_x1m64(a64, mask64)
#else
#define xwbop_x1m64(a64, mask64)        *((xwu64_t *)a64) ^= (mask64)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS64) && (1 == ARCHCFG_LIB_XWBOP_FFS64))
#define xwbop_ffs64(x)                  arch_xwbop_ffs64(x)
#else
__xwlib_code
xwssq_t xwbop_ffs64(xwu64_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FLS64) && (1 == ARCHCFG_LIB_XWBOP_FLS64))
#define xwbop_fls64(x)                  arch_xwbop_fls64(x)
#else
__xwlib_code
xwssq_t xwbop_fls64(xwu64_t x);
#endif

static __xwlib_inline
xwssq_t xwbop_ffz64(xwu64_t x)
{
        return xwbop_ffs64(~x);
}

static __xwlib_inline
xwssq_t xwbop_flz64(xwu64_t x)
{
        return xwbop_ffs64(~x);
}

#if (defined(ARCHCFG_LIB_XWBOP_RE64) && (1 == ARCHCFG_LIB_XWBOP_RE64))
#define xwbop_re64(x)           arch_xwbop_re64(x)
#else
__xwlib_code
xwu64_t xwbop_re64(xwu64_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_RBIT64) && (1 == ARCHCFG_LIB_XWBOP_RBIT64))
#define xwbop_rbit64(x)         arch_xwbop_rbit64(x)
#else
__xwlib_code
xwu64_t xwbop_rbit64(xwu64_t x);
#endif

/******** ******** type bit operation template ******** ********/
/**
 * @brief 定义位操作模板：s1m
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_S1M(type, bw)                                              \
static __xwlib_inline                                                           \
void xwbop_s1m__##type(type * x, type m)                                        \
{                                                                               \
        xwbop_s1m##bw((xwu##bw##_t *)x, (xwu##bw##_t)m);                        \
}

/**
 * @brief 定义位操作模板：c0m
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_C0M(type, bw)                                              \
static __xwlib_inline                                                           \
void xwbop_c0m__##type(type * x, type m)                                        \
{                                                                               \
        xwbop_c0m##bw((xwu##bw##_t *)x, (xwu##bw##_t)m);                        \
}

/**
 * @brief 定义位操作模板：x1m
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_X1M(type, bw)                                              \
static __xwlib_inline                                                           \
void xwbop_x1m__##type(type * x, type m)                                        \
{                                                                               \
        xwbop_x1m##bw((xwu##bw##_t *)x, (xwu##bw##_t)m);                        \
}

/**
 * @brief 定义位操作模板：rbit
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_RBIT(type, bw)                                             \
static __xwlib_inline                                                           \
type xwbop_rbit__##type(type x)                                                 \
{                                                                               \
        return (type)xwbop_rbit##bw((xwu##bw##_t)x);                            \
}

/**
 * @brief 定义位操作模板：re
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_RE(type, bw)                                               \
static __xwlib_inline                                                           \
type xwbop_re__##type(type x)                                                   \
{                                                                               \
        return (type)xwbop_re##bw((xwu##bw##_t)x);                              \
}

/**
 * @brief 定义位操作模板：ffs
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_FFS(type, bw)                                              \
static __xwlib_inline                                                           \
xwssq_t xwbop_ffs__##type(type x)                                               \
{                                                                               \
        return xwbop_ffs##bw((xwu##bw##_t)x);                                   \
}

/**
 * @brief 定义位操作模板：fls
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_FLS(type, bw)                                              \
static __xwlib_inline                                                           \
xwssq_t xwbop_fls__##type(type x)                                               \
{                                                                               \
        return xwbop_fls##bw((xwu##bw##_t)x);                                   \
}

/**
 * @brief 定义位操作模板：flz
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_FLZ(type, bw)                                              \
static __xwlib_inline                                                           \
xwssq_t xwbop_flz__##type(type x)                                               \
{                                                                               \
        return xwbop_flz##bw((xwu##bw##_t)x);                                   \
}

/**
 * @brief 定义位操作模板：ffz
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP_FFZ(type, bw)                                              \
static __xwlib_inline                                                           \
xwssq_t xwbop_ffz__##type(type x)                                               \
{                                                                               \
        return xwbop_ffz##bw((xwu##bw##_t)x);                                   \
}

/**
 * @brief 使用模板为类型定义所有的位操作函数
 * @param type: (I) 类型
 * @param bw: (I) 位宽
 */
#define DEFINE_XWBOP(type, bw)                                                  \
        DEFINE_XWBOP_S1M(type, bw)                                              \
        DEFINE_XWBOP_C0M(type, bw)                                              \
        DEFINE_XWBOP_X1M(type, bw)                                              \
        DEFINE_XWBOP_RBIT(type, bw)                                             \
        DEFINE_XWBOP_RE(type, bw)                                               \
        DEFINE_XWBOP_FFS(type, bw)                                              \
        DEFINE_XWBOP_FLS(type, bw)                                              \
        DEFINE_XWBOP_FLZ(type, bw)                                              \
        DEFINE_XWBOP_FFZ(type, bw)

DEFINE_XWBOP(xwu8_t, 8)
DEFINE_XWBOP(xws8_t, 8)

DEFINE_XWBOP(xwu16_t, 16)
DEFINE_XWBOP(xws16_t, 16)

DEFINE_XWBOP(xwu32_t, 32)
DEFINE_XWBOP(xws32_t, 32)

DEFINE_XWBOP(xwu64_t, 64)
DEFINE_XWBOP(xws64_t, 64)

#if (16 == BITS_PER_XWSZ_T)
DEFINE_XWBOP(xwsz_t, 16)
#elif (32 == BITS_PER_XWSZ_T)
DEFINE_XWBOP(xwsz_t, 32)
#elif (64 == BITS_PER_XWSZ_T)
DEFINE_XWBOP(xwsz_t, 64)
#endif

#if (16 == BITS_PER_XWPTR_T)
DEFINE_XWBOP(xwptr_t, 16)
#elif (32 == BITS_PER_XWPTR_T)
DEFINE_XWBOP(xwptr_t, 32)
#elif (64 == BITS_PER_XWPTR_T)
DEFINE_XWBOP(xwptr_t, 64)
#endif

#if (8 == BITS_PER_XWID_T)
DEFINE_XWBOP(xwid_t, 8)
#elif (16 == BITS_PER_XWID_T)
DEFINE_XWBOP(xwid_t, 16)
#elif (32 == BITS_PER_XWID_T)
DEFINE_XWBOP(xwid_t, 32)
#elif (64 == BITS_PER_XWID_T)
DEFINE_XWBOP(xwid_t, 64)
#endif

#if (8 == BITS_PER_XWREG_T)
DEFINE_XWBOP(xwreg_t, 8)
#elif (16 == BITS_PER_XWREG_T)
DEFINE_XWBOP(xwreg_t, 16)
#elif (32 == BITS_PER_XWREG_T)
DEFINE_XWBOP(xwreg_t, 32)
#elif (64 == BITS_PER_XWREG_T)
DEFINE_XWBOP(xwreg_t, 64)
#endif

#if (16 == BITS_PER_XWSQ_T)
DEFINE_XWBOP(xwsq_t, 16)
#elif (32 == BITS_PER_XWSQ_T)
DEFINE_XWBOP(xwsq_t, 32)
#elif (64 == BITS_PER_XWSQ_T)
DEFINE_XWBOP(xwsq_t, 64)
#endif

#if (8 == BITS_PER_XWBMP_T)
DEFINE_XWBOP(xwbmp_t, 8)
#elif (16 == BITS_PER_XWBMP_T)
DEFINE_XWBOP(xwbmp_t, 16)
#elif (32 == BITS_PER_XWBMP_T)
DEFINE_XWBOP(xwbmp_t, 32)
#elif (64 == BITS_PER_XWBMP_T)
DEFINE_XWBOP(xwbmp_t, 64)
#endif

/******** ******** bitmap operations ******** ********/
__xwlib_code
void xwbmpop_assign(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

__xwlib_code
xwssq_t xwbmpop_cmp(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

__xwlib_code
void xwbmpop_s1all(xwbmp_t * bmp, xwsq_t num);

__xwlib_code
void xwbmpop_c0all(xwbmp_t * bmp, xwsq_t num);

#if (defined(ARCHCFG_LIB_XWBMPOP_S1I) && (1 == ARCHCFG_LIB_XWBMPOP_S1I))
  #define xwbmpop_s1i(bmp, n)           arch_xwbmpop_s1i(bmp, n)
#else
__xwlib_code
void xwbmpop_s1i(xwbmp_t * bmp, xwsq_t n);
#endif

__xwlib_code
void xwbmpop_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

#if (defined(ARCHCFG_LIB_XWBMPOP_C0I) && (1 == ARCHCFG_LIB_XWBMPOP_C0I))
  #define xwbmpop_c0i(bmp, n)           arch_xwbmpop_c0i(bmp, n)
#else
__xwlib_code
void xwbmpop_c0i(xwbmp_t * bmp, xwsq_t n);
#endif

__xwlib_code
void xwbmpop_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

#if (defined(ARCHCFG_LIB_XWBMPOP_X1I) && (1 == ARCHCFG_LIB_XWBMPOP_X1I))
#define xwbmpop_x1i(bmp, n)             arch_xwbmpop_x1i(bmp, n)
#else
__xwlib_code
void xwbmpop_x1i(xwbmp_t * bmp, xwsq_t n);
#endif

__xwlib_code
void xwbmpop_x1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

#if (defined(ARCHCFG_LIB_XWBMPOP_T1I) && (1 == ARCHCFG_LIB_XWBMPOP_T1I))
#define xwbmpop_t1i(bmp, n)             arch_xwbmpop_t1i(bmp, n)
#else
__xwlib_code
bool xwbmpop_t1i(xwbmp_t * bmp, xwsq_t n);
#endif

__xwlib_code
bool xwbmpop_t1ma(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

__xwlib_code
bool xwbmpop_t1ma_then_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

__xwlib_code
bool xwbmpop_t1mo(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

__xwlib_code
bool xwbmpop_t1mo_then_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

__xwlib_code
bool xwbmpop_t0ma(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

__xwlib_code
bool xwbmpop_t0ma_then_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

__xwlib_code
bool xwbmpop_t0mo(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

__xwlib_code
bool xwbmpop_t0mo_then_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

__xwlib_code
void xwbmpop_and(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

__xwlib_code
void xwbmpop_or(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

__xwlib_code
void xwbmpop_xor(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

#if (defined(ARCHCFG_LIB_XWBMPOP_FFS) && (1 == ARCHCFG_LIB_XWBMPOP_FFS))
#define xwbmpop_ffs(bmp, num)           arch_xwbmpop_ffs(bmp, num)
#else
__xwlib_code
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num);
#endif

#if (defined(ARCHCFG_LIB_XWBMPOP_FLS) && (1 == ARCHCFG_LIB_XWBMPOP_FLS))
#define xwbmpop_fls(bmp, num)           arch_xwbmpop_fls(bmp, num)
#else
__xwlib_code
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t num);
#endif

#if (defined(ARCHCFG_LIB_XWBMPOP_FFZ) && (1 == ARCHCFG_LIB_XWBMPOP_FFZ))
#define xwbmpop_ffz(bmp, num)           arch_xwbmpop_ffz(bmp, num)
#else
__xwlib_code
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t num);
#endif

#if (defined(ARCHCFG_LIB_XWBMPOP_FLZ) && (1 == ARCHCFG_LIB_XWBMPOP_FLZ))
#define xwbmpop_flz(bmp, num)           arch_xwbmpop_flz(bmp, num)
#else
__xwlib_code
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t num);
#endif

#endif /* xwos/lib/xwbop.h */
