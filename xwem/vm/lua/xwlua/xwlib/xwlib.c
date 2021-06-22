/**
 * @file
 * @brief 玄武Lua库：通用函数库
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
