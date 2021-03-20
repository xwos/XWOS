/**
 * @file
 * @brief 玄武OS Lua Lib：顺序锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_xwos_seqlock_h__
#define __xwem_vm_lua_xwlua_xwos_seqlock_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/seqlock.h>
#include "src/lauxlib.h"

enum xwlua_sqlk_lock_state_em {
        XWLUA_SQLK_LKST_UNLOCK,
        XWLUA_SQLK_LKST_RDEX_LOCK,
        XWLUA_SQLK_LKST_WR_LOCK,
};

struct xwlua_sqlk {
        struct xwos_object xwobj;
        struct xwos_sqlk ossqlk;
        enum xwlua_sqlk_lock_state_em lkst;
};

typedef struct {
        struct xwlua_sqlk * luasqlk;
        xwsq_t tik;
} xwlua_sqlk_sp;

#define XWLUA_SQLK_NULLSP ((xwlua_sqlk_sp){NULL, 0,})

void xwlua_os_open_sqlk(lua_State * L);

xwsq_t xwlua_sqlk_gettik(struct xwlua_sqlk * luasqlk);
xwlua_sqlk_sp xwlua_sqlk_getd(struct xwlua_sqlk * luasqlk);
xwer_t xwlua_sqlk_acquire(xwlua_sqlk_sp sqlksp);
xwer_t xwlua_sqlk_release(xwlua_sqlk_sp sqlksp);
xwer_t xwlua_sqlk_grab(struct xwlua_sqlk * luasqlk);
xwer_t xwlua_sqlk_put(struct xwlua_sqlk * luasqlk);

#endif /* xwem/vm/lua/xwlua/xwos/seqlock.h */
