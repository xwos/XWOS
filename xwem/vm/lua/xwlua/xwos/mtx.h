/**
 * @file
 * @brief XWLUA库：XWOS内核：互斥锁
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

#ifndef __xwem_vm_lua_xwlua_xwos_mtx_h__
#define __xwem_vm_lua_xwlua_xwos_mtx_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/mtx.h>
#include "xwem/vm/lua/src/lauxlib.h"

typedef xwos_mtx_d xwlua_mtx_sp; /**< 互斥锁的强指针 */
#define XWLUA_MTX_NULLSP XWOS_MTX_NILD

void xwlua_os_open_mtx(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/mtx.h */
