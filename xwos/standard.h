/**
 * @file
 * @brief 玄武OS的标准头文件
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>
#include <xwos/rule.h>
#include <xwos/compiler.h>
#include <xwos/version.h>
#include <xwos/type.h>
#include <xwos/lib/rule.h>
#include <xwos/lib/error.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 接口
 */
#define interface struct

#ifndef xw_array_size
/**
 * @brief 得到结构体数组中结构体的个数
 * @param ay: (I) 数组名
 */
  #define xw_array_size(ay) (sizeof(ay) / sizeof((ay)[0]))
#endif

#ifndef __XWOS_COMPILER__
  #error "Can't find the configure of compilier."
#endif

#if defined(XWKNCFG_BUG) && (1 == XWKNCFG_BUG)
  #define XWOS_BUG()            do {} while (1)
  #define XWOS_BUG_ON(x)        if (__unlikely(x)) XWOS_BUG()
#else
  #define XWOS_BUG()
  #define XWOS_BUG_ON(x)
#endif

#define XWOS_UNUSED_ARGUMENT    0

#define XWOS_UNUSED(x)          ((void)(x))     /**< 去除未使用变量的警告 */

#if defined(XWKNCFG_CHECK_PARAMETERS) && (1 == XWKNCFG_CHECK_PARAMETERS)
  #define XWOS_VALIDATE(exp, errstr, ...) \
          if (__unlikely((!(exp)))) {     \
              return __VA_ARGS__; \
          }
#else /* XWKNCFG_CHECK_PARAMETERS */
  #define XWOS_VALIDATE(exp, errstr, ...)
#endif /* !XWKNCFG_CHECK_PARAMETERS */

#if defined(ARCHCFG_PTRSIZE)
  #define XWOS_PTRSIZE        (ARCHCFG_PTRSIZE)
#else /* ARCHCFG_PTRSIZE */
  #if (1 == ARCHCFG_16BIT)
    #define XWOS_PTRSIZE        4
  #elif (1 == ARCHCFG_32BIT)
    #define XWOS_PTRSIZE        4
  #elif (1 == ARCHCFG_64BIT)
    #define XWOS_PTRSIZE        8
  #else
    #error "Unkown CPU bits!"
  #endif
#endif /* !ARCHCFG_PTRSIZE */

#endif /* xwos/standard.h */
