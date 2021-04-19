/**
 * @file
 * @brief XWOS通用库：系统调用与系统特权
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_sc_h__
#define __xwos_lib_sc_h__

#include <xwos/standard.h>
#include <xwos/ospl/soc/xwsc.h>

#define xwsc(func, argnum, ...) soc_xwsc((func), (argnum), ##__VA_ARGS__)

void xwlib_privilege_start(void);
void xwlib_privilege_end(void);

#endif /* xwos/lib/sc.h */
