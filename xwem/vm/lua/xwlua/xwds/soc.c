/**
 * @file
 * @brief 玄武设备栈 Lua Lib：SOC
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwcd/ds/soc/chip.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwds/soc.h"

void xwlua_soc_register(lua_State * L, const char * name, struct xwds_soc * soc)
{
        lua_pushlightuserdata(L, soc);
        luaL_setmetatable(L, "xwds_soc");
        lua_setglobal(L, name);
}

void xwlua_soc_unregister(lua_State * L, const char * name)
{
        lua_pushnil(L);
        lua_setglobal(L, name);
}

int xwlua_soc_tostring(lua_State * L)
{
        struct xwds_soc * soc;

        soc = (struct xwds_soc *)luaL_checkudata(L, 1, "xwds_soc");
        lua_pushstring(L, soc->dev.name);
        return 1;
}

const luaL_Reg xwlua_soc_metamethod[] = {
        {"__tostring", xwlua_soc_tostring},
        {NULL, NULL}
};

void xwlua_ds_init_soc(lua_State * L)
{
        /* metatable for xwds_soc */
        luaL_newmetatable(L, "xwds_soc");
        luaL_setfuncs(L, xwlua_soc_metamethod, 0); /* add metamethods */
        lua_pop(L, 1); /* pop metatable xwds_soc */
}

const luaL_Reg xwlua_soc_method[] = {
        {NULL, NULL},
};

void xwlua_ds_open_soc(lua_State * L)
{
        xwlua_ds_init_soc(L);
        luaL_newlib(L, xwlua_soc_method);
}
