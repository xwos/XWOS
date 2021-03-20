/**
 * @file
 * @brief 玄武OS Lua Lib：事件标志
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_xwos_flg_h__
#define __xwem_vm_lua_xwlua_xwos_flg_h__

#include <xwos/standard.h>
#include <xwos/osal/sync/flg.h>
#include "src/lauxlib.h"

typedef xwos_flg_d xwlua_flg_sp; /**< 信号量的强指针 */
#define XWLUA_FLG_NULLSP XWOS_FLG_NILD

void xwlua_os_open_flg(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwos/flg.h */
