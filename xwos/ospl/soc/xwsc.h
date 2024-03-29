/**
 * @file
 * @brief XWOS移植层：SOC系统调用与系统特权
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_soc_xwsc_h__
#define __xwos_ospl_soc_xwsc_h__

#include <xwos/standard.h>
#include <xwosimpl_soc_xwsc.h>

xws64_t xwsc(xwsc_f func, xwreg_t argnum, ...);

#endif /* xwos/ospl/soc/xwsc.h */
