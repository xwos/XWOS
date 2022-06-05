/**
 * @file
 * @brief 电路板Lua模块：玄武设备栈
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#include <xwos/standard.h>
#include <xwem/vm/lua/src/lauxlib.h>
#include <xwem/vm/lua/xwlua/xwds/soc.h>
#include <xwem/vm/lua/xwlua/xwds/uart.h>
#include <bm/stm32cube/mif.h>

void xwlua_open_brdlibs(lua_State * L)
{
        xwlua_soc_register(L, "stm32", &stm32soc);
        xwlua_uart_register(L, "usart1", &stm32usart1);
        xwlua_uart_register(L, "usart3", &stm32usart3);
}
