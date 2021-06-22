/**
 * @file
 * @brief 玄武Lua库：UART
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
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
