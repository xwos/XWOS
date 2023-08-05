/**
 * @file
 * @brief XWLUA库：XWLUA VM的调试库
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

void xwlua_vm_dump_table(lua_State * vm, int idx, int nest)
{
        bool hasmt;
        int type;
        int i;

        printf("{\n");
        idx = lua_absindex(vm, idx);
        hasmt = lua_getmetatable(vm, idx); /* Push metatable */
        if (hasmt) {
                for (i = 0; i < nest; i++) {
                        printf("\t");
                }
                printf("[__metatable] = ");
                xwlua_vm_dump_table(vm, -1, nest + 1);
                printf("\n");
                lua_pop(vm, 1); /* Pop metatable */
        }
        lua_pushnil(vm); /* first key */
        while (0 != lua_next(vm, idx)) {
                for (i = 0; i < nest; i++) {
                        printf("\t");
                }
                type = lua_type(vm, -2); /* 'key' : index -2 */
                switch (type) {
                case LUA_TBOOLEAN:
                        printf("[boolean:%s] = ",
                               lua_toboolean(vm, -2) ? "true" : "false");
                        break;
                case LUA_TNUMBER:
                        printf("[%g] = ", lua_tonumber(vm, -2));
                        break;
                case LUA_TSTRING:
                        printf("\"%s\" = ", lua_tostring(vm, -2));
                        break;
                case LUA_TTABLE:
                        printf("[%s: ", lua_typename(vm, type));
                        xwlua_vm_dump_table(vm, -2, nest + 1);
                        printf("] = ");
                        break;
                case LUA_TFUNCTION:
                case LUA_TUSERDATA:
                case LUA_TLIGHTUSERDATA:
                        printf("[%s] = ", lua_typename(vm, type));
                        break;
                default:
                        printf("[%s] = ", lua_typename(vm, type));
                        break;
                }

                type = lua_type(vm, -1); /* 'value' : index -1 */
                switch (type) {
                case LUA_TBOOLEAN:
                        printf("%s,\n",
                               lua_toboolean(vm, -1) ? "true" : "false");
                        break;
                case LUA_TNUMBER:
                        printf("%g,\n", lua_tonumber(vm, -1));
                        break;
                case LUA_TSTRING:
                        printf("\"%s\",\n", lua_tostring(vm, -1));
                        break;
                case LUA_TTABLE:
                        xwlua_vm_dump_table(vm, -1, nest + 1);
                        printf("\n");
                        break;
                case LUA_TFUNCTION:
                case LUA_TUSERDATA:
                case LUA_TLIGHTUSERDATA:
                        printf("%s,\n", lua_typename(vm, type));
                        break;
                default:
                        printf("%s,\n", lua_typename(vm, type));
                        break;
                }
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(vm, 1);
        }
        for (i = 0; i < nest - 1; i++) {
                printf("\t");
        }
        printf("},");
}

void xwlua_vm_dump_stack(lua_State * vm)
{
        int i;
        int num;
        int type;

        num = lua_gettop(vm);
        printf("++++++++ stack depth: %d ++++++++\n", num);
        for (i = num; i >= 1; i--) {
                type = lua_type(vm, i);
                switch (type) {
                case LUA_TBOOLEAN:
                        printf("boolean: %s\n",
                               lua_toboolean(vm, i) ? "true" : "false");
                        break;
                case LUA_TNUMBER:
                        printf("number: %g\n", lua_tonumber(vm, i));
                        break;
                case LUA_TSTRING:
                        printf("string: \"%s\"\n", lua_tostring(vm, i));
                        break;
                case LUA_TTABLE:
                        printf("table: ");
                        xwlua_vm_dump_table(vm, i, 1);
                        printf("\n");
                        break;
                case LUA_TFUNCTION:
                case LUA_TUSERDATA:
                case LUA_TLIGHTUSERDATA:
                        printf("%s\n", lua_typename(vm, type));
                        break;
                default:
                        printf("%s\n", lua_typename(vm, type));
                        break;
                }
        }
        printf("-------- stack depth: %d --------\n", num);
}

int xwlua_vm_dump(lua_State * vm)
{
        int type;

        type = lua_type(vm, -1);
        switch (type) {
        case LUA_TBOOLEAN:
                printf("boolean: %s\n", lua_toboolean(vm, -1) ? "true" : "false");
                break;
        case LUA_TNUMBER:
                printf("number: %g\n", lua_tonumber(vm, -1));
                break;
        case LUA_TSTRING:
                printf("string: \"%s\"\n", lua_tostring(vm, -1));
                break;
        case LUA_TTABLE:
                printf("table: ");
                xwlua_vm_dump_table(vm, -1, 1);
                printf("\n");
                break;
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
                printf("%s\n", lua_typename(vm, type));
                break;
        default:
                printf("%s\n", lua_typename(vm, type));
                break;
        }
        return 0;
}
