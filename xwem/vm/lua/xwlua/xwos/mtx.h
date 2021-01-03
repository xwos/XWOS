/**
 * @file
 * @brief 玄武OS Lua Lib：互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwem_vm_lua_xwlua_xwos_mtx_h__
#define __xwem_vm_lua_xwlua_xwos_mtx_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/mtx.h>
#include "src/lauxlib.h"

typedef xwos_mtx_d xwlua_mtx_sp; /**< 互斥锁的强指针 */
#define XWLUA_MTX_NULLSP XWOS_MTX_NILD

void xwlua_os_open_mtx(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/mtx.h */
