/**
 * @file
 * @brief 板级描述层：XWOS适配层：LUA：XWOS设备栈
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#include "board/std.h"
#include <xwem/vm/lua/src/lauxlib.h>
#include <xwem/vm/lua/xwlua/xwds/soc.h>
#include <xwem/vm/lua/xwlua/xwds/uart.h>
#include <xwem/vm/lua/xwlua/xwds/spim.h>
#include <xwem/vm/lua/xwlua/xwds/i2cm.h>
#include "bm/Stm32Hal/xwds/device.h"

void xwlua_open_brdlibs(lua_State * L)
{
        xwlua_soc_register(L, "stm32", &stm32xwds_soc);
        xwlua_uart_register(L, "uart1", &stm32xwds_usart1);
        xwlua_uart_register(L, "uart3", &stm32xwds_usart3);
        xwlua_spim_register(L, "spim1", &stm32xwds_spim1);
        xwlua_spim_register(L, "spim4", &stm32xwds_spim4);
        xwlua_i2cm_register(L, "i2cm1", &stm32xwds_i2cm1);
}
