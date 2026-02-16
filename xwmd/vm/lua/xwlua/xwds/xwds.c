/**
 * @file
 * @brief XWLUA库：设备栈
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwds/uart.h"
#include "xwmd/vm/lua/xwlua/xwds/i2cm.h"
#include "xwmd/vm/lua/xwlua/xwds/spim.h"

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
