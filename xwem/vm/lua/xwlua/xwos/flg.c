/**
 * @file
 * @brief 玄武OS Lua Lib：事件标志
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/flg.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwlib/bmp.h"
#include "xwlua/xwos/sel.h"
#include "xwlua/xwos/flg.h"

int xwlua_flg_new(lua_State * L)
{
        xwer_t rc;
        xwlua_bmp_t * msk;
        xwlua_flg_sp * flgsp;

        if (lua_isnoneornil(L, 1)) {
                flgsp = lua_newuserdatauv(L, sizeof(xwlua_flg_sp), 0);
                rc = xwos_flg_create(&flgsp->flg, NULL);
                if (XWOK == rc) {
                        flgsp->tik = xwos_flg_gettik(flgsp->flg);
                        luaL_setmetatable(L, "xwlua_flg_sp");
                } else {
                        *flgsp = XWLUA_FLG_NULLSP;
                }
        } else {
                msk = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
                flgsp = lua_newuserdatauv(L, sizeof(xwlua_flg_sp), 0);
                rc = xwos_flg_create(&flgsp->flg, msk->bmp);
                if (XWOK == rc) {
                        flgsp->tik = xwos_flg_gettik(flgsp->flg);
                        luaL_setmetatable(L, "xwlua_flg_sp");
                } else {
                        *flgsp = XWLUA_FLG_NULLSP;
                }
        }
        return 1;
}

int xwlua_flg_msk(lua_State * L)
{
        xwsz_t bmpsz;
        xwlua_bmp_t * msk;

        bmpsz = (sizeof(xwbmp_t) * BITS_TO_BMPS(XWOS_FLG_MAXNUM)) +
                sizeof(xwlua_bmp_t);
        msk = lua_newuserdatauv(L, bmpsz, 0);
        msk->bits = XWOS_FLG_MAXNUM;
        xwbmpop_c0all(msk->bmp, msk->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        return 1;
}

const luaL_Reg xwlua_flg_method[] = {
        {"new", xwlua_flg_new},
        {"msk", xwlua_flg_msk},
        {NULL, NULL},
};

void xwlua_os_init_flgsp(lua_State * L);

void xwlua_os_open_flg(lua_State * L)
{
        xwlua_os_init_flgsp(L);
        luaL_newlib(L, xwlua_flg_method);
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
        D = (lua_State *)lua_touserdata(L, 2);
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

int xwlua_flgsp_intr_all(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        rc = xwos_flg_intr_all(flgsp->flg);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_flgsp_read(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwlua_bmp_t * out;
        xwsz_t bmpsz;
        xwer_t rc;

        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_BMPS(XWOS_FLG_MAXNUM)) +
                sizeof(xwlua_bmp_t);
        out = lua_newuserdatauv(L, bmpsz, 0);
        out->bits = XWOS_FLG_MAXNUM;
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

#define XWLUA_FLG_OPT_TRY       0
const char * const xwlua_flg_opt[] = {"t", NULL};

int xwlua_flgsp_wait(lua_State * L)
{
        xwlua_flg_sp * flgsp;
        xwsq_t tgidx;
        xwsq_t trigger;
        bool consumption;
        xwsq_t action;
        xwlua_bmp_t * origin;
        xwlua_bmp_t * msk;
        xwtm_t time;
        int top;
        int opt;
        int type;
        xwer_t rc;

        top = lua_gettop(L);
        flgsp = (xwlua_flg_sp *)luaL_checkudata(L, 1, "xwlua_flg_sp");
        tgidx = luaL_checkoption(L, 2, NULL, xwlua_flg_trigger_opt);
        trigger = xwlua_flg_trigger[tgidx];
        consumption = lua_toboolean(L, 3);
        if (consumption) {
                action = XWOS_FLG_ACTION_CONSUMPTION;
        } else {
                action = XWOS_FLG_ACTION_NONE;
        }
        origin = (xwlua_bmp_t *)luaL_checkudata(L, 4, "xwlua_bmp_t");
        msk = (xwlua_bmp_t *)luaL_checkudata(L, 5, "xwlua_bmp_t");
        if (top >= 5) {
                type = lua_type(L, 5);
                switch (type) {
                case LUA_TNUMBER:
                        time = (xwtm_t)luaL_checknumber(L, 5);
                        rc = xwos_flg_timedwait(flgsp->flg, trigger, action,
                                                origin->bmp, msk->bmp, &time);
                        break;
                case LUA_TSTRING:
                        opt = luaL_checkoption(L, 5, "t", xwlua_flg_opt);
                        switch (opt) {
                        case XWLUA_FLG_OPT_TRY:
                                rc = xwos_flg_trywait(flgsp->flg, trigger, action,
                                                      origin->bmp, msk->bmp);
                                break;
                        default:
                                luaL_error(L, "Invalid arg: %s", lua_tostring(L, 5));
                                rc = -EINVAL;
                                break;
                        }
                        break;
                case LUA_TNIL:
                        rc = xwos_flg_wait(flgsp->flg, trigger, action,
                                           origin->bmp, msk->bmp);
                        break;
                default:
                        luaL_error(L, "Invalid arg type: %s", lua_typename(L, type));
                        rc = -EINVAL;
                        break;
                }
        } else {
                rc = xwos_flg_wait(flgsp->flg, trigger, action,
                                   origin->bmp, msk->bmp);
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_flgsp_method[] = {
        {"bind", xwlua_flgsp_bind},
        {"unbind", xwlua_flgsp_unbind},
        {"intr_all", xwlua_flgsp_intr_all},
        {"read", xwlua_flgsp_read},
        {"s1i", xwlua_flgsp_s1i},
        {"s1m", xwlua_flgsp_s1m},
        {"c0i", xwlua_flgsp_c0i},
        {"c0m", xwlua_flgsp_c0m},
        {"x1i", xwlua_flgsp_x1i},
        {"x1m", xwlua_flgsp_x1m},
        {"wait", xwlua_flgsp_wait},
        {NULL, NULL},
};

void xwlua_os_init_flgsp(lua_State * L)
{
        /* metatable for xwlua_flg_sp */
        luaL_newmetatable(L, "xwlua_flg_sp");
        luaL_setfuncs(L, xwlua_flgsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_flgsp_method); /* create flg method table */
        luaL_setfuncs(L, xwlua_flgsp_method, 0); /* add flg methods */
        lua_setfield(L, -2, "__index");  /* metatable.__index = xwlua_flgsp_method */
        lua_pop(L, 1); /* pop metatable */
}
