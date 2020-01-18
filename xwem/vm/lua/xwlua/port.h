/**
 * @file
 * @brief xwlua init
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_port_h__
#define __xwem_vm_lua_port_h__

#include <xwos/standard.h>
#include "lua.h"
#include "lauxlib.h"

void xwlua_openlibs(lua_State * L);
int xwlua_panic(lua_State * L);
lua_State * xwlua_newstate(lua_Alloc f);
int xwlua_writestring(const char * s, xwsz_t l);
int xwlua_writestringerror(const char * s, const char * p);

#endif /* xwem/vm/lua/port.h */
