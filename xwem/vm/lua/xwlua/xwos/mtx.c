/**
 * @file
 * @brief 玄武OS Lua Lib：互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/lock/mtx.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwos/mtx.h"

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
        rc = xwos_mtx_create(&mtxsp->mtx, XWOS_SKD_PRIORITY_RT_MAX);
        if (XWOK == rc) {
                mtxsp->tik = xwos_mtx_gettik(mtxsp->mtx);
                luaL_setmetatable(L, "xwlua_mtx_sp");
        } else {
                *mtxsp = XWLUA_MTX_NULLSP;
        }
        return 1;
}

const luaL_Reg xwlua_mtx_method[] = {
        {"new", xwlua_mtx_new},
        {NULL, NULL},
};

void xwlua_os_init_mtxsp(lua_State * L);

void xwlua_os_open_mtx(lua_State * L)
{
        xwlua_os_init_mtxsp(L);
        luaL_newlib(L, xwlua_mtx_method);
}

/******** class xwlua_mtx_sp ********/
int xwlua_mtxsp_gc(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;

        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        xwos_mtx_release(*mtxsp);
        *mtxsp = XWLUA_MTX_NULLSP;
        return 0;
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
        D = (lua_State *)lua_touserdata(L, 2);
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
        {"__tostring", xwlua_mtxsp_tostring},
        {"__copy", xwlua_mtxsp_copy},
        {NULL, NULL}
};

int xwlua_mtxsp_unlock(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;
        xwer_t rc;

        if (lua_istable(L, 1)) {
                lua_getfield(L, 1, "xwlualock"); /* top = autolock.xwlualock */
                lua_replace(L, 1); /* bottom = autolock.xwlualock */
        }
        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        rc = xwos_mtx_unlock(mtxsp->mtx);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

#define XWLUA_MTX_LKOPT_TRY             0
const char * const xwlua_mtx_lkopt[] = {"t", NULL};

int xwlua_mtxsp_lock(lua_State * L)
{
        xwlua_mtx_sp * mtxsp;
        int top;
        int type;
        int lkopt;
        xwtm_t time;
        xwer_t rc;

        top = lua_gettop(L);
        if (lua_istable(L, 1)) {
                lua_getfield(L, 1, "xwlualock"); /* top = autolock.xwlualock */
                lua_replace(L, 1); /* bottom = autolock.xwlualock */
        }
        mtxsp = (xwlua_mtx_sp *)luaL_checkudata(L, 1, "xwlua_mtx_sp");
        if (top >= 2) {
                type = lua_type(L, 2);
                switch (type) {
                case LUA_TNUMBER:
                        time = (xwtm_t)lua_tonumber(L, 2);
                        rc = xwos_mtx_timedlock(mtxsp->mtx, &time);
                        break;
                case LUA_TSTRING:
                        lkopt = luaL_checkoption(L, 2, "t", xwlua_mtx_lkopt);
                        switch (lkopt) {
                        case XWLUA_MTX_LKOPT_TRY:
                                rc = xwos_mtx_trylock(mtxsp->mtx);
                                break;
                        default:
                                luaL_error(L, "Invalid arg: %s", lua_tostring(L, 2));
                                rc = -EINVAL;
                                break;
                        }
                        break;
                default:
                        luaL_error(L, "Invalid arg type: %s", lua_typename(L, type));
                        rc = -EINVAL;
                        break;
                }
        } else {
                rc = xwos_mtx_lock(mtxsp->mtx);
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_mtxsp_method[] = {
        {"unlock", xwlua_mtxsp_unlock},
        {"lock", xwlua_mtxsp_lock},
        {NULL, NULL},
};

void xwlua_os_init_mtxsp(lua_State * L)
{
        /* metatable for xwlua_mtx_sp */
        luaL_newmetatable(L, "xwlua_mtx_sp");
        luaL_setfuncs(L, xwlua_mtxsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_mtxsp_method); /* create mtx method table */
        luaL_setfuncs(L, xwlua_mtxsp_method, 0); /* add mtx methods */
        lua_setfield(L, -2, "__index");  /* metatable.__index = xwlua_mtxsp_method */
        lua_pop(L, 1); /* pop metatable */
}
