/**
 * @file
 * @brief Port Lua for XuanWuOS
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * + http://www.lua.org/license.html
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/mm/mempool/allocator.h>
#include "src/lua.h"
#include "src/lualib.h"
#include "src/lauxlib.h"
#include "xwlua/port.h"

const luaL_Reg xwlua_loadedlibs[] = {
        {LUA_GNAME, luaopen_base},
        {LUA_LOADLIBNAME, luaopen_package},
        {LUA_COLIBNAME, luaopen_coroutine},
        {LUA_TABLIBNAME, luaopen_table},
        {LUA_IOLIBNAME, luaopen_io},
        {LUA_STRLIBNAME, luaopen_string},
        {LUA_MATHLIBNAME, luaopen_math},
        {LUA_UTF8LIBNAME, luaopen_utf8},
        {LUA_DBLIBNAME, luaopen_debug},
        {XWLUA_LIB_NAME, xwlua_open_lib},
        {XWLUA_OS_NAME, xwlua_open_os},
        {XWLUA_DS_NAME, xwlua_open_ds},
        {NULL, NULL},
};

#if defined(XWEMCFG_vm_lua_BRDLIBS) && (1 == XWEMCFG_vm_lua_BRDLIBS)
extern void xwlua_open_brdlibs(lua_State * L);
#endif

void xwlua_openlibs(lua_State * L)
{
        const luaL_Reg *lib;

        for (lib = xwlua_loadedlibs; lib->func; lib++) {
                luaL_requiref(L, lib->name, lib->func, 1);
                lua_pop(L, 1);
        }
        luaL_requiref(L, XWLUA_XT_NAME, xwlua_open_xt, 1);
        lua_pop(L, 1);
#if defined(XWEMCFG_vm_lua_BRDLIBS) && (1 == XWEMCFG_vm_lua_BRDLIBS)
        xwlua_open_brdlibs(L);
#endif
}

void xwlua_xt_openlibs(lua_State * L)
{
        const luaL_Reg *lib;

        for (lib = xwlua_loadedlibs; lib->func; lib++) {
                luaL_requiref(L, lib->name, lib->func, 1);
                lua_pop(L, 1);
        }
}

void * xwlua_alloc(void * ud, void * ptr, size_t osize, size_t nsize)
{
        void * mem;

        XWOS_UNUSED(ud);

        if (NULL == ptr) {
                xwmm_mempool_malloc(xwlua_mempool, nsize, &mem);
        } else if (0 == nsize) {
                xwmm_mempool_free(xwlua_mempool, ptr);
                mem = NULL;
        } else if (osize >= nsize) {
                mem = ptr;
        } else {
                mem = ptr;
                xwmm_mempool_realloc(xwlua_mempool, nsize, &mem);
        }
        return mem;
}
