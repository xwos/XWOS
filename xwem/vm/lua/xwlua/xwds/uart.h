/**
 * @file
 * @brief 玄武设备栈 Lua Lib：UART
 * @author
 * + 玄武设备栈 Lua Lib：I2C master
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#ifndef __xwem_vm_lua_xwlua_xwds_uart_h__
#define __xwem_vm_lua_xwlua_xwds_uart_h__

#include <xwos/standard.h>
#include <xwcd/ds/uart/dma.h>
#include "src/lauxlib.h"

struct xwlua_uart {
        struct xwds_dmauartc * dmauartc;
        xwsq_t tik;
};

void xwlua_uart_register(lua_State * L, const char * name,
                         struct xwds_dmauartc * dmauartc);
void xwlua_uart_unregister(lua_State * L, const char * name);
void xwlua_ds_open_uart(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwds/uart.h */
