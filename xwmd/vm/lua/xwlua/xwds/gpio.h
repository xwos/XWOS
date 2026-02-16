/**
 * @file
 * @brief XWLUA库：设备栈：GPIO
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwds_gpio_h__
#define __xwmd_vm_lua_xwlua_xwds_gpio_h__

#include <xwos/standard.h>
#include "xwmd/vm/lua/src/lauxlib.h"

int xwlua_gpio_req(lua_State * L);
int xwlua_gpio_rls(lua_State * L);
int xwlua_gpio_set(lua_State * L);
int xwlua_gpio_reset(lua_State * L);
int xwlua_gpio_toggle(lua_State * L);
int xwlua_gpio_output(lua_State * L);
int xwlua_gpio_input(lua_State * L);
int xwlua_gpio_pin(lua_State * L);

#endif /* xwmd/vm/lua/xwlua/xwds/gpio.h */
