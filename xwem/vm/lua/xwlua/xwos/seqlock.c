/**
 * @file
 * @brief 玄武Lua库：顺序锁
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

const luaL_Reg xwlua_sqlk_libconstructor[] = {
        {"new", xwlua_sqlk_new},
        {NULL, NULL},
};

void xwlua_os_init_sqlksp(lua_State * L);

void xwlua_os_open_sqlk(lua_State * L)
{
        xwlua_os_init_sqlksp(L);
        luaL_newlib(L, xwlua_sqlk_libconstructor);
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
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
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

int xwlua_sqlksp_wr_lock(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        xwlua_sqlk_wr_lock(sqlksp->luasqlk);
        return 1;
}

int xwlua_sqlksp_wr_trylock(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;
        xwer_t rc;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        rc = xwlua_sqlk_wr_trylock(sqlksp->luasqlk);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_sqlksp_rdex_lock(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        xwlua_sqlk_rdex_lock(sqlksp->luasqlk);
        return 0;
}

int xwlua_sqlksp_rdex_trylock(lua_State * L)
{
        xwlua_sqlk_sp * sqlksp;
        xwer_t rc;

        sqlksp = (xwlua_sqlk_sp *)luaL_checkudata(L, 1, "xwlua_sqlk_sp");
        rc = xwlua_sqlk_rdex_trylock(sqlksp->luasqlk);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_sqlksp_indexmethod[] = {
        {"rd_begin", xwlua_sqlksp_rd_begin},
        {"rd_retry", xwlua_sqlksp_rd_retry},
        {"get_seq", xwlua_sqlksp_get_seq},
        {"unlock", xwlua_sqlksp_unlock},
        {"wr_lock", xwlua_sqlksp_wr_lock},
        {"wr_trylock", xwlua_sqlksp_wr_trylock},
        {"rdex_lock", xwlua_sqlksp_rdex_lock},
        {"rdex_trylock", xwlua_sqlksp_rdex_trylock},
        {NULL, NULL},
};

void xwlua_os_init_sqlksp(lua_State * L)
{
        /* metatable for xwlua_sqlksp_d */
        luaL_newmetatable(L, "xwlua_sqlk_sp");
        luaL_setfuncs(L, xwlua_sqlksp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_sqlksp_indexmethod); /* create sqlk method table */
        luaL_setfuncs(L, xwlua_sqlksp_indexmethod, 0); /* add sqlk indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
