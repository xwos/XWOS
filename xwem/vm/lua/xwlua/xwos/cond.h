/**
 * @file
 * @brief 玄武OS Lua Lib：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_xwos_cond_h__
#define __xwem_vm_lua_xwlua_xwos_cond_h__

#include <xwos/standard.h>
#include <xwos/osal/sync/cond.h>
#include "src/lauxlib.h"

typedef xwos_cond_d xwlua_cond_sp; /**< 信号量的强指针 */
#define XWLUA_COND_NULLSP XWOS_COND_NILD

void xwlua_os_open_cond(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/cond.h */
