/**
 * @file
 * @brief 玄武OS的标准C++头文件
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
 */

#ifndef __xwos_standard_hxx__
#define __xwos_standard_hxx__
#ifdef __cplusplus

#if __cplusplus < 201103L
#  error "Should use C++11 or later C++ standard!"
#endif

extern "C" {
#include <xwos/standard.h>
}

#endif /* __cplusplus */
#endif /* xwos/standard.hxx */
