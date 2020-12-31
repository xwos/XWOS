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

int xwlua_xt_function_writer(lua_State * S, const void * b, size_t size, void * ud)
{
        luaL_Buffer * sb;

        XWOS_UNUSED(S);
        sb = (struct luaL_Buffer *)ud;
        luaL_addlstring(sb, (const char *)b, size);
        return 0;
}

/**
 * @brief 将虚拟机S栈中的函数拷贝到虚拟机D的栈顶
 * @param S: (I) 虚拟机S
 * @param sidx: (I) 函数在虚拟机S栈中的位置
 * @param D: (I) 虚拟机D
 */
void xwlua_xt_copy_function(lua_State * S, int sidx, lua_State * D)
{
        luaL_Buffer lb;
        const char * func;
        size_t fl;
        int rc;

        lua_pushvalue(S, sidx); /* push function */
        luaL_buffinit(S, &lb);
        rc = lua_dump(S, xwlua_xt_function_writer, &lb, true);
        if (0 == rc) {
                luaL_pushresult(&lb); /* push function string */
                func = lua_tolstring(S, -1, &fl);
                rc = luaL_loadbufferx(D, func, fl, "function", "bt");
                if ( LUA_OK != rc) {
                        lua_pop(D, 1);
                        lua_pushnil(D);
                }
                lua_pop(S, 1); /* pop function string */
        } else {
                lua_pushnil(D);
        }
        lua_pop(S, 1); /* pop function */
}

/**
 * @brief 将虚拟机S栈中的表元素拷贝到虚拟机D的栈中
 * @param S: (I) 虚拟机S
 * @param sidx: (I) 表在虚拟机S栈中的位置
 * @param D: (I) 虚拟机D
 * @param didx: (I) 表在虚拟机D栈中的位置
 * @note
 * + 本函数只会对表中索引(key)为字符串的键值对进行拷贝，其他类型的索引忽略。
 */
void xwlua_xt_copy_table(lua_State * S, int sidx, lua_State * D, int didx)
{
        int kt;
        const char * key;
        size_t kl;
        int vt;
        union {
                const char * str;
                int boolean;
                lua_Number number;
                lua_Integer integer;
        } value;
        size_t vl;

        sidx = lua_absindex(S, sidx);
        didx = lua_absindex(D, didx);
        /* table is in the stack at index sidx */
        lua_pushnil(S); /* first key */
        while (lua_next(S, sidx) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                kt = lua_type(S, -2);
                if (LUA_TSTRING == kt) {
                        key = lua_tolstring(S, -2, &kl);
                        vt = lua_type(S, -1);
                        switch (vt) {
                        case LUA_TSTRING:
                                value.str = lua_tolstring(S, -1, &vl);
                                lua_pushlstring(D, value.str, vl);
                                lua_setfield(D, didx, key);
                                break;
                        case LUA_TBOOLEAN:
                                value.boolean = lua_toboolean(S, -1);
                                lua_pushboolean(D, value.boolean);
                                lua_setfield(D, didx, key);
                                break;
                        case LUA_TNUMBER:
                                if (lua_isinteger(S, -1)) {
                                        value.integer = lua_tointegerx(S, -1, NULL);
                                        lua_pushinteger(D, value.integer);
                                } else {
                                        value.number = lua_tonumberx(S, -1, NULL);
                                        lua_pushnumber(D, value.number);
                                }
                                lua_setfield(D, didx, key);
                                break;
                        case LUA_TTABLE:
                                lua_newtable(D);
                                xwlua_xt_copy_table(S, -1, D, -1);
                                lua_setfield(D, didx, key);
                                break;
                        case LUA_TFUNCTION:
                                xwlua_xt_copy_function(S, -1, D);
                                lua_setfield(D, didx, key);
                                break;
                        default:
                                break;
                        }
                }
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(S, 1);
        }
}

/**
 * @brief 将虚拟机S中的userdata拷贝到虚拟机D的栈顶
 * @param S: (I) 源虚拟机
 * @param idx: (I) userdata在虚拟机S栈中的位置
 * @param D: (I) 目的虚拟机
 */
void xwlua_xt_copy_ud(lua_State * S, int idx, lua_State * D)
{
        int rc;
        int type;
        bool hasmt;

        idx = lua_absindex(S, idx);
        hasmt = lua_getmetatable(S, idx); /* push metatable */
        if (hasmt) {
                type = lua_getfield(S, -1, "__copy"); /* function */
                if (LUA_TFUNCTION == type) {
                        lua_pushvalue(S, idx); /* arg1 */
                        lua_pushlightuserdata(S, (void *)D); /* arg2 */
                        /* call then pop function & arg1, arg2 */
                        rc = lua_pcall(S, 2, LUA_MULTRET, 0);
                        if (LUA_OK != rc) {
                                lua_pop(S, 1); /* pop error */
                                lua_pushnil(D);
                        }
                } else {
                        lua_pop(S, 1); /* pop "__copy" */
                        lua_pushnil(D);
                }
                lua_pop(S, 1); /* pop metatable */
        } else {
                lua_pushnil(D);
        }
}

/**
 * @brief 将虚拟机S中的表元素拷贝到虚拟机D的环境_ENV中
 * @param S: (I) 虚拟机S
 * @param sidx: (I) 表在虚拟机S栈中的索引
 * @param D: (I) 虚拟机D
 * @note
 * + 本函数只会将表中索引(key)为字符串的键值对拷贝到D的_ENV中，其他类型的索引忽略。
 */
void xwlua_xt_copy_env(lua_State * S, int idx, lua_State * D)
{
        int type;
        const char * key;
        size_t kl;
        union {
                const char * str;
                int boolean;
                lua_Number number;
                lua_Integer integer;
        } value;
        size_t vl;

        /* table is in the stack at index 't' */
        lua_pushnil(S); /* first key */
        while (lua_next(S, idx) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                if (lua_type(S, -2) == LUA_TSTRING) {
                        key = lua_tolstring(S, -2, &kl);
                        type = lua_type(S, -1);
                        switch (type) {
                        case LUA_TSTRING:
                                value.str = lua_tolstring(S, -1, &vl);
                                lua_pushlstring(D, value.str, vl);
                                lua_setglobal(D, key);
                                break;
                        case LUA_TBOOLEAN:
                                value.boolean = lua_toboolean(S, -1);
                                lua_pushboolean(D, value.boolean);
                                lua_setglobal(D, key);
                                break;
                        case LUA_TNUMBER:
                                if (lua_isinteger(S, -1)) {
                                        value.integer = lua_tointegerx(S, -1, NULL);
                                        lua_pushinteger(D, value.integer);
                                } else {
                                        value.number = lua_tonumberx(S, -1, NULL);
                                        lua_pushnumber(D, value.number);
                                }
                                lua_setglobal(D, key);
                                break;
                        case LUA_TTABLE:
                                lua_newtable(D);
                                xwlua_xt_copy_table(S, -1, D, -1);
                                lua_setglobal(D, key);
                                break;
                        case LUA_TFUNCTION:
                                xwlua_xt_copy_function(S, -1, D);
                                lua_setglobal(D, key);
                                break;
                        default:
                                break;
                        }
                }
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(S, 1);
        }
}

int xwlua_xt_setup(lua_State * L)
{
        xwer_t rc;

        luaL_checktype(L, 1, LUA_TTABLE);
        rc = xwos_mtx_lock(&xwlua_xt_lock);
        if (XWOK == rc) {
                xwlua_xt_copy_env(L, 1, xwlua_xt);
                xwos_mtx_unlock(&xwlua_xt_lock);
        }
        return 0;
}

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
        {"setup", xwlua_xt_setup},
        {NULL, NULL},
};

int xwlua_xt_import(lua_State * L)
{
        union {
                const char * str;
                int boolean;
                lua_Number number;
                lua_Integer integer;
        } value;
        size_t vl;
        int vt;

        vt = luaL_checkinteger(L, 1);
        switch (vt) {
        case LUA_TSTRING:
                value.str = lua_tolstring(xwlua_xt, -1, &vl);
                lua_pushlstring(L, value.str, vl);
                break;
        case LUA_TBOOLEAN:
                value.boolean = lua_toboolean(xwlua_xt, -1);
                lua_pushboolean(L, value.boolean);
                break;
        case LUA_TNUMBER:
                if (lua_isinteger(xwlua_xt, -1)) {
                        value.integer = lua_tointegerx(xwlua_xt, -1, NULL);
                        lua_pushinteger(L, value.integer);
                } else {
                        value.number = lua_tonumberx(xwlua_xt, -1, NULL);
                        lua_pushnumber(L, value.number);
                }
                break;
        case LUA_TTABLE:
                lua_newtable(L);
                xwlua_xt_copy_table(xwlua_xt, -1, L, -1);
                break;
        case LUA_TFUNCTION:
                xwlua_xt_copy_function(xwlua_xt, -1, L);
                break;
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
                xwlua_xt_copy_ud(xwlua_xt, -1, L);
                break;
        default:
                lua_pushnil(L);
                break;
        }
        return 1;
}

int xwlua_xt_index(lua_State * L)
{
        const char * key;
        int vt;
        int rc;

        key = luaL_checklstring(L, 2, NULL);
        lua_pushcfunction(L, xwlua_xt_import); /* function */
        xwos_mtx_lock_unintr(&xwlua_xt_lock);
        vt = lua_getglobal(xwlua_xt, key); /* get value of key */
        lua_pushinteger(L, vt); /* arg1 */
        rc = lua_pcall(L, 1, 1, 0); /* call */
        lua_pop(xwlua_xt, 1); /* remove value of key */
        xwos_mtx_unlock(&xwlua_xt_lock);
        if (LUA_OK != rc) {
                lua_error(L);
        }
        return 1;
}

int xwlua_xt_export(lua_State * L)
{
        const char * key;
        union {
                const char * str;
                int boolean;
                lua_Number number;
                lua_Integer integer;
        } value;
        size_t vl;
        int vt;

        key = luaL_checklstring(L, 1, NULL);
        vt = lua_type(L, 2);
        switch (vt) {
        case LUA_TSTRING:
                value.str = lua_tolstring(L, 2, &vl);
                lua_pushlstring(xwlua_xt, value.str, vl);
                lua_setglobal(xwlua_xt, key);
                break;
        case LUA_TBOOLEAN:
                value.boolean = lua_toboolean(L, 2);
                lua_pushboolean(xwlua_xt, value.boolean);
                lua_setglobal(xwlua_xt, key);
                break;
        case LUA_TNUMBER:
                if (lua_isinteger(L, 2)) {
                        value.integer = lua_tointegerx(L, 2, NULL);
                        lua_pushinteger(xwlua_xt, value.integer);
                } else {
                        value.number = lua_tonumberx(L, 2, NULL);
                        lua_pushnumber(xwlua_xt, value.number);
                }
                lua_setglobal(xwlua_xt, key);
                break;
        case LUA_TTABLE:
                lua_newtable(xwlua_xt);
                xwlua_xt_copy_table(L, 2, xwlua_xt, -1);
                lua_setglobal(xwlua_xt, key);
                break;
        case LUA_TFUNCTION:
                xwlua_xt_copy_function(L, 2, xwlua_xt);
                lua_setglobal(xwlua_xt, key);
                break;
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
                xwlua_xt_copy_ud(L, 2, xwlua_xt);
                lua_setglobal(xwlua_xt, key);
                break;
        default:
                break;
        }
        return 0;
}

int xwlua_xt_newindex(lua_State * L)
{
        const char * key;
        int rc;

        key = luaL_checklstring(L, 2, NULL);
        if (lua_isnoneornil(L, 3)) { /* remove */
                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                lua_pushnil(xwlua_xt);
                lua_setglobal(xwlua_xt, key);
                xwos_mtx_unlock(&xwlua_xt_lock);
        } else { /* assign */
                lua_pushcfunction(L, xwlua_xt_export); /* function */
                lua_pushvalue(L, 2); /* arg1 */
                lua_pushvalue(L, 3); /* arg2 */
                xwos_mtx_lock_unintr(&xwlua_xt_lock);
                rc = lua_pcall(L, 2, LUA_MULTRET, 0);
                xwos_mtx_unlock(&xwlua_xt_lock);
                if (LUA_OK != rc) {
                        lua_error(L);
                }
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
        /* libs in xwxt */
        xwlua_xt_openlibs(xwlua_xt);

        luaL_newmetatable(L, "xwlua_xt");
        luaL_setfuncs(L, xwlua_xt_metamethod, 0); /* add metamethods */
        lua_pop(L, 1); /* pop metatable */

        luaL_newlib(L, xwlua_xt_method);
        luaL_setmetatable(L, "xwlua_xt");
        return 1;
}