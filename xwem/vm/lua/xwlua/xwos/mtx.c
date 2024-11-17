/**
 * @file
 * @brief XWLUA库：XWOS内核：互斥锁
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
#include <xwos/osal/lock/mtx.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwos/mtx.h"

void xwlua_mtx_register(lua_State * L, const char * name,
                        struct xwos_mtx * mtx)
{
        xwlua_mtx_sp * mtxsp;
        xwer_t rc;

        mtxsp = lua_newuserdatauv(L, sizeof(xwlua_mtx_sp), 0);
        mtxsp->mtx = mtx;
        mtxsp->tik = xwos_mtx_get_tik(mtx);
        rc = xwos_mtx_acquire(*mtxsp);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_mtx_sp");
                lua_setglobal(L, name);
        } else {
                lua_pop(L, 1);
        }
}

void xwlua_mtx_unlock(struct xwos_mtx * mtx)
{
        xwer_t rc;
        do {
                rc = xwos_mtx_unlock(mtx);
        } while (XWOK == rc);
}

int xwlua_mtx_new(lua_State * L)
{
        xwer_t rc;
        xwlua_mtx_sp * mtxsp;

        mtxsp = lua_newuserdatauv(L, sizeof(xwlua_mtx_sp), 0);
        rc = xwos_mtx_create(mtxsp, XWOS_SKD_PRIORITY_RT_MAX);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_mtx_sp");
        } else {
                lua_pop(L, 1);
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_mtx_libconstructor[] = {
        {"new", xwlua_mtx_new},
        {NULL, NULL},
};

void xwlua_os_init_mtxsp(lua_State * L);

void xwlua_os_open_mtx(lua_State * L)
{
        xwlua_os_init_mtxsp(L);
        luaL_newlib(L, xwlua_mtx_libconstructor);
}

/******** class xwlua_mtx_sp ********/
int xwlua_mtxsp_gc(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        xwlua_mtx_unlock(mtxsp->mtx);
        xwos_mtx_release(*mtxsp);
        *mtxsp = XWLUA_MTX_NULLSP;
        return 0;
}

int xwlua_mtxsp_unlock(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;
        xwer_t rc;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        rc = xwos_mtx_unlock(mtxsp->mtx);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_mtxsp_tostring(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        lua_pushfstring(L, "{%p, %d}", mtxsp->mtx, mtxsp->tik);
        return 1;
}

int xwlua_mtxsp_copy(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;
        xwlua_mtx_sp * newmtxsp;
        lua_State * D;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
        if (D) {
                xwer_t rc;
                rc = xwos_mtx_acquire(*mtxsp);
                if (XWOK == rc) {
                        newmtxsp = lua_newuserdatauv(D, sizeof(xwlua_mtx_sp), 0);
                        newmtxsp->mtx = mtxsp->mtx;
                        newmtxsp->tik = mtxsp->tik;
                        luaL_setmetatable(D, "xwlua_mtx_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_mtxsp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_mtxsp_gc},
        {"__close", xwlua_mtxsp_unlock},
        {"__tostring", xwlua_mtxsp_tostring},
        {"__copy", xwlua_mtxsp_copy},
        {NULL, NULL}
};

int xwlua_mtxsp_lock(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;
        xwer_t rc;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        rc = xwos_mtx_lock(mtxsp->mtx);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_mtxsp_trylock(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;
        xwer_t rc;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        rc = xwos_mtx_trylock(mtxsp->mtx);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_mtxsp_lock_to(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;
        xwtm_t to;
        xwer_t rc;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        to = (xwtm_t)lua_tonumber(L, 2);
        rc = xwos_mtx_lock_to(mtxsp->mtx, to);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_mtxsp_get_status(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;
        xwsq_t lkst;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        xwos_mtx_get_status(mtxsp->mtx, &lkst);
        lua_pushinteger(L, (lua_Integer)lkst);
        return 1;
}

const luaL_Reg xwlua_mtxsp_indexmethod[] = {
        {"unlock", xwlua_mtxsp_unlock},
        {"lock", xwlua_mtxsp_lock},
        {"trylock", xwlua_mtxsp_trylock},
        {"lock_to", xwlua_mtxsp_lock_to},
        {"status", xwlua_mtxsp_get_status},
        {NULL, NULL},
};

void xwlua_os_init_mtxsp(lua_State * L)
{
        /* metatable for xwlua_mtx_sp */
        luaL_newmetatable(L, "xwlua_mtx_sp");
        luaL_setfuncs(L, xwlua_mtxsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_mtxsp_indexmethod); /* create mtx method table */
        luaL_setfuncs(L, xwlua_mtxsp_indexmethod, 0); /* add mtx indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
