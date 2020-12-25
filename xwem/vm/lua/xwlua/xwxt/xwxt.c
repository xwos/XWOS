/**
 * @file
 * @brief 玄武 Lua Lib：导出表
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <stdio.h>
#include "xwlua/port.h"
#include "src/lauxlib.h"
#include "xwlua/xwos/skd.h"
#include "xwlua/xwos/sem.h"
#include "xwlua/xwos/cond.h"
#include "xwlua/xwos/mtx.h"
#include "xwlua/xwos/spinlock.h"
#include "xwlua/xwos/seqlock.h"

lua_State * xwlua_xt;
struct xwos_mtx xwlua_xt_lock;

int xwlua_xt_gc(lua_State * L)
{
        xwer_t rc;

        rc = xwos_mtx_lock(&xwlua_xt_lock);
        if (XWOK == rc) {
                lua_gc(xwlua_xt, LUA_GCCOLLECT);
                xwos_mtx_unlock(&xwlua_xt_lock);
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_xt_method[] = {
        {"gc", xwlua_xt_gc},
        {NULL, NULL},
};

int xwlua_xt_index(lua_State * L)
{
        const char * key;
        void * data;
        xwsz_t sl;
        bool gotnil;

        gotnil = true;
        key = luaL_checklstring(L, 2, &sl);
        xwos_mtx_lock_unintr(&xwlua_xt_lock);
        lua_getglobal(xwlua_xt, key);
        do {
                data = luaL_testudata(xwlua_xt, -1, "xwlua_thd_sp");
                if (data) {
                        xwlua_thdsp_xt_copy(L, data);
                        gotnil = false;
                        break;
                }
                data = luaL_testudata(xwlua_xt, -1, "xwlua_sem_sp");
                if (data) {
                        xwlua_semsp_xt_copy(L, data);
                        gotnil = false;
                        break;
                }
                data = luaL_testudata(xwlua_xt, -1, "xwlua_cond_sp");
                if (data) {
                        xwlua_condsp_xt_copy(L, data);
                        gotnil = false;
                        break;
                }
                data = luaL_testudata(xwlua_xt, -1, "xwlua_mtx_sp");
                if (data) {
                        xwlua_mtxsp_xt_copy(L, data);
                        gotnil = false;
                        break;
                }
                data = luaL_testudata(xwlua_xt, -1, "xwlua_splk_sp");
                if (data) {
                        xwlua_splksp_xt_copy(L, data);
                        gotnil = false;
                        break;
                }
                data = luaL_testudata(xwlua_xt, -1, "xwlua_sqlk_sp");
                if (data) {
                        xwlua_sqlksp_xt_copy(L, data);
                        gotnil = false;
                        break;
                }
        } while (false);

        lua_pop(xwlua_xt, 1);
        xwos_mtx_unlock(&xwlua_xt_lock);
        if (gotnil) {
                lua_pushnil(L);
        }
        return 1;
}

int xwlua_xt_newindex(lua_State * L)
{
        const char * key;
        void * data;
        xwsz_t sl;

        key = luaL_checklstring(L, 2, &sl);
        if (lua_isnil(L, 3)) { /* remove */
                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                lua_pushnil(xwlua_xt);
                lua_setglobal(xwlua_xt, key);
                xwos_mtx_unlock(&xwlua_xt_lock);
        } else { /* assign */
                do {
                        data = luaL_testudata(L, 3, "xwlua_thd_sp");
                        if (data) {
                                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                                xwlua_thdsp_xt_export(xwlua_xt, key, data);
                                xwos_mtx_unlock(&xwlua_xt_lock);
                                break;
                        }
                        data = luaL_testudata(L, 3, "xwlua_sem_sp");
                        if (data) {
                                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                                xwlua_semsp_xt_export(xwlua_xt, key, data);
                                xwos_mtx_unlock(&xwlua_xt_lock);
                                break;
                        }
                        data = luaL_testudata(L, 3, "xwlua_cond_sp");
                        if (data) {
                                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                                xwlua_condsp_xt_export(xwlua_xt, key, data);
                                xwos_mtx_unlock(&xwlua_xt_lock);
                                break;
                        }
                        data = luaL_testudata(L, 3, "xwlua_mtx_sp");
                        if (data) {
                                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                                xwlua_mtxsp_xt_export(xwlua_xt, key, data);
                                xwos_mtx_unlock(&xwlua_xt_lock);
                                break;
                        }
                        data = luaL_testudata(L, 3, "xwlua_splk_sp");
                        if (data) {
                                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                                xwlua_splksp_xt_export(xwlua_xt, key, data);
                                xwos_mtx_unlock(&xwlua_xt_lock);
                                break;
                        }
                        data = luaL_testudata(L, 3, "xwlua_sqlk_sp");
                        if (data) {
                                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                                xwlua_sqlksp_xt_export(xwlua_xt, key, data);
                                xwos_mtx_unlock(&xwlua_xt_lock);
                                break;
                        }
                } while (false);
        }
        return 0;
}

const luaL_Reg xwlua_xt_metamethod[] = {
        {"__index", xwlua_xt_index},
        {"__newindex", xwlua_xt_newindex},
        {NULL, NULL},
};

LUAMOD_API int xwlua_open_xt(lua_State * L)
{
        xwos_mtx_init(&xwlua_xt_lock, XWOS_SKD_PRIORITY_RT_MAX);
        xwlua_xt = luaL_newstate();
        if (NULL == xwlua_xt) {
                lua_writestringerror("%s", "Cannot create xt: not enough memory");
        }
        /* metatable in xwos.xt */
        xwlua_thdsp_xt_init(xwlua_xt);
        xwlua_semsp_xt_init(xwlua_xt);
        xwlua_condsp_xt_init(xwlua_xt);
        xwlua_mtxsp_xt_init(xwlua_xt);
        xwlua_splksp_xt_init(xwlua_xt);
        xwlua_sqlksp_xt_init(xwlua_xt);

        luaL_newmetatable(L, "xwlua_xt");
        luaL_setfuncs(L, xwlua_xt_metamethod, 0); /* add metamethods */
        lua_pop(L, 1); /* pop metatable */

        luaL_newlib(L, xwlua_xt_method);
        luaL_setmetatable(L, "xwlua_xt");
        return 1;
}
