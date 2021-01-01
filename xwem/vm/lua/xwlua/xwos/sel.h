/**
 * @file
 * @brief 玄武OS Lua Lib：信号选择器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwem_vm_lua_xwlua_xwos_sel_h__
#define __xwem_vm_lua_xwlua_xwos_sel_h__

#include <xwos/standard.h>
#include <xwos/osal/sync/sel.h>
#include "src/lauxlib.h"

typedef xwos_sel_d xwlua_sel_sp; /**< 信号量的强指针 */
#define XWLUA_SEL_NULLSP XWOS_SEL_NILD

void xwlua_os_open_sel(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/sel.h */
