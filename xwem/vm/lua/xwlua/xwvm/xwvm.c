/**
 * @file
 * @brief XWLUA库：虚拟机
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
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/xwvm/debug.h"
#include "xwem/vm/lua/xwlua/xwvm/mq.h"

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
