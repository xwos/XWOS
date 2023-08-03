/**
 * @file
 * @brief 玄武Lua库：自旋锁
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

#ifndef __xwem_vm_lua_xwlua_xwos_spinlock_h__
#define __xwem_vm_lua_xwlua_xwos_spinlock_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/osal/lock/spinlock.h>
#include "xwem/vm/lua/src/lauxlib.h"

enum xwlua_splk_lock_state_em {
        XWLUA_SPLK_LKST_UNLOCK,
        XWLUA_SPLK_LKST_LOCK,
};

struct xwlua_splk {
        struct xwos_object xwobj;
        struct xwos_splk ossplk;
        enum xwlua_splk_lock_state_em lkst;
};

typedef struct {
        struct xwlua_splk * luasplk;
        xwsq_t tik;
} xwlua_splk_sp;

#define XWLUA_SPLK_NULLSP ((xwlua_splk_sp){NULL, 0,})

void xwlua_os_open_splk(lua_State * L);

xwsq_t xwlua_splk_gettik(struct xwlua_splk * luasplk);
xwlua_splk_sp xwlua_splk_getd(struct xwlua_splk * luasplk);
xwer_t xwlua_splk_acquire(xwlua_splk_sp splksp);
xwer_t xwlua_splk_release(xwlua_splk_sp splksp);
xwer_t xwlua_splk_grab(struct xwlua_splk * luasplk);
xwer_t xwlua_splk_put(struct xwlua_splk * luasplk);

#endif /* xwem/vm/lua/xwlua/xwos/spinlock.h */
