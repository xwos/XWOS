/**
 * @file
 * @brief xwlua init
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * + http://www.lua.org/license.html
 */

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static const luaL_Reg xwlua_loadedlibs[] = {
        {LUA_GNAME, luaopen_base},
        {LUA_LOADLIBNAME, luaopen_package},
        {LUA_COLIBNAME, luaopen_coroutine},
        {LUA_TABLIBNAME, luaopen_table},
        {LUA_IOLIBNAME, luaopen_io},
        //{LUA_OSLIBNAME, luaopen_os},
        {LUA_STRLIBNAME, luaopen_string},
        {LUA_MATHLIBNAME, luaopen_math},
        {LUA_UTF8LIBNAME, luaopen_utf8},
        {LUA_DBLIBNAME, luaopen_debug},
        {NULL, NULL}
};


void xwlua_openlibs(lua_State * L)
{
        const luaL_Reg *lib;

        for (lib = xwlua_loadedlibs; lib->func; lib++) {
                luaL_requiref(L, lib->name, lib->func, 1);
                lua_pop(L, 1);
        }
}

int xwlua_panic(lua_State * L)
{
        lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n",
                             lua_tostring(L, -1));
        return 0; /* return to Lua to abort */
}

lua_State * xwlua_newstate(lua_Alloc f)
{
        lua_State * L;

        if (f) {
                L = lua_newstate(f, NULL);
                if (L) {
                        lua_atpanic(L, xwlua_panic);
                }
        } else {
                L = NULL;
        }
        return L;
}

int xwlua_writestring(const char * s, xwsz_t l)
{
        XWOS_UNUSED(s);
        XWOS_UNUSED(l);
        return xwpf(s);
}

int xwlua_writestringerror(const char * s, const char * p)
{
        XWOS_UNUSED(s);
        XWOS_UNUSED(p);
        return xwpf(s, p);
}
