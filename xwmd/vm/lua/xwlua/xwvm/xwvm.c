/**
 * @file
 * @brief XWLUA库：虚拟机
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
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/xwvm/debug.h"

extern int xwlua_vm_dump(lua_State * vm);

const luaL_Reg xwlua_xwvm_libconstructor[] = {
        {"dump", xwlua_vm_dump},
        {NULL, NULL},
};

LUAMOD_API int xwlua_open_vm(lua_State * L)
{
        luaL_newmetatable(L, "xwlua_vm");
        lua_pop(L, 1); /* pop metatable */

        luaL_newlib(L, xwlua_xwvm_libconstructor);
        return 1;
}
