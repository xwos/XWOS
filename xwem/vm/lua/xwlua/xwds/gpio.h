/**
 * @file
 * @brief XWLUA库：GPIO
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

#ifndef __xwem_vm_lua_xwlua_xwds_gpio_h__
#define __xwem_vm_lua_xwlua_xwds_gpio_h__

#include <xwos/standard.h>
#include "xwem/vm/lua/src/lauxlib.h"

int xwlua_gpio_req(lua_State * L);
int xwlua_gpio_rls(lua_State * L);
int xwlua_gpio_set(lua_State * L);
int xwlua_gpio_reset(lua_State * L);
int xwlua_gpio_toggle(lua_State * L);
int xwlua_gpio_output(lua_State * L);
int xwlua_gpio_input(lua_State * L);
int xwlua_gpio_pin(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwds/gpio.h */
