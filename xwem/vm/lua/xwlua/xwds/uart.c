/**
 * @file
 * @brief XWLUA库：UART控制器
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
#include <xwcd/ds/uart/controller.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwds/uart.h"

void xwlua_uart_register(lua_State * L, const char * name,
                         struct xwds_uartc * uartc)
{
        struct xwlua_uart * luart;

        luart = lua_newuserdatauv(L, sizeof(struct xwlua_uart), 0);
        luart->uartc = uartc;
        luart->tik = uartc->dev.obj.xwobj.tik;
        luaL_setmetatable(L, "xwlua_uart");
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, name);
        lua_setglobal(L, name);
}

void xwlua_uart_unregister(lua_State * L, const char * name)
{
        lua_pushnil(L);
        lua_setglobal(L, name);
}

int xwlua_uart_tostring(lua_State * L)
{
        struct xwlua_uart * luart;

        luart = (struct xwlua_uart *)luaL_checkudata(L, 1, "xwlua_uart");
        lua_pushstring(L, luart->uartc->dev.name);
        return 1;
}

int xwlua_uart_rx(lua_State * L)
{
        int top;
        struct xwlua_uart * luart;
        xwsz_t size;
        luaL_Buffer b;
        xwu8_t * rxb;
        xwtm_t to;
        xwer_t rc;

        top = lua_gettop(L);
        luart = (struct xwlua_uart *)luaL_checkudata(L, 1, "xwlua_uart");
        size = (xwsz_t)luaL_checkinteger(L, 2);
        if (top >= 3) {
                to = (xwtm_t)luaL_checknumber(L, 3);
        } else {
                to = XWTM_MAX;
        }
        luaL_buffinit(L, &b);
        rxb = (xwu8_t *)luaL_prepbuffsize(&b, size);
        rc = xwds_uartc_rx(luart->uartc, rxb, &size, to);
        luaL_addsize(&b, size);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_pushinteger(L, (lua_Integer)size);
        luaL_pushresult(&b);
        return 3;
}


int xwlua_uart_try_rx(lua_State * L)
{
        struct xwlua_uart * luart;
        xwsz_t size;
        luaL_Buffer b;
        xwu8_t * rxb;
        xwer_t rc;

        luart = (struct xwlua_uart *)luaL_checkudata(L, 1, "xwlua_uart");
        size = (xwsz_t)luaL_checkinteger(L, 2);
        luaL_buffinit(L, &b);
        rxb = (xwu8_t *)luaL_prepbuffsize(&b, size);
        rc = xwds_uartc_try_rx(luart->uartc, rxb, &size);
        luaL_addsize(&b, size);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_pushinteger(L, (lua_Integer)size);
        luaL_pushresult(&b);
        return 3;
}

int xwlua_uart_tx(lua_State * L)
{
        int top;
        struct xwlua_uart * luart;
        xwsz_t size;
        xwsz_t txdsize;
        const xwu8_t * txd;
        xwtm_t to;
        xwer_t rc;

        top = lua_gettop(L);
        luart = (struct xwlua_uart *)luaL_checkudata(L, 1, "xwlua_uart");
        txd = (const xwu8_t *)luaL_checklstring(L, 2, &txdsize);
        if (top >= 3) {
                to = (xwtm_t)luaL_checknumber(L, 3);
        } else {
                to = XWTM_MAX;
        }
        rc = xwds_uartc_tx(luart->uartc, txd, &size, to);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_pushinteger(L, (lua_Integer)size);
        return 1;
}

const luaL_Reg xwlua_uart_indexmethod[] = {
        {"rx", xwlua_uart_rx},
        {"try_rx", xwlua_uart_try_rx},
        {"tx", xwlua_uart_tx},
        {NULL, NULL},
};

const luaL_Reg xwlua_uart_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__tostring", xwlua_uart_tostring},
        {NULL, NULL}
};

void xwlua_ds_init_uart(lua_State * L)
{
        /* metatable for xwlua_uart */
        luaL_newmetatable(L, "xwlua_uart");
        luaL_setfuncs(L, xwlua_uart_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_uart_indexmethod); /* create uart method table */
        luaL_setfuncs(L, xwlua_uart_indexmethod, 0); /* add uart indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}

const luaL_Reg xwlua_uart_libconstructor[] = {
        {"rx", xwlua_uart_rx},
        {"tx", xwlua_uart_tx},
        {NULL, NULL},
};

void xwlua_ds_open_uart(lua_State * L)
{
        xwlua_ds_init_uart(L);
        luaL_newlib(L, xwlua_uart_libconstructor);
}
