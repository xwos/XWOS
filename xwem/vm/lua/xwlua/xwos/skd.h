/**
 * @file
 * @brief 玄武OS Lua Lib：调度
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_xwos_skd_h__
#define __xwem_vm_lua_xwlua_xwos_skd_h__

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include "src/lauxlib.h"

typedef xwos_thd_d xwlua_thd_sp; /**< 线程的强指针 */
#define XWLUA_THD_NULLSP XWOS_THD_NILD

void xwlua_os_open_skd(lua_State * L);
void xwlua_os_open_thd(lua_State * L);
void xwlua_os_open_cthd(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/skd.h */
