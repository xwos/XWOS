/**
 * @file
 * @brief XWLUA库：XWOS内核：信号选择器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwos_sel_h__
#define __xwmd_vm_lua_xwlua_xwos_sel_h__

#include <xwos/standard.h>
#include <xwos/osal/sync/sel.h>
#include "xwmd/vm/lua/src/lauxlib.h"

typedef xwos_sel_d xwlua_sel_sp; /**< 信号选择器的强指针 */
#define XWLUA_SEL_NULLSP XWOS_SEL_NILD

void xwlua_os_open_sel(lua_State * L);

#endif /* xwmd/vm/lua/xwlua/xwos/sel.h */
