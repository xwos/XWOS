/**
 * @file
 * @brief Port Lua for XuanWuOS
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_port_h__
#define __xwem_vm_lua_xwlua_port_h__

#include <xwos/standard.h>
#include "src/lua.h"
#include "src/lauxlib.h"

struct xwlua_arg {
        int argc;
        char ** argv;
};

extern struct xwmm_mempool * xwlua_mempool;

int xwlua_readline(lua_State * L, char buffer[], const char * prompt);
void xwlua_openlibs(lua_State * L);
void xwlua_xt_openlibs(lua_State * L);
xwer_t xwlua_task(void * arg);

#define XWLUA_LIB_NAME          "xwlib"
#define XWLUA_OS_NAME           "xwos"
#define XWLUA_XT_NAME           "xwxt"
#define XWLUA_THD_STACK         XWEMCFG_vm_lua_THD_STACK

LUAMOD_API int xwlua_open_lib(lua_State * L);
LUAMOD_API int xwlua_open_xt(lua_State * L);
LUAMOD_API int xwlua_open_os(lua_State * L);

#endif /* xwem/vm/lua/xwlua/port.h */
