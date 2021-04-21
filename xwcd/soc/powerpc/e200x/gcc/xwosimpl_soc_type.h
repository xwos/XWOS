/**
 * @file
 * @brief 玄武OS移植实现层：SOC基本类型
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

#ifndef __xwosimpl_soc_type_h__
#define __xwosimpl_soc_type_h__

#ifndef __xwos_ospl_soc_type_h__
  #error "This file should be included from <xwos/ospl/soc/type.h>."
#endif

#include <inttypes.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

typedef uint8_t arch_uint8_t;
typedef int8_t arch_int8_t;
typedef uint16_t arch_uint16_t;
typedef int16_t arch_int16_t;
typedef uint32_t arch_uint32_t;
typedef int32_t arch_int32_t;
typedef uint64_t arch_uint64_t;
typedef int64_t arch_int64_t;

/* Use 32-bit data on PowerPC because of atomic operations */
#define ARCH_HAVE_XWU8_T        1
typedef uint8_t xwu8_t;

#define ARCH_HAVE_XWS8_T        1
typedef int8_t xws8_t;

#define ARCH_HAVE_XWU16_T       1
typedef uint16_t xwu16_t;

#define ARCH_HAVE_XWS16_T       1
typedef int16_t xws16_t;

#define ARCH_HAVE_XWU32_T       1
typedef uint32_t xwu32_t;

#define ARCH_HAVE_XWS32_T       1
typedef int32_t xws32_t;

#define ARCH_HAVE_XWU64_T       1
typedef uint64_t xwu64_t;

#define ARCH_HAVE_XWS64_T       1
typedef int64_t xws64_t;

#define ARCH_HAVE_XWSZ_T        1
typedef unsigned long xwsz_t; /**< 大小值 (无符号) */

#define ARCH_HAVE_XWSSZ_T       1
typedef long xwssz_t; /**< 大小值 (有符号) */

#define ARCH_HAVE_XWSTK_T       1
typedef unsigned long xwstk_t; /**< 栈槽 */

#define ARCH_HAVE_XWPTR_T       1
typedef unsigned long xwptr_t; /**< 指针数值 */

#define ARCH_HAVE_XWREG_T       1
typedef unsigned long xwreg_t; /**< 寄存器数值 (无符号) */

#define ARCH_HAVE_XWSREG_T      1
typedef signed long xwsreg_t; /**< 寄存器数值 (有符号) */

#define ARCH_HAVE_XWSQ_T        1
typedef unsigned long xwsq_t; /**< 顺序值 (无符号) */

#define ARCH_HAVE_XWSSQ_T       1
typedef signed long xwssq_t; /**< 顺序值 (有符号) */

#define ARCH_HAVE_XWID_T        1
typedef unsigned long xwid_t; /**< ID (无符号) */

#define ARCH_HAVE_XWSID_T       1
typedef signed long xwsid_t; /**< ID (有符号) */

#define ARCH_HAVE_XWISR_F       1
typedef void (* xwisr_f)(void); /**< 中断向量 */

#define ARCH_HAVE_XWIRQ_T       1
typedef signed int xwirq_t; /**< 中断号 (有符号) */

#define ARCH_HAVE_XWPR_T        1
typedef signed int xwpr_t; /**< 优先级 (有符号) */

#ifndef UCHAR_MAX
  #define UCHAR_MAX             ((unsigned char)(~((unsigned char)0)))
#endif
#ifndef CHAR_MAX
  #define CHAR_MAX              ((signed char)(UCHAR_MAX >> 1))
#endif
#ifndef CHAR_MIN
  #define CHAR_MIN              ((signed char)(-CHAR_MAX - 1))
#endif

#ifndef USHRT_MAX
  #define USHRT_MAX             ((unsigned short)(~((unsigned short)0)))
#endif
#ifndef SHRT_MAX
  #define SHRT_MAX              ((signed short)(USHRT_MAX>>1))
#endif
#ifndef SHRT_MIN
  #define SHRT_MIN              ((signed short)(-SHRT_MAX - 1))
#endif

#ifndef UINT_MAX
  #define UINT_MAX              ((unsigned int)(~((unsigned int)0)))
#endif
#ifndef INT_MAX
  #define INT_MAX               ((signed int)(UINT_MAX >> 1))
#endif
#ifndef INT_MIN
  #define INT_MIN               ((signed int)(-INT_MAX - 1))
#endif

#ifndef ULONG_MAX
  #define ULONG_MAX             ((unsigned long)(~((unsigned long)0)))
#endif
#ifndef LONG_MAX
  #define LONG_MAX              ((signed long)(ULONG_MAX >> 1))
#endif
#ifndef LONG_MIN
  #define LONG_MIN              ((signed long)(-LONG_MAX - 1))
#endif

#ifndef ULLONG_MAX
  #define ULLONG_MAX            ((unsigned long long)(~((unsigned long long)0)))
#endif
#ifndef LLONG_MAX
  #define LLONG_MAX             ((signed long long)(ULLONG_MAX >> 1))
#endif
#ifndef LLONG_MIN
  #define LLONG_MIN             ((signed long long)(-LLONG_MAX - 1))
#endif

#define XWU8_MAX                ((xwu8_t)(~((xwu8_t)0)))
#define XWS8_MAX                ((xws8_t)(XWU8_MAX >> 1))
#define XWS8_MIN                ((xws8_t)(-XWS8_MAX - 1))

#define XWU16_MAX               ((xwu16_t)(~((xwu16_t)0)))
#define XWS16_MAX               ((xws16_t)(XWU16_MAX >> 1))
#define XWS16_MIN               ((xws16_t)(-XWS16_MAX - 1))

#define XWU32_MAX               ((xwu32_t)(~((xwu32_t)0)))
#define XWS32_MAX               ((xws32_t)(XWU32_MAX >> 1))
#define XWS32_MIN               ((xws32_t)(-XWS32_MAX - 1))

#define XWU64_MAX               ((xwu64_t)(~((xwu64_t)0)))
#define XWS64_MAX               ((xws64_t)(XWU64_MAX >> 1))
#define XWS64_MIN               ((xws64_t)(-XWS64_MAX - 1))

#ifndef XWSZ_MAX
  #define XWSZ_MAX              ((xwsz_t)(~((xwsz_t)0)))
#endif
#ifndef XWSSZ_MAX
  #define XWSSZ_MAX             ((xwssz_t)(XWSZ_MAX >> 1))
#endif
#ifndef XWSSZ_MIN
  #define XWSSZ_MIN             ((xwssz_t)(-XWSSZ_MAX - 1))
#endif

#ifndef XWPTR_MAX
  #define XWPTR_MAX             ((xwptr_t)(~0UL))
#endif

#ifndef XWREG_MAX
  #define XWREG_MAX             ((xwreg_t)(~((xwreg_t)0)))
#endif
#ifndef XWSREG_MAX
  #define XWSREG_MAX            ((xwsreg_t)(REG_MAX >> 1))
#endif
#ifndef XWSREG_MIN
  #define XWSREG_MIN            ((xwsreg_t)(-XWSREG_MAX - 1))
#endif

#ifndef XWSQ_MAX
  #define XWSQ_MAX              ((xwsq_t)(~((xwsq_t)0)))
#endif
#ifndef XWSSQ_MAX
  #define XWSSQ_MAX             ((xwssq_t)(XWSQ_MAX >> 1))
#endif
#ifndef XWSSQ_MIN
  #define XWSSQ_MIN             ((xwssq_t)(-XWSSQ_MAX - 1))
#endif

#ifndef XWID_MAX
  #define XWID_MAX              ((xwid_t)(~((xwid_t)0)))
#endif
#ifndef XWSID_MAX
  #define XWSID_MAX             ((xwsid_t)(XWID_MAX >> 1))
#endif
#ifndef XWSID_MIN
  #define XWSID_MIN             ((xwsid_t)(-XWSID_MAX - 1))
#endif

#ifndef XWIRQ_MAX
  #define XWIRQ_MAX             ((xwirq_t)((~0UL) >> 1))
#endif
#ifndef XWIRQ_MIN
  #define XWIRQ_MIN             ((xwirq_t)(-XWIRQ_MAX - 1))
#endif

#ifndef XWPR_MAX
  #define XWPR_MAX              ((xwpr_t)((~0UL) >> 1))
#endif
#ifndef XWPR_MIN
  #define XWPR_MIN              ((xwpr_t)(-XWPR_MAX - 1))
#endif

#define XWU8_T__SIZE
#define BITS_PER_XWU8_T         8
#define XWU8_T_SHIFT            3
#define XWS8_T__SIZE
#define BITS_PER_XWS8_T         8
#define XWS8_T_SHIFT            3

#define XWU16_T__SIZE
#define BITS_PER_XWU16_T        16
#define XWU16_T_SHIFT           4
#define XWS16_T__SIZE
#define BITS_PER_XWS16_T        16
#define XWS16_T_SHIFT           4

#define XWU32_T__SIZE
#define BITS_PER_XWU32_T        32
#define XWU32_T_SHIFT           5
#define XWS32_T__SIZE
#define BITS_PER_XWS32_T        32
#define XWS32_T_SHIFT           5

#define XWU64_T__SIZE
#define BITS_PER_XWU64_T        64
#define XWU64_T_SHIFT           6
#define XWS64_T__SIZE
#define BITS_PER_XWS64_T        64
#define XWS64_T_SHIFT           6

#define CHAR__SIZE
#define BITS_PER_CHAR           8
#define CHAR_SHIFT              3
#define UCHAR__SIZE
#define BITS_PER_UCHAR          8
#define UCHAR_SHIFT             3

#define SHORT__SIZE
#define BITS_PER_SHORT          16
#define SHORT_SHIFT             4
#define USHORT__SIZE
#define BITS_PER_USHORT         16
#define USHORT_SHIFT            4

#define INT__SIZE
#define BITS_PER_INT            32
#define INT_SHIFT               5
#define UINT__SIZE
#define BITS_PER_UINT           32
#define UINT_SHIFT              5

#define LONG__SIZE
#define BITS_PER_LONG           32
#define LONG_SHIFT              5
#define ULONG__SIZE
#define BITS_PER_ULONG          32
#define LONG_SHIFT              5

#define LONGLONG__SIZE
#define BITS_PER_LONGLONG       64
#define LONGLONG_SHIFT          6
#define ULONGLONG__SIZE
#define BITS_PER_ULONGLONG      64
#define ULONGLONG_SHIFT         6

#define FLOAT__SIZE
#define BITS_PER_FLOAT          32
#define FLOAT_SHIFT             5

#define DOUBLE__SIZE
#define BITS_PER_DOUBLE         64
#define DOUBLE_SHIFT            6

#define XWBMP_T__SIZE
#define BITS_PER_XWBMP_T        BITS_PER_LONG
#define XWBMP_T_SHIFT           LONG_SHIFT

#define XWSZ_T__SIZE
#define BITS_PER_XWSZ_T         BITS_PER_LONG
#define XWSZ_T_SHIFT            LONG_SHIFT
#define XWSSZ_T__SIZE
#define BITS_PER_XWSSZ_T        BITS_PER_LONG
#define XWSSZ_T_SHIFT           LONG_SHIFT

#define XWPTR_T__SIZE
#define BITS_PER_XWPTR_T        BITS_PER_LONG
#define XWPTR_T_SHIFT           LONG_SHIFT

#define XWSID_T__SIZE
#define BITS_PER_XWSID_T        BITS_PER_LONG
#define XWSID_T_SHIFT           LONG_SHIFT
#define XWID_T__SIZE
#define BITS_PER_XWID_T         BITS_PER_LONG
#define XWID_T_SHIFT            LONG_SHIFT

#define XWREG_T__SIZE
#define BITS_PER_XWREG_T        BITS_PER_LONG
#define XWREG_T_SHIFT           LONG_SHIFT
#define XWSREG_T__SIZE
#define BITS_PER_XWSREG_T       BITS_PER_LONG
#define XWSREG_T_SHIFT          LONG_SHIFT

#define XWSQ_T__SIZE
#define BITS_PER_XWSQ_T         BITS_PER_LONG
#define XWSQ_T_SHIFT            LONG_SHIFT
#define XWSSQ_T__SIZE
#define BITS_PER_XWSSQ_T        BITS_PER_LONG
#define XWSSQ_T_SHIFT           LONG_SHIFT

#endif /* xwosimpl_soc_type.h */
