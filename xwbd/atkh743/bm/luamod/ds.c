/**
 * @file
 * @brief 电路板Lua模块：玄武设备栈
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

#include <xwos/standard.h>
#include <xwem/vm/lua/src/lauxlib.h>
#include <xwem/vm/lua/xwlua/xwds/soc.h>
#include <xwem/vm/lua/xwlua/xwds/uart.h>
#include <xwem/vm/lua/xwlua/xwds/i2cm.h>
#include <xwem/vm/lua/xwlua/xwds/spim.h>
#include <bm/stm32cube/mif.h>

void xwlua_open_brdlibs(lua_State * L)
{
        xwlua_soc_register(L, "stm32", &stm32cube_soc_cb);
        xwlua_uart_register(L, "uart1", &stm32cube_usart1_cb);
        xwlua_uart_register(L, "uart2", &stm32cube_usart2_cb);
        xwlua_i2cm_register(L, "i2c2m", &stm32cube_i2c2m_cb);
        xwlua_spim_register(L, "spi1m", &stm32cube_spi1m_cb);
        xwlua_spim_register(L, "spi2m", &stm32cube_spi2m_cb);
}
