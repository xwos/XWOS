/**
 * @file
 * @brief Port Lua for XWOS
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_port_h__
#define __xwmd_vm_lua_xwlua_port_h__

#include <xwos/standard.h>
#include "xwmd/vm/lua/src/lua.h"
#include "xwmd/vm/lua/src/lauxlib.h"

struct xwlua_arg {
        int argc;
        char ** argv;
};

extern struct xwmm_mempool * xwlua_mempool;

void xwlua_cherryrl_init(void);
int xwlua_cherryrl_readline(char buffer[], const char * prompt);
void xwlua_openlibs(lua_State * L);
void xwlua_xt_openlibs(lua_State * L);
xwer_t xwlua_task(void * arg);

#define XWLUA_VM_NAME           "xwvm"
#define XWLUA_XT_NAME           "xwxt"
#define XWLUA_TM_NAME           "xwtm"
#define XWLUA_LIB_NAME          "xwlib"
#define XWLUA_OS_NAME           "xwos"
#define XWLUA_DS_NAME           "xwds"

LUAMOD_API int xwlua_open_vm(lua_State * L);
LUAMOD_API int xwlua_open_xt(lua_State * L);
LUAMOD_API int xwlua_open_tm(lua_State * L);
LUAMOD_API int xwlua_open_lib(lua_State * L);
LUAMOD_API int xwlua_open_os(lua_State * L);
LUAMOD_API int xwlua_open_ds(lua_State * L);

#endif /* xwmd/vm/lua/xwlua/port.h */
