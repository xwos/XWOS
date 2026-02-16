/**
 * @file
 * @brief XWLUA库：XWOS内核：自旋锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwos_spinlock_h__
#define __xwmd_vm_lua_xwlua_xwos_spinlock_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/osal/lock/spinlock.h>
#include "xwmd/vm/lua/src/lauxlib.h"

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

xwsq_t xwlua_splk_get_tik(struct xwlua_splk * luasplk);
xwlua_splk_sp xwlua_splk_get_d(struct xwlua_splk * luasplk);
xwer_t xwlua_splk_acquire(xwlua_splk_sp splksp);
xwer_t xwlua_splk_release(xwlua_splk_sp splksp);
xwer_t xwlua_splk_grab(struct xwlua_splk * luasplk);
xwer_t xwlua_splk_put(struct xwlua_splk * luasplk);

#endif /* xwmd/vm/lua/xwlua/xwos/spinlock.h */
