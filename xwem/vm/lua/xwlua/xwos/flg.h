/**
 * @file
 * @brief 玄武Lua库：事件标志
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

#ifndef __xwem_vm_lua_xwlua_xwos_flg_h__
#define __xwem_vm_lua_xwlua_xwos_flg_h__

#include <xwos/standard.h>
#include <xwos/osal/sync/flg.h>
#include "src/lauxlib.h"

typedef xwos_flg_d xwlua_flg_sp; /**< 信号量的强指针 */
#define XWLUA_FLG_NULLSP XWOS_FLG_NILD

void xwlua_os_open_flg(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/flg.h */
