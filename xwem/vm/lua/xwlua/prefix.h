/**
 * @file
 * @brief Prefix Include File to Lua in XWOS
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_prefix_h__
#define __xwem_vm_lua_prefix_h__

#include <xwos/standard.h>

extern int xwlua_writestring(const char * s, xwsz_t l);
extern int xwlua_writestringerror(const char * s, const char * p);

#define lua_writestring(s, l)           xwlua_writestring(s, l)
#define lua_writestringerror(s, p)      xwlua_writestringerror(s, p)

#endif /* xwem/vm/lua/prefix.h */
