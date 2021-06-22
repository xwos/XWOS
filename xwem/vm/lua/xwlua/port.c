/**
 * @file
 * @brief Port Lua for XuanWuOS
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

        for (lib = xwlua_loadedlibs; lib->func; lib++) {
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

void xwlua_dump_stack(lua_State* L)
{
        int i = 0;
        int top = lua_gettop(L);
        printf("++++++++ lua stack ++++++++\n");
        for (i = top; i <= top; --i) {
                if (i == 0) {
                        break;
                }
                int t = lua_type(L, i);
                switch (t) {
                case LUA_TSTRING:
                        printf("|INDEX='%d','LUA_TSTRING=%s'|\n",
                               i, lua_tostring(L, i));
                        break;
                case LUA_TBOOLEAN:
                        printf("|INDEX='%d','LUA_TBOOLEAN=%s'|\n",
                               i, lua_toboolean(L, i) ? "true" : "false");
                        break;
                case LUA_TNUMBER:
                        printf("|INDEX='%d','LUA_TNUMBER=%g'|\n",
                               i, lua_tonumber(L, i));
                        break;
                default:
                        printf("|INDEX='%d','DEFAULT=%s'|\n",
                               i, lua_typename(L, t));
                        break;
                }
        }
        printf("-------- lua stack --------\n");
}
