/**
 * @file
 * @brief XWLUA库：SOC
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
#include <string.h>
#include <xwcd/ds/soc/chip.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwds/gpio.h"
#include "xwem/vm/lua/xwlua/xwds/soc.h"

void xwlua_soc_register(lua_State * L, const char * name, struct xwds_soc * soc)
{
        struct xwlua_soc * luasoc;

        luasoc = lua_newuserdatauv(L, sizeof(struct xwlua_soc), 0);
        luasoc->soc = soc;
        luasoc->tik = soc->dev.obj.xwobj.tik;
        luaL_setmetatable(L, "xwlua_soc");
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, name);
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
