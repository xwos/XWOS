/**
 * @file
 * @brief XWLUA库：XWOS内核
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include "xwem/vm/lua/xwlua/xwos/skd.h"
#include "xwem/vm/lua/xwlua/xwos/thd.h"
#include "xwem/vm/lua/xwlua/xwos/sem.h"
#include "xwem/vm/lua/xwlua/xwos/cond.h"
#include "xwem/vm/lua/xwlua/xwos/flg.h"
#include "xwem/vm/lua/xwlua/xwos/br.h"
#include "xwem/vm/lua/xwlua/xwos/sel.h"
#include "xwem/vm/lua/xwlua/xwos/mtx.h"
#include "xwem/vm/lua/xwlua/xwos/spinlock.h"
#include "xwem/vm/lua/xwlua/xwos/seqlock.h"
#include "xwem/vm/lua/xwlua/xwos/pm.h"

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
