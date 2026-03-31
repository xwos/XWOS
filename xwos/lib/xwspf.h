/**
 * @file
 * @brief XWOS通用库：格式化字符串
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_xwspf_h__
#define __xwos_lib_xwspf_h__

#include <xwos/standard.h>

/**
 * @defgroup xwos_lib_spf spf
 * @ingroup xwos_lib
 * @{
 */

/**
 * @brief 格式化字符串（同vsnprintf）
 */
int xwvsnpf(char * buf, xwsz_t size, const char * fmt, va_list args);

/**
 * @brief 格式化字符串（同snprintf）
 */
int xwsnpf(char * buf, xwsz_t size, const char * fmt, ...);

/**
 * @brief 格式化字符串（同vsprintf）
 */
int xwvspf(char * buf, const char * fmt, va_list args);

/**
 * @brief 格式化字符串（同sprintf）
 */
int xwspf(char * buf, const char * fmt, ...);

/**
 * @} xwos_lib_spf
 */

#endif /* xwos/lib/xwspf.h */
