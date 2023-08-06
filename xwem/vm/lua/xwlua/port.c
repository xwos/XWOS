/**
 * @file
 * @brief Port Lua for XWOS
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#include <xwos/standard.h>
#include <string.h>
#include <stdio.h>
#include <xwos/mm/mempool/allocator.h>
#include "xwem/vm/lua/src/lua.h"
#include "xwem/vm/lua/src/lualib.h"
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"

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

#if defined(XWEMCFG_vm_lua_BRDLIBS) && (1 == XWEMCFG_vm_lua_BRDLIBS)
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
#if defined(XWEMCFG_vm_lua_BRDLIBS) && (1 == XWEMCFG_vm_lua_BRDLIBS)
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

void * xwlua_alloc(void * ud, void * ptr, size_t osize, size_t nsize)
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
