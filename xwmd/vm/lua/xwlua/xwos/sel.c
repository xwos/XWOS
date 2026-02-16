/**
 * @file
 * @brief XWLUA库：XWOS内核：信号选择器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/sel.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwlib/bmp.h"
#include "xwmd/vm/lua/xwlua/xwos/sel.h"

void xwlua_sel_register(lua_State * L, const char * name,
                        struct xwos_sel * sel)
{
        xwlua_sel_sp * selsp;
        xwer_t rc;

        selsp = lua_newuserdatauv(L, sizeof(xwlua_sel_sp), 0);
        selsp->sel = sel;
        selsp->tik = xwos_sel_get_tik(sel);
        rc = xwos_sel_acquire(*selsp);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_sel_sp");
                lua_setglobal(L, name);
        } else {
                lua_pop(L, 1);
        }
}

int xwlua_sel_new(lua_State * L)
{
        xwer_t rc;
        xwsz_t bitnum;
        xwlua_sel_sp * selsp;

        bitnum = luaL_checkinteger(L, 1);
        selsp = lua_newuserdatauv(L, sizeof(xwlua_sel_sp), 0);
        rc = xwos_sel_create(selsp, bitnum);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_sel_sp");
        } else {
                lua_pop(L, 1);
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_sel_libconstructor[] = {
        {"new", xwlua_sel_new},
        {NULL, NULL},
};

void xwlua_os_init_selsp(lua_State * L);

void xwlua_os_open_sel(lua_State * L)
{
        xwlua_os_init_selsp(L);
        luaL_newlib(L, xwlua_sel_libconstructor);
}

/******** class xwlua_sel_sp ********/
int xwlua_selsp_gc(lua_State * L)
{
        xwlua_sel_sp * selsp;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        xwos_sel_release(*selsp);
        *selsp = XWLUA_SEL_NULLSP;
        return 0;
}

int xwlua_selsp_tostring(lua_State * L)
{
        xwlua_sel_sp * selsp;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        lua_pushfstring(L, "{%p, %d}", selsp->sel, selsp->tik);
        return 1;
}

int xwlua_selsp_copy(lua_State * L)
{
        xwlua_sel_sp * selsp;
        xwlua_sel_sp * newselsp;
        lua_State * D;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
        if (D) {
                xwer_t rc;
                rc = xwos_sel_acquire(*selsp);
                if (XWOK == rc) {
                        newselsp = lua_newuserdatauv(D, sizeof(xwlua_sel_sp), 0);
                        newselsp->sel = selsp->sel;
                        newselsp->tik = selsp->tik;
                        luaL_setmetatable(D, "xwlua_sel_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_selsp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_selsp_gc},
        {"__tostring", xwlua_selsp_tostring},
        {"__copy", xwlua_selsp_copy},
        {NULL, NULL}
};

int xwlua_selsp_bmp(lua_State * L)
{
        xwlua_sel_sp * selsp;
        xwsz_t bitnum;
        xwsz_t bmpsz;
        xwlua_bmp_t * bmp;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        xwos_sel_get_num(selsp->sel, &bitnum);
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(bitnum)) + sizeof(xwlua_bmp_t);
        bmp = lua_newuserdatauv(L, bmpsz, 0);
        bmp->bits = bitnum;
        xwbmpop_c0all(bmp->bmp, bmp->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        return 1;
}

int xwlua_selsp_num(lua_State * L)
{
        xwlua_sel_sp * selsp;
        xwsz_t bitnum;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        xwos_sel_get_num(selsp->sel, &bitnum);
        lua_pushinteger(L, (lua_Integer)bitnum);
        return 1;
}

int xwlua_selsp_bind(lua_State * L)
{
        xwlua_sel_sp * selsp;
        xwlua_sel_sp * dstsp;
        xwsq_t pos;
        xwer_t rc;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        dstsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        pos = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwos_sel_bind(selsp->sel, dstsp->sel, pos);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_selsp_unbind(lua_State * L)
{
        xwlua_sel_sp * selsp;
        xwlua_sel_sp * dstsp;
        xwer_t rc;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        dstsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        rc = xwos_sel_unbind(selsp->sel, dstsp->sel);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_selsp_select(lua_State * L)
{
        xwlua_sel_sp * selsp;
        xwlua_bmp_t * msk;
        xwlua_bmp_t * trg;
        xwsz_t bitnum;
        xwsz_t bmpsz;
        xwer_t rc;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        xwos_sel_get_num(selsp->sel, &bitnum);
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(bitnum)) + sizeof(xwlua_bmp_t);
        trg = lua_newuserdatauv(L, bmpsz, 0);
        trg->bits = bitnum;
        xwbmpop_c0all(trg->bmp, trg->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        rc = xwos_sel_select(selsp->sel, msk->bmp, trg->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_insert(L, -2);
        return 2;
}

int xwlua_selsp_tryselect(lua_State * L)
{
        xwlua_sel_sp * selsp;
        xwlua_bmp_t * msk;
        xwlua_bmp_t * trg;
        xwsz_t bitnum;
        xwsz_t bmpsz;
        xwer_t rc;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        xwos_sel_get_num(selsp->sel, &bitnum);
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(bitnum)) + sizeof(xwlua_bmp_t);
        trg = lua_newuserdatauv(L, bmpsz, 0);
        trg->bits = bitnum;
        xwbmpop_c0all(trg->bmp, trg->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        rc = xwos_sel_tryselect(selsp->sel, msk->bmp, trg->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_insert(L, -2);
        return 2;
}

int xwlua_selsp_select_to(lua_State * L)
{
        xwlua_sel_sp * selsp;
        xwlua_bmp_t * msk;
        xwlua_bmp_t * trg;
        xwtm_t to;
        xwsz_t bitnum;
        xwsz_t bmpsz;
        xwer_t rc;

        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 1, "xwlua_sel_sp");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        xwos_sel_get_num(selsp->sel, &bitnum);
        to = (xwtm_t)luaL_checknumber(L, 3);
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(bitnum)) + sizeof(xwlua_bmp_t);
        trg = lua_newuserdatauv(L, bmpsz, 0);
        trg->bits = bitnum;
        xwbmpop_c0all(trg->bmp, trg->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        rc = xwos_sel_select_to(selsp->sel, msk->bmp, trg->bmp, to);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_insert(L, -2);
        return 2;
}

const luaL_Reg xwlua_selsp_indexmethod[] = {
        {"bmp", xwlua_selsp_bmp},
        {"num", xwlua_selsp_num},
        {"bind", xwlua_selsp_bind},
        {"unbind", xwlua_selsp_unbind},
        {"select", xwlua_selsp_select},
        {"tryselect", xwlua_selsp_tryselect},
        {"select_to", xwlua_selsp_select_to},
        {NULL, NULL},
};

void xwlua_os_init_selsp(lua_State * L)
{
        /* metatable for xwlua_sel_sp */
        luaL_newmetatable(L, "xwlua_sel_sp");
        luaL_setfuncs(L, xwlua_selsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_selsp_indexmethod); /* create sel method table */
        luaL_setfuncs(L, xwlua_selsp_indexmethod, 0); /* add sel indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
