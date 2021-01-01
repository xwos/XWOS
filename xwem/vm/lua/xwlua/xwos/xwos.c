/**
 * @file
 * @brief Lua Lib：玄武OS
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
#include "xwlua/xwos/skd.h"
#include "xwlua/xwos/sem.h"
#include "xwlua/xwos/cond.h"
#include "xwlua/xwos/br.h"
#include "xwlua/xwos/sel.h"
#include "xwlua/xwos/lock.h"
#include "xwlua/xwos/mtx.h"
#include "xwlua/xwos/spinlock.h"
#include "xwlua/xwos/seqlock.h"

const luaL_Reg xwlua_os[] = {
        {"skd", NULL},
        {"thd", NULL},
        {"cthd", NULL},
        {"sem", NULL},
        {"cond", NULL},
        {"br", NULL},
        {"sel", NULL},
        {"lock", NULL},
        {"mtx", NULL},
        {"splk", NULL},
        {"sqlk", NULL},
        {NULL, NULL},
};

LUAMOD_API int xwlua_open_os(lua_State * L)
{
        luaL_newlib(L, xwlua_os);
        xwlua_os_open_skd(L);
        lua_setfield(L, -2, "skd");
        xwlua_os_open_thd(L);
        lua_setfield(L, -2, "thd");
        xwlua_os_open_cthd(L);
        lua_setfield(L, -2, "cthd");
        xwlua_os_open_sem(L);
        lua_setfield(L, -2, "sem");
        xwlua_os_open_cond(L);
        lua_setfield(L, -2, "cond");
        xwlua_os_open_br(L);
        lua_setfield(L, -2, "br");
        xwlua_os_open_sel(L);
        lua_setfield(L, -2, "sel");
        xwlua_os_open_lock(L);
        lua_setfield(L, -2, "lock");
        xwlua_os_open_mtx(L);
        lua_setfield(L, -2, "mtx");
        xwlua_os_open_splk(L);
        lua_setfield(L, -2, "splk");
        xwlua_os_open_sqlk(L);
        lua_setfield(L, -2, "sqlk");
        return 1;
}
