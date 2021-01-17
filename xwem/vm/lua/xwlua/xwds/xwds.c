/**
 * @file
 * @brief Lua Lib：玄武设备栈
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwds/soc.h"
#include "xwlua/xwds/gpio.h"

const luaL_Reg xwlua_ds[] = {
        {"soc", NULL},
        {"gpio", NULL},
        {"i2cm", NULL},
        {"spim", NULL},
        {"uart", NULL},
        {"can", NULL},
        {NULL, NULL},
};

LUAMOD_API int xwlua_open_ds(lua_State * L)
{
        luaL_newlib(L, xwlua_ds);
        xwlua_ds_open_soc(L);
        lua_setfield(L, -2, "soc");
        xwlua_ds_open_gpio(L);
        lua_setfield(L, -2, "gpio");
        return 1;
}
