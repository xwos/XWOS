/**
 * @file
 * @brief 玄武Lua库：GPIO
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
#include <xwcd/ds/soc/gpio.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwds/soc.h"
#include "xwlua/xwds/gpio.h"

int xwlua_gpio_req(lua_State * L)
{
        struct xwlua_soc * luasoc;
        xwid_t port;
        xwsq_t pinmsk;
        xwer_t rc;

        luasoc = (struct xwlua_soc *)luaL_checkudata(L, 1, "xwlua_soc");
        port = (xwid_t)luaL_checkinteger(L, 2);
        pinmsk = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwds_gpio_req(luasoc->soc, port, pinmsk);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_gpio_rls(lua_State * L)
{
        struct xwlua_soc * luasoc;
        xwid_t port;
        xwsq_t pinmsk;
        xwer_t rc;

        luasoc = (struct xwlua_soc *)luaL_checkudata(L, 1, "xwlua_soc");
        port = (xwid_t)luaL_checkinteger(L, 2);
        pinmsk = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwds_gpio_rls(luasoc->soc, port, pinmsk);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_gpio_set(lua_State * L)
{
        struct xwlua_soc * luasoc;
        xwid_t port;
        xwsq_t pinmsk;
        xwer_t rc;

        luasoc = (struct xwlua_soc *)luaL_checkudata(L, 1, "xwlua_soc");
        port = (xwid_t)luaL_checkinteger(L, 2);
        pinmsk = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwds_gpio_set(luasoc->soc, port, pinmsk);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_gpio_reset(lua_State * L)
{
        struct xwlua_soc * luasoc;
        xwid_t port;
        xwsq_t pinmsk;
        xwer_t rc;

        luasoc = (struct xwlua_soc *)luaL_checkudata(L, 1, "xwlua_soc");
        port = (xwid_t)luaL_checkinteger(L, 2);
        pinmsk = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwds_gpio_reset(luasoc->soc, port, pinmsk);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_gpio_toggle(lua_State * L)
{
        struct xwlua_soc * luasoc;
        xwid_t port;
        xwsq_t pinmsk;
        xwer_t rc;

        luasoc = (struct xwlua_soc *)luaL_checkudata(L, 1, "xwlua_soc");
        port = (xwid_t)luaL_checkinteger(L, 2);
        pinmsk = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwds_gpio_toggle(luasoc->soc, port, pinmsk);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_gpio_output(lua_State * L)
{
        struct xwlua_soc * luasoc;
        xwid_t port;
        xwsq_t pinmsk;
        xwsq_t out;
        xwer_t rc;

        luasoc = (struct xwlua_soc *)luaL_checkudata(L, 1, "xwlua_soc");
        port = (xwid_t)luaL_checkinteger(L, 2);
        pinmsk = (xwsq_t)luaL_checkinteger(L, 3);
        out = (xwsq_t)luaL_checkinteger(L, 4);
        rc = xwds_gpio_output(luasoc->soc, port, pinmsk, out);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_gpio_input(lua_State * L)
{
        struct xwlua_soc * luasoc;
        xwid_t port;
        xwsq_t pinmsk;
        xwsq_t in;
        xwer_t rc;

        luasoc = (struct xwlua_soc *)luaL_checkudata(L, 1, "xwlua_soc");
        port = (xwid_t)luaL_checkinteger(L, 2);
        pinmsk = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwds_gpio_input(luasoc->soc, port, pinmsk, &in);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_pushinteger(L, (lua_Integer)in);
        return 2;
}

int xwlua_gpio_pin(lua_State * L)
{
        xwsq_t pin, pinmsk;

        luaL_checkudata(L, 1, "xwlua_soc");
        pin = (xwsq_t)luaL_checkinteger(L, 2);
        pinmsk = (xwsq_t)(1 << pin);
        lua_pushinteger(L, (lua_Integer)pinmsk);
        return 1;
}
