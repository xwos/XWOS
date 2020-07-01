/**
 * @file
 * @brief Port Lua for XuanWuOS
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_port_h__
#define __xwem_vm_lua_port_h__

#include <xwos/standard.h>
#include "lua.h"
#include "lauxlib.h"

struct xwlua_arg {
        int argc;
        char ** argv;
};

extern struct xwmm_mempool * xwlua_mempool;

int xwlua_readline(lua_State * L, char buffer[], const char * prompt);
void * xwlua_alloc(void * ud, void * ptr, size_t osize, size_t nsize);
void xwlua_openlibs(lua_State * L);
lua_State * xwlua_newstate(lua_Alloc f);
xwer_t xwlua_thrd(void * arg);

#endif /* xwem/vm/lua/port.h */
