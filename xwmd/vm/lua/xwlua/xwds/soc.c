/**
 * @file
 * @brief XWLUA库：设备栈：SOC
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
#include <xwcd/ds/soc/chip.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwds/gpio.h"
#include "xwmd/vm/lua/xwlua/xwds/soc.h"

void xwlua_soc_register(lua_State * L, const char * name, struct xwds_soc * soc)
{
        struct xwlua_soc * luasoc;

        luasoc = lua_newuserdatauv(L, sizeof(struct xwlua_soc), 0);
        luasoc->soc = soc;
        luasoc->tik = soc->dev.obj.xwobj.tik;
        luaL_setmetatable(L, "xwlua_soc");
        lua_setglobal(L, name);
}

void xwlua_soc_unregister(lua_State * L, const char * name)
{
        lua_pushnil(L);
        lua_setglobal(L, name);
}

int xwlua_soc_tostring(lua_State * L)
{
        struct xwlua_soc * luasoc;

        luasoc = (struct xwlua_soc *)luaL_checkudata(L, 1, "xwlua_soc");
        lua_pushstring(L, luasoc->soc->dev.name);
        return 1;
}

const luaL_Reg xwlua_soc_indexmethod[] = {
        {"pin", xwlua_gpio_pin},
        {"req", xwlua_gpio_req},
        {"rls", xwlua_gpio_rls},
        {"set", xwlua_gpio_set},
        {"reset", xwlua_gpio_reset},
        {"toggle", xwlua_gpio_toggle},
        {"output", xwlua_gpio_output},
        {"input", xwlua_gpio_output},
        {NULL, NULL},
};

const luaL_Reg xwlua_soc_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__tostring", xwlua_soc_tostring},
        {NULL, NULL}
};

void xwlua_ds_init_soc(lua_State * L)
{
        /* metatable for xwlua_soc */
        luaL_newmetatable(L, "xwlua_soc");
        luaL_setfuncs(L, xwlua_soc_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_soc_indexmethod); /* create soc indexmethod table */
        luaL_setfuncs(L, xwlua_soc_indexmethod, 0); /* add soc indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}

const luaL_Reg xwlua_soc_libconstructor[] = {
        {NULL, NULL},
};

void xwlua_ds_open_soc(lua_State * L)
{
        xwlua_ds_init_soc(L);
        luaL_newlib(L, xwlua_soc_libconstructor);
}
