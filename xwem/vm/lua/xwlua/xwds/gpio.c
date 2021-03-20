/**
 * @file
 * @brief 玄武设备栈 Lua Lib：GPIO
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
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

const luaL_Reg xwlua_gpio_method[] = {
        {"req", xwlua_gpio_req},
        {"rls", xwlua_gpio_rls},
        {"set", xwlua_gpio_set},
        {"reset", xwlua_gpio_reset},
        {"toggle", xwlua_gpio_toggle},
        {"output", xwlua_gpio_output},
        {"input", xwlua_gpio_output},
        {"a", NULL},
        {"b", NULL},
        {"c", NULL},
        {"d", NULL},
        {"e", NULL},
        {"f", NULL},
        {"g", NULL},
        {"h", NULL},
        {"i", NULL},
        {"j", NULL},
        {"k", NULL},
        {"l", NULL},
        {"m", NULL},
        {"n", NULL},
        {"o", NULL},
        {"p", NULL},
        {"q", NULL},
        {"r", NULL},
        {"s", NULL},
        {"t", NULL},
        {"u", NULL},
        {"v", NULL},
        {"w", NULL},
        {"x", NULL},
        {"y", NULL},
        {"z", NULL},
        {"pin0", NULL},
        {"pin1", NULL},
        {"pin2", NULL},
        {"pin3", NULL},
        {"pin4", NULL},
        {"pin5", NULL},
        {"pin6", NULL},
        {"pin7", NULL},
        {"pin8", NULL},
        {"pin9", NULL},
        {"pin10", NULL},
        {"pin11", NULL},
        {"pin12", NULL},
        {"pin13", NULL},
        {"pin14", NULL},
        {"pin15", NULL},
        {"pin16", NULL},
        {"pin17", NULL},
        {"pin18", NULL},
        {"pin19", NULL},
        {"pin20", NULL},
        {"pin21", NULL},
        {"pin22", NULL},
        {"pin23", NULL},
        {"pin24", NULL},
        {"pin25", NULL},
        {"pin26", NULL},
        {"pin27", NULL},
        {"pin28", NULL},
        {"pin29", NULL},
        {"pin30", NULL},
        {"pin31", NULL},
        {NULL, NULL},
};

void xwlua_ds_open_gpio(lua_State * L)
{
        luaL_newlib(L, xwlua_gpio_method);

        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "a");
        lua_pushinteger(L, 1);
        lua_setfield(L, -2, "b");
        lua_pushinteger(L, 2);
        lua_setfield(L, -2, "c");
        lua_pushinteger(L, 3);
        lua_setfield(L, -2, "d");
        lua_pushinteger(L, 4);
        lua_setfield(L, -2, "e");
        lua_pushinteger(L, 5);
        lua_setfield(L, -2, "f");
        lua_pushinteger(L, 6);
        lua_setfield(L, -2, "g");
        lua_pushinteger(L, 7);
        lua_setfield(L, -2, "h");
        lua_pushinteger(L, 8);
        lua_setfield(L, -2, "i");
        lua_pushinteger(L, 9);
        lua_setfield(L, -2, "j");
        lua_pushinteger(L, 10);
        lua_setfield(L, -2, "k");
        lua_pushinteger(L, 11);
        lua_setfield(L, -2, "l");
        lua_pushinteger(L, 12);
        lua_setfield(L, -2, "m");
        lua_pushinteger(L, 13);
        lua_setfield(L, -2, "n");
        lua_pushinteger(L, 14);
        lua_setfield(L, -2, "o");
        lua_pushinteger(L, 15);
        lua_setfield(L, -2, "p");
        lua_pushinteger(L, 16);
        lua_setfield(L, -2, "q");
        lua_pushinteger(L, 17);
        lua_setfield(L, -2, "r");
        lua_pushinteger(L, 18);
        lua_setfield(L, -2, "s");
        lua_pushinteger(L, 19);
        lua_setfield(L, -2, "t");
        lua_pushinteger(L, 20);
        lua_setfield(L, -2, "u");
        lua_pushinteger(L, 21);
        lua_setfield(L, -2, "v");
        lua_pushinteger(L, 22);
        lua_setfield(L, -2, "w");
        lua_pushinteger(L, 23);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, 24);
        lua_setfield(L, -2, "y");
        lua_pushinteger(L, 25);
        lua_setfield(L, -2, "z");

        lua_pushinteger(L, 0x1);
        lua_setfield(L, -2, "pin0");
        lua_pushinteger(L, 0x2);
        lua_setfield(L, -2, "pin1");
        lua_pushinteger(L, 0x4);
        lua_setfield(L, -2, "pin2");
        lua_pushinteger(L, 0x8);
        lua_setfield(L, -2, "pin3");
        lua_pushinteger(L, 0x10);
        lua_setfield(L, -2, "pin4");
        lua_pushinteger(L, 0x20);
        lua_setfield(L, -2, "pin5");
        lua_pushinteger(L, 0x40);
        lua_setfield(L, -2, "pin6");
        lua_pushinteger(L, 0x80);
        lua_setfield(L, -2, "pin7");
        lua_pushinteger(L, 0x100);
        lua_setfield(L, -2, "pin8");
        lua_pushinteger(L, 0x200);
        lua_setfield(L, -2, "pin9");
        lua_pushinteger(L, 0x400);
        lua_setfield(L, -2, "pin10");
        lua_pushinteger(L, 0x800);
        lua_setfield(L, -2, "pin11");
        lua_pushinteger(L, 0x1000);
        lua_setfield(L, -2, "pin12");
        lua_pushinteger(L, 0x2000);
        lua_setfield(L, -2, "pin13");
        lua_pushinteger(L, 0x4000);
        lua_setfield(L, -2, "pin14");
        lua_pushinteger(L, 0x8000);
        lua_setfield(L, -2, "pin15");
        lua_pushinteger(L, 0x10000);
        lua_setfield(L, -2, "pin16");
        lua_pushinteger(L, 0x20000);
        lua_setfield(L, -2, "pin17");
        lua_pushinteger(L, 0x40000);
        lua_setfield(L, -2, "pin18");
        lua_pushinteger(L, 0x80000);
        lua_setfield(L, -2, "pin19");
        lua_pushinteger(L, 0x100000);
        lua_setfield(L, -2, "pin20");
        lua_pushinteger(L, 0x200000);
        lua_setfield(L, -2, "pin21");
        lua_pushinteger(L, 0x400000);
        lua_setfield(L, -2, "pin22");
        lua_pushinteger(L, 0x800000);
        lua_setfield(L, -2, "pin23");
        lua_pushinteger(L, 0x1000000);
        lua_setfield(L, -2, "pin24");
        lua_pushinteger(L, 0x2000000);
        lua_setfield(L, -2, "pin25");
        lua_pushinteger(L, 0x4000000);
        lua_setfield(L, -2, "pin26");
        lua_pushinteger(L, 0x8000000);
        lua_setfield(L, -2, "pin27");
        lua_pushinteger(L, 0x10000000);
        lua_setfield(L, -2, "pin28");
        lua_pushinteger(L, 0x20000000);
        lua_setfield(L, -2, "pin29");
        lua_pushinteger(L, 0x40000000);
        lua_setfield(L, -2, "pin30");
        lua_pushinteger(L, 0x80000000);
        lua_setfield(L, -2, "pin31");
}
