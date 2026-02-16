/**
 * @file
 * @brief XWLUA库：设备栈：SPI主机模式控制器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwcd/ds/spi/master.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwds/spim.h"

void xwlua_spim_register(lua_State * L, const char * name, struct xwds_spim * spim)
{
        struct xwlua_spim * luaspim;

        luaspim = lua_newuserdatauv(L, sizeof(struct xwlua_spim), 0);
        luaspim->spim = spim;
        luaspim->tik = spim->dev.obj.xwobj.tik;
        luaL_setmetatable(L, "xwlua_spim");
        lua_setglobal(L, name);
}

void xwlua_spim_unregister(lua_State * L, const char * name)
{
        lua_pushnil(L);
        lua_setglobal(L, name);
}

int xwlua_spim_tostring(lua_State * L)
{
        struct xwlua_spim * luaspim;

        luaspim = (struct xwlua_spim *)luaL_checkudata(L, 1, "xwlua_spim");
        lua_pushstring(L, luaspim->spim->dev.name);
        return 1;
}

int xwlua_spim_xfer(lua_State * L)
{
        int top;
        struct xwlua_spim * luaspim;
        int txdtype;
        xwid_t cfgid;
        const xwu8_t * txd;
        int isrx;
        luaL_Buffer b;
        xwu8_t * rxb;
        xwsz_t size;
        xwtm_t time;
        xwer_t rc;
        int ret;

        top = lua_gettop(L);
        luaspim = (struct xwlua_spim *)luaL_checkudata(L, 1, "xwlua_spim");
        cfgid = (xwid_t)luaL_checkinteger(L, 2);
        txdtype =  lua_type(L, 3);
        if (LUA_TSTRING == txdtype) {
                txd = (xwu8_t *)luaL_checkstring(L, 3);
        } else {
                txd = NULL;
        }
        isrx = lua_toboolean(L, 4);
        size = (xwsz_t)luaL_checkinteger(L, 5);
        if (isrx) {
                luaL_buffinit(L, &b);
                rxb = (xwu8_t *)luaL_prepbuffsize(&b, size);
        } else {
                rxb = NULL;
        }
        if (top >= 6) {
                time = (xwtm_t)luaL_checknumber(L, 6);
        } else {
                time = XWTM_MAX;
        }
        rc = xwds_spim_xfer(luaspim->spim, cfgid, txd, rxb, &size, time);
        if (isrx) {
                luaL_addsize(&b, size);
        }
        lua_pushinteger(L, (lua_Integer)rc);
        lua_pushinteger(L, (lua_Integer)size);
        if (isrx) {
                luaL_pushresult(&b);
                ret = 3;
        } else {
                ret = 2;
        }
        return ret;
}

const luaL_Reg xwlua_spim_indexmethod[] = {
        {"xfer", xwlua_spim_xfer},
        {NULL, NULL},
};

const luaL_Reg xwlua_spim_metamethod[] = {
        {"__tostring", xwlua_spim_tostring},
        {NULL, NULL}
};

void xwlua_ds_init_spim(lua_State * L)
{
        /* metatable for xwlua_spim */
        luaL_newmetatable(L, "xwlua_spim");
        luaL_setfuncs(L, xwlua_spim_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_spim_indexmethod); /* create spim method table */
        luaL_setfuncs(L, xwlua_spim_indexmethod, 0); /* add spim indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}

const luaL_Reg xwlua_spim_libconstructor[] = {
        {"xfer", xwlua_spim_xfer},
        {NULL, NULL},
};

void xwlua_ds_open_spim(lua_State * L)
{
        xwlua_ds_init_spim(L);
        luaL_newlib(L, xwlua_spim_libconstructor);
}
