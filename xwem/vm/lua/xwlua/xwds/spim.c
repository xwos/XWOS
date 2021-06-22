/**
 * @file
 * @brief 玄武Lua库：SPI master
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#include <string.h>
#include <xwcd/ds/spi/master.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwds/spim.h"

void xwlua_spim_register(lua_State * L, const char * name, struct xwds_spim * spim)
{
        struct xwlua_spim * luaspim;

        luaspim = lua_newuserdatauv(L, sizeof(struct xwlua_spim), 0);
        luaspim->spim = spim;
        luaspim->tik = spim->dev.obj.xwobj.tik;
        luaL_setmetatable(L, "xwlua_spim");
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, name);
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
        txdtype =  lua_type(L, 2);
        if (LUA_TSTRING == txdtype) {
                txd = (xwu8_t *)luaL_checkstring(L, 2);
        } else {
                txd = NULL;
        }
        isrx = lua_toboolean(L, 3);
        if (isrx) {
                luaL_buffinit(L, &b);
                rxb = (xwu8_t *)luaL_prepbuffer(&b);
        } else {
                rxb = NULL;
        }
        size = (xwsz_t)luaL_checkinteger(L, 4);
        if (top >= 5) {
                time = (xwtm_t)luaL_checknumber(L, 5);
        } else {
                time = XWTM_MAX;
        }
        rc = xwds_spim_xfer(luaspim->spim, txd, rxb, size, &time);
        lua_pushinteger(L, (lua_Integer)rc);
        if (isrx) {
                luaL_pushresult(&b);
                ret = 2;
        } else {
                ret = 1;
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
