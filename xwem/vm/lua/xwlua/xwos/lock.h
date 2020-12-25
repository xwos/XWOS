/**
 * @file
 * @brief 玄武OS Lua Lib：锁容器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwem_vm_lua_xwlua_xwos_lock_h__
#define __xwem_vm_lua_xwlua_xwos_lock_h__

#include <xwos/standard.h>
#include "src/lauxlib.h"

void xwlua_os_open_lock(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/lock.h */
