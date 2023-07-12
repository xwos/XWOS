/**
 * @file
 * @brief XWOS的标准头文件
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_standard_h__
#define __xwos_standard_h__

#include <cfg/project.h>
#include <xwos/lib/rule.h>
#include <xwos/lib/compiler.h>
#include <xwos/version.h>
#include <xwos/lib/type.h>
#include <xwos/lib/error.h>
#if defined(SOCCFG_BKPT) && (1 == SOCCFG_BKPT)
#  include <xwos/ospl/soc/bkpt.h>
#endif

/**
 * @defgroup xwos_standard XWOS的标准头文件
 * @ingroup xwos
 * XWOS中所有源码文件的顶级头文件，此头文件中包含了一些基本定义：
 * + 配置
 * + 编译器的宏
 * + 基本类型
 * + 错误码
 * + 版本号
 * @{
 */

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

#if defined(XWOSCFG_BUG) && (1 == XWOSCFG_BUG)
#  if defined(SOCCFG_BKPT) && (1 == SOCCFG_BKPT)
#    define XWOS_BUG()          soc_bkpt() /**< 报告BUG */
#  else
#    define XWOS_BUG()          do {} while (1) /**< 报告BUG */
#  endif
#  define XWOS_BUG_ON(x)        if (__xwcc_unlikely(x)) XWOS_BUG() /**< 当条件x成立时报告BUG */
#else
#  define XWOS_BUG() /**< 报告BUG */
#  define XWOS_BUG_ON(x) /**< 当条件x成立时报告BUG */
#endif
#define XWOS_LOOP_ON(x)         do {} while (__xwcc_unlikely(x)) /**< 当条件x成立时循环 */

#define XWOS_UNUSED_ARGUMENT    0 /**< 未使用的参数的占位符 */

#define XWOS_UNUSED(x)          ((void)(x))     /**< 去除未使用变量的警告 */


/**
 * @brief 检查函数参数是否有效
 * @param[in] exp: 条件表达式
 * @param[in] errstr: 条件表达式为假时的错误信息
 * @param[in] ...: 返回值
 */
#if defined(XWOSCFG_CHECK_PARAMETERS) && (1 == XWOSCFG_CHECK_PARAMETERS)
#  define XWOS_VALIDATE(exp, errstr, ...)       \
          if (__xwcc_unlikely((!(exp)))) {      \
              return __VA_ARGS__;               \
          }
#else
#  define XWOS_VALIDATE(exp, errstr, ...)
#endif

#if defined(ARCHCFG_PTRSIZE)
#  define XWOS_PTRSIZE          (ARCHCFG_PTRSIZE) /**< 指针的长度 */
#else
#  if (1 == ARCHCFG_16BIT)
#    define XWOS_PTRSIZE        4 /**< 指针的长度 */
#  elif (1 == ARCHCFG_32BIT)
#    define XWOS_PTRSIZE        4 /**< 指针的长度 */
#  elif (1 == ARCHCFG_64BIT)
#    define XWOS_PTRSIZE        8 /**< 指针的长度 */
#  else
#    error "Unkown CPU bits!"
#  endif
#endif

/**
 * @} xwos_standard
 */

#endif /* xwos/standard.h */
