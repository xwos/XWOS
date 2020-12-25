/**
 * @file
 * @brief 玄武OS Lua Lib：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/sem.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwos/sem.h"

int xwlua_sem_new(lua_State * L)
{
        xwer_t rc;
        xwssq_t val;
        xwssq_t max;
        xwlua_sem_sp * semsp;

        val = luaL_checkinteger(L, 1);
        max = luaL_checkinteger(L, 2);
        if (max < 0) {
                max = XWSSQ_MAX;
        }
        semsp = lua_newuserdatauv(L, sizeof(xwlua_sem_sp), 0);
        rc = xwos_sem_create(&semsp->sem, val, max);
        if (XWOK == rc) {
                semsp->tik = xwos_sem_gettik(semsp->sem);
                luaL_setmetatable(L, "xwlua_sem_sp");
        } else {
                *semsp = XWLUA_SEM_NULLSP;
        }
        return 1;
}

const luaL_Reg xwlua_sem_method[] = {
        {"new", xwlua_sem_new},
        {NULL, NULL},
};

void xwlua_os_init_semsp(lua_State * L);

void xwlua_os_open_sem(lua_State * L)
{
        xwlua_os_init_semsp(L);
        luaL_newlib(L, xwlua_sem_method);
}

/******** class xwlua_sem_sp ********/
int xwlua_semsp_gc(lua_State * L)
{
        xwlua_sem_sp * semsp;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        xwos_sem_release(*semsp);
        *semsp = XWLUA_SEM_NULLSP;
        return 0;
}

int xwlua_semsp_tostring(lua_State * L)
{
        xwlua_sem_sp * semsp;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        lua_pushfstring(L, "{%p, %d}", semsp->sem, semsp->tik);
        return 1;
}

const luaL_Reg xwlua_semsp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_semsp_gc},
        {"__tostring", xwlua_semsp_tostring},
        {NULL, NULL}
};

int xwlua_semsp_freeze(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwer_t rc;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        rc = xwos_sem_freeze(semsp->sem);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_semsp_thaw(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwer_t rc;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        rc = xwos_sem_thaw(semsp->sem);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_semsp_post(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwer_t rc;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        rc = xwos_sem_post(semsp->sem);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

#define XWLUA_SEM_LKOPT_TRY             0
const char * const xwlua_sem_lkopt[] = {"t", NULL};

int xwlua_semsp_wait(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwtm_t time;
        int lkopt;
        int type;
        int top;
        xwer_t rc;

        top = lua_gettop(L);
        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        if (top >= 2) {
                type = lua_type(L, 2);
                switch (type) {
                case LUA_TNUMBER:
                        time = (xwtm_t)lua_tonumber(L, 2);
                        rc = xwos_sem_timedwait(semsp->sem, &time);
                        break;
                case LUA_TSTRING:
                        lkopt = luaL_checkoption(L, 2, "t", xwlua_sem_lkopt);
                        switch (lkopt) {
                        case XWLUA_SEM_LKOPT_TRY:
                                rc = xwos_sem_trywait(semsp->sem);
                                break;
                        default:
                                luaL_error(L, "Invalid arg: %s", lua_tostring(L, 2));
                                rc = -EINVAL;
                                break;
                        }
                        break;
                case LUA_TNIL:
                        rc = xwos_sem_wait(semsp->sem);
                        break;
                default:
                        luaL_error(L, "Invalid arg type: %s", lua_typename(L, type));
                        rc = -EINVAL;
                        break;
                }
        } else {
                rc = xwos_sem_wait(semsp->sem);
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_semsp_getvalue(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwssq_t val;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        xwos_sem_getvalue(semsp->sem, &val);
        lua_pushinteger(L, (lua_Integer)val);
        return 1;
}

const luaL_Reg xwlua_semsp_method[] = {
        {"freeze", xwlua_semsp_freeze},
        {"thaw", xwlua_semsp_thaw},
        {"post", xwlua_semsp_post},
        {"wait", xwlua_semsp_wait},
        {"getvalue", xwlua_semsp_getvalue},
        {NULL, NULL},
};

void xwlua_os_init_semsp(lua_State * L)
{
        /* metatable for xwlua_sem_sp */
        luaL_newmetatable(L, "xwlua_sem_sp");
        luaL_setfuncs(L, xwlua_semsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_semsp_method); /* create sem method table */
        luaL_setfuncs(L, xwlua_semsp_method, 0); /* add sem methods */
        lua_setfield(L, -2, "__index");  /* metatable.__index = xwlua_semsp_method */
        lua_pop(L, 1); /* pop metatable */
}

void xwlua_semsp_xt_init(lua_State * xt)
{
        luaL_newmetatable(xt, "xwlua_sem_sp");
        luaL_setfuncs(xt, xwlua_semsp_metamethod, 0); /* add metamethods */
        lua_pop(xt, 1); /* pop metatable */
}

void xwlua_semsp_xt_export(lua_State * xt, const char * key, xwlua_sem_sp * semsp)
{
        xwlua_sem_sp * exp;
        xwer_t rc;

        /* 增加对象的强引用 */
        rc = xwos_sem_acquire(*semsp);
        if (XWOK == rc) {
                exp = lua_newuserdatauv(xt, sizeof(xwlua_sem_sp), 0);
                exp->sem = semsp->sem;
                exp->tik = semsp->tik;
                luaL_setmetatable(xt, "xwlua_sem_sp");
                lua_setglobal(xt, key);
        }
}

void xwlua_semsp_xt_copy(lua_State * L, xwlua_sem_sp * semsp)
{
        xwlua_sem_sp * cp;
        xwer_t rc;

        /* 增加对象的强引用 */
        rc = xwos_sem_acquire(*semsp);
        if (XWOK == rc) {
                cp = lua_newuserdatauv(L, sizeof(xwlua_sem_sp), 0);
                cp->sem = semsp->sem;
                cp->tik = semsp->tik;
                luaL_setmetatable(L, "xwlua_sem_sp");
        }
}
