/**
 * @file
 * @brief 玄武设备栈 Lua Lib：SOC
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwcd/ds/soc/chip.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwds/soc.h"

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

int xwlua_soc_lpm(lua_State * L)
{
        luaL_checkudata(L, 1, "xwlua_soc");
        return 0;
}

const luaL_Reg xwlua_soc_indexmethod[] = {
        {"lpm", xwlua_soc_lpm},
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
        luaL_newlibtable(L, xwlua_soc_indexmethod); /* create soc method table */
        luaL_setfuncs(L, xwlua_soc_indexmethod, 0); /* add soc indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}

const luaL_Reg xwlua_soc_libconstructor[] = {
        {"lpm", xwlua_soc_lpm},
        {NULL, NULL},
};

void xwlua_ds_open_soc(lua_State * L)
{
        xwlua_ds_init_soc(L);
        luaL_newlib(L, xwlua_soc_libconstructor);
}
