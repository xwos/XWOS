/**
 * @file
 * @brief 玄武OS Lua Lib：线程栅栏
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/br.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwlib/bmp.h"
#include "xwlua/xwos/sel.h"
#include "xwlua/xwos/br.h"

int xwlua_br_new(lua_State * L)
{
        xwer_t rc;
        xwlua_br_sp * brsp;

        brsp = lua_newuserdatauv(L, sizeof(xwlua_br_sp), 0);
        rc = xwos_br_create(&brsp->br);
        if (XWOK == rc) {
                brsp->tik = xwos_br_gettik(brsp->br);
                luaL_setmetatable(L, "xwlua_br_sp");
        } else {
                *brsp = XWLUA_BR_NULLSP;
        }
        return 1;
}

int xwlua_br_msk(lua_State * L)
{
        xwsz_t bmpsz;
        xwlua_bmp_t * msk;

        bmpsz = (sizeof(xwbmp_t) * BITS_TO_BMPS(XWOS_BR_MAXNUM)) +
                sizeof(xwlua_bmp_t);
        msk = lua_newuserdatauv(L, bmpsz, 0);
        msk->bits = XWOS_BR_MAXNUM;
        xwbmpop_c0all(msk->bmp, msk->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        return 1;
}

const luaL_Reg xwlua_br_method[] = {
        {"new", xwlua_br_new},
        {"msk", xwlua_br_msk},
        {NULL, NULL},
};

void xwlua_os_init_brsp(lua_State * L);

void xwlua_os_open_br(lua_State * L)
{
        xwlua_os_init_brsp(L);
        luaL_newlib(L, xwlua_br_method);
}

/******** class xwlua_br_sp ********/
int xwlua_brsp_gc(lua_State * L)
{
        xwlua_br_sp * brsp;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        xwos_br_release(*brsp);
        *brsp = XWLUA_BR_NULLSP;
        return 0;
}

int xwlua_brsp_tostring(lua_State * L)
{
        xwlua_br_sp * brsp;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        lua_pushfstring(L, "{%p, %d}", brsp->br, brsp->tik);
        return 1;
}

int xwlua_brsp_copy(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwlua_br_sp * newbrsp;
        lua_State * D;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        D = (lua_State *)lua_touserdata(L, 2);
        if (D) {
                xwer_t rc;
                rc = xwos_br_acquire(*brsp);
                if (XWOK == rc) {
                        newbrsp = lua_newuserdatauv(D, sizeof(xwlua_br_sp), 0);
                        newbrsp->br = brsp->br;
                        newbrsp->tik = brsp->tik;
                        luaL_setmetatable(D, "xwlua_br_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_brsp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_brsp_gc},
        {"__tostring", xwlua_brsp_tostring},
        {"__copy", xwlua_brsp_copy},
        {NULL, NULL}
};

int xwlua_brsp_bind(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwlua_sel_sp * selsp;
        xwsq_t pos;
        xwer_t rc;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        pos = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwos_br_bind(brsp->br, selsp->sel, pos);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_brsp_unbind(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwlua_sel_sp * selsp;
        xwer_t rc;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        rc = xwos_br_unbind(brsp->br, selsp->sel);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_brsp_intr_all(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwer_t rc;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        rc = xwos_br_intr_all(brsp->br);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_brsp_sync(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwlua_bmp_t * msk;
        xwlua_bmp_t * out;
        xwsq_t pos;
        xwtm_t time;
        xwsz_t bmpsz;
        int top;
        xwer_t rc;

        top = lua_gettop(L);
        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 3, "xwlua_bmp_t");
        pos = (xwsq_t)luaL_checkinteger(L, 2);
        if ((0 == pos) || (pos > XWOS_BR_MAXNUM)) {
                lua_pushfstring(L, "Position is out of range [1:%d]!", XWOS_BR_MAXNUM);
                lua_error(L);
        } else {
                pos--;
        }
        if (top >= 4) {
                time = (xwtm_t)luaL_checknumber(L, 4);
                bmpsz = (sizeof(xwbmp_t) * BITS_TO_BMPS(XWOS_BR_MAXNUM)) +
                        sizeof(xwlua_bmp_t);
                out = lua_newuserdatauv(L, bmpsz, 0);
                out->bits = XWOS_BR_MAXNUM;
                xwbmpop_c0all(out->bmp, out->bits);
                luaL_setmetatable(L, "xwlua_bmp_t");
                rc = xwos_br_timedsync(brsp->br, pos, msk->bmp, out->bmp, &time);
        } else {
                bmpsz = (sizeof(xwbmp_t) * BITS_TO_BMPS(XWOS_BR_MAXNUM)) +
                        sizeof(xwlua_bmp_t);
                out = lua_newuserdatauv(L, bmpsz, 0);
                out->bits = XWOS_BR_MAXNUM;
                xwbmpop_c0all(out->bmp, out->bits);
                luaL_setmetatable(L, "xwlua_bmp_t");
                rc = xwos_br_sync(brsp->br, pos, msk->bmp, out->bmp);
        }
        lua_pushinteger(L, (lua_Integer)rc);
        lua_insert(L, -2);
        return 2;
}

const luaL_Reg xwlua_brsp_method[] = {
        {"bind", xwlua_brsp_bind},
        {"unbind", xwlua_brsp_unbind},
        {"intr_all", xwlua_brsp_intr_all},
        {"sync", xwlua_brsp_sync},
        {NULL, NULL},
};

void xwlua_os_init_brsp(lua_State * L)
{
        /* metatable for xwlua_br_sp */
        luaL_newmetatable(L, "xwlua_br_sp");
        luaL_setfuncs(L, xwlua_brsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_brsp_method); /* create br method table */
        luaL_setfuncs(L, xwlua_brsp_method, 0); /* add br methods */
        lua_setfield(L, -2, "__index");  /* metatable.__index = xwlua_brsp_method */
        lua_pop(L, 1); /* pop metatable */
}
