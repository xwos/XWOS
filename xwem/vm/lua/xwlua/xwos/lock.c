/**
 * @file
 * @brief 玄武OS Lua Lib：锁容器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <string.h>
#include <stdio.h>
#include "xwlua/port.h"
#include "src/lauxlib.h"
#include "xwlua/xwos/mtx.h"
#include "xwlua/xwos/spinlock.h"
#include "xwlua/xwos/seqlock.h"
#include "xwlua/xwos/lock.h"

int xwlua_autolock_newindex(lua_State * L)
{
        lua_pushliteral(L, "xwos.autolock is readonly");
        lua_error(L);
        return 0;
}

int xwlua_autolock_index(lua_State * L)
{
        lua_getfield(L, 1, "xwlualock"); /* top = autolock.xwlualock */
        lua_pushvalue(L, 2); /* L->top = key */
        lua_gettable(L, -2); /* autolock.lock[key] */
        return 1;
}

int xwlua_autolock_close(lua_State * L)
{
        void * ud;

        lua_getfield(L, 1, "xwlualock"); /* top = autolock.xwlualock */
        do {
                ud = luaL_testudata(L, -1, "xwlua_mtx_sp");
                if (ud) {
                        xwlua_mtx_sp * mtxsp;

                        mtxsp = ud;
                        xwlua_mtx_unlock(mtxsp->mtx);
                        break;
                }
                ud = luaL_testudata(L, -1, "xwlua_splk_sp");
                if (ud) {
                        xwlua_splk_sp * splksp;

                        splksp = ud;
                        xwlua_splk_unlock(splksp->luasplk);
                        break;
                }
                ud = luaL_testudata(L, -1, "xwlua_sqlk_sp");
                if (ud) {
                        xwlua_sqlk_sp * sqlksp;

                        sqlksp = ud;
                        xwlua_sqlk_unlock(sqlksp->luasqlk);
                        break;
                }
        } while (false);
        return 0;
}

int xwlua_autolock_gc(lua_State * L)
{
        void * ud;

        lua_getfield(L, 1, "xwlualock"); /* top = autolock.xwlualock */
        do {
                ud = luaL_testudata(L, -1, "xwlua_mtx_sp");
                if (ud) {
                        xwlua_mtx_sp * mtxsp;

                        mtxsp = ud;
                        xwlua_mtx_unlock(mtxsp->mtx);
                        xwos_mtx_put(mtxsp->mtx);
                        break;
                }
                ud = luaL_testudata(L, -1, "xwlua_splk_sp");
                if (ud) {
                        xwlua_splk_sp * splksp;

                        splksp = ud;
                        xwlua_splk_unlock(splksp->luasplk);
                        xwlua_splk_put(splksp->luasplk);
                        break;
                }
                ud = luaL_testudata(L, -1, "xwlua_sqlk_sp");
                if (ud) {
                        xwlua_sqlk_sp * sqlksp;

                        sqlksp = ud;
                        xwlua_sqlk_unlock(sqlksp->luasqlk);
                        xwlua_sqlk_put(sqlksp->luasqlk);
                        break;
                }
        } while (false);
        return 0;
}

const luaL_Reg xwlua_autolock_metamethod[] = {
        {"__newindex", xwlua_autolock_newindex},
        {"__index", xwlua_autolock_index},
        {"__close", xwlua_autolock_close},
        {"__gc", xwlua_autolock_gc},
        {NULL, NULL},
};

int xwlua_lock_call(lua_State * L)
{
        void * ud;
        bool islock;

        if (lua_isnil(L, 2)) {
                islock = false;
        } else {
                do {
                        ud = luaL_testudata(L, 2, "xwlua_mtx_sp");
                        if (ud) {
                                xwlua_mtx_sp * mtxsp = ud;
                                xwos_mtx_grab(mtxsp->mtx);
                                islock = true;
                                break;
                        }
                        ud = luaL_testudata(L, 2, "xwlua_splk_sp");
                        if (ud) {
                                xwlua_splk_sp * splksp = ud;
                                xwlua_splk_grab(splksp->luasplk);
                                islock = true;
                                break;
                        }
                        ud = luaL_testudata(L, 2, "xwlua_sqlk_sp");
                        if (ud) {
                                xwlua_sqlk_sp * sqlksp = ud;
                                xwlua_sqlk_grab(sqlksp->luasqlk);
                                islock = true;
                                break;
                        }
                } while (false);
        }
        if (islock) {
                lua_createtable(L, 1, 1);
                lua_pushvalue(L, 2);
                lua_setfield(L, -2, "xwlualock");
                luaL_setmetatable(L, "xwlua_autolock");
        } else {
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_lock_metamethod[] = {
        {"__call", xwlua_lock_call},
        {NULL, NULL},
};

const luaL_Reg xwlua_lock_method[] = {
        {NULL, NULL},
};

void xwlua_os_open_lock(lua_State * L)
{
        luaL_newmetatable(L, "xwlua_lock");
        luaL_setfuncs(L, xwlua_lock_metamethod, 0); /* add metamethods */
        lua_pop(L, 1); /* pop metatable */

        luaL_newmetatable(L, "xwlua_autolock");
        luaL_setfuncs(L, xwlua_autolock_metamethod, 0); /* add metamethods */
        lua_pop(L, 1); /* pop metatable */

        luaL_newlib(L, xwlua_lock_method);
        luaL_setmetatable(L, "xwlua_lock");
}
