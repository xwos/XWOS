/**
 * @file
 * @brief XWLUA库：XWOS内核：互斥锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwos_mtx_h__
#define __xwmd_vm_lua_xwlua_xwos_mtx_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/mtx.h>
#include "xwmd/vm/lua/src/lauxlib.h"

typedef xwos_mtx_d xwlua_mtx_sp; /**< 互斥锁的强指针 */
#define XWLUA_MTX_NULLSP XWOS_MTX_NILD

void xwlua_os_open_mtx(lua_State * L);

#endif /* xwmd/vm/lua/xwlua/xwos/mtx.h */
