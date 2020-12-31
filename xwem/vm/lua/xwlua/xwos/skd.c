/**
 * @file
 * @brief 玄武OS Lua Lib：调度
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
#include <xwos/osal/skd.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwos/skd.h"

/******** xwos.skd ********/
int xwlua_skd_get_timetick_lc(lua_State * L)
{
        xwtm_t timetick;

        timetick = xwos_skd_get_timetick_lc();
        lua_pushnumber(L, (lua_Number)timetick);
        return 1;
}

int xwlua_skd_get_tickcount_lc(lua_State * L)
{
        xwtm_t tickcount;

        tickcount = xwos_skd_get_tickcount_lc();
        lua_pushnumber(L, (lua_Number)tickcount);
        return 1;
}

int xwlua_skd_get_timestamp_lc(lua_State * L)
{
        xwtm_t timestamp;

        timestamp = xwos_skd_get_timestamp_lc();
        lua_pushnumber(L, (lua_Number)timestamp);
        return 1;
}

const luaL_Reg xwlua_skd_method[] = {
        {"tt", xwlua_skd_get_timetick_lc},
        {"tc", xwlua_skd_get_tickcount_lc},
        {"ts", xwlua_skd_get_timestamp_lc},
        {NULL, NULL},
};

void xwlua_os_open_skd(lua_State * L)
{
        luaL_newlib(L, xwlua_skd_method);
}

/******** xwos.thd ********/
#define XWLUA_SCRIPT_PRIORITY XWOS_SKD_PRIORITY_RT_MIN

void xwlua_os_init_thdsp(lua_State * L);

xwer_t xwlua_thd_script_main(void * arg)
{
        lua_State * L;
        const char * script;
        xwer_t rc;
        int ret;

        L = arg;
        script = lua_tolstring(L, -1, NULL);
        ret = lua_pcall(L, 1, LUA_MULTRET, 0);
        if (LUA_OK != ret) {
                lua_writestringerror("Cannot run %s", script);
                rc = -ENOEXEC;
        } else {
                rc = XWOK;
        }
        lua_close(L);
        return rc;
}

int xwlua_thd_dofile(lua_State * L)
{
        xwer_t rc;
        const char * arg;
        xwlua_thd_sp * thdsp;
        lua_State * thdl;

        arg = luaL_checkstring(L, 1);
        thdsp = lua_newuserdatauv(L, sizeof(xwlua_thd_sp), 0);
        thdl = luaL_newstate();
        if (thdl) {
                xwlua_openlibs(thdl);
                luaL_loadfile(thdl, arg);
                arg = lua_pushstring(thdl, arg);
                rc = xwos_thd_create(&thdsp->thd, arg,
                                     xwlua_thd_script_main, thdl, XWLUA_THD_STACK,
                                     XWLUA_SCRIPT_PRIORITY, XWOS_SKDATTR_PRIVILEGED);
                if (XWOK == rc) {
                        xwos_thd_grab(thdsp->thd); /* 增加对象的强引用 */
                        thdsp->tik = xwos_thd_gettik(thdsp->thd);
                        luaL_setmetatable(L, "xwlua_thd_sp");
                } else {
                        *thdsp = XWLUA_THD_NULLSP;
                        lua_pop(L, 1);
                        lua_pushnil(L);
                        lua_writestringerror("Cannot create thread: %d.", rc);
                }
        } else {
                *thdsp = XWLUA_THD_NULLSP;
                lua_pop(L, 1);
                lua_pushnil(L);
                lua_writestringerror("%s", "Cannot create VM: not enough memory");
        }
        return 1;
}

int xwlua_thd_dostring(lua_State * L)
{
        xwer_t rc;
        const char * arg;
        const char * name;
        xwlua_thd_sp * thdsp;
        lua_State * thdl;
        size_t sl;

        arg = luaL_checklstring(L, 1, &sl);
        thdsp = lua_newuserdatauv(L, sizeof(xwlua_thd_sp), 0);
        thdl = luaL_newstate();
        if (thdl) {
                xwlua_openlibs(thdl);
                luaL_loadbufferx(thdl, arg, sl, "main", NULL);
                name = lua_pushstring(thdl, "main");
                rc = xwos_thd_create(&thdsp->thd, name,
                                     xwlua_thd_script_main, thdl, XWLUA_THD_STACK,
                                     XWLUA_SCRIPT_PRIORITY, XWOS_SKDATTR_PRIVILEGED);
                if (XWOK == rc) {
                        xwos_thd_grab(thdsp->thd); /* 增加对象的强引用 */
                        thdsp->tik = xwos_thd_gettik(thdsp->thd);
                        luaL_setmetatable(L, "xwlua_thd_sp");
                } else {
                        *thdsp = XWLUA_THD_NULLSP;
                        lua_pop(L, 1);
                        lua_pushnil(L);
                        lua_writestringerror("Cannot create thread: %d.", rc);
                }
        } else {
                *thdsp = XWLUA_THD_NULLSP;
                lua_pop(L, 1);
                lua_pushnil(L);
                lua_writestringerror("%s", "Cannot create VM: not enough memory");
        }
        return 1;
}

int xwlua_xt_function_writer(lua_State * L, const void * b, size_t size, void * ud);

int xwlua_thd_call(lua_State * L)
{
        xwer_t rc;
        luaL_Buffer lb;
        xwlua_thd_sp * thdsp;
        lua_State * thdl;
        const char * name;
        const char * func;
        size_t fl;

        luaL_checktype(L, 1, LUA_TFUNCTION);
        lua_pushvalue(L, 1); /* Ensure that top is the function */
        luaL_buffinit(L, &lb);
        rc = lua_dump(L, xwlua_xt_function_writer, &lb, true);
        lua_pop(L, 1);
        if (0 != rc) {
                luaL_error(L, "unable to dump given function");
        }
        luaL_pushresult(&lb);
        func = lua_tolstring(L, -1, &fl);

        thdsp = lua_newuserdatauv(L, sizeof(xwlua_thd_sp), 0);
        thdl = luaL_newstate();
        if (thdl) {
                xwlua_openlibs(thdl);
                luaL_loadbufferx(thdl, func, fl, "main", "bt");
                name = lua_pushstring(thdl, "main");
                rc = xwos_thd_create(&thdsp->thd, name,
                                     xwlua_thd_script_main, thdl, XWLUA_THD_STACK,
                                     XWLUA_SCRIPT_PRIORITY, XWOS_SKDATTR_PRIVILEGED);
                if (XWOK == rc) {
                        xwos_thd_grab(thdsp->thd); /* 增加对象的强引用 */
                        thdsp->tik = xwos_thd_gettik(thdsp->thd);
                        luaL_setmetatable(L, "xwlua_thd_sp");
                } else {
                        *thdsp = XWLUA_THD_NULLSP;
                        lua_pop(L, 1);
                        lua_pushnil(L);
                        lua_writestringerror("Cannot create thread: %d.", rc);
                }
        } else {
                *thdsp = XWLUA_THD_NULLSP;
                lua_pop(L, 1);
                lua_pushnil(L);
                lua_writestringerror("%s", "Cannot create VM: not enough memory");
        }
        lua_remove(L, -2);
        return 1;
}

const luaL_Reg xwlua_thd_method[] = {
        {"dofile", xwlua_thd_dofile},
        {"do", xwlua_thd_dostring},
        {"call", xwlua_thd_call},
        {NULL, NULL},
};

void xwlua_os_open_thd(lua_State * L)
{
        xwlua_os_init_thdsp(L);
        luaL_newlib(L, xwlua_thd_method);
}

/******** xwos.cthd ********/
int xwlua_cthd_sp(lua_State * L)
{
        xwlua_thd_sp thdsp;
        xwlua_thd_sp * out;

        thdsp = xwos_thd_getd(xwos_cthd_self());
        out = lua_newuserdatauv(L, sizeof(xwlua_thd_sp), 0);
        out->thd = thdsp.thd;
        out->tik = thdsp.tik;
        xwos_thd_grab(out->thd); /* 增加对象的强引用 */
        luaL_setmetatable(L, "xwlua_thd_sp");
        return 1;
}

int xwlua_cthd_sleep(lua_State * L)
{
        xwtm_t time;
        xwer_t rc;

        time = (xwtm_t)luaL_checknumber(L, 1);
        rc = xwosdl_cthd_sleep(&time);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_cthd_sleep_from(lua_State * L)
{
        xwtm_t origin, inc;
        xwer_t rc;

        origin = (xwtm_t)luaL_checknumber(L, 1);
        inc = (xwtm_t)luaL_checknumber(L, 2);
        rc = xwosdl_cthd_sleep_from(&origin, inc);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_cthd_yield(lua_State * L)
{
        XWOS_UNUSED(L);

        xwos_cthd_yield();
        return 0;
}

int xwlua_cthd_exit(lua_State * L)
{
        xwer_t rc;

        rc = (xwer_t)luaL_checkinteger(L, 1);
        xwos_cthd_exit(rc);
        return 0;
}

int xwlua_cthd_shld_frz(lua_State * L)
{
        bool frz;

        frz = xwos_cthd_shld_frz();
        lua_pushboolean(L, (int)frz);
        return 1;
}

int xwlua_cthd_shld_stop(lua_State * L)
{
        bool stop;

        stop = xwos_cthd_shld_stop();
        lua_pushboolean(L, (int)stop);
        return 1;
}

int xwlua_cthd_frz_shld_stop(lua_State * L)
{
        bool stop;
        bool frz;

        stop = xwos_cthd_frz_shld_stop(&frz);
        lua_pushboolean(L, (int)stop);
        lua_pushboolean(L, (int)frz);
        return 2;
}

int xwlua_cthd_freeze(lua_State * L)
{
        XWOS_UNUSED(L);

        xwos_cthd_freeze();
        return 0;
}

const luaL_Reg xwcthdlib[] = {
        {"sp", xwlua_cthd_sp},
        {"sleep", xwlua_cthd_sleep},
        {"sleep_from", xwlua_cthd_sleep_from},
        {"yield", xwlua_cthd_yield},
        {"exit", xwlua_cthd_exit},
        {"shld_frz", xwlua_cthd_shld_frz},
        {"shld_stop", xwlua_cthd_shld_stop},
        {"frz_shld_stop", xwlua_cthd_frz_shld_stop},
        {"freeze", xwlua_cthd_freeze},
        {NULL, NULL},
};

void xwlua_os_open_cthd(lua_State * L)
{
        luaL_newlib(L, xwcthdlib);
}

/******** class xwlua_thd_sp ********/
int xwlua_thdsp_gc(lua_State * L)
{
        xwlua_thd_sp * thdsp;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        xwos_thd_release(*thdsp);
        *thdsp = XWLUA_THD_NULLSP;
        return 0;
}

int xwlua_thdsp_tostring(lua_State * L)
{
        xwlua_thd_sp * thdsp;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        lua_pushfstring(L, "{%p, %d}", thdsp->thd, thdsp->tik);
        return 1;
}

int xwlua_thdsp_copy(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwlua_thd_sp * newthdsp;
        lua_State * D;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        D = (lua_State *)lua_touserdata(L, 2);
        if (D) {
                xwer_t rc;
                rc = xwos_thd_acquire(*thdsp);
                if (XWOK == rc) {
                        newthdsp = lua_newuserdatauv(D, sizeof(xwlua_thd_sp), 0);
                        newthdsp->thd = thdsp->thd;
                        newthdsp->tik = thdsp->tik;
                        luaL_setmetatable(D, "xwlua_thd_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_thdsp_metamethod[] = {
        {"__index", NULL},
        {"__gc", xwlua_thdsp_gc},
        {"__tostring", xwlua_thdsp_tostring},
        {"__copy", xwlua_thdsp_copy},
        {NULL, NULL},
};

int xwlua_thdsp_stop(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwer_t rc, trc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_stop(thdsp->thd, &trc);
        if (XWOK == rc) {
                lua_pushinteger(L, (lua_Integer)rc);
                lua_pushinteger(L, (lua_Integer)trc);
        } else {
                lua_pushinteger(L, (lua_Integer)rc);
                lua_pushnil(L);
        }
        return 2;
}

int xwlua_thdsp_cancel(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwer_t rc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_cancel(thdsp->thd);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_thdsp_join(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwer_t rc, trc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_join(thdsp->thd, &trc);
        if (XWOK == rc) {
                lua_pushinteger(L, (lua_Integer)rc);
                lua_pushinteger(L, (lua_Integer)trc);
        } else {
                lua_pushinteger(L, (lua_Integer)rc);
                lua_pushnil(L);
        }
        return 2;
}

int xwlua_thdsp_detach(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwer_t rc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_detach(thdsp->thd);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_thdsp_migrate(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwid_t cpu;
        xwer_t rc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        cpu = luaL_checkinteger(L, 2);
        rc = xwos_thd_migrate(thdsp->thd, cpu);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_thdsp_method[] = {
        {"stop", xwlua_thdsp_stop},
        {"cancel", xwlua_thdsp_cancel},
        {"join", xwlua_thdsp_join},
        {"detach", xwlua_thdsp_detach},
        {"migrate", xwlua_thdsp_migrate},
        {NULL, NULL},
};

void xwlua_os_init_thdsp(lua_State * L)
{
        /* metatable for struct xwlua_thd_sp */
        luaL_newmetatable(L, "xwlua_thd_sp");
        luaL_setfuncs(L, xwlua_thdsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_thdsp_method); /* create thdsp method table */
        luaL_setfuncs(L, xwlua_thdsp_method, 0); /* add thdsp methods */
        lua_setfield(L, -2, "__index");  /* metatable.__index = xwlua_thdsp_method */
        lua_pop(L, 1); /* pop metatable */
}