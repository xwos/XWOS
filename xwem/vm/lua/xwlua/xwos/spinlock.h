/**
 * @file
 * @brief 玄武OS Lua Lib：自旋锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwem_vm_lua_xwlua_xwos_spinlock_h__
#define __xwem_vm_lua_xwlua_xwos_spinlock_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include "src/lauxlib.h"

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
void xwlua_splk_unlock(struct xwlua_splk * luasplk);

#endif /* xwem/vm/lua/xwlua/xwos/spinlock.h */
