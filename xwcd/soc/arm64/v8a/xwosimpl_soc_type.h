/**
 * @file
 * @brief XWOS移植实现层：SOC：基本类型
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwosimpl_soc_type_h__
#define __xwosimpl_soc_type_h__

#ifndef __xwos_ospl_soc_type_h__
#  error "This file should be included from <xwos/ospl/soc/type.h>."
#endif

#include <cfg/project.h>
#include <inttypes.h>
#include <stdbool.h>
#include <float.h>
#include <limits.h>

#define BITS_PER_CHAR           8U
#define CHAR_SHIFT              3U
#define BITS_PER_UCHAR          8U
#define UCHAR_SHIFT             3U

#define BITS_PER_SHRT           16U
#define SHRT_SHIFT              4U
#define BITS_PER_USHRT          16U
#define USHRT_SHIFT             4U

#define BITS_PER_INT            32U
#define INT_SHIFT               5U
#define BITS_PER_UINT           32U
#define UINT_SHIFT              5U

#define BITS_PER_LONG           32U
#define LONG_SHIFT              5U
#define BITS_PER_ULONG          32U
#define LONG_SHIFT              5U

#define BITS_PER_LLONG          64U
#define LLONG_SHIFT             6U
#define BITS_PER_ULLONG         64U
#define ULLONG_SHIFT            6U

#define BITS_PER_FLT            32U
#define FLT_SHIFT               5U

#define BITS_PER_DBL            64U
#define DBL_SHIFT               6U

#define BITS_PER_LDBL           128U
#define LDBL_SHIFT              7U

#define ARCH_HAVE_XWU8_T        1
typedef uint8_t xwu8_t; /**< 无符号8位整数 */
#define BITS_PER_XWU8_T         8U
#define XWU8_T_SHIFT            3U

#define ARCH_HAVE_XWS8_T        1
typedef int8_t xws8_t; /**< 有符号8位整数 */
#define BITS_PER_XWS8_T         8U
#define XWS8_T_SHIFT            3U

#define ARCH_HAVE_XWU16_T       1
typedef uint16_t xwu16_t; /**< 无符号16位整数 */
#define BITS_PER_XWU16_T        16U
#define XWU16_T_SHIFT           4U

#define ARCH_HAVE_XWS16_T       1
typedef int16_t xws16_t; /**< 有符号16位整数 */
#define BITS_PER_XWS16_T        16U
#define XWS16_T_SHIFT           4U

#define ARCH_HAVE_XWU32_T       1
typedef uint32_t xwu32_t; /**< 无符号32位整数 */
#define BITS_PER_XWU32_T        32U
#define XWU32_T_SHIFT           5U

#define ARCH_HAVE_XWS32_T       1
typedef int32_t xws32_t; /**< 有符号32位整数 */
#define BITS_PER_XWS32_T        32U
#define XWS32_T_SHIFT           5U

#define ARCH_HAVE_XWU64_T       1
typedef uint64_t xwu64_t; /**< 无符号64位整数 */
#define BITS_PER_XWU64_T        64U
#define XWU64_T_SHIFT           6U

#define ARCH_HAVE_XWS64_T       1
typedef int64_t xws64_t; /**< 有符号64位整数 */
#define BITS_PER_XWS64_T        64U
#define XWS64_T_SHIFT           6U

#define ARCH_HAVE_XWU128_T      1
typedef __uint128_t xwu128_t; /**< 无符号128位整数 */
#define ARCH_HAVE_ATOMIC_XWU128_T       1
typedef __xwcc_atomic xwu128_t atomic_xwu128_t;
#define BITS_PER_XWU128_T       128U
#define XWU128_T_SHIFT          7U
#define XWU128_MAX              ((xwu128_t)(~((xwu128_t)0)))
#define XWU128_MIN              ((xwu128_t)0)

#define ARCH_HAVE_XWS128_T      1
typedef __int128_t xws128_t; /**< 有符号128位整数 */
#define ARCH_HAVE_ATOMIC_XWS128_T       1
typedef __xwcc_atomic xws128_t atomic_xws128_t;
#define BITS_PER_XWS128_T       128U
#define XWS128_T_SHIFT          7U
#define XWS128_MAX              ((xws128_t)(XWU128_MAX >> 1))
#define XWS128_MIN              ((xws128_t)(-XWS128_MAX - 1))

#define ARCH_HAVE_XWSZ_T        1
typedef unsigned long xwsz_t; /**< 大小值 (无符号) */
#define BITS_PER_XWSZ_T         BITS_PER_ULONG
#define XWSZ_T_SHIFT            ULONG_SHIFT

#define ARCH_HAVE_XWSSZ_T       1
typedef long xwssz_t; /**< 大小值 (有符号) */
#define BITS_PER_XWSSZ_T        BITS_PER_LONG
#define XWSSZ_T_SHIFT           LONG_SHIFT

#define ARCH_HAVE_XWPTR_T       1
typedef xwu64_t xwptr_t; /**< 指针数值 */
#define BITS_PER_XWPTR_T        BITS_PER_XWU64_T
#define XWPTR_T_SHIFT           XWU64_T_SHIFT

#define ARCH_HAVE_XWSTK_T       1
typedef xwptr_t xwstk_t; /**< 栈槽 */
#define BITS_PER_XWSTK_T        BITS_PER_XWPTR_T
#define XWSTK_T_SHIFT           XWPTR_T_SHIFT

#define ARCH_HAVE_XWREG_T       1
typedef xwu64_t xwreg_t; /**< 寄存器数值 (无符号) */
#define BITS_PER_XWREG_T        BITS_PER_XWU64_T
#define XWREG_T_SHIFT           XWU64_T_SHIFT

#define ARCH_HAVE_XWSREG_T      1
typedef xws64_t xwsreg_t; /**< 寄存器数值 (有符号) */
#define BITS_PER_XWSREG_T       BITS_PER_XWS64_T
#define XWSREG_T_SHIFT          XWS64_T_SHIFT

#define ARCH_HAVE_XWSQ_T        1
typedef xwu64_t xwsq_t; /**< 顺序值 (无符号) */
#define BITS_PER_XWSQ_T         BITS_PER_XWU64_T
#define XWSQ_T_SHIFT            XWU64_T_SHIFT

#define ARCH_HAVE_XWSSQ_T       1
typedef xws64_t xwssq_t; /**< 顺序值 (有符号) */
#define BITS_PER_XWSSQ_T        BITS_PER_XWS64_T
#define XWSSQ_T_SHIFT           XWS64_T_SHIFT

#define ARCH_HAVE_XWID_T        1
typedef xwu64_t xwid_t; /**< ID (无符号) */
#define BITS_PER_XWID_T         BITS_PER_XWU64_T
#define XWID_T_SHIFT            XWU64_T_SHIFT

#define ARCH_HAVE_XWSID_T       1
typedef xws64_t xwsid_t; /**< ID (有符号) */
#define BITS_PER_XWSID_T        BITS_PER_XWS64_T
#define XWSID_T_SHIFT           XWS64_T_SHIFT

#define ARCH_HAVE_XWER_T        1
typedef xws64_t xwer_t; /**< 错误码 (有符号) */
#define BITS_PER_XWER_T         BITS_PER_XWS64_T
#define XWER_T_SHIFT            XWS64_T_SHIFT

#define ARCH_HAVE_XWBMP_T       1
typedef xwu64_t xwbmp_t; /**< 位图 (无符号) */
#define BITS_PER_XWBMP_T        BITS_PER_XWU64_T
#define XWBMP_T_SHIFT           XWU64_T_SHIFT

#endif /* xwcd/soc/arm64/v8a/xwosimpl_soc_type.h */
