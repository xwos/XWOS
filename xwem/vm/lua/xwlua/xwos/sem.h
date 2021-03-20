/**
 * @file
 * @brief 玄武OS Lua Lib：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_xwos_sem_h__
#define __xwem_vm_lua_xwlua_xwos_sem_h__

#include <xwos/standard.h>
#include <xwos/osal/sync/sem.h>
#include "src/lauxlib.h"

typedef xwos_sem_d xwlua_sem_sp; /**< 信号量的强指针 */
#define XWLUA_SEM_NULLSP XWOS_SEM_NILD

void xwlua_os_open_sem(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/sem.h */
