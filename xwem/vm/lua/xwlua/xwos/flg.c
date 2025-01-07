/**
 * @file
 * @brief XWLUA库：XWOS内核：事件标志
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
#include <xwos/osal/sync/flg.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwlib/bmp.h"
#include "xwem/vm/lua/xwlua/xwos/sel.h"
#include "xwem/vm/lua/xwlua/xwos/flg.h"

void xwlua_flg_register(lua_State * L, const char * name,
                        struct xwos_flg * flg)
{
        xwlua_flg_sp * flgsp;
        xwer_t rc;

        flgsp = lua_newuserdatauv(L, sizeof(xwlua_flg_sp), 0);
        flgsp->flg = flg;
        flgsp->tik = xwos_flg_get_tik(flg);
        rc = xwos_flg_acquire(*flgsp);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_flg_sp");
                lua_setglobal(L, name);
        } else {
                lua_pop(L, 1);
        }
}

int xwlua_flg_new(lua_State * L)
{
        xwer_t rc;
        xwsz_t bitnum;
        xwlua_flg_sp * flgsp;

        bitnum = luaL_checkinteger(L, 1);
        flgsp = lua_newuserdatauv(L, sizeof(xwlua_flg_sp), 0);
        rc = xwos_flg_create(flgsp, bitnum);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_flg_sp");
        } else {
                lua_pop(L, 1);
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_flg_libconstructor[] = {
        {"new", xwlua_flg_new},
        {NULL, NULL},
};

void xwlua_os_init_flgsp(lua_State * L);

void xwlua_os_open_flg(lua_State * L)
{
        xwlua_os_init_flgsp(L);
        luaL_newlib(L, xwlua_flg_libconstructor);
}

/******** class xwlua_flg_sp ********/
int xwlua_flgsp_gc(lua_State * L)
{
        xwlua_flg_sp * flgsp;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        xwos_flg_release(*flgsp);
        *flgsp = XWLUA_FLG_NULLSP;
        return 0;
}

int xwlua_flgsp_tostring(lua_State * L)
{
        xwlua_flg_sp * flgsp;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        lua_pushfstring(L, "{%p, %d}", flgsp->flg, flgsp->tik);
        return 1;
}

int xwlua_flgsp_copy(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwlua_flg_sp * newflgsp;
        lua_State * D;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
        if (D) {
                xwer_t rc;
                rc = xwos_flg_acquire(*flgsp);
                if (XWOK == rc) {
                        newflgsp = lua_newuserdatauv(D, sizeof(xwlua_flg_sp), 0);
                        newflgsp->flg = flgsp->flg;
                        newflgsp->tik = flgsp->tik;
                        luaL_setmetatable(D, "xwlua_flg_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_flgsp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_flgsp_gc},
        {"__tostring", xwlua_flgsp_tostring},
        {"__copy", xwlua_flgsp_copy},
        {NULL, NULL}
};

int xwlua_flgsp_bmp(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsz_t bitnum;
        xwsz_t bmpsz;
        xwlua_bmp_t * bmp;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        xwos_flg_get_num(flgsp->flg, &bitnum);
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(bitnum)) + sizeof(xwlua_bmp_t);
        bmp = lua_newuserdatauv(L, bmpsz, 0);
        bmp->bits = bitnum;
        xwbmpop_c0all(bmp->bmp, bmp->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        return 1;
}

int xwlua_flgsp_num(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsz_t bitnum;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        xwos_flg_get_num(flgsp->flg, &bitnum);
        lua_pushinteger(L, (lua_Integer)bitnum);
        return 1;
}

int xwlua_flgsp_bind(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwlua_sel_sp * selsp;
        xwsq_t pos;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        pos = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwos_flg_bind(flgsp->flg, selsp->sel, pos);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_unbind(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwlua_sel_sp * selsp;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        rc = xwos_flg_unbind(flgsp->flg, selsp->sel);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_read(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwlua_bmp_t * out;
        xwsz_t bitnum;
        xwsz_t bmpsz;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        xwos_flg_get_num(flgsp->flg, &bitnum);
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(bitnum)) + sizeof(xwlua_bmp_t);
        out = lua_newuserdatauv(L, bmpsz, 0);
        out->bits = bitnum;
        xwbmpop_c0all(out->bmp, out->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        rc = xwos_flg_read(flgsp->flg, out->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_insert(L, -2);
        return 2;
}

int xwlua_flgsp_s1m(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwlua_bmp_t * msk;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        rc = xwos_flg_s1m(flgsp->flg, msk->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_s1i(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsq_t pos;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        pos = (xwsq_t)luaL_checkinteger(L, 2);
        rc = xwos_flg_s1i(flgsp->flg, pos);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_c0m(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwlua_bmp_t * msk;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        rc = xwos_flg_c0m(flgsp->flg, msk->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_c0i(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsq_t pos;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        pos = (xwsq_t)luaL_checkinteger(L, 2);
        rc = xwos_flg_c0i(flgsp->flg, pos);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_x1m(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwlua_bmp_t * msk;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        rc = xwos_flg_x1m(flgsp->flg, msk->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_x1i(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsq_t pos;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        pos = (xwsq_t)luaL_checkinteger(L, 2);
        rc = xwos_flg_x1i(flgsp->flg, pos);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const char * const xwlua_flg_trigger_opt[] = {
        "sa", "so", "ca", "co", "ta", "to", NULL,
};
const xwsq_t xwlua_flg_trigger[] = {
        XWOS_FLG_TRIGGER_SET_ALL,
        XWOS_FLG_TRIGGER_CLR_ANY,
        XWOS_FLG_TRIGGER_SET_ALL,
        XWOS_FLG_TRIGGER_CLR_ANY,
        XWOS_FLG_TRIGGER_TGL_ALL,
        XWOS_FLG_TRIGGER_TGL_ANY,
};

int xwlua_flgsp_wait(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsq_t tgidx;
        xwsq_t trigger;
        bool consumption;
        xwlua_bmp_t * origin;
        xwlua_bmp_t * msk;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        tgidx = luaL_checkoption(L, 2, NULL, xwlua_flg_trigger_opt);
        trigger = xwlua_flg_trigger[tgidx];
        consumption = lua_toboolean(L, 3);
        origin = (xwlua_bmp_t *)luaL_checkudata(L, 4, "xwlua_bmp_t");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 5, "xwlua_bmp_t");
        rc = xwos_flg_wait(flgsp->flg, trigger, consumption,
                           origin->bmp, msk->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_trywait(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsq_t tgidx;
        xwsq_t trigger;
        bool consumption;
        xwlua_bmp_t * origin;
        xwlua_bmp_t * msk;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        tgidx = luaL_checkoption(L, 2, NULL, xwlua_flg_trigger_opt);
        trigger = xwlua_flg_trigger[tgidx];
        consumption = lua_toboolean(L, 3);
        origin = (xwlua_bmp_t *)luaL_checkudata(L, 4, "xwlua_bmp_t");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 5, "xwlua_bmp_t");
        rc = xwos_flg_trywait(flgsp->flg, trigger, consumption,
                              origin->bmp, msk->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_wait_to(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsq_t tgidx;
        xwsq_t trigger;
        bool consumption;
        xwlua_bmp_t * origin;
        xwlua_bmp_t * msk;
        xwtm_t to;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        tgidx = luaL_checkoption(L, 2, NULL, xwlua_flg_trigger_opt);
        trigger = xwlua_flg_trigger[tgidx];
        consumption = lua_toboolean(L, 3);
        origin = (xwlua_bmp_t *)luaL_checkudata(L, 4, "xwlua_bmp_t");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 5, "xwlua_bmp_t");
        to = (xwtm_t)luaL_checknumber(L, 6);
        rc = xwos_flg_wait_to(flgsp->flg, trigger, consumption,
                              origin->bmp, msk->bmp, to);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_wait_unintr(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsq_t tgidx;
        xwsq_t trigger;
        bool consumption;
        xwlua_bmp_t * origin;
        xwlua_bmp_t * msk;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        tgidx = luaL_checkoption(L, 2, NULL, xwlua_flg_trigger_opt);
        trigger = xwlua_flg_trigger[tgidx];
        consumption = lua_toboolean(L, 3);
        origin = (xwlua_bmp_t *)luaL_checkudata(L, 4, "xwlua_bmp_t");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 5, "xwlua_bmp_t");
        rc = xwos_flg_wait_unintr(flgsp->flg, trigger, consumption,
                                  origin->bmp, msk->bmp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_flgsp_indexmethod[] = {
        {"bmp", xwlua_flgsp_bmp},
        {"num", xwlua_flgsp_num},
        {"bind", xwlua_flgsp_bind},
        {"unbind", xwlua_flgsp_unbind},
        {"read", xwlua_flgsp_read},
        {"s1i", xwlua_flgsp_s1i},
        {"s1m", xwlua_flgsp_s1m},
        {"c0i", xwlua_flgsp_c0i},
        {"c0m", xwlua_flgsp_c0m},
        {"x1m", xwlua_flgsp_x1m},
        {"x1i", xwlua_flgsp_x1i},
        {"wait", xwlua_flgsp_wait},
        {"wait_to", xwlua_flgsp_wait_to},
        {"trywait", xwlua_flgsp_trywait},
        {"wait_unintr", xwlua_flgsp_wait_to},
        {NULL, NULL},
};

void xwlua_os_init_flgsp(lua_State * L)
{
        /* metatable for xwlua_flg_sp */
        luaL_newmetatable(L, "xwlua_flg_sp");
        luaL_setfuncs(L, xwlua_flgsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_flgsp_indexmethod); /* create flg method table */
        luaL_setfuncs(L, xwlua_flgsp_indexmethod, 0); /* add flg indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
