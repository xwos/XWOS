/**
 * @file
 * @brief XWLUA库：通用函数库
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <stdio.h>
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/xwlib/bmp.h"

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
