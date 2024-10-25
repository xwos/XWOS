/**
 * @file
 * @brief XWOS通用库：基本类型定义
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_type_h__
#define __xwos_lib_type_h__

#include <cfg/project.h>

/**
 * @defgroup xwos_lib_type XWOS的基本类型
 * @ingroup xwos_lib
 * @{
 */

#include <xwos/ospl/soc/type.h>
#include <stdarg.h>

#ifndef NULL
#  define NULL ((void *)0)
#endif
#ifndef NIL
#  define NIL  NULL
#endif

#ifndef BITS_PER_CHAR
#  define BITS_PER_CHAR         8U
#endif
#ifndef CHAR_SHIFT
#  define CHAR_SHIFT            3U
#endif
#ifndef CHAR_MAX
#  define CHAR_MAX      ((signed char)(UCHAR_MAX >> (unsigned char)1))
#endif
#ifndef CHAR_MIN
#  define CHAR_MIN      ((signed char)(-CHAR_MAX - (signed char)1))
#endif

#ifndef BITS_PER_UCHAR
#  define BITS_PER_UCHAR        8U
#endif
#ifndef UCHAR_SHIFT
#  define UCHAR_SHIFT           3U
#endif
#ifndef UCHAR_MAX
#  define UCHAR_MAX     ((unsigned char)(~((unsigned char)0)))
#endif
#ifndef UCHAR_MIN
#  define UCHAR_MIN     ((unsigned char)0)
#endif


#ifndef BITS_PER_SHRT
#  define BITS_PER_SHRT         16U
#endif
#ifndef SHRT_SHIFT
#  define SHRT_SHIFT            4U
#endif
#ifndef SHRT_MAX
#  define SHRT_MAX      ((signed short)(USHRT_MAX >> (unsigned short)1))
#endif
#ifndef SHRT_MIN
#  define SHRT_MIN      ((signed short)(-SHRT_MAX - (signed short)1))
#endif

#ifndef BITS_PER_USHRT
#  define BITS_PER_USHRT        16U
#endif
#ifndef USHRT_SHIFT
#  define USHRT_SHIFT           4U
#endif
#ifndef USHRT_MAX
#  define USHRT_MAX     ((unsigned short)(~((unsigned short)0)))
#endif
#ifndef USHRT_MIN
#  define USHRT_MIN     (unsigned short)0)
#endif


#ifndef BITS_PER_INT
#  define BITS_PER_INT          32U
#endif
#ifndef INT_SHIFT
#  define INT_SHIFT             5U
#endif
#ifndef INT_MAX
#  define INT_MAX       ((signed int)(UINT_MAX >> (unsigned int)1))
#endif
#ifndef INT_MIN
#  define INT_MIN       ((signed int)(-INT_MAX - (signed int)1))
#endif

#ifndef BITS_PER_UINT
#  define BITS_PER_UINT         32U
#endif
#ifndef UINT_SHIFT
#  define UINT_SHIFT            5U
#endif
#ifndef UINT_MAX
#  define UINT_MAX      ((unsigned int)(~((unsigned int)0)))
#endif
#ifndef UINT_MIN
#  define UINT_MIN      (unsigned int)0)
#endif


#ifndef BITS_PER_LONG
#  define BITS_PER_LONG         32U
#endif
#ifndef LONG_SHIFT
#  define LONG_SHIFT            5U
#endif
#ifndef LONG_MAX
#  define LONG_MAX      ((signed long)(ULONG_MAX >> (unsigned long)1))
#endif
#ifndef LONG_MIN
#  define LONG_MIN      ((signed long)(-LONG_MAX - (signed long)1))
#endif

#ifndef BITS_PER_ULONG
#  define BITS_PER_ULONG        32U
#endif
#ifndef LONG_SHIFT
#  define LONG_SHIFT            5U
#endif
#ifndef ULONG_MAX
#  define ULONG_MAX     ((unsigned long)(~((unsigned long)0)))
#endif
#ifndef ULONG_MIN
#  define ULONG_MIN     ((unsigned long)0)
#endif


#ifndef BITS_PER_LLONG
#  define BITS_PER_LLONG        64U
#endif
#ifndef LLONG_SHIFT
#  define LLONG_SHIFT           6U
#endif
#ifndef LLONG_MAX
#  define LLONG_MAX     ((signed long long)(ULLONG_MAX >> (unsigned long long)1))
#endif
#ifndef LLONG_MIN
#  define LLONG_MIN     ((signed long long)(-LLONG_MAX - (signed long long)1))
#endif

#ifndef BITS_PER_ULLONG
#  define BITS_PER_ULLONG       64U
#endif
#ifndef ULLONG_SHIFT
#  define ULLONG_SHIFT          6U
#endif
#ifndef ULLONG_MAX
#  define ULLONG_MAX    ((unsigned long long)(~((unsigned long long)0)))
#endif
#ifndef ULLONG_MIN
#  define ULLONG_MIN    (unsigned long long)0)
#endif


#ifndef BITS_PER_FLT
#  define BITS_PER_FLT          32U
#endif
#ifndef FLT_SHIFT
#  define FLT_SHIFT             5U
#endif


#ifndef BITS_PER_DBL
#  define BITS_PER_DBL          64U
#endif
#ifndef DBL_SHIFT
#  define DBL_SHIFT             6U
#endif


#ifndef BITS_PER_LDBL
#  define BITS_PER_LDBL         128U
#endif
#ifndef LDBL_SHIFT
#  define LDBL_SHIFT            7U
#endif


#if !defined(ARCH_HAVE_XWU8_T) || defined(__DOXYGEN__)
typedef uint8_t xwu8_t; /**< 8位无符号整数 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWU8_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwu8_t atomic_xwu8_t; /**< 原子的8位无符号整数 */
#endif
#ifndef BITS_PER_XWU8_T
#  define BITS_PER_XWU8_T       8U
#endif
#ifndef XWU8_T_SHIFT
#  define XWU8_T_SHIFT          3U
#endif
#ifndef XWU8_MAX
#  define XWU8_MAX      ((xwu8_t)(~((xwu8_t)0)))
#endif

#if !defined(ARCH_HAVE_XWS8_T) || defined(__DOXYGEN__)
typedef int8_t xws8_t; /**< 8位有符号整数 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWS8_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xws8_t atomic_xws8_t; /**< 原子的8位有符号整数 */
#endif
#ifndef BITS_PER_XWS8_T
#  define BITS_PER_XWS8_T       8U
#endif
#ifndef XWS8_T_SHIFT
#  define XWS8_T_SHIFT          3U
#endif
#ifndef XWS8_MAX
#  define XWS8_MAX      ((xws8_t)(XWU8_MAX >> (xwu8_t)1))
#endif
#ifndef XWS8_MIN
#  define XWS8_MIN      ((xws8_t)(-XWS8_MAX - (xws8_t)1))
#endif


#if !defined(ARCH_HAVE_XWU16_T) || defined(__DOXYGEN__)
typedef uint16_t xwu16_t; /**< 16位无符号整数 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWU16_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwu16_t atomic_xwu16_t; /**< 原子的16位无符号整数 */
#endif
#ifndef BITS_PER_XWU16_T
#  define BITS_PER_XWU16_T      16U
#endif
#ifndef XWU16_T_SHIFT
#  define XWU16_T_SHIFT         4U
#endif
#ifndef XWU16_MAX
#  define XWU16_MAX     ((xwu16_t)(~((xwu16_t)0)))
#endif

#if !defined(ARCH_HAVE_XWS16_T) || defined(__DOXYGEN__)
typedef int16_t xws16_t; /**< 16位有符号整数 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWS16_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xws16_t atomic_xws16_t; /**< 原子的16位有符号整数 */
#endif
#ifndef BITS_PER_XWS16_T
#  define BITS_PER_XWS16_T      16U
#endif
#ifndef XWS16_T_SHIFT
#  define XWS16_T_SHIFT         4U
#endif
#ifndef XWS16_MAX
#  define XWS16_MAX     ((xws16_t)(XWU16_MAX >> (xwu16_t)1))
#endif
#ifndef XWS16_MIN
#  define XWS16_MIN     ((xws16_t)(-XWS16_MAX - (xws16_t)1))
#endif


#if !defined(ARCH_HAVE_XWU32_T) || defined(__DOXYGEN__)
typedef uint32_t xwu32_t; /**< 32位无符号整数 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWU32_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwu32_t atomic_xwu32_t; /**< 原子的32位无符号整数 */
#endif
#ifndef BITS_PER_XWU32_T
#  define BITS_PER_XWU32_T      32U
#endif
#ifndef XWU32_T_SHIFT
#  define XWU32_T_SHIFT         5U
#endif
#ifndef XWU32_MAX
#  define XWU32_MAX     ((xwu32_t)(~((xwu32_t)0)))
#endif


#if !defined(ARCH_HAVE_XWS32_T) || defined(__DOXYGEN__)
typedef int32_t xws32_t; /**< 32位有符号整数 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWS32_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xws32_t atomic_xws32_t; /**< 原子的32位有符号整数 */
#endif
#ifndef BITS_PER_XWS32_T
#  define BITS_PER_XWS32_T      32U
#endif
#ifndef XWS32_T_SHIFT
#  define XWS32_T_SHIFT         5U
#endif
#ifndef XWS32_MAX
#  define XWS32_MAX     ((xws32_t)(XWU32_MAX >> (xwu32_t)1))
#endif
#ifndef XWS32_MIN
#  define XWS32_MIN     ((xws32_t)(-XWS32_MAX - (xws32_t)1))
#endif


#if !defined(ARCH_HAVE_XWU64_T) || defined(__DOXYGEN__)
typedef uint64_t xwu64_t; /**< 64位无符号整数 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWU64_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwu64_t atomic_xwu64_t; /**< 原子的64位无符号整数 */
#endif
#ifndef BITS_PER_XWU64_T
#  define BITS_PER_XWU64_T      64U
#endif
#ifndef XWU64_T_SHIFT
#  define XWU64_T_SHIFT         6U
#endif
#ifndef XWU64_MAX
#  define XWU64_MAX     ((xwu64_t)(~((xwu64_t)0)))
#endif

#if !defined(ARCH_HAVE_XWS64_T) || defined(__DOXYGEN__)
typedef int64_t xws64_t; /**< 64位有符号整数 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWS64_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xws64_t atomic_xws64_t; /**< 原子的64位有符号整数 */
#endif
#ifndef BITS_PER_XWS64_T
#  define BITS_PER_XWS64_T      64U
#endif
#ifndef XWS64_T_SHIFT
#  define XWS64_T_SHIFT         6U
#endif
#ifndef XWS64_MAX
#  define XWS64_MAX     ((xws64_t)(XWU64_MAX >> (xwu64_t)1))
#endif
#ifndef XWS64_MIN
#  define XWS64_MIN     ((xws64_t)(-XWS64_MAX - (xws64_t)1))
#endif


#if !defined(ARCH_HAVE_XWSZ_T) || defined(__DOXYGEN__)
typedef unsigned long xwsz_t; /**< 大小值 (无符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWSZ_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwsz_t atomic_xwsz_t; /**< 大小值 (无符号，原子的) */
#endif
#ifndef BITS_PER_XWSZ_T
#  define BITS_PER_XWSZ_T       BITS_PER_ULONG
#endif
#ifndef XWSZ_T_SHIFT
#  define XWSZ_T_SHIFT          ULONG_SHIFT
#endif
#ifndef XWSZ_MAX
#  define XWSZ_MAX      ((xwsz_t)(~((xwsz_t)0)))
#endif

#if !defined(ARCH_HAVE_XWSSZ_T) || defined(__DOXYGEN__)
typedef signed long xwssz_t; /**< 大小值 (有符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWSSZ_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwssz_t atomic_xwssz_t; /**< 大小值 (有符号，原子的) */
#endif
#ifndef BITS_PER_XWSSZ_T
#  define BITS_PER_XWSSZ_T      BITS_PER_LONG
#endif
#ifndef XWSSZ_T_SHIFT
#  define XWSSZ_T_SHIFT         LONG_SHIFT
#endif
#ifndef XWSSZ_MAX
#  define XWSSZ_MAX     ((xwssz_t)(XWSZ_MAX >> (xwsz_t)1))
#endif
#ifndef XWSSZ_MIN
#  define XWSSZ_MIN     ((xwssz_t)(-XWSSZ_MAX - (xwssz_t)1))
#endif


#if !defined(ARCH_HAVE_XWPTR_T) || defined(__DOXYGEN__)
typedef unsigned long xwptr_t; /**< 指针数值 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWPTR_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwptr_t atomic_xwptr_t; /**< 指针数值 (原子的) */
#endif
#ifndef BITS_PER_XWPTR_T
#  define BITS_PER_XWPTR_T      BITS_PER_ULONG
#endif
#ifndef XWPTR_T_SHIFT
#  define XWPTR_T_SHIFT         ULONG_SHIFT
#endif
#ifndef XWPTR_MAX
#  define XWPTR_MAX     ((xwptr_t)(~(xwptr_t)0))
#endif


#if !defined(ARCH_HAVE_XWSTK_T) || defined(__DOXYGEN__)
typedef xwptr_t xwstk_t; /**< 栈槽 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWSTK_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwstk_t atomic_xwstk_t; /**< 栈槽 (原子的) */
#endif
#ifndef BITS_PER_XWSTK_T
#  define BITS_PER_XWSTK_T      BITS_PER_XWPTR_T
#endif
#ifndef XWSTK_T_SHIFT
#  define XWSTK_T_SHIFT         XWPTR_T_SHIFT
#endif
#ifndef XWSTK_MAX
#  define XWSTK_MAX     ((xwstk_t)(~(xwstk_t)0))
#endif


#if !defined(ARCH_HAVE_XWREG_T) || defined(__DOXYGEN__)
typedef xwptr_t xwreg_t; /**< 寄存器数值 (无符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWREG_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwreg_t atomic_xwreg_t; /**< 寄存器数值 (无符号，原子的) */
#endif
#ifndef BITS_PER_XWREG_T
#  define BITS_PER_XWREG_T      BITS_PER_XWPTR_T
#endif
#ifndef XWREG_T_SHIFT
#  define XWREG_T_SHIFT         XWPTR_T_SHIFT
#endif
#ifndef XWREG_MAX
#  define XWREG_MAX     ((xwreg_t)(~((xwreg_t)0)))
#endif

#if !defined(ARCH_HAVE_XWSREG_T) || defined(__DOXYGEN__)
typedef ptrdiff_t xwsreg_t; /**< 寄存器数值 (有符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWSREG_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwsreg_t atomic_xwsreg_t; /**< 寄存器数值 (有符号，原子的) */
#endif
#ifndef BITS_PER_XWSREG_T
#  define BITS_PER_XWSREG_T     BITS_PER_XWREG_T
#endif
#ifndef XWSREG_T_SHIFT
#  define XWSREG_T_SHIFT        XWREG_T_SHIFT
#endif
#ifndef XWSREG_MAX
#  define XWSREG_MAX    ((xwsreg_t)(XWREG_MAX >> (xwreg_t)1))
#endif
#ifndef XWSREG_MIN
#  define XWSREG_MIN    ((xwsreg_t)(-XWSREG_MAX - (xwsreg_t)1))
#endif


#if !defined(ARCH_HAVE_XWSQ_T) || defined(__DOXYGEN__)
typedef unsigned long xwsq_t; /**< 顺序值 (无符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWSQ_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwsq_t atomic_xwsq_t; /**< 顺序值 (无符号，原子的) */
#endif
#ifndef BITS_PER_XWSQ_T
#  define BITS_PER_XWSQ_T       BITS_PER_ULONG
#endif
#ifndef XWSQ_T_SHIFT
#  define XWSQ_T_SHIFT          ULONG_SHIFT
#endif
#ifndef XWSQ_MAX
#  define XWSQ_MAX      ((xwsq_t)(~((xwsq_t)0)))
#endif

#if !defined(ARCH_HAVE_XWSSQ_T) || defined(__DOXYGEN__)
typedef signed long xwssq_t; /**< 顺序值 (有符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWSSQ_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwssq_t atomic_xwssq_t; /**< 顺序值 (有符号，原子的) */
#endif
#ifndef BITS_PER_XWSSQ_T
#  define BITS_PER_XWSSQ_T      BITS_PER_LONG
#endif
#ifndef XWSSQ_T_SHIFT
#  define XWSSQ_T_SHIFT         LONG_SHIFT
#endif
#ifndef XWSSQ_MAX
#  define XWSSQ_MAX     ((xwssq_t)(XWSQ_MAX >> (xwsq_t)1))
#endif
#ifndef XWSSQ_MIN
#  define XWSSQ_MIN     ((xwssq_t)(-XWSSQ_MAX - (xwssq_t)1))
#endif


#if !defined(ARCH_HAVE_XWID_T) || defined(__DOXYGEN__)
typedef unsigned long xwid_t; /**< ID (无符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWID_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwid_t atomic_xwid_t; /**< ID (无符号，原子的) */
#endif
#ifndef BITS_PER_XWID_T
#  define BITS_PER_XWID_T       BITS_PER_ULONG
#endif
#ifndef XWID_T_SHIFT
#  define XWID_T_SHIFT          ULONG_SHIFT
#endif
#ifndef XWID_MAX
#  define XWID_MAX      ((xwid_t)(~((xwid_t)0)))
#endif

#if !defined(ARCH_HAVE_XWSID_T) || defined(__DOXYGEN__)
typedef signed long xwsid_t; /**< ID (有符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWSID_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwsid_t atomic_xwsid_t; /**< ID (有符号，原子的) */
#endif
#ifndef BITS_PER_XWSID_T
#  define BITS_PER_XWSID_T      BITS_PER_LONG
#endif
#ifndef XWSID_T_SHIFT
#  define XWSID_T_SHIFT         LONG_SHIFT
#endif
#ifndef XWSID_MAX
#  define XWSID_MAX     ((xwsid_t)(XWID_MAX >> (xwid_t)1))
#endif
#ifndef XWSID_MIN
#  define XWSID_MIN     ((xwsid_t)(-XWSID_MAX - (xwsid_t)1))
#endif


#if !defined(ARCH_HAVE_XWIRQ_T) || defined(__DOXYGEN__)
typedef signed int xwirq_t; /**< 中断号 (有符号) */
#endif
#ifndef BITS_PER_XWIRQ_T
#  define BITS_PER_XWIRQ_T      BITS_PER_INT
#endif
#ifndef XWIRQ_T_SHIFT
#  define XWIRQ_T_SHIFT         INT_SHIFT
#endif
#ifndef XWIRQ_MAX
#  define XWIRQ_MAX     ((xwirq_t)((~(unsigned int)0) >> (unsigned int)1))
#endif
#ifndef XWIRQ_MIN
#  define XWIRQ_MIN     ((xwirq_t)(-XWIRQ_MAX - (xwirq_t)1))
#endif


#if !defined(ARCH_HAVE_XWPR_T) || defined(__DOXYGEN__)
typedef xws32_t xwpr_t; /**< 优先级 (有符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWPR_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwpr_t atomic_xwpr_t; /**< 优先级 (有符号，原子的) */
#endif
#ifndef BITS_PER_XWPR_T
#  define BITS_PER_XWPR_T       BITS_PER_XWS32_T
#endif
#ifndef XWPR_T_SHIFT
#  define XWPR_T_SHIFT          XWS32_T_SHIFT
#endif
#ifndef XWPR_MAX
#  define XWPR_MAX      ((xwpr_t)((~(xwu32_t)0) >> (xwu32_t)1))
#endif
#ifndef XWPR_MIN
#  define XWPR_MIN      ((xwpr_t)(-XWPR_MAX - (xwpr_t)1))
#endif


#if !defined(ARCH_HAVE_XWER_T) || defined(__DOXYGEN__)
typedef signed long xwer_t; /**< 错误码 (有符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWER_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwer_t atomic_xwer_t; /**< 错误码 (有符号，原子的) */
#endif
#ifndef BITS_PER_XWER_T
#  define BITS_PER_XWER_T       BITS_PER_LONG
#endif
#ifndef XWER_T_SHIFT
#  define XWER_T_SHIFT          LONG_SHIFT
#endif
#ifndef XWER_MAX
#  define XWER_MAX      ((xwer_t)OK)
#endif
#ifndef XWER_MIN
#  define XWER_MIN      ((xwer_t)(-MAX_ERRNO))
#endif


#if !defined(ARCH_HAVE_XWBMP_T) || defined(__DOXYGEN__)
typedef xwu32_t xwbmp_t; /**< 位图 (无符号) */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWBMP_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwbmp_t atomic_xwbmp_t; /**< 位图 (无符号，原子的) */
#endif
#ifndef BITS_PER_XWBMP_T
#  define BITS_PER_XWBMP_T      BITS_PER_XWU32_T
#endif
#ifndef XWBMP_T_SHIFT
#  define XWBMP_T_SHIFT         XWU32_T_SHIFT
#endif
#ifndef XWBMP_MAX
#  define XWBMP_MAX     ((xwbmp_t)(~((xwbmp_t)0)))
#endif


#if !defined(ARCH_HAVE_XWLFQ_T) || defined(__DOXYGEN__)
typedef xwptr_t xwlfq_t; /**< 无锁队列 */
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWLFQ_T) || defined(__DOXYGEN__)
typedef __xwcc_atomic xwlfq_t atomic_xwlfq_t; /**< 无锁队列 (原子的) */
#endif
#ifndef BITS_PER_XWLFQ_T
#  define BITS_PER_XWLFQ_T      BITS_PER_XWPTR_T
#endif
#ifndef XWLFQ_T_SHIFT
#  define XWLFQ_T_SHIFT         XWPTR_T_SHIFT
#endif


typedef void (* ctor_f)(void * /*obj*/); /**< 构造函数 */
typedef void (* dtor_f)(void * /*obj*/); /**< 析构函数 */
typedef void (* xwisr_f)(void); /**< 中断向量 */
typedef xws64_t (* xwsc_f)(void * /*arg*/, ...); /**< 系统调用 */

/**
 * @brief 原子变量测试函数指针类型
 */
typedef xwer_t (* xwaop_tst_f)(const void * /* ov */ ,
                               void * /* args */);

/**
 * @brief 原子变量操作函数指针类型
 */
typedef void (* xwaop_op_f)(void * /* nv */,
                            const void * /* ov */,
                            void * /* args */);

/**
 * @brief 原子操作内存模型
 * @details
 * 对应于 `stdatomic.h` 中定义的六种内存模型。
 */
enum xwaop_memory_order_em {
        xwaop_mo_relaxed = 0,
        xwaop_mo_consume,
        xwaop_mo_acquire,
        xwaop_mo_release,
        xwaop_mo_acq_rel,
        xwaop_mo_seq_cst,
};

/**
 * @brief XWOS的锁类型枚举
 */
enum xwos_lock_type_em {
        XWOS_LK_NONE = 0U, /**< 无 */
        XWOS_LK_MTX = 1U, /**< 互斥锁 */
        XWOS_LK_SPLK = 2U, /**< 自旋锁 */
        XWOS_LK_SQLK_WR = 3U, /**< 顺序写锁 */
        XWOS_LK_SQLK_RDEX = 4U, /**< 顺序读锁 */
        XWOS_LK_CALLBACK = 5U, /**< 抽象回调锁 */
        XWOS_LK_NUM, /**< 锁类型的数量 */
};

/**
 * @brief 类型为XWOS_LK_CALLBACK时的抽象锁函数
 */
struct xwos_cblk {
        xwer_t (* lock)(void * /*arg*/); /**< 加锁函数 */
        xwer_t (* unlock)(void * /*arg*/); /**< 解锁函数*/
};

/**
 * @brief XWOS锁状态
 */
enum xwos_lkst_em {
        XWOS_LKST_UNLOCKED = 0U, /**< 未锁定 */
        XWOS_LKST_LOCKED = 1U, /**< 锁定 */
};

struct xwos_mtx;
struct xwos_splk;
struct xwos_sqlk;
struct xwmp_mtx;
struct xwmp_splk;
struct xwmp_sqlk;
struct xwmp_mtx;
struct xwmp_splk;
struct xwmp_sqlk;

/**
 * @brief 所有锁的联合
 * @note
 * - 此联合只是一个指针数值，使用联合是为了避免强制类型转换。
 */
union xwos_ulock {
        union {
                struct xwmp_mtx * mtx; /**< 互斥锁 */
                struct xwmp_splk * splk; /**< 自旋锁 */
                struct xwmp_sqlk * sqlk; /**< 顺序锁 */
        } xwmp;
        union {
                struct xwup_mtx * mtx; /**< 互斥锁 */
                struct xwup_splk * splk; /**< 自旋锁 */
                struct xwup_sqlk * sqlk; /**< 顺序锁 */
        } xwup;
        union {
                struct xwos_mtx * mtx; /**< 互斥锁 */
                struct xwos_splk * splk; /**< 自旋锁 */
                struct xwos_sqlk * sqlk; /**< 顺序锁 */
        } osal;
        struct xwos_cblk * cb; /**< 回调锁 */
        void * anon; /**< 匿名指针 */
        xwptr_t ptr; /**< 指针值 */
};

/**
 * @defgroup xwos_lib_type_time XWOS的时间类型
 * @ingroup xwos_lib_type
 * @{
 */
#if !defined(ARCH_HAVE_XWTK_T) || defined(__DOXYGEN__)
/**
 * @brief XWOS系统滴答计数 (无符号)
 */
typedef xwu64_t xwtk_t;
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWTK_T) || defined(__DOXYGEN__)
/**
 * @brief XWOS系统滴答计数 (无符号，原子的)
 */
typedef __xwcc_atomic xwtk_t atomic_xwtk_t;
#endif

#ifndef XWTK_MAX
/**
 * @brief `xwtk_t` 的最大值
 */
#  define XWTK_MAX      ((xwtk_t)((~(xwu64_t)0)))
#endif
#ifndef XWTK_MIN
/**
 * @brief `xwtk_t` 的最小值
 */
#  define XWTK_MIN      ((xwtk_t)0)
#endif
#ifndef BITS_PER_XWTK_T
#  define BITS_PER_XWTK_T       BITS_PER_XWU64_T
#endif
#ifndef XWTK_T_SHIFT
#  define XWTK_T_SHIFT          XWU64_T_SHIFT
#endif

#if !defined(ARCH_HAVE_XWTM_T) || defined(__DOXYGEN__)
/**
 * @brief XWOS系统时间 (有符号)
 */
typedef xws64_t xwtm_t;
#endif
#if !defined(ARCH_HAVE_ATOMIC_XWTM_T) || defined(__DOXYGEN__)
/**
 * @brief XWOS系统时间 (有符号，原子的)
 */
typedef __xwcc_atomic xwtm_t atomic_xwtm_t;
#endif

#ifndef XWTM_MAX
/**
 * @brief `xwtm_t` 的最大值
 */
#  define XWTM_MAX      ((xwtm_t)((~(xwu64_t)0) >> 1))
#endif
#ifndef XWTM_MIN
/**
 * @brief `xwtm_t` 的最小值
 */
#  define XWTM_MIN      ((xwtm_t)(-XWTM_MAX - (xwtm_t)1))
#endif
#ifndef BITS_PER_XWTM_T
#  define BITS_PER_XWTM_T       BITS_PER_XWS64_T
#endif
#ifndef XWTM_T_SHIFT
#  define XWTM_T_SHIFT          XWS64_T_SHIFT
#endif

/**
 * @brief 系统时间的单位：纳秒
 * @param[in] ns: 纳秒
 */
#define XWTM_NS(ns)     (ns)

/**
 * @brief 系统时间的单位：微秒
 * @param[in] us: 微秒
 */
#define XWTM_US(us)     ((us) * 1000LL)

/**
 * @brief 系统时间的单位：毫秒
 * @param[in] ms: 毫秒
 */
#define XWTM_MS(ms)     ((ms) * 1000000LL)

/**
 * @brief 系统时间的单位：秒
 * @param[in] s: 秒
 */
#define XWTM_S(s)       ((s) * 1000000000LL)

/**
 * @brief 系统时间的单位：分
 * @param[in] m: 分
 */
#define XWTM_M(m)       ((m) * XWTM_S(60))

/**
 * @brief 系统时间的单位：时
 * @param[in] h: 时
 */
#define XWTM_H(h)       ((h) * XWTM_M(60))

/**
 * @brief 系统时间的单位：天
 * @param[in] d: 天
 */
#define XWTM_D(d)       ((d) * XWTM_H(24))

#if !defined(ARCH_HAVE_XWTM_T)
/**
 * @brief 将两个系统时间相加，并检查溢出
 * @param[in] a: 加数a
 * @param[in] b: 加数b
 * @return 结果
 * @note
 * - 如果结果已经溢出，返回xwtm_t的最大值。
 */
static __xwcc_inline
xwtm_t xwtm_add_safely(const xwtm_t a, const xwtm_t b)
{
        xwtm_t res;

        res = a + b;
        if (res < 0) {
                res = XWTM_MAX;
        }
        return res;
}

/**
 * @brief 将两个系统时间相加
 * @param[in] a: 加数a
 * @param[in] b: 加数b
 * @return 结果
 */
static __xwcc_inline
xwtm_t xwtm_add(const xwtm_t a, const xwtm_t b)
{
        return a + b;
}

/**
 * @brief 将两个系统时间相减
 * @param[in] a: 被减数
 * @param[in] b: 减数
 * @return 结果
 */
static __xwcc_inline
xwtm_t xwtm_sub(const xwtm_t a, const xwtm_t b)
{
        return a - b;
}

/**
 * @brief 比较两个系统时间
 * @param[in] a: 系统时间a
 * @param[in] b: 系统时间b
 * @return 结果
 * @retval <0: a < b
 * @retval ==0: a == b
 * @retval >0: a > b
 */
static __xwcc_inline
xwer_t xwtm_cmp(const xwtm_t a, const xwtm_t b)
{
        xwer_t rc;

        if (a > b) {
                rc = 1;
        } else if (a < b) {
                rc = -1;
        } else {
                rc = 0;
        }
        return rc;
}

/**
 * @brief 返回以 **纳秒** 为单位的系统时间
 * @param[in] ns: 纳秒
 * @return 系统时间
 */
static __xwcc_inline
xwtm_t xwtm_ns(xwtm_t ns)
{
        return XWTM_NS(ns);
}

/**
 * @brief 返回以 **微秒** 为单位的系统时间
 * @param[in] us: 微秒
 * @return 系统时间
 */
static __xwcc_inline
xwtm_t xwtm_us(xwtm_t us)
{
        return XWTM_US(us);
}

/**
 * @brief 返回以 **毫秒** 为单位的系统时间
 * @param[in] ms: 毫秒
 * @return 系统时间
 */
static __xwcc_inline
xwtm_t xwtm_ms(xwtm_t ms)
{
        return XWTM_MS(ms);
}

/**
 * @brief 返回以 **秒** 为单位的系统时间
 * @param[in] s: 秒
 * @return 系统时间
 */
static __xwcc_inline
xwtm_t xwtm_s(xwtm_t s)
{
        return XWTM_S(s);
}

/**
 * @brief 返回以 **分** 为单位的系统时间
 * @param[in] m: 分
 * @return 系统时间
 */
static __xwcc_inline
xwtm_t xwtm_m(xwtm_t m)
{
        return XWTM_M(m);
}

/**
 * @brief 返回以 **时** 为单位的系统时间
 * @param[in] h: 时
 * @return 系统时间
 */
static __xwcc_inline
xwtm_t xwtm_h(xwtm_t h)
{
        return XWTM_H(h);
}

/**
 * @brief 返回以 **天** 为单位的系统时间
 * @param[in] d: 天
 * @return 系统时间
 */
static __xwcc_inline
xwtm_t xwtm_d(xwtm_t d)
{
        return XWTM_D(d);
}
#endif

/** @} */ // xwos_lib_type_time
/** @} */ // xwos_lib_type

#endif /* xwos/lib/type.h */
