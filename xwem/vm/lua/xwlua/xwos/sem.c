/**
 * @file
 * @brief XWLUA库：信号量
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
#include <xwos/osal/sync/sem.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwos/sel.h"
#include "xwem/vm/lua/xwlua/xwos/sem.h"

int xwlua_sem_new(lua_State * L)
{
        xwer_t rc;
        xwssq_t val;
        xwssq_t max;
        xwlua_sem_sp * semsp;

        val = luaL_checkinteger(L, 1);
        max = luaL_checkinteger(L, 2);
        if (max <= 0) {
                max = XWSSQ_MAX;
        }
        semsp = lua_newuserdatauv(L, sizeof(xwlua_sem_sp), 0);
        rc = xwos_sem_create(semsp, val, max);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_sem_sp");
        } else {
                lua_pop(L, 1);
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_sem_libconstructor[] = {
        {"new", xwlua_sem_new},
        {NULL, NULL},
};

void xwlua_os_init_semsp(lua_State * L);

void xwlua_os_open_sem(lua_State * L)
{
        xwlua_os_init_semsp(L);
        luaL_newlib(L, xwlua_sem_libconstructor);
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

int xwlua_semsp_copy(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwlua_sem_sp * newsemsp;
        lua_State * D;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
        if (D) {
                xwer_t rc;
                rc = xwos_sem_acquire(*semsp);
                if (XWOK == rc) {
                        newsemsp = lua_newuserdatauv(D, sizeof(xwlua_sem_sp), 0);
                        newsemsp->sem = semsp->sem;
                        newsemsp->tik = semsp->tik;
                        luaL_setmetatable(D, "xwlua_sem_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_semsp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_semsp_gc},
        {"__tostring", xwlua_semsp_tostring},
        {"__copy", xwlua_semsp_copy},
        {NULL, NULL}
};

int xwlua_semsp_bind(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwlua_sel_sp * selsp;
        xwsq_t pos;
        xwer_t rc;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        pos = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwos_sem_bind(semsp->sem, selsp->sel, pos);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_semsp_unbind(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwlua_sel_sp * selsp;
        xwer_t rc;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        rc = xwos_sem_unbind(semsp->sem, selsp->sel);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

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

int xwlua_semsp_wait(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwer_t rc;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        rc = xwos_sem_wait(semsp->sem);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_semsp_trywait(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwer_t rc;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        rc = xwos_sem_trywait(semsp->sem);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_semsp_wait_to(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwtm_t to;
        xwer_t rc;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        to = (xwtm_t)lua_tonumber(L, 2);
        rc = xwos_sem_wait_to(semsp->sem, to);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_semsp_get_max(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwssq_t max;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        xwos_sem_get_max(semsp->sem, &max);
        lua_pushinteger(L, (lua_Integer)max);
        return 1;
}

int xwlua_semsp_get_value(lua_State * L)
{
        xwlua_sem_sp * semsp;
        xwssq_t val;

        semsp = (xwlua_sem_sp *)luaL_checkudata(L, 1, "xwlua_sem_sp");
        xwos_sem_get_value(semsp->sem, &val);
        lua_pushinteger(L, (lua_Integer)val);
        return 1;
}

const luaL_Reg xwlua_semsp_indexmethod[] = {
        {"bind", xwlua_semsp_bind},
        {"unbind", xwlua_semsp_unbind},
        {"freeze", xwlua_semsp_freeze},
        {"thaw", xwlua_semsp_thaw},
        {"post", xwlua_semsp_post},
        {"wait", xwlua_semsp_wait},
        {"trywait", xwlua_semsp_trywait},
        {"wait_to", xwlua_semsp_wait_to},
        {"get_max", xwlua_semsp_get_max},
        {"get_value", xwlua_semsp_get_value},
        {NULL, NULL},
};

void xwlua_os_init_semsp(lua_State * L)
{
        /* metatable for xwlua_sem_sp */
        luaL_newmetatable(L, "xwlua_sem_sp");
        luaL_setfuncs(L, xwlua_semsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_semsp_indexmethod); /* create sem method table */
        luaL_setfuncs(L, xwlua_semsp_indexmethod, 0); /* add sem indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
