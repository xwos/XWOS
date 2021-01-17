/**
 * @file
 * @brief 玄武设备栈 Lua Lib：SOC
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwem_vm_lua_xwlua_xwds_soc_h__
#define __xwem_vm_lua_xwlua_xwds_soc_h__

#include <xwos/standard.h>
#include <xwcd/ds/soc/chip.h>
#include "src/lauxlib.h"

void xwlua_soc_register(lua_State * L, const char * name, struct xwds_soc * soc);
void xwlua_soc_unregister(lua_State * L, const char * name);
void xwlua_ds_open_soc(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwds/soc.h */
