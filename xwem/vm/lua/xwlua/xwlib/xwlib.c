/**
 * @file
 * @brief 玄武 Lua Lib：C库
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <stdio.h>
#include "xwlua/port.h"
#include "src/lauxlib.h"
#include "xwlua/xwlib/bmp.h"

int xwlua_lib_dumpstack(lua_State * L)
{
        int i;
        int top;

        top = lua_gettop(L);
        printf("top:%d\n", top);
        for (i = 1; i <= top; i++) {
                int t = lua_type(L, i);
                switch (t) {
                case LUA_TBOOLEAN:
                        printf("%s\n", lua_toboolean(L, i)? "true" : "false");
                        break;
                case LUA_TNUMBER:
                        printf("%g\n", lua_tonumber(L, i));
                        break;
                case LUA_TSTRING:
                        printf("\"%s\"\n", lua_tostring(L, i));
                        break;
                default:
                        printf("%s\n", lua_typename(L, t));
                        break;
                }
        }
        printf("\n");
        return 0;
}

const luaL_Reg xwlua_lib[] = {
        {"bmp", NULL},
        {NULL, NULL},
};

LUAMOD_API int xwlua_open_lib(lua_State * L)
{
        luaL_newlib(L, xwlua_lib);
        xwlua_lib_open_bmp(L);
        lua_setfield(L, -2, "bmp");
        return 1;
}
