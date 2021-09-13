/**
 * @file
 * @brief 玄武OS的标准头文件
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * XWOS中所有.c文件的顶级头文件，此头文件中包含了一些基本定义：
 * - 配置
 * - 编译器的宏
 * - 基本类型
 * - 错误码
 * - 版本号
 */

#ifndef __xwos_standard_h__
#define __xwos_standard_h__

#include <cfg/XuanWuOS.h>
#include <xwos/lib/rule.h>
#include <xwos/lib/compiler.h>
#include <xwos/lib/version.h>
#include <xwos/lib/type.h>
#include <xwos/lib/rule.h>
#include <xwos/lib/error.h>
#if defined(SOCCFG_BKPT) && (1 == SOCCFG_BKPT)
#  include <xwos/ospl/soc/bkpt.h>
#endif

/**
 * @brief 接口
 */
#define interface struct

#ifndef xw_array_size
/**
 * @brief 获取结构体数组中结构体的个数
 * @param[in] ay: 数组名
 */
#  define xw_array_size(ay) (sizeof(ay) / sizeof((ay)[0]))
#endif

#if defined(XWKNCFG_BUG) && (1 == XWKNCFG_BUG)
#  if defined(SOCCFG_BKPT) && (1 == SOCCFG_BKPT)
#    define XWOS_BUG()          soc_bkpt()
#  else
#    define XWOS_BUG()          do {} while (1)
#  endif
#  define XWOS_BUG_ON(x)        if (__xwcc_unlikely(x)) XWOS_BUG()
#else
#  define XWOS_BUG()
#  define XWOS_BUG_ON(x)
#endif

#define XWOS_UNUSED_ARGUMENT    0

#define XWOS_UNUSED(x)          ((void)(x))     /**< 去除未使用变量的警告 */

#if defined(XWKNCFG_CHECK_PARAMETERS) && (1 == XWKNCFG_CHECK_PARAMETERS)
#  define XWOS_VALIDATE(exp, errstr, ...)       \
          if (__xwcc_unlikely((!(exp)))) {      \
              return __VA_ARGS__;               \
          }
#else
#  define XWOS_VALIDATE(exp, errstr, ...)
#endif

#define XWOS_VALIDATE_FORCE(exp, errstr, ...)   \
        if (__xwcc_unlikely((!(exp)))) {        \
                return __VA_ARGS__;             \
        }

#if defined(ARCHCFG_PTRSIZE)
#  define XWOS_PTRSIZE          (ARCHCFG_PTRSIZE)
#else
#  if (1 == ARCHCFG_16BIT)
#    define XWOS_PTRSIZE        4
#  elif (1 == ARCHCFG_32BIT)
#    define XWOS_PTRSIZE        4
#  elif (1 == ARCHCFG_64BIT)
#    define XWOS_PTRSIZE        8
#  else
#    error "Unkown CPU bits!"
#  endif
#endif

#endif /* xwos/standard.h */
