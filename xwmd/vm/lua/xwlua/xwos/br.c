/**
 * @file
 * @brief XWLUA库：XWOS内核：线程栅栏
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/br.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwlib/bmp.h"
#include "xwmd/vm/lua/xwlua/xwos/sel.h"
#include "xwmd/vm/lua/xwlua/xwos/br.h"

void xwlua_br_register(lua_State * L, const char * name,
                       struct xwos_br * br)
{
        xwlua_br_sp * brsp;
        xwer_t rc;

        brsp = lua_newuserdatauv(L, sizeof(xwlua_br_sp), 0);
        brsp->br = br;
        brsp->tik = xwos_br_get_tik(br);
        rc = xwos_br_acquire(*brsp);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_br_sp");
                lua_setglobal(L, name);
        } else {
                lua_pop(L, 1);
        }
}

int xwlua_br_new(lua_State * L)
{
        xwer_t rc;
        xwsz_t bitnum;
        xwlua_br_sp * brsp;

        bitnum = luaL_checkinteger(L, 1);
        brsp = lua_newuserdatauv(L, sizeof(xwlua_br_sp), 0);
        rc = xwos_br_create(brsp, bitnum);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_br_sp");
        } else {
                lua_pop(L, 1);
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_br_libconstructor[] = {
        {"new", xwlua_br_new},
        {NULL, NULL},
};

void xwlua_os_init_brsp(lua_State * L);

void xwlua_os_open_br(lua_State * L)
{
        xwlua_os_init_brsp(L);
        luaL_newlib(L, xwlua_br_libconstructor);
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
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
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

int xwlua_brsp_bmp(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwsz_t bitnum;
        xwsz_t bmpsz;
        xwlua_bmp_t * bmp;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        xwos_br_get_num(brsp->br, &bitnum);
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(bitnum)) + sizeof(xwlua_bmp_t);
        bmp = lua_newuserdatauv(L, bmpsz, 0);
        bmp->bits = bitnum;
        xwbmpop_c0all(bmp->bmp, bmp->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        return 1;
}

int xwlua_brsp_num(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwsz_t bitnum;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        xwos_br_get_num(brsp->br, &bitnum);
        lua_pushinteger(L, (lua_Integer)bitnum);
        return 1;
}

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

int xwlua_brsp_wait(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwer_t rc;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        rc = xwos_br_wait(brsp->br);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_brsp_wait_to(lua_State * L)
{
        xwlua_br_sp * brsp;
        xwtm_t to;
        xwer_t rc;

        brsp = (xwlua_br_sp *)luaL_checkudata(L, 1, "xwlua_br_sp");
        to = (xwtm_t)luaL_checknumber(L, 2);
        rc = xwos_br_wait_to(brsp->br, to);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_brsp_indexmethod[] = {
        {"bmp", xwlua_brsp_bmp},
        {"num", xwlua_brsp_num},
        {"bind", xwlua_brsp_bind},
        {"unbind", xwlua_brsp_unbind},
        {"wait", xwlua_brsp_wait},
        {"wait_to", xwlua_brsp_wait_to},
        {NULL, NULL},
};

void xwlua_os_init_brsp(lua_State * L)
{
        /* metatable for xwlua_br_sp */
        luaL_newmetatable(L, "xwlua_br_sp");
        luaL_setfuncs(L, xwlua_brsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_brsp_indexmethod); /* create br method table */
        luaL_setfuncs(L, xwlua_brsp_indexmethod, 0); /* add br indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
