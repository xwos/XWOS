/**
 * @file
 * @brief Prefix Include File for Lua in XWOS
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_prefix_h__
#define __xwem_vm_lua_xwlua_prefix_h__

#include <xwos/standard.h>
#include <xwos/lib/setjmp.h>

void * xwlua_alloc(void * ud, void * ptr, size_t osize, size_t nsize);
#define LUA_ALLOC       xwlua_alloc

#define lua_writeline() (lua_writestring("\r\n", 2), fflush(stdout))
#define LUA_INT_TYPE XWEMCFG_vm_lua_INT_TYPE
#define LUA_FLOAT_TYPE XWEMCFG_vm_lua_FLOAT_TYPE

#define LUAI_THROW(L,c)		xwlib_longjmp((c)->b, 1)
#define LUAI_TRY(L,c,a)		if (xwlib_setjmp((c)->b) == 0) { a }
#define luai_jmpbuf		xwjmpbuf_b

#endif /* xwem/vm/lua/xwlua/prefix.h */
