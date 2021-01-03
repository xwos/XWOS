/**
 * @file
 * @brief 玄武OS Lua Lib：顺序锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/lock/seqlock.h>
#include <stdlib.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwos/seqlock.h"

xwer_t xwlua_sqlk_gc(void * gcobj)
{
        free(gcobj);
        return XWOK;
}

xwsq_t xwlua_sqlk_gettik(struct xwlua_sqlk * luasqlk)
{
        xwsq_t tik;

        if (luasqlk) {
                tik = luasqlk->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

xwlua_sqlk_sp xwlua_sqlk_getd(struct xwlua_sqlk * luasqlk)
{
        xwlua_sqlk_sp sqlksp;

        sqlksp.luasqlk = luasqlk;
        sqlksp.tik = xwlua_sqlk_gettik(luasqlk);
        return sqlksp;
}

xwer_t xwlua_sqlk_acquire(xwlua_sqlk_sp sqlksp)
{
        xwer_t rc;

        if (sqlksp.luasqlk) {
                rc = xwos_object_acquire(&sqlksp.luasqlk->xwobj, sqlksp.tik);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

xwer_t xwlua_sqlk_release(xwlua_sqlk_sp sqlksp)
{
        xwer_t rc;

        if (sqlksp.luasqlk) {
                rc = xwos_object_release(&sqlksp.luasqlk->xwobj, sqlksp.tik);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

xwer_t xwlua_sqlk_grab(struct xwlua_sqlk * luasqlk)
{
        xwer_t rc;

        if (luasqlk) {
                rc =xwos_object_grab(&luasqlk->xwobj);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

xwer_t xwlua_sqlk_put(struct xwlua_sqlk * luasqlk)
{
        xwer_t rc;

        if (luasqlk) {
                rc = xwos_object_put(&luasqlk->xwobj);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

void xwlua_sqlk_rdex_lock(struct xwlua_sqlk * luasqlk)
{
        xwos_sqlk_rdex_lock(&luasqlk->ossqlk);
        luasqlk->lkst = XWLUA_SQLK_LKST_RDEX_LOCK;
}

xwer_t xwlua_sqlk_rdex_trylock(struct xwlua_sqlk * luasqlk)
{
        xwer_t rc;

        rc = xwos_sqlk_rdex_trylock(&luasqlk->ossqlk);
        if (XWOK == rc) {
                luasqlk->lkst = XWLUA_SQLK_LKST_RDEX_LOCK;
        }
        return rc;
}

void xwlua_sqlk_wr_lock(struct xwlua_sqlk * luasqlk)
{
        xwos_sqlk_wr_lock(&luasqlk->ossqlk);
        luasqlk->lkst = XWLUA_SQLK_LKST_WR_LOCK;
}

xwer_t xwlua_sqlk_wr_trylock(struct xwlua_sqlk * luasqlk)
{
        xwer_t rc;

        rc = xwos_sqlk_wr_trylock(&luasqlk->ossqlk);
        if (XWOK == rc) {
                luasqlk->lkst = XWLUA_SQLK_LKST_WR_LOCK;
        }
        return rc;
}

void xwlua_sqlk_unlock(struct xwlua_sqlk * luasqlk)
{
        switch (luasqlk->lkst) {
        case XWLUA_SQLK_LKST_RDEX_LOCK:
                luasqlk->lkst = XWLUA_SQLK_LKST_UNLOCK;
                xwos_sqlk_rdex_unlock(&luasqlk->ossqlk);
                break;
        case XWLUA_SQLK_LKST_WR_LOCK:
                luasqlk->lkst = XWLUA_SQLK_LKST_UNLOCK;
                xwos_sqlk_wr_unlock(&luasqlk->ossqlk);
                break;
        default:
                break;
        }
}

int xwlua_sqlk_new(lua_State * L)
{
        struct xwlua_sqlk * luasqlk;
        xwlua_sqlk_sp * sqlksp;

        sqlksp = (xwlua_sqlk_sp *)lua_newuserdatauv(L, sizeof(xwlua_sqlk_sp), 0);
        luasqlk = malloc(sizeof(struct xwlua_sqlk));
        if (luasqlk) {
                xwos_object_construct(&luasqlk->xwobj);
                xwos_object_activate(&luasqlk->xwobj, xwlua_sqlk_gc);
                xwos_sqlk_init(&luasqlk->ossqlk);
                luasqlk->lkst = XWLUA_SQLK_LKST_UNLOCK;
                sqlksp->luasqlk = luasqlk;
                sqlksp->tik = xwlua_sqlk_gettik(luasqlk);
                luaL_setmetatable(L, "xwlua_sqlk_sp");
        } else {
                lua_pop(L, 1);
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_sqlk_method[] = {
        {"new", xwlua_sqlk_new},
        {NULL, NULL},
};

void xwlua_os_init_sqlksp(lua_State * L);

void xwlua_os_open_sqlk(lua_State * L)
{
        xwlua_os_init_sqlksp(L);
        luaL_newlib(L, xwlua_sqlk_method);
}

/******** class xwlua_sqlk_sp ********/
int xwlua_sqlksp_gc(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        xwlua_sqlk_unlock(sqlksp->luasqlk);
        xwlua_sqlk_release(*sqlksp);
        *sqlksp = XWLUA_SQLK_NULLSP;
        return 0;
}

int xwlua_sqlksp_unlock(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        xwlua_sqlk_unlock(sqlksp->luasqlk);
        return 0;
}

int xwlua_sqlksp_tostring(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        lua_pushfstring(L, "{%p, %d}", sqlksp->luasqlk, sqlksp->tik);
        return 1;
}

int xwlua_sqlksp_copy(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;
        xwlua_sqlk_sp * newsqlksp;
        lua_State * D;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        D = (lua_State *)lua_touserdata(L, 2);
        if (D) {
                xwer_t rc;
                rc = xwlua_sqlk_acquire(*sqlksp);
                if (XWOK == rc) {
                        newsqlksp = lua_newuserdatauv(D, sizeof(xwlua_sqlk_sp), 0);
                        newsqlksp->luasqlk = sqlksp->luasqlk;
                        newsqlksp->tik = sqlksp->tik;
                        luaL_setmetatable(D, "xwlua_sqlk_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_sqlksp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_sqlksp_gc},
        {"__close", xwlua_sqlksp_unlock},
        {"__tostring", xwlua_sqlksp_tostring},
        {"__copy", xwlua_sqlksp_copy},
        {NULL, NULL}
};

int xwlua_sqlksp_rd_begin(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;
        xwsq_t seq;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        seq = xwos_sqlk_rd_begin(&sqlksp->luasqlk->ossqlk);
        lua_pushinteger(L, (lua_Integer)seq);
        return 1;
}

int xwlua_sqlksp_rd_retry(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;
        xwsq_t start;
        bool ret;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        start = luaL_checkinteger(L, 2);
        ret = xwos_sqlk_rd_retry(&sqlksp->luasqlk->ossqlk, start);
        lua_pushboolean(L, ret);
        return 1;
}

int xwlua_sqlksp_get_seq(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;
        xwsq_t seq;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        seq = xwos_sqlk_get_seq(&sqlksp->luasqlk->ossqlk);
        lua_pushinteger(L, (lua_Integer)seq);
        return 1;
}

#define XWLUA_SQLK_LKTYPE_WR    0
#define XWLUA_SQLK_LKTYPE_RDEX  1
const char * const xwlua_sqlk_lktype[] = {"wr", "rdex", NULL};

#define XWLUA_SQLK_LKOPT_TRY             0
const char * const xwlua_sqlk_lkopt[] = {"t", NULL};

int xwlua_sqlksp_lock(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;
        int lktype;
        int lkopt;
        int top;
        int type;
        xwer_t rc;

        top = lua_gettop(L);
        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        lktype = luaL_checkoption(L, 2, "wr", xwlua_sqlk_lktype);
        switch (lktype) {
        case XWLUA_SQLK_LKTYPE_WR:
                if (top >= 3) {
                        type = lua_type(L, 3);
                        switch (type) {
                        case LUA_TSTRING:
                                lkopt = luaL_checkoption(L, 3, "t", xwlua_sqlk_lkopt);
                                switch (lkopt) {
                                case XWLUA_SQLK_LKOPT_TRY:
                                        rc = xwlua_sqlk_wr_trylock(sqlksp->luasqlk);
                                        break;
                                default:
                                        luaL_error(L, "Invalid arg: %s",
                                                   lua_tostring(L, 3));
                                        rc = -EINVAL;
                                        break;
                                }
                                break;
                        default:
                                luaL_error(L, "Invalid arg type: %s",
                                           lua_typename(L, type));
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        xwlua_sqlk_wr_lock(sqlksp->luasqlk);
                        rc = XWOK;
                }
                break;
        case XWLUA_SQLK_LKTYPE_RDEX:
                if (top >= 3) {
                        type = lua_type(L, 3);
                        switch (type) {
                        case LUA_TSTRING:
                                lkopt = luaL_checkoption(L, 3, "t", xwlua_sqlk_lkopt);
                                switch (lkopt) {
                                case XWLUA_SQLK_LKOPT_TRY:
                                        rc = xwlua_sqlk_rdex_trylock(sqlksp->luasqlk);
                                        break;
                                default:
                                        luaL_error(L, "Invalid arg: %s",
                                                   lua_tostring(L, 3));
                                        rc = -EINVAL;
                                        break;
                                }
                                break;
                        default:
                                luaL_error(L, "Invalid arg type: %s",
                                           lua_typename(L, type));
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        xwlua_sqlk_rdex_lock(sqlksp->luasqlk);
                        rc = XWOK;
                }
                break;
        default:
                luaL_error(L, "Invalid arg: %s", lua_tostring(L, 2));
                rc = -EINVAL;
                break;
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_sqlksp_method[] = {
        {"rd_begin", xwlua_sqlksp_rd_begin},
        {"rd_retry", xwlua_sqlksp_rd_retry},
        {"get_seq", xwlua_sqlksp_get_seq},
        {"unlock", xwlua_sqlksp_unlock},
        {"lock", xwlua_sqlksp_lock},
        {NULL, NULL},
};

void xwlua_os_init_sqlksp(lua_State * L)
{
        /* metatable for xwlua_sqlksp_d */
        luaL_newmetatable(L, "xwlua_sqlk_sp");
        luaL_setfuncs(L, xwlua_sqlksp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_sqlksp_method); /* create sqlk method table */
        luaL_setfuncs(L, xwlua_sqlksp_method, 0); /* add sqlk methods */
        lua_setfield(L, -2, "__index");  /* metatable.__index = xwlua_sqlksp_method */
        lua_pop(L, 1); /* pop metatable */
}
