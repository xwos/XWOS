/**
 * @file
 * @brief XWLUA库：设备栈：GPIO
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
#include <xwcd/ds/soc/gpio.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwds/soc.h"
#include "xwmd/vm/lua/xwlua/xwds/gpio.h"

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
