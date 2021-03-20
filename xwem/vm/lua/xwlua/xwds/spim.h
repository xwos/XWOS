/**
 * @file
 * @brief 玄武设备栈 Lua Lib：SPI master
 * @author
 * + 玄武设备栈 Lua Lib：SPI master
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_xwds_spim_h__
#define __xwem_vm_lua_xwlua_xwds_spim_h__

#include <xwos/standard.h>
#include <xwcd/ds/spi/master.h>
#include "src/lauxlib.h"

struct xwlua_spim {
        struct xwds_spim * spim;
        xwsq_t tik;
};

void xwlua_spim_register(lua_State * L, const char * name, struct xwds_spim * spim);
void xwlua_spim_unregister(lua_State * L, const char * name);
void xwlua_ds_open_spim(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwds/spim.h */
