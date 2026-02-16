/**
 * @file
 * @brief XWLUA库：XWOS内核：调度
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwos_thd_h__
#define __xwmd_vm_lua_xwlua_xwos_thd_h__

#include <xwos/standard.h>
#include <xwos/osal/thd.h>
#include "xwmd/vm/lua/src/lauxlib.h"

typedef xwos_thd_d xwlua_thd_sp; /**< 线程的强指针 */
#define XWLUA_THD_NULLSP XWOS_THD_NILD

void xwlua_os_open_thd(lua_State * L);
void xwlua_os_open_cthd(lua_State * L);

#endif /* xwmd/vm/lua/xwlua/xwos/thd.h */
