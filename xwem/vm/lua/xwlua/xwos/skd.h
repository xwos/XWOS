/**
 * @file
 * @brief 玄武OS Lua Lib：调度
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwem_vm_lua_xwlua_xwos_skd_h__
#define __xwem_vm_lua_xwlua_xwos_skd_h__

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include "src/lauxlib.h"

typedef xwos_thd_d xwlua_thd_sp; /**< 线程的强指针 */
#define XWLUA_THD_NULLSP XWOS_THD_NILD

void xwlua_os_open_skd(lua_State * L);
void xwlua_os_open_thd(lua_State * L);
void xwlua_os_open_cthd(lua_State * L);

void xwlua_thdsp_xt_init(lua_State * xt);
void xwlua_thdsp_xt_export(lua_State * xt, const char * key, xwlua_thd_sp * thdsp);
void xwlua_thdsp_xt_copy(lua_State * L, xwlua_thd_sp * thdsp);

#endif /* xwem/vm/lua/xwlua/xwos/skd.h */
