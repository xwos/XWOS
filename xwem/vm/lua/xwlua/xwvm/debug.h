/**
 * @file
 * @brief XWLUA库：虚拟机：VM的调试库
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwem_vm_lua_xwlua_xwvm_debug_h__
#define __xwem_vm_lua_xwlua_xwvm_debug_h__

#include <xwos/standard.h>
#include "xwem/vm/lua/src/lua.h"

/**
 * @ingroup xwlua_xwvm
 * @{
 */

/**
 * @brief 显示表的内容
 * @param[in] vm: 虚拟机
 * @param[in] idx: 表在虚拟机栈中的索引
 * @param[in] nest: 嵌套的层数
 * @details
 * 此函数可将索引为 `idx` 的表中的内容显示到终端上。
 *
 * 此函数是递归函数， `nest` 指明此函数递归了多少次。
 */
void xwlua_vm_dump_table(lua_State * vm, int idx, int nest);

/**
 * @brief 显示栈上的内容
 * @param[in] vm: 虚拟机
 * @details
 * 此函数可将虚拟机栈上的内容显示到终端上。
 */
void xwlua_vm_dump_stack(lua_State * vm);

/**
 * @} xwlua_xwvm
 */

#endif /* xwem/vm/lua/xwlua/xwvm/debug.h */
