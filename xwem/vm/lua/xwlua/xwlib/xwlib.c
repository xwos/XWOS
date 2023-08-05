/**
 * @file
 * @brief XWLUA库：通用函数库
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
#include <stdio.h>
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/xwlib/bmp.h"

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
