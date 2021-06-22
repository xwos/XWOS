/**
 * @file
 * @brief 玄武Lua库：调度
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

#ifndef __xwem_vm_lua_xwlua_xwos_skd_h__
#define __xwem_vm_lua_xwlua_xwos_skd_h__

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include "src/lauxlib.h"

typedef xwos_thd_d xwlua_thd_sp; /**< 线程的强指针 */
#define XWLUA_THD_NULLSP XWOS_THD_NILD

void xwlua_os_open_skd(lua_State * L);
void xwlua_os_open_thd(lua_State * L);
void xwlua_os_open_cthd(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/skd.h */
