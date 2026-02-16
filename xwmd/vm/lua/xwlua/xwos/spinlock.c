/**
 * @file
 * @brief XWLUA库：XWOS内核：自旋锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <stdlib.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwos/spinlock.h"

xwer_t xwlua_splk_gc(struct xwos_object * obj)
{
        free(obj);
        return XWOK;
}

xwsq_t xwlua_splk_get_tik(struct xwlua_splk * luasplk)
{
        xwsq_t tik;

        if (luasplk) {
                tik = luasplk->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

xwlua_splk_sp xwlua_splk_get_d(struct xwlua_splk * luasplk)
{
        xwlua_splk_sp splksp;

        splksp.luasplk = luasplk;
        splksp.tik = xwlua_splk_get_tik(luasplk);
        return splksp;
}

xwer_t xwlua_splk_acquire(xwlua_splk_sp splksp)
{
        xwer_t rc;

        if (splksp.luasplk) {
                rc = xwos_object_acquire(&splksp.luasplk->xwobj, splksp.tik);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

xwer_t xwlua_splk_release(xwlua_splk_sp splksp)
{
        xwer_t rc;

        if (splksp.luasplk) {
                rc = xwos_object_release(&splksp.luasplk->xwobj, splksp.tik);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

xwer_t xwlua_splk_grab(struct xwlua_splk * luasplk)
{
        xwer_t rc;

        if (luasplk) {
                rc =xwos_object_grab(&luasplk->xwobj);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

xwer_t xwlua_splk_put(struct xwlua_splk * luasplk)
{
        xwer_t rc;

        if (luasplk) {
                rc = xwos_object_put(&luasplk->xwobj);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

void xwlua_splk_lock(struct xwlua_splk * luasplk)
{
        xwos_splk_lock(&luasplk->ossplk);
        luasplk->lkst = XWLUA_SPLK_LKST_LOCK;
}

xwer_t xwlua_splk_trylock(struct xwlua_splk * luasplk)
{
        xwer_t rc;

        rc = xwos_splk_trylock(&luasplk->ossplk);
        if (XWOK == rc) {
                luasplk->lkst = XWLUA_SPLK_LKST_LOCK;
        }
        return rc;
}

void xwlua_splk_unlock(struct xwlua_splk * luasplk)
{
        switch (luasplk->lkst) {
        case XWLUA_SPLK_LKST_LOCK:
                luasplk->lkst = XWLUA_SPLK_LKST_UNLOCK;
                xwos_splk_unlock(&luasplk->ossplk);
                break;
        default:
                break;
        }
}

int xwlua_splk_new(lua_State * L)
{
        struct xwlua_splk * luasplk;
        xwlua_splk_sp * splksp;

        splksp = (xwlua_splk_sp *)lua_newuserdatauv(L, sizeof(xwlua_splk_sp), 0);
        luasplk = malloc(sizeof(struct xwlua_splk));
        if (luasplk) {
                xwos_object_construct(&luasplk->xwobj);
                xwos_object_activate(&luasplk->xwobj, xwlua_splk_gc);
                xwos_splk_init(&luasplk->ossplk);
                luasplk->lkst = XWLUA_SPLK_LKST_UNLOCK;
                splksp->luasplk = luasplk;
                splksp->tik = xwlua_splk_get_tik(luasplk);
                luaL_setmetatable(L, "xwlua_splk_sp");
        } else {
                lua_pop(L, 1);
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_splk_libconstructor[] = {
        {"new", xwlua_splk_new},
        {NULL, NULL},
};

void xwlua_os_init_splksp(lua_State * L);

void xwlua_os_open_splk(lua_State * L)
{
        xwlua_os_init_splksp(L);
        luaL_newlib(L, xwlua_splk_libconstructor);
}

/******** class xwlua_splk_sp ********/
int xwlua_splksp_gc(lua_State * L)
{
        xwlua_splk_sp * splksp;

        splksp = (xwlua_splk_sp *)luaL_checkudata(L, 1, "xwlua_splk_sp");
        xwlua_splk_unlock(splksp->luasplk);
        xwlua_splk_release(*splksp);
        *splksp = XWLUA_SPLK_NULLSP;
        return 0;
}

int xwlua_splksp_unlock(lua_State * L)
{
        xwlua_splk_sp * splksp;

        splksp = (xwlua_splk_sp *)luaL_checkudata(L, 1, "xwlua_splk_sp");
        xwlua_splk_unlock(splksp->luasplk);
        return 0;
}

int xwlua_splksp_tostring(lua_State * L)
{
        xwlua_splk_sp * splksp;

        splksp = (xwlua_splk_sp *)luaL_checkudata(L, 1, "xwlua_splk_sp");
        lua_pushfstring(L, "{%p, %d}", splksp->luasplk, splksp->tik);
        return 1;
}

int xwlua_splksp_copy(lua_State * L)
{
        xwlua_splk_sp * splksp;
        xwlua_splk_sp * newsplksp;
        lua_State * D;

        splksp = (xwlua_splk_sp *)luaL_checkudata(L, 1, "xwlua_splk_sp");
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
        if (D) {
                xwer_t rc;
                rc = xwlua_splk_acquire(*splksp);
                if (XWOK == rc) {
                        newsplksp = lua_newuserdatauv(D, sizeof(xwlua_splk_sp), 0);
                        newsplksp->luasplk = splksp->luasplk;
                        newsplksp->tik = splksp->tik;
                        luaL_setmetatable(D, "xwlua_splk_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_splksp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_splksp_gc},
        {"__close", xwlua_splksp_unlock},
        {"__tostring", xwlua_splksp_tostring},
        {"__copy", xwlua_splksp_copy},
        {NULL, NULL}
};

int xwlua_splksp_lock(lua_State * L)
{
        xwlua_splk_sp * splksp;

        splksp = (xwlua_splk_sp *)luaL_checkudata(L, 1, "xwlua_splk_sp");
        xwlua_splk_lock(splksp->luasplk);
        return 0;
}

int xwlua_splksp_trylock(lua_State * L)
{
        xwlua_splk_sp * splksp;
        xwer_t rc;

        splksp = (xwlua_splk_sp *)luaL_checkudata(L, 1, "xwlua_splk_sp");
        rc = xwlua_splk_trylock(splksp->luasplk);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_splksp_get_lkst(lua_State * L)
{
        xwlua_splk_sp * splksp;
        xwsq_t lkst;

        splksp = (xwlua_splk_sp *)luaL_checkudata(L, 1, "xwlua_splk_sp");
        lkst = splksp->luasplk->lkst;
        lua_pushinteger(L, (lua_Integer)lkst);
        return 1;
}

const luaL_Reg xwlua_splksp_indexmethod[] = {
        {"unlock", xwlua_splksp_unlock},
        {"lock", xwlua_splksp_lock},
        {"trylock", xwlua_splksp_trylock},
        {"get_lkst", xwlua_splksp_get_lkst},
        {NULL, NULL},
};

void xwlua_os_init_splksp(lua_State * L)
{
        /* metatable for xwlua_splk_sp */
        luaL_newmetatable(L, "xwlua_splk_sp");
        luaL_setfuncs(L, xwlua_splksp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_splksp_indexmethod); /* create splksp method table */
        luaL_setfuncs(L, xwlua_splksp_indexmethod, 0); /* add splk indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
