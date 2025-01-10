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

#ifndef __xwos_lib_xwbop_h__
#define __xwos_lib_xwbop_h__

#include <xwos/standard.h>

/**
 * @defgroup xwos_lib_xwbop 位操作
 * @ingroup xwos_lib
 * @{
 */

#include <xwos/ospl/soc/xwbop.h>

/******** ******** macros ******** ********/
#define XWBOP_BIT(n)  ((xwbmp_t)1 << (n))
#define XWBOP_BMP_MASK(n)  ((xwbmp_t)1 << (xwbmp_t)((n) % BITS_PER_XWBMP_T))
#define XWBOP_BMP(n)  ((n) / BITS_PER_XWBMP_T)
#define XWBOP_DIV_ROUND(n, d)  ((n) / (d))
#define XWBOP_DIV_ROUND_UP(n, d)  (((n) + (d) - 1U) / (d))
#define XWBOP_SHIFT_ROUND(n, s)  ((n) >> (s))
#define XWBOP_SHIFT_ROUND_UP(n, s)  (((n) + (1ULL << (s)) - 1ULL) >> (s))
#define BITS_TO_XWU8_T(n)  XWBOP_DIV_ROUND_UP(n, BITS_PER_XWU8_T)
#define BITS_TO_XWBMP_T(n)  XWBOP_DIV_ROUND_UP(n, BITS_PER_XWU8_T * sizeof(xwbmp_t))
#define XWBOP_ROUND(x, n)  ((x) & (~((n) - 1U)))
#define XWBOP_ALIGN(x, n)  (((x) + ((n) - 1U)) & (~((n) - 1U)))
#define XWBOP_TBIT(x, n)  (((x) >> (n)) & 1U)

/******** ******** 8-bit 位操作 ******** ********/
#define xwbop_s1m8(a, mask)     *(a) |= (mask)
#define xwbop_c0m8(a, mask)     *(a) &= (~(mask))
#define xwbop_x1m8(a, mask)     *(a) ^= (mask)

xwssq_t xwbop_ffs8(xwu8_t x);
xwssq_t xwbop_fls8(xwu8_t x);
xwssq_t xwbop_ffz8(xwu8_t x);
xwssq_t xwbop_flz8(xwu8_t x);
xwu8_t xwbop_rbit8(xwu8_t x);

static __xwlib_inline
xwu8_t xwbop_re8(xwu8_t x)
{
        return x;
}

xwsz_t xwbop_weight8(xwu8_t x);

/******** ******** 16-bit 位操作 ******** ********/
#define xwbop_s1m16(a, mask)    *(a) |= (mask)
#define xwbop_c0m16(a, mask)    *(a) &= (~(mask))
#define xwbop_x1m16(a, mask)    *(a) ^= (mask)

xwssq_t xwbop_ffs16(xwu16_t x);
xwssq_t xwbop_fls16(xwu16_t x);
xwssq_t xwbop_ffz16(xwu16_t x);
xwssq_t xwbop_flz16(xwu16_t x);
xwu16_t xwbop_rbit16(xwu16_t x);
xwu16_t xwbop_re16(xwu16_t a);
xwsz_t xwbop_weight16(xwu16_t x);

/******** ******** 32-bit 位操作 ******** ********/
#define xwbop_s1m32(a, mask)    *(a) |= (mask)
#define xwbop_c0m32(a, mask)    *(a) &= (~(mask))
#define xwbop_x1m32(a, mask)    *(a) ^= (mask)

xwssq_t xwbop_ffs32(xwu32_t x);
xwssq_t xwbop_fls32(xwu32_t x);
xwssq_t xwbop_ffz32(xwu32_t x);
xwssq_t xwbop_flz32(xwu32_t x);
xwu32_t xwbop_rbit32(xwu32_t x);
xwu32_t xwbop_re32(xwu32_t x);
xwsz_t xwbop_weight32(xwu32_t x);

/******** ******** 64-bit 位操作 ******** ********/
#define xwbop_s1m64(a64, mask64)        *((xwu64_t *)(a64)) |= (mask64)
#define xwbop_c0m64(a64, mask64)        *((xwu64_t *)(a64)) &= (~(mask64))
#define xwbop_x1m64(a64, mask64)        *((xwu64_t *)(a64)) ^= (mask64)

xwssq_t xwbop_ffs64(xwu64_t x);
xwssq_t xwbop_fls64(xwu64_t x);
xwssq_t xwbop_ffz64(xwu64_t x);
xwssq_t xwbop_flz64(xwu64_t x);
xwu64_t xwbop_re64(xwu64_t x);
xwu64_t xwbop_rbit64(xwu64_t x);
xwsz_t xwbop_weight64(xwu64_t x);

/******** ******** 位操作 ******** ********/
/**
 * @brief 调用位操作函数模板
 * @param[in] type: 数据类型
 * @param[in] op: 位操作函数
 * @param[in] ...: 函数的参数
 */
#define xwbop(type, op, ...)            xwbop_##op##__##type(__VA_ARGS__)

/**
 * @brief 将数据掩码部分的位全部置1
 * @param[in] type: 数据类型
 * @param[in] addr: 数据的地址
 * @param[in] mask: 位的掩码
 */
#define xwbop_s1m(type, addr, mask)     xwbop(type, s1m, (addr), (mask))

/**
 * @brief 将数据掩码部分的位全部清0
 * @param[in] type: 数据类型
 * @param[in] addr: 数据的地址
 * @param[in] mask: 位的掩码
 */
#define xwbop_c0m(type, addr, mask)     xwbop(type, c0m, (addr), (mask))

/**
 * @brief 将数据掩码部分的位全部翻转
 * @param[in] type: 数据类型
 * @param[in] addr: 数据的地址
 * @param[in] mask: 位的掩码
 */
#define xwbop_x1m(type, addr, mask)     xwbop(type, x1m, (addr), (mask))

/**
 * @brief 将数据的位镜面翻转
 * @param[in] type: 数据类型
 * @param[in] data: 数据（注：非指针）
 * @return 镜面翻转后的结果
 */
#define xwbop_rbit(type, data)          xwbop(type, rbit, (data))

/**
 * @brief 将数据的大小端翻转
 * @param[in] type: 数据类型
 * @param[in] data: 数据（注：非指针）
 * @return 大小端翻转后的结果
 */
#define xwbop_re(type, data)            xwbop(type, re, (data))

/**
 * @brief 在数据中从最低位起查找第一个被置1的位
 * @param[in] type: 数据类型
 * @param[in] data: 数据（注：非指针）
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
#define xwbop_ffs(type, data)           xwbop(type, ffs, (data))

/**
 * @brief 在数据中从最高位起查找第一个被置1的位
 * @param[in] type: 数据类型
 * @param[in] data: 数据（注：非指针）
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_fls(type, data)           xwbop(type, fls, (data))

/**
 * @brief 在数据中从最低位起查找第一个被清0的位
 * @param[in] type: 数据类型
 * @param[in] data: 数据（注：非指针）
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_ffz(type, data)           xwbop(type, flz, (data))

/**
 * @brief 在数据中从最高位起查找第一个被清0的位
 * @param[in] type: 数据类型
 * @param[in] data: 数据（注：非指针）
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_flz(type, data)           xwbop(type, flz, (data))

/**
 * @brief 统计数据中1的个数
 * @param[in] type: 数据类型
 * @param[in] data: 数据（注：非指针）
 * @return 数据中1的个数
 */
#define xwbop_weight(type, data)        xwbop(type, weight, (data))

/******** ******** 位操作模板 ******** ********/
/**
 * @brief 定义位操作模板：s1m
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_S1M(type, bw)                                              \
static __xwlib_inline                                                           \
void xwbop_s1m__##type(type * x, type m)                                        \
{                                                                               \
        xwbop_s1m##bw((xwu##bw##_t *)x, (xwu##bw##_t)m);                        \
}

/**
 * @brief 定义位操作模板：c0m
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_C0M(type, bw)                                              \
static __xwlib_inline                                                           \
void xwbop_c0m__##type(type * x, type m)                                        \
{                                                                               \
        xwbop_c0m##bw((xwu##bw##_t *)x, (xwu##bw##_t)m);                        \
}

/**
 * @brief 定义位操作模板：x1m
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_X1M(type, bw)                                              \
static __xwlib_inline                                                           \
void xwbop_x1m__##type(type * x, type m)                                        \
{                                                                               \
        xwbop_x1m##bw((xwu##bw##_t *)x, (xwu##bw##_t)m);                        \
}

/**
 * @brief 定义位操作模板：rbit
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_RBIT(type, bw)                                             \
static __xwlib_inline                                                           \
type xwbop_rbit__##type(type x)                                                 \
{                                                                               \
        return (type)xwbop_rbit##bw((xwu##bw##_t)x);                            \
}

/**
 * @brief 定义位操作模板：re
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_RE(type, bw)                                               \
static __xwlib_inline                                                           \
type xwbop_re__##type(type x)                                                   \
{                                                                               \
        return (type)xwbop_re##bw((xwu##bw##_t)x);                              \
}

/**
 * @brief 定义位操作模板：ffs
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_FFS(type, bw)                                              \
static __xwlib_inline                                                           \
xwssq_t xwbop_ffs__##type(type x)                                               \
{                                                                               \
        return xwbop_ffs##bw((xwu##bw##_t)x);                                   \
}

/**
 * @brief 定义位操作模板：fls
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_FLS(type, bw)                                              \
static __xwlib_inline                                                           \
xwssq_t xwbop_fls__##type(type x)                                               \
{                                                                               \
        return xwbop_fls##bw((xwu##bw##_t)x);                                   \
}

/**
 * @brief 定义位操作模板：flz
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_FLZ(type, bw)                                              \
static __xwlib_inline                                                           \
xwssq_t xwbop_flz__##type(type x)                                               \
{                                                                               \
        return xwbop_flz##bw((xwu##bw##_t)x);                                   \
}

/**
 * @brief 定义位操作模板：ffz
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_FFZ(type, bw)                                              \
static __xwlib_inline                                                           \
xwssq_t xwbop_ffz__##type(type x)                                               \
{                                                                               \
        return xwbop_ffz##bw((xwu##bw##_t)x);                                   \
}

/**
 * @brief 定义位操作模板：weight
 * @param[in] type: 类型
 * @param[in] bw: 位宽
 */
#define DEFINE_XWBOP_WEIGHT(type, bw)                                           \
static __xwlib_inline                                                           \
xwsq_t xwbop_weight__##type(type x)                                             \
{                                                                               \
        return xwbop_weight##bw((xwu##bw##_t)x);                                \
}

/**
 * @brief 使用模板为类型定义所有的位操作函数
 * @param[in] type: 类型
 * @param[in] bw: 位宽
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
        DEFINE_XWBOP_FFZ(type, bw)                                              \
        DEFINE_XWBOP_WEIGHT(type, bw)

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

DEFINE_XWBOP(xwtk_t, 64)

/******** ******** 位图操作 ******** ********/
/**
 * @brief 声明位图
 * @param[in] name: 符号名
 * @param[in] bits: 位图中的位数
 */
#define xwbmpop_define(name, bits)  xwbmp_t name[BITS_TO_XWBMP_T(bits)]

/**
 * @brief 赋值操作数到位图
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] opd: 操作数
 * @param[in] num: 位图中总的位数
 */
void xwbmpop_assign(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num);

/**
 * @brief 从数组最高元素开始比较两个位图的数值大小
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] opd: 操作数
 * @param[in] num: 位图中总的位数
 * @return 比较结果
 * @retval 0: 相等
 * @retval <0: 小于
 * @retval >0: 大于
 */
xwssq_t xwbmpop_cmp(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num);

/**
 * @brief 将位图中所有位置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 */
void xwbmpop_s1all(xwbmp_t * bmp, xwsq_t num);

/**
 * @brief 将位图中所有位清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 */
void xwbmpop_c0all(xwbmp_t * bmp, xwsq_t num);

/**
 * @brief 将位图中单个位置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] n: 位的序号
 */
void xwbmpop_s1i(xwbmp_t * bmp, xwsq_t n);

/**
 * @brief 将位图中掩码部分置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 */
void xwbmpop_s1m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 将位图中单个位清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] n: 被清0的位的序号
 */
void xwbmpop_c0i(xwbmp_t * bmp, xwsq_t n);

/**
 * @brief 将位图中掩码部分清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 */
void xwbmpop_c0m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 将位图中单个位翻转
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] n: 被翻转的位的序号
 */
void xwbmpop_x1i(xwbmp_t * bmp, xwsq_t n);

/**
 * @brief 将位图中掩码部分翻转
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 */
void xwbmpop_x1m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 测试位图中的单个位是否为1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] n: 被测试的位的序号
 * @return 布尔值
 * @retval true: 置位
 * @retval false: 复位
 */
bool xwbmpop_t1i(xwbmp_t * bmp, xwsq_t n);

/**
 * @brief 测试位图中掩码部分是否全部为1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 * @return 布尔值
 * @retval true: 全部为1
 * @retval false: 至少一位为0
 */
bool xwbmpop_t1ma(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 测试位图中掩码部分是否全部为1，如果是，就将掩码部分全部清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 * @return 布尔值
 * @retval true: 全部为1
 * @retval false: 至少一位为0
 */
bool xwbmpop_t1ma_then_c0m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 测试位图中掩码部分是否至少有一位为1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 * @return 布尔值
 * @retval true: 至少一位为1
 * @retval false: 全部为0
 */
bool xwbmpop_t1mo(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 测试位图中掩码部分是否至少有一位为1，如果是，就将掩码部分全部清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 * @return 布尔值
 * @retval true: 至少一位为1
 * @retval false: 全部为0
 */
bool xwbmpop_t1mo_then_c0m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 测试位图中掩码部分是否全部为0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 * @return 布尔值
 * @retval true: 全部为0
 * @retval false: 至少一位为1
 */
bool xwbmpop_t0ma(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 测试位图中掩码部分是否全部为0，如果是，就将掩码部分全部置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 * @return 布尔值
 * @retval true: 全部为0
 * @retval false: 至少一位为1
 */
bool xwbmpop_t0ma_then_s1m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 测试位图中掩码部分是否至少有一位为0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 * @return 布尔值
 * @retval true: 至少一位为0
 * @retval false: 全部为1
 */
bool xwbmpop_t0mo(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 测试位图中掩码部分是否至少有一位为0，如果是，就将掩码部分全部置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] msk: 掩码
 * @param[in] num: 掩码的有效位数
 * @return 布尔值
 * @retval true: 至少一位为0
 * @retval false: 全部为1
 */
bool xwbmpop_t0mo_then_s1m(xwbmp_t * bmp, const xwbmp_t msk[], xwsz_t num);

/**
 * @brief 将位图按位取反
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 掩码的有效位数
 */
void xwbmpop_not(xwbmp_t * bmp, xwsz_t num);

/**
 * @brief 将位图与操作数进行逐位“与”运算
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] opd: 操作数
 * @param[in] num: 掩码的有效位数
 */
void xwbmpop_and(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num);

/**
 * @brief 将位图与操作数进行逐位“或”运算
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] opd: 操作数
 * @param[in] num: 掩码的有效位数
 */
void xwbmpop_or(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num);

/**
 * @brief 将位图与操作数进行逐位“异或”运算
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] opd: 操作数
 * @param[in] num: 掩码的有效位数
 */
void xwbmpop_xor(xwbmp_t * bmp, const xwbmp_t opd[], xwsz_t num);

/**
 * @brief 在位图中从最低位起查找第一个被置1的位
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中 `ffs()` 函数返回值不同。
 */
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num);

/**
 * @brief 在位图中从最低位起查找第一个被清0的位
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位被清0
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中 `ffs()` 函数返回值不同。
 */
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t num);

/**
 * @brief 在位图中从最高位起查找第一个被置1的位
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中 `ffs()` 函数返回值不同。
 */
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t num);

/**
 * @brief 在位图中从最高位起查找第一个被清0的位
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中 `ffs()` 函数返回值不同。
 */
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t num);

/**
 * @brief 在位图中统计被置1的位的个数
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 数据中1的个数
 */
xwsz_t xwbmpop_weight(xwbmp_t * bmp, xwsz_t num);

/**
 * @} xwos_lib_xwbop
 */

#endif /* xwos/lib/xwbop.h */
