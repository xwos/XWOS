/**
 * @file
 * @brief XWLUA库：全局导出表
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <stdio.h>
#include <xwos/osal/lock/mtx.h>
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/xwvm/debug.h"
#include "xwem/vm/lua/xwlua/xwvm/dataxchg.h"

lua_State * xwlua_xt = NULL;
struct xwos_mtx xwlua_xt_lock;

/**
 * @brief 对全局导出表进行一次垃圾回收操作
 */
int xwlua_xt_gc(lua_State * L)
{
        xwer_t rc;

        rc = xwos_mtx_lock(&xwlua_xt_lock);
        if (XWOK == rc) {
                lua_gc(xwlua_xt, LUA_GCCOLLECT);
                xwos_mtx_unlock(&xwlua_xt_lock);
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

/**
 * @brief 将源虚拟机中的表元素导出到全局导出表
 */
int xwlua_xt_append(lua_State * L)
{
        xwer_t rc;

        luaL_checktype(L, 1, LUA_TTABLE);
        rc = xwos_mtx_lock(&xwlua_xt_lock);
        if (XWOK == rc) {
                xwlua_vm_copy_env(L, 1, xwlua_xt);
                xwos_mtx_unlock(&xwlua_xt_lock);
        }
        return 0;
}

const luaL_Reg xwlua_xt_libconstructor[] = {
        {"gc", xwlua_xt_gc},
        {"append", xwlua_xt_append},
        {NULL, NULL},
};

/**
 * @brief 从全局导出表中导入键值对
 */
int xwlua_xt_import(lua_State * L)
{
        xwlua_vm_copy_element(xwlua_xt, -1, L);
        return 1;
}

int xwlua_xt_index(lua_State * L)
{
        const char * key;
        int rc;

        key = luaL_checklstring(L, 2, NULL);
        lua_pushcfunction(L, xwlua_xt_import); /* function */
        xwos_mtx_lock_unintr(&xwlua_xt_lock);
        lua_getglobal(xwlua_xt, key); /* push value of key */
        rc = lua_pcall(L, 0, 1, 0); /* call xwlua_xt_import() then pop */
        lua_pop(xwlua_xt, 1); /* pop value of key */
        xwos_mtx_unlock(&xwlua_xt_lock);
        if (LUA_OK != rc) {
                lua_error(L);
        }
        return 1;
}

/**
 * @brief 将键值对导出到全局导出表中
 */
int xwlua_xt_export(lua_State * L)
{
        const char * key;

        key = luaL_checklstring(L, 1, NULL);
        xwlua_vm_copy_element(L, 2, xwlua_xt);
        lua_setglobal(xwlua_xt, key);
        return 0;
}

int xwlua_xt_newindex(lua_State * L)
{
        const char * key;
        int rc;

        key = luaL_checklstring(L, 2, NULL);
        if (lua_isnoneornil(L, 3)) { /* remove */
                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                lua_pushnil(xwlua_xt);
                lua_setglobal(xwlua_xt, key);
                xwos_mtx_unlock(&xwlua_xt_lock);
        } else { /* assign */
                lua_pushcfunction(L, xwlua_xt_export); /* function */
                lua_pushvalue(L, 2); /* arg1 */
                lua_pushvalue(L, 3); /* arg2 */
                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                /* call xwlua_xt_export(arg1, arg2) then pop */
                rc = lua_pcall(L, 2, LUA_MULTRET, 0);
                xwos_mtx_unlock(&xwlua_xt_lock);
                if (LUA_OK != rc) {
                        lua_error(L);
                }
        }
        return 0;
}

const luaL_Reg xwlua_xt_metamethod[] = {
        {"__index", xwlua_xt_index},
        {"__newindex", xwlua_xt_newindex},
        {NULL, NULL},
};

void xwlua_init_xt(void)
{
        xwos_mtx_init(&xwlua_xt_lock, XWOS_SKD_PRIORITY_RT_MAX);
        xwlua_xt = luaL_newstate();
        if (NULL == xwlua_xt) {
                lua_writestringerror("%s", "Cannot create xt: not enough memory");
        }
        /* libs in xwxt */
        xwlua_xt_openlibs(xwlua_xt);
}

LUAMOD_API int xwlua_open_xt(lua_State * L)
{
        if (NULL == xwlua_xt) {
                xwlua_init_xt();
        }

        luaL_newmetatable(L, "xwlua_xt");
        luaL_setfuncs(L, xwlua_xt_metamethod, 0); /* add metamethods */
        lua_pop(L, 1); /* pop metatable */

        luaL_newlib(L, xwlua_xt_libconstructor);
        luaL_setmetatable(L, "xwlua_xt");
        return 1;
}
