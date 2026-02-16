/**
 * @file
 * @brief XWLUA库：虚拟机：VM的调试库
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwvm_debug_h__
#define __xwmd_vm_lua_xwlua_xwvm_debug_h__

#include <xwos/standard.h>
#include "xwmd/vm/lua/src/lua.h"

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

#endif /* xwmd/vm/lua/xwlua/xwvm/debug.h */
