/**
 * @file
 * @brief XWOS通用库：系统调用与系统特权
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_sc_h__
#define __xwos_lib_sc_h__

#include <xwos/standard.h>

/**
 * @defgroup xwsc 系统调用与系统特权
 * @{
 */

#include <xwos/ospl/soc/xwsc.h>

xws64_t xwsc(xwsc_f func, xwreg_t argnum, ...);
void xwlib_privilege_start(void);
void xwlib_privilege_end(void);

/**
 * @} xwsc
 */

#endif /* xwos/lib/sc.h */
