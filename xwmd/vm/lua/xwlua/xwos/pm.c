/**
 * @file
 * @brief XWLUA库：XWOS内核：电源管理
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/pm.h>
#include "xwmd/vm/lua/src/lauxlib.h"
#include "xwmd/vm/lua/xwlua/port.h"
#include "xwmd/vm/lua/xwlua/xwos/pm.h"

int xwlua_pm_suspend(lua_State * L)
{
        XWOS_UNUSED(L);
        xwos_pm_suspend();
        return 0;
}

const luaL_Reg xwlua_pm_libconstructor[] = {
        {"suspend", xwlua_pm_suspend},
        {NULL, NULL},
};

void xwlua_os_open_pm(lua_State * L)
{
        luaL_newlib(L, xwlua_pm_libconstructor);
}
