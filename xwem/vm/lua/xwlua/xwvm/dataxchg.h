/**
 * @file
 * @brief 玄武Lua库：XWLUA VM之间的数据交换
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

#ifndef __xwem_vm_lua_xwlua_xwvm_dataxchg_h__
#define __xwem_vm_lua_xwlua_xwvm_dataxchg_h__

#include <xwos/standard.h>
#include "xwem/vm/lua/src/lua.h"

int xwlua_vm_function_writer(lua_State * src, const void * b, size_t size, void * ud);
xwer_t xwlua_vm_copy_function(lua_State * src, int sidx, lua_State * dst);
xwer_t xwlua_vm_copy_userdata(lua_State * src, int idx, lua_State * dst);
xwer_t xwlua_vm_copy_table(lua_State * src, int sidx, lua_State * dst);
xwer_t xwlua_vm_copy_element(lua_State * src, int sidx, lua_State * dst);
xwer_t xwlua_vm_move_element(lua_State * src, int sidx, lua_State * dst);
void xwlua_vm_copy_env(lua_State * src, int sidx, lua_State * dst);

#endif /* xwem/vm/lua/xwlua/xwvm/dataxchg.h */
