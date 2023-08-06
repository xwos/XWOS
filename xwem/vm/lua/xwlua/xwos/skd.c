/**
 * @file
 * @brief XWLUA库：XWOS内核：调度
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
#include <string.h>
#include <xwos/osal/skd.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwos/skd.h"

/******** xwos.skd ********/
int xwlua_skd_id_lc(lua_State * L)
{
        xwid_t id;

        id = xwos_skd_id_lc();
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
        {"id", xwlua_skd_id_lc},
        {"dspmpt", xwlua_skd_get_dspmpt_lc},
        {"enpmpt", xwlua_skd_get_enpmpt_lc},
        {NULL, NULL},
};

void xwlua_os_open_skd(lua_State * L)
{
        luaL_newlib(L, xwlua_skd_libconstructor);
}
