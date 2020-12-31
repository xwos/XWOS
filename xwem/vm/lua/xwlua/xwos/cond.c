/**
 * @file
 * @brief 玄武OS Lua Lib：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/cond.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwos/mtx.h"
#include "xwlua/xwos/spinlock.h"
#include "xwlua/xwos/seqlock.h"
#include "xwlua/xwos/lock.h"
#include "xwlua/xwos/cond.h"

int xwlua_cond_new(lua_State * L)
{
        xwer_t rc;
        xwlua_cond_sp * condsp;

        condsp = lua_newuserdatauv(L, sizeof(xwlua_cond_sp), 0);
        rc = xwos_cond_create(&condsp->cond);
        if (XWOK == rc) {
                condsp->tik = xwos_cond_gettik(condsp->cond);
                luaL_setmetatable(L, "xwlua_cond_sp");
        } else {
                *condsp = XWLUA_COND_NULLSP;
        }
        return 1;
}

const luaL_Reg xwlua_cond_method[] = {
        {"new", xwlua_cond_new},
        {NULL, NULL},
};

void xwlua_os_init_condsp(lua_State * L);

void xwlua_os_open_cond(lua_State * L)
{
        xwlua_os_init_condsp(L);
        luaL_newlib(L, xwlua_cond_method);
}

/******** class xwlua_cond_sp ********/
int xwlua_condsp_gc(lua_State * L)
{
        xwlua_cond_sp * condsp;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        xwos_cond_release(*condsp);
        *condsp = XWLUA_COND_NULLSP;
        return 0;
}

int xwlua_condsp_tostring(lua_State * L)
{
        xwlua_cond_sp * condsp;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        lua_pushfstring(L, "{%p, %d}", condsp->cond, condsp->tik);
        return 1;
}

int xwlua_condsp_copy(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwlua_cond_sp * newcondsp;
        lua_State * D;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        D = (lua_State *)lua_touserdata(L, 2);
        if (D) {
                xwer_t rc;
                rc = xwos_cond_acquire(*condsp);
                if (XWOK == rc) {
                        newcondsp = lua_newuserdatauv(D, sizeof(xwlua_cond_sp), 0);
                        newcondsp->cond = condsp->cond;
                        newcondsp->tik = condsp->tik;
                        luaL_setmetatable(D, "xwlua_cond_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_condsp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_condsp_gc},
        {"__tostring", xwlua_condsp_tostring},
        {"__copy", xwlua_condsp_copy},
        {NULL, NULL}
};

int xwlua_condsp_freeze(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        rc = xwos_cond_freeze(condsp->cond);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_thaw(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        rc = xwos_cond_thaw(condsp->cond);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_unicast(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        rc = xwos_cond_unicast(condsp->cond);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_broadcast(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        rc = xwos_cond_broadcast(condsp->cond);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_wait(lua_State * L)
{
        xwlua_cond_sp * condsp;
        union {
                void * ud;
                xwlua_mtx_sp * mtxsp;
                xwlua_splk_sp * splksp;
                xwlua_sqlk_sp * sqlksp;
        } ulualksp;
        union xwos_ulock ulock;
        int top;
        xwsq_t lktype;
        xwsq_t lkst;
        xwtm_t time;
        int isnum;
        xwer_t rc;

        top = lua_gettop(L);
        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        if (1 == top) {
                lktype = XWOS_LK_NONE;
                ulock.anon = NULL;
                time = XWTM_MAX;
        } else if (LUA_TNIL == lua_type(L, 2)) {
                lktype = XWOS_LK_NONE;
                ulock.anon = NULL;
                time = XWTM_MAX;
        } else {
                do {
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_mtx_sp");
                        if (ulualksp.ud) {
                                ulock.osal.mtx = ulualksp.mtxsp->mtx;
                                xwos_mtx_getlkst(ulualksp.mtxsp->mtx, &lkst);
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        lktype = XWOS_LK_NONE;
                                } else {
                                        lktype = XWOS_LK_MTX;
                                }
                                break;
                        }
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_splk_sp");
                        if (ulualksp.ud) {
                                ulock.osal.splk = &ulualksp.splksp->luasplk->ossplk;
                                lktype = XWOS_LK_SPLK;
                                break;
                        }
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_sqlk_sp");
                        if (ulualksp.ud) {
                                ulock.osal.sqlk = &ulualksp.sqlksp->luasqlk->ossqlk;
                                switch (ulualksp.sqlksp->luasqlk->lkst) {
                                case XWLUA_SQLK_LKST_RDEX_LOCK:
                                        lktype = XWOS_LK_SQLK_RDEX;
                                        break;
                                case XWLUA_SQLK_LKST_WR_LOCK:
                                        lktype = XWOS_LK_SQLK_WR;
                                        break;
                                default:
                                        lktype = XWOS_LK_NONE;
                                        break;
                                }
                                break;
                        }
                        lktype = XWOS_LK_NONE;
                        ulock.anon = NULL;
                } while (false);
                time = (xwtm_t)lua_tonumberx(L, 3, &isnum);
                if (!isnum) {
                        time = XWTM_MAX;
                }
        }
        rc = xwos_cond_timedwait(condsp->cond, ulock, lktype, NULL, &time, &lkst);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_condsp_method[] = {
        {"freeze", xwlua_condsp_freeze},
        {"thaw", xwlua_condsp_thaw},
        {"unicast", xwlua_condsp_unicast},
        {"broadcast", xwlua_condsp_broadcast},
        {"wait", xwlua_condsp_wait},
        {NULL, NULL},
};

void xwlua_os_init_condsp(lua_State * L)
{
        /* metatable for xwlua_cond_sp */
        luaL_newmetatable(L, "xwlua_cond_sp");
        luaL_setfuncs(L, xwlua_condsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_condsp_method); /* create cond method table */
        luaL_setfuncs(L, xwlua_condsp_method, 0); /* add cond methods */
        lua_setfield(L, -2, "__index");  /* metatable.__index = xwlua_condsp_method */
        lua_pop(L, 1); /* pop metatable */
}