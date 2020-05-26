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
#include <xwos/lib/xwbop_internal.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         macros & functions          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** 位操作 ******** ********/
/**
 * @brief XWOS BOPLIB：调用位操作函数模板
 * @param type: (I) 数据类型
 * @param op: (I) 位操作函数
 * @param ...: (I) 函数的参数
 */
#define xwbop(type, op, ...)            xwbop_##op##__##type(__VA_ARGS__)

/**
 * @brief XWOS BOPLIB：将数据掩码部分的位全部置1
 * @param type: (I) 数据类型
 * @param addr: (I) 数据的地址
 * @param mask: (I) 位的掩码
 */
#define xwbop_s1m(type, addr, mask)     xwbop(type, s1m, (addr), (mask))

/**
 * @brief XWOS BOPLIB：将数据掩码部分的位全部清0
 * @param type: (I) 数据类型
 * @param addr: (I) 数据的地址
 * @param mask: (I) 位的掩码
 */
#define xwbop_c0m(type, addr, mask)     xwbop(type, c0m, (addr), (mask))

/**
 * @brief XWOS BOPLIB：将数据掩码部分的位全部翻转
 * @param type: (I) 数据类型
 * @param addr: (I) 数据的地址
 * @param mask: (I) 位的掩码
 */
#define xwbop_x1m(type, addr, mask)     xwbop(type, x1m, (addr), (mask))

/**
 * @brief XWOS BOPLIB：将数据的位镜面翻转
 * @param type: (I) 数据类型
 * @param data: (I) 数据（注：非指针）
 * @return 镜面翻转后的结果
 */
#define xwbop_rbit(type, data)          xwbop(type, rbit, (data))

/**
 * @brief XWOS BOPLIB：将数据的大小端翻转
 * @param type: (I) 数据类型
 * @param data: (I) 数据（注：非指针）
 * @return 大小端翻转后的结果
 */
#define xwbop_re(type, data)            xwbop(type, re, (data))

/**
 * @brief XWOS BOPLIB：在数据中从最低位起查找第一个被置1的位
 * @param type: (I) 数据类型
 * @param data: (I) 数据（注：非指针）
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
#define xwbop_ffs(type, data)           xwbop(type, ffs, (data))

/**
 * @brief XWOS BOPLIB：在数据中从最高位起查找第一个被置1的位
 * @param type: (I) 数据类型
 * @param data: (I) 数据（注：非指针）
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_fls(type, data)           xwbop(type, fls, (data))

/**
 * @brief XWOS BOPLIB：在数据中从最低位起查找第一个被清0的位
 * @param type: (I) 数据类型
 * @param data: (I) 数据（注：非指针）
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_ffz(type, data)           xwbop(type, flz, (data))

/**
 * @brief XWOS BOPLIB：在数据中从最高位起查找第一个被清0的位
 * @param type: (I) 数据类型
 * @param data: (I) 数据（注：非指针）
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的。
 */
#define xwbop_flz(type, data)           xwbop(type, flz, (data))

/******** ******** 位操作模板 ******** ********/
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

/******** ******** 位图操作 ******** ********/
/**
 * @brief XWOS BOPLIB：声明位图
 * @param name: (I) 符号名
 * @param bits: (I) 位图中的位数
 */
#define xwbmpop_declare(name, bits)  xwbmp_t name[BITS_TO_BMPS(bits)]

/**
 * @brief XWOS BOPLIB：赋值操作数到位图
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 位图中总的位数
 */
__xwlib_code
void xwbmpop_assign(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：从最高字节开始比较两个位图的数值大小
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 位图中总的位数
 * @return 比较结果
 * @retval 0: 相等
 * @retval <0: 小于
 * @retval >0: 大于
 */
__xwlib_code
xwssq_t xwbmpop_cmp(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：将位图中所有位置1
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 */
__xwlib_code
void xwbmpop_s1all(xwbmp_t * bmp, xwsq_t num);

/**
 * @brief XWOS BOPLIB：将位图中所有位清0
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 */
__xwlib_code
void xwbmpop_c0all(xwbmp_t * bmp, xwsq_t num);

/**
 * @brief XWOS BOPLIB：将位图中某位置1
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 位的序号
 */
__xwlib_code
void xwbmpop_s1i(xwbmp_t * bmp, xwsq_t n);

/**
 * @brief XWOS BOPLIB：将位图中掩码部分置1
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：将位图中某位清0
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被清0的位的序号
 */
__xwlib_code
void xwbmpop_c0i(xwbmp_t * bmp, xwsq_t n);

/**
 * @brief XWOS BOPLIB：将位图中掩码部分清0
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：将位图中某位翻转
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被翻转的位的序号
 */
__xwlib_code
void xwbmpop_x1i(xwbmp_t * bmp, xwsq_t n);

/**
 * @brief XWOS BOPLIB：将位图中掩码部分翻转
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_x1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：测试位图中的某位是否为1
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被测试的位的序号
 * @return 布尔值
 * @retval true: 置位
 * @retval false: 复位
 */
__xwlib_code
bool xwbmpop_t1i(xwbmp_t * bmp, xwsq_t n);

/**
 * @brief XWOS BOPLIB：测试位图中掩码部分是否全部为1
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔值
 * @retval true: 全部为1
 * @retval false: 至少一位为0
 */
__xwlib_code
bool xwbmpop_t1ma(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：测试位图中掩码部分是否全部为1，如果是，就将掩码位全部清0。
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔值
 * @retval true: 全部为1
 * @retval false: 至少一位为0
 */
__xwlib_code
bool xwbmpop_t1ma_then_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：测试位图中掩码位是否至少有一位为1
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔值
 * @retval true: 至少一位为1
 * @retval false: 全部为0
 */
__xwlib_code
bool xwbmpop_t1mo(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：测试位图中掩码位是否至少有一位为1，如果是，就将掩码位全部清0。
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔值
 * @retval true: 至少一位为1
 * @retval false: 全部为0
 */
__xwlib_code
bool xwbmpop_t1mo_then_c0m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：测试位图中掩码位是否全部为0
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔值
 * @retval true: 全部为0
 * @retval false: 至少一位为1
 */
__xwlib_code
bool xwbmpop_t0ma(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：测试位图中掩码位是否全部为0，如果是，就将掩码位全部置1。
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔值
 * @retval true: 全部为0
 * @retval false: 至少一位为1
 */
__xwlib_code
bool xwbmpop_t0ma_then_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：测试位图中掩码位是否至少有一位为0
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔值
 * @retval true: 至少一位为0
 * @retval false: 全部为1
 */
__xwlib_code
bool xwbmpop_t0mo(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：测试位图中掩码位是否至少有一位为0，如果是，就将掩码位全部置1。
 * @param bmp: (I) 位图的起始地址指针
 * @param msk: (I) 掩码
 * @param num: (I) 掩码的有效位数
 * @return 布尔值
 * @retval true: 至少一位为0
 * @retval false: 全部为1
 */
__xwlib_code
bool xwbmpop_t0mo_then_s1m(xwbmp_t * bmp, xwbmp_t msk[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：将位图与操作数进行逐位“与”操作
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_and(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：将位图与操作数进行逐位“或”操作
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_or(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：将位图与操作数进行逐位“异或”操作
 * @param bmp: (I) 位图的起始地址指针
 * @param opd: (I) 操作数
 * @param num: (I) 掩码的有效位数
 */
__xwlib_code
void xwbmpop_xor(xwbmp_t * bmp, xwbmp_t opd[], xwsz_t num);

/**
 * @brief XWOS BOPLIB：在位图中从最低位起查找第一个被置1的位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
__xwlib_code
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS BOPLIB：在位图中从最低位起查找第一个被清0的位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位被清0
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
__xwlib_code
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS BOPLIB：在位图中从最高位起查找第一个被置1的位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为1
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
__xwlib_code
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS BOPLIB：在位图中从最高位起查找第一个被清0的位
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @return 位的序号
 * @retval >=0: 位的序号
 * @retval -1: 没有任何一个位为0
 * @note
 * - 返回的序号是从0开始编号的，与C标准库中ffs()函数返回值不同。
 */
__xwlib_code
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t num);

#endif /* xwos/lib/xwbop.h */
