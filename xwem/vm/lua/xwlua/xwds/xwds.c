/**
 * @file
 * @brief Lua Lib：玄武设备栈
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#include <xwos/standard.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwds/soc.h"
#include "xwlua/xwds/gpio.h"
#include "xwlua/xwds/uart.h"
#include "xwlua/xwds/i2cm.h"
#include "xwlua/xwds/spim.h"

const luaL_Reg xwlua_ds[] = {
        {"soc", NULL},
        {"gpio", NULL},
        {"uart", NULL},
        {"i2cm", NULL},
        {"spim", NULL},
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
        xwlua_ds_open_uart(L);
        lua_setfield(L, -2, "uart");
        xwlua_ds_open_i2cm(L);
        lua_setfield(L, -2, "i2cm");
        xwlua_ds_open_spim(L);
        lua_setfield(L, -2, "spim");
        return 1;
}
