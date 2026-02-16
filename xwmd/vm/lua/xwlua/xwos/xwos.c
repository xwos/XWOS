/**
 * @file
 * @brief XWLUA库：XWOS内核
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
#include "xwmd/vm/lua/xwlua/xwos/skd.h"
#include "xwmd/vm/lua/xwlua/xwos/thd.h"
#include "xwmd/vm/lua/xwlua/xwos/sem.h"
#include "xwmd/vm/lua/xwlua/xwos/cond.h"
#include "xwmd/vm/lua/xwlua/xwos/flg.h"
#include "xwmd/vm/lua/xwlua/xwos/br.h"
#include "xwmd/vm/lua/xwlua/xwos/sel.h"
#include "xwmd/vm/lua/xwlua/xwos/mtx.h"
#include "xwmd/vm/lua/xwlua/xwos/spinlock.h"
#include "xwmd/vm/lua/xwlua/xwos/seqlock.h"
#include "xwmd/vm/lua/xwlua/xwos/pm.h"

const luaL_Reg xwlua_os[] = {
        {"skd", NULL},
        {"thd", NULL},
        {"cthd", NULL},
        {"sem", NULL},
        {"cond", NULL},
        {"flg", NULL},
        {"br", NULL},
        {"sel", NULL},
        {"mtx", NULL},
        {"splk", NULL},
        {"sqlk", NULL},
        {"pm", NULL},
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
        xwlua_os_open_flg(L);
        lua_setfield(L, -2, "flg");
        xwlua_os_open_br(L);
        lua_setfield(L, -2, "br");
        xwlua_os_open_sel(L);
        lua_setfield(L, -2, "sel");
        xwlua_os_open_mtx(L);
        lua_setfield(L, -2, "mtx");
        xwlua_os_open_splk(L);
        lua_setfield(L, -2, "splk");
        xwlua_os_open_sqlk(L);
        lua_setfield(L, -2, "sqlk");
        xwlua_os_open_pm(L);
        lua_setfield(L, -2, "pm");
        return 1;
}
