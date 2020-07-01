/**
 * @file
 * @brief Prefix Include File for Lua in XWOS
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_prefix_h__
#define __xwem_vm_lua_prefix_h__

#define lua_writeline() (lua_writestring("\r\n", 2), fflush(stdout))

#endif /* xwem/vm/lua/prefix.h */
