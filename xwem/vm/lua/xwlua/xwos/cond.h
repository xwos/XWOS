/**
 * @file
 * @brief 玄武OS Lua Lib：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwem_vm_lua_xwlua_xwos_cond_h__
#define __xwem_vm_lua_xwlua_xwos_cond_h__

#include <xwos/standard.h>
#include <xwos/osal/sync/cond.h>
#include "src/lauxlib.h"

typedef xwos_cond_d xwlua_cond_sp; /**< 信号量的强指针 */
#define XWLUA_COND_NULLSP XWOS_COND_NILD

void xwlua_os_open_cond(lua_State * L);

void xwlua_condsp_xt_init(lua_State * xt);
void xwlua_condsp_xt_export(lua_State * xt, const char * key, xwlua_cond_sp * condsp);
void xwlua_condsp_xt_copy(lua_State * L, xwlua_cond_sp * condsp);

#endif /* xwem/vm/lua/xwlua/xwos/cond.h */
