/**
 * @file
 * @brief 玄武设备栈 Lua Lib：I2C master
 * @author
 * + 玄武设备栈 Lua Lib：I2C master
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_xwds_i2cm_h__
#define __xwem_vm_lua_xwlua_xwds_i2cm_h__

#include <xwos/standard.h>
#include <xwcd/ds/i2c/master.h>
#include "src/lauxlib.h"

struct xwlua_i2cm {
        struct xwds_i2cm * i2cm;
        xwsq_t tik;
};

void xwlua_i2cm_register(lua_State * L, const char * name, struct xwds_i2cm * i2cm);
void xwlua_i2cm_unregister(lua_State * L, const char * name);
void xwlua_ds_open_i2cm(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwds/i2cm.h */
