/**
 * @file
 * @brief XWLUA库：XWOS内核
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
#include <xwos/osal/time.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"

int xwlua_xwtm_now(lua_State * L)
{
        xwtm_t now;

        now = xwtm_now();
        lua_pushnumber(L, (lua_Number)now);
        return 1;
}

int xwlua_xwtm_ft(lua_State * L)
{
        xwtm_t time;

        time = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_ft(time);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_nowts(lua_State * L)
{
        xwtm_t nowts;

        nowts = xwtm_nowts();
        lua_pushnumber(L, (lua_Number)nowts);
        return 1;
}

int xwlua_xwtm_fts(lua_State * L)
{
        xwtm_t time;

        time = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_fts(time);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_nowtc(lua_State * L)
{
        xwtm_t nowtc;

        nowtc = xwtm_nowtc();
        lua_pushnumber(L, (lua_Number)nowtc);
        return 1;
}

int xwlua_xwtm_ns(lua_State * L)
{
        xwtm_t ns, time;

        ns = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_ns(ns);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_us(lua_State * L)
{
        xwtm_t us, time;

        us = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_us(us);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_ms(lua_State * L)
{
        xwtm_t ms, time;

        ms = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_ms(ms);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_s(lua_State * L)
{
        xwtm_t s, time;

        s = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_s(s);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_m(lua_State * L)
{
        xwtm_t m, time;

        m = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_m(m);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_h(lua_State * L)
{
        xwtm_t h, time;

        h = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_h(h);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_d(lua_State * L)
{
        xwtm_t d, time;

        d = (xwtm_t)luaL_checknumber(L, 1);
        time = xwtm_d(d);
        lua_pushnumber(L, (lua_Number)time);
        return 1;
}

int xwlua_xwtm_max(lua_State * L)
{
        xwtm_t max;

        max = XWTM_MAX;
        lua_pushnumber(L, (lua_Number)max);
        return 1;
}


const luaL_Reg xwlua_xwtm_libconstructor[] = {
        {"now", xwlua_xwtm_now},
        {"ft", xwlua_xwtm_ft},
        {"nowts", xwlua_xwtm_nowts},
        {"fts", xwlua_xwtm_fts},
        {"nowtc", xwlua_xwtm_nowtc},
        {"ns", xwlua_xwtm_ns},
        {"us", xwlua_xwtm_us},
        {"ms", xwlua_xwtm_ms},
        {"s", xwlua_xwtm_s},
        {"m", xwlua_xwtm_m},
        {"h", xwlua_xwtm_h},
        {"d", xwlua_xwtm_d},
        {"max", xwlua_xwtm_max},
        {NULL, NULL},
};

LUAMOD_API int xwlua_open_tm(lua_State * L)
{
        luaL_newlib(L, xwlua_xwtm_libconstructor);
        return 1;
}
