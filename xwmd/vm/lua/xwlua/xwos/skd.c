/**
 * @file
 * @brief XWLUA库：XWOS内核：调度
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/osal/skd.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwos/skd.h"

/******** xwos.skd ********/
int xwlua_skd_get_cpuid_lc(lua_State * L)
{
        xwid_t id;

        id = xwos_skd_get_cpuid_lc();
        lua_pushnumber(L, (lua_Number)id);
        return 1;
}

int xwlua_skd_get_dspmpt_lc(lua_State * L)
{
        XWOS_UNUSED(L);
        xwos_skd_dspmpt_lc();
        return 0;
}

int xwlua_skd_get_enpmpt_lc(lua_State * L)
{
        XWOS_UNUSED(L);
        xwos_skd_enpmpt_lc();
        return 0;
}

const luaL_Reg xwlua_skd_libconstructor[] = {
        {"cpuid", xwlua_skd_get_cpuid_lc},
        {"dspmpt", xwlua_skd_get_dspmpt_lc},
        {"enpmpt", xwlua_skd_get_enpmpt_lc},
        {NULL, NULL},
};

void xwlua_os_open_skd(lua_State * L)
{
        luaL_newlib(L, xwlua_skd_libconstructor);
}
