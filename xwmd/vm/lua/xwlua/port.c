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

#include <xwos/standard.h>
#include <string.h>
#include <stdio.h>
#include <xwos/mm/mempool/allocator.h>
#include "xwmd/vm/lua/src/lua.h"
#include "xwmd/vm/lua/src/lualib.h"
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"

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
        {XWLUA_VM_NAME, xwlua_open_vm},
        {XWLUA_TM_NAME, xwlua_open_tm},
        {XWLUA_LIB_NAME, xwlua_open_lib},
        {XWLUA_OS_NAME, xwlua_open_os},
        {XWLUA_DS_NAME, xwlua_open_ds},
        {NULL, NULL},
};

const luaL_Reg xwlua_xt_loadedlibs[] = {
        {XWLUA_VM_NAME, xwlua_open_vm},
        {XWLUA_TM_NAME, xwlua_open_tm},
        {XWLUA_LIB_NAME, xwlua_open_lib},
        {XWLUA_OS_NAME, xwlua_open_os},
        {XWLUA_DS_NAME, xwlua_open_ds},
        {NULL, NULL},
};

#if defined(XWLUA_BRDLIBS) && (1 == XWLUA_BRDLIBS)
extern void xwlua_open_brdlibs(lua_State * L);
#endif

void xwlua_openlibs(lua_State * L)
{
        const luaL_Reg *lib;

        /* open xwxt */
        luaL_requiref(L, XWLUA_XT_NAME, xwlua_open_xt, 1);
        lua_pop(L, 1);

        /* open all libs */
        for (lib = xwlua_loadedlibs; lib->func; lib++) {
                luaL_requiref(L, lib->name, lib->func, 1);
                lua_pop(L, 1);
        }

        /* open brd libs */
#if defined(XWLUA_BRDLIBS) && (1 == XWLUA_BRDLIBS)
        xwlua_open_brdlibs(L);
#endif
}

void xwlua_xt_openlibs(lua_State * L)
{
        const luaL_Reg *lib;

        for (lib = xwlua_xt_loadedlibs; lib->func; lib++) {
                luaL_requiref(L, lib->name, lib->func, 1);
                lua_pop(L, 1);
        }
}

void * xwlua_alloc(void * ud, void * ptr, xwsz_t osize, xwsz_t nsize)
{
        void * mem;

        XWOS_UNUSED(ud);
        XWOS_UNUSED(osize);

        if (0 == nsize) {
                xwmm_mempool_free(xwlua_mempool, ptr);
                mem = NULL;
        } else {
                mem = ptr;
                xwmm_mempool_realloc(xwlua_mempool, nsize, &mem);
        }
        return mem;
}
