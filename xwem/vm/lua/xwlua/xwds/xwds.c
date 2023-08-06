/**
 * @file
 * @brief XWLUA库：设备栈
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
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwds/uart.h"
#include "xwem/vm/lua/xwlua/xwds/i2cm.h"
#include "xwem/vm/lua/xwlua/xwds/spim.h"

const luaL_Reg xwlua_ds[] = {
        {"soc", NULL},
        {"uart", NULL},
        {"i2cm", NULL},
        {"spim", NULL},
        {NULL, NULL},
};

extern void xwlua_ds_open_soc(lua_State * L);
extern void xwlua_ds_open_i2cm(lua_State * L);
extern void xwlua_ds_open_spim(lua_State * L);
extern void xwlua_ds_open_uart(lua_State * L);

LUAMOD_API int xwlua_open_ds(lua_State * L)
{
        luaL_newlib(L, xwlua_ds);
        xwlua_ds_open_soc(L);
        lua_setfield(L, -2, "soc");
        xwlua_ds_open_uart(L);
        lua_setfield(L, -2, "uart");
        xwlua_ds_open_i2cm(L);
        lua_setfield(L, -2, "i2cm");
        xwlua_ds_open_spim(L);
        lua_setfield(L, -2, "spim");
        return 1;
}
