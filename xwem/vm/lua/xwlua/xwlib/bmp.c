/**
 * @file
 * @brief XWLUA库：通用函数库：位图
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
#include <xwos/lib/xwbop.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwlib/bmp.h"

int xwlua_bmp_new(lua_State * L)
{
        xwsz_t bits;
        xwsz_t bmpsz;
        xwlua_bmp_t * luabmp;

        bits = (xwsz_t)luaL_checkinteger(L, 1);
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(bits)) + sizeof(xwlua_bmp_t);
        luabmp = lua_newuserdatauv(L, bmpsz, 0);
        luabmp->bits = bits;
        xwbmpop_c0all(luabmp->bmp, luabmp->bits);
        luaL_setmetatable(L, "xwlua_bmp_t");
        return 1;
}

const luaL_Reg xwlua_bmp_libconstructor[] = {
        {"new", xwlua_bmp_new},
        {NULL, NULL},
};

int xwlua_bmp_fill(lua_State * L)
{
        xwlua_bmp_t * luabmp;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        xwbmpop_s1all(luabmp->bmp, luabmp->bits);
        return 0;
}

int xwlua_bmp_zero(lua_State * L)
{
        xwlua_bmp_t * luabmp;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        xwbmpop_c0all(luabmp->bmp, luabmp->bits);
        return 0;
}

int xwlua_bmp_assign(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (luabmp->bits != opd->bits) {
                luaL_error(L, "Bit number is mismatched!");
        } else {
                xwbmpop_assign(luabmp->bmp, opd->bmp, luabmp->bits);
        }
        return 0;
}

int xwlua_bmp_weight(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwsz_t w;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        w = xwbmpop_weight(luabmp->bmp, luabmp->bits);
        lua_pushinteger(L, w);
        return 1;
}

int xwlua_bmp_ffs(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwsq_t idx;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        idx = xwbmpop_ffs(luabmp->bmp, luabmp->bits);
        lua_pushinteger(L, idx);
        return 1;
}

int xwlua_bmp_ffz(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwsq_t idx;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        idx = xwbmpop_ffz(luabmp->bmp, luabmp->bits);
        lua_pushinteger(L, idx);
        return 1;
}

int xwlua_bmp_fls(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwsq_t idx;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        idx = xwbmpop_fls(luabmp->bmp, luabmp->bits);
        lua_pushinteger(L, idx);
        return 1;
}

int xwlua_bmp_flz(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwsq_t idx;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        idx = xwbmpop_flz(luabmp->bmp, luabmp->bits);
        lua_pushinteger(L, idx);
        return 1;
}

int xwlua_bmp_s1i(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        int top, i;
        xwsq_t idx;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        top = lua_gettop(L);
        for (i = 2; i <= top; i++) {
                idx = (xwsq_t)luaL_checkinteger(L, i);
                if (idx >= luabmp->bits) {
                        luaL_error(L, "Bit index is out of range!");
                } else {
                        xwbmpop_s1i(luabmp->bmp, idx);
                }
        }
        return 0;
}

int xwlua_bmp_s1m(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (luabmp->bits != opd->bits) {
                luaL_error(L, "Bit number is mismatched!");
        } else {
                xwbmpop_s1m(luabmp->bmp, opd->bmp, luabmp->bits);
        }
        return 0;
}

int xwlua_bmp_c0i(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        int top, i;
        xwsq_t idx;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        top = lua_gettop(L);
        for (i = 2; i <= top; i++) {
                idx = (xwsq_t)luaL_checkinteger(L, i);
                if (idx >= luabmp->bits) {
                        luaL_error(L, "Bit index is out of range!");
                } else {
                        xwbmpop_c0i(luabmp->bmp, idx);
                }
        }
        return 0;
}

int xwlua_bmp_c0m(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (luabmp->bits != opd->bits) {
                luaL_error(L, "Bit number is mismatched!");
        } else {
                xwbmpop_c0m(luabmp->bmp, opd->bmp, luabmp->bits);
        }
        return 0;
}

int xwlua_bmp_x1i(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        int top, i;
        xwsq_t idx;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        top = lua_gettop(L);
        for (i = 2; i <= top; i++) {
                idx = luaL_checkinteger(L, i);
                if (idx >= luabmp->bits) {
                        luaL_error(L, "Bit index is out of range!");
                } else {
                        xwbmpop_x1i(luabmp->bmp, idx);
                }
        }
        return 0;
}

int xwlua_bmp_x1m(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (luabmp->bits != opd->bits) {
                luaL_error(L, "Bit number is mismatched!");
        } else {
                xwbmpop_x1m(luabmp->bmp, opd->bmp, luabmp->bits);
        }
        return 0;
}

int xwlua_bmp_t1i(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwsq_t idx;
        bool res;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        idx = luaL_checkinteger(L, 2);
        if (idx >= luabmp->bits) {
                luaL_error(L, "Bit index is out of range!");
                lua_pushnil(L);
        } else {
                res = xwbmpop_t1i(luabmp->bmp, idx);
                lua_pushboolean(L, (int)res);
        }
        return 1;
}

int xwlua_bmp_t1ma(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;
        bool res;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (luabmp->bits != opd->bits) {
                luaL_error(L, "Bit number is mismatched!");
                lua_pushnil(L);
        } else {
                res = xwbmpop_t1ma(luabmp->bmp, opd->bmp, luabmp->bits);
                lua_pushboolean(L, (int)res);
        }
        return 1;
}

int xwlua_bmp_t1mo(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;
        bool res;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (luabmp->bits != opd->bits) {
                luaL_error(L, "Bit number is mismatched!");
                lua_pushnil(L);
        } else {
                res = xwbmpop_t1mo(luabmp->bmp, opd->bmp, luabmp->bits);
                lua_pushboolean(L, (int)res);
        }
        return 1;
}

int xwlua_bmp_t0ma(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;
        bool res;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (luabmp->bits != opd->bits) {
                luaL_error(L, "Bit number is mismatched!");
                lua_pushnil(L);
        } else {
                res = xwbmpop_t0ma(luabmp->bmp, opd->bmp, luabmp->bits);
                lua_pushboolean(L, (int)res);
        }
        return 1;
}

int xwlua_bmp_t0mo(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;
        bool res;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (luabmp->bits != opd->bits) {
                luaL_error(L, "Bit number is mismatched!");
                lua_pushnil(L);
        } else {
                res = xwbmpop_t0mo(luabmp->bmp, opd->bmp, luabmp->bits);
                lua_pushboolean(L, (int)res);
        }
        return 1;
}

const luaL_Reg xwlua_bmp_indexmethod[] = {
        {"fill", xwlua_bmp_fill},
        {"zero", xwlua_bmp_zero},
        {"assign", xwlua_bmp_assign},
        {"weight", xwlua_bmp_weight},
        {"ffs", xwlua_bmp_ffs},
        {"ffz", xwlua_bmp_ffz},
        {"fls", xwlua_bmp_fls},
        {"flz", xwlua_bmp_flz},
        {"s1i", xwlua_bmp_s1i},
        {"s1m", xwlua_bmp_s1m},
        {"c0i", xwlua_bmp_c0i},
        {"c0m", xwlua_bmp_c0m},
        {"x1i", xwlua_bmp_x1i},
        {"x1m", xwlua_bmp_x1m},
        {"t1i", xwlua_bmp_t1i},
        {"t1ma", xwlua_bmp_t1ma},
        {"t1mo", xwlua_bmp_t1mo},
        {"t0ma", xwlua_bmp_t0ma},
        {"t0mo", xwlua_bmp_t0mo},
        {NULL, NULL}
};

int xwlua_bmp_band(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd1;
        xwlua_bmp_t * opd2;
        xwsz_t bmpsz;

        opd1 = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd2 = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (opd1->bits != opd2->bits) {
                luaL_error(L, "Bit number is mismatched!");
                lua_pushnil(L);
        } else {
                bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(opd1->bits)) +
                        sizeof(xwlua_bmp_t);
                luabmp = lua_newuserdatauv(L, bmpsz, 0);
                luabmp->bits = opd1->bits;
                luaL_setmetatable(L, "xwlua_bmp_t");
                xwbmpop_assign(luabmp->bmp, opd1->bmp, luabmp->bits);
                xwbmpop_and(luabmp->bmp, opd2->bmp, luabmp->bits);
        }
        return 1;
}

int xwlua_bmp_bor(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd1;
        xwlua_bmp_t * opd2;
        xwsz_t bmpsz;

        opd1 = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd2 = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (opd1->bits != opd2->bits) {
                luaL_error(L, "Bit number is mismatched!");
                lua_pushnil(L);
        } else {
                bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(opd1->bits)) +
                        sizeof(xwlua_bmp_t);
                luabmp = lua_newuserdatauv(L, bmpsz, 0);
                luabmp->bits = opd1->bits;
                luaL_setmetatable(L, "xwlua_bmp_t");
                xwbmpop_assign(luabmp->bmp, opd1->bmp, luabmp->bits);
                xwbmpop_or(luabmp->bmp, opd2->bmp, luabmp->bits);
        }
        return 1;
}

int xwlua_bmp_bxor(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd1;
        xwlua_bmp_t * opd2;
        xwsz_t bmpsz;

        opd1 = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd2 = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (opd1->bits != opd2->bits) {
                luaL_error(L, "Bit number is mismatched!");
                lua_pushnil(L);
        } else {
                bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(opd1->bits)) +
                        sizeof(xwlua_bmp_t);
                luabmp = lua_newuserdatauv(L, bmpsz, 0);
                luabmp->bits = opd1->bits;
                luaL_setmetatable(L, "xwlua_bmp_t");
                xwbmpop_assign(luabmp->bmp, opd1->bmp, luabmp->bits);
                xwbmpop_xor(luabmp->bmp, opd2->bmp, luabmp->bits);
        }
        return 1;
}

int xwlua_bmp_bnot(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * opd;
        xwsz_t bmpsz;

        opd = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(opd->bits)) + sizeof(xwlua_bmp_t);
        luabmp = lua_newuserdatauv(L, bmpsz, 0);
        luabmp->bits = opd->bits;
        luaL_setmetatable(L, "xwlua_bmp_t");
        xwbmpop_assign(luabmp->bmp, opd->bmp, luabmp->bits);
        xwbmpop_not(luabmp->bmp, luabmp->bits);
        return 1;
}

int xwlua_bmp_len(lua_State * L)
{
        xwlua_bmp_t * luabmp;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        lua_pushinteger(L, luabmp->bits);
        return 1;
}

int xwlua_bmp_eq(lua_State * L)
{
        xwlua_bmp_t * opd1;
        xwlua_bmp_t * opd2;
        xwssq_t cmp;

        opd1 = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        opd2 = (xwlua_bmp_t *)luaL_checkudata(L, 2, "xwlua_bmp_t");
        if (opd1->bits != opd2->bits) {
                luaL_error(L, "Bit number is mismatched!");
                lua_pushnil(L);
        } else {
                cmp = xwbmpop_cmp(opd1->bmp, opd2->bmp, opd1->bits);
                lua_pushboolean(L, !!(0 == cmp));
        }
        return 1;
}

int xwlua_bmp_tostring(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        luaL_Buffer sb;
        char strbuf[16];
        xwsz_t aysz;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        aysz = BITS_TO_XWBMP_T(luabmp->bits);
        luaL_buffinit(L, &sb);
        while (aysz > 0) {
                aysz--;
#if (LUA_INT_TYPE == LUA_INT_LONGLONG)
                sprintf(strbuf, "%016llX", luabmp->bmp[aysz]);
#else
                sprintf(strbuf, "%08lX", luabmp->bmp[aysz]);
#endif
                luaL_addlstring(&sb, strbuf, 8);
        }
        luaL_pushresult(&sb);
        return 1;
}

int xwlua_bmp_copy(lua_State * L)
{
        xwlua_bmp_t * luabmp;
        xwlua_bmp_t * newluabmp;
        lua_State * D;
        xwsz_t bmpsz;

        luabmp = (xwlua_bmp_t *)luaL_checkudata(L, 1, "xwlua_bmp_t");
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
        if (D) {
                bmpsz = (sizeof(xwbmp_t) * BITS_TO_XWBMP_T(luabmp->bits)) + sizeof(xwlua_bmp_t);
                newluabmp = lua_newuserdatauv(D, bmpsz, 0);
                newluabmp->bits = luabmp->bits;
                luaL_setmetatable(D, "xwlua_bmp_t");
                xwbmpop_assign(newluabmp->bmp, luabmp->bmp, luabmp->bits);
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_bmp_metamethod[] = {
        {"__band", xwlua_bmp_band},
        {"__bor", xwlua_bmp_bor},
        {"__bxor", xwlua_bmp_bxor},
        {"__bnot", xwlua_bmp_bnot},
        {"__len", xwlua_bmp_len},
        {"__eq", xwlua_bmp_eq},
        {"__tostring", xwlua_bmp_tostring},
        {"__copy", xwlua_bmp_copy},
        {"__index", NULL},  /* place holder */
        {NULL, NULL}
};

void xwlua_lib_init_bmp(lua_State * L)
{
        /* metatable for xwlua_bmp_t */
        luaL_newmetatable(L, "xwlua_bmp_t");
        luaL_setfuncs(L, xwlua_bmp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_bmp_indexmethod); /* create bmp indexmethod table */
        luaL_setfuncs(L, xwlua_bmp_indexmethod, 0); /* add bmp indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}

void xwlua_lib_open_bmp(lua_State * L)
{
        xwlua_lib_init_bmp(L);
        luaL_newlib(L, xwlua_bmp_libconstructor);
}
