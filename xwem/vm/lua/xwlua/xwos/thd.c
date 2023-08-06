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
#include <xwos/osal/thd.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwos/thd.h"
#include "xwem/vm/lua/xwlua/xwvm/dataxchg.h"
#include "xwem/vm/lua/xwlua/xwvm/debug.h"

/******** xwos.thd ********/
#define XWLUA_SCRIPT_PRIORITY XWOS_SKD_PRIORITY_RT_MIN

void xwlua_os_init_thdsp(lua_State * L);

xwer_t xwlua_thd_script_main(void * arg)
{
        struct xwos_thd_attr attr;
        lua_State * L;
        const char * name;
        xwer_t rc;
        int ret;

        L = arg;
        name = lua_tolstring(L, -1, NULL);
        ret = lua_pcall(L, 1, LUA_MULTRET, 0);
        if (LUA_OK != ret) {
                lua_writestringerror("Cannot run %s\n", name);
                lua_error(L);
                rc = -ENOEXEC;
        } else {
                rc = XWOK;
        }
        xwos_cthd_get_attr(&attr);
        if (attr.detached) {
                lua_close(L);
        } else {
                rc = ptr_err(L);
        }
        return rc;
}

int xwlua_thd_dofile(lua_State * L)
{
        struct xwos_thd_attr attr;
        xwer_t rc;
        const char * arg;
        const char * name;
        xwos_thd_d thdd;
        xwlua_thd_sp * thdsp;
        lua_State * thdl;
        int top;
        int luarc;

        top = lua_gettop(L);
        if (top < 1) {
                lua_writestringerror("%s", "No script file !");
                goto err_nofile;
        }
        arg = luaL_checkstring(L, 1); // May throw an exception
        thdsp = lua_newuserdatauv(L, sizeof(xwlua_thd_sp), 0); // May throw an exception
        thdl = luaL_newstate();
        if (NULL == thdl) {
                lua_writestringerror("%s", "Create VM ... no memory");
                goto err_new_vm;
        }
        xwlua_openlibs(thdl);
        luarc = luaL_loadfile(thdl, arg);
        if (LUA_ERRFILE == luarc) {
                const char * msg = lua_tostring(thdl, -1);
                lua_writestringerror("%s\n", msg);
                lua_pop(thdl, 1);  /* remove error message */
                goto err_loadfile;
        }
        xwos_thd_attr_init(&attr);
        attr.stack = NULL;
        attr.stack_size = XWLUA_THD_STACK_SIZE;
        attr.priority = XWLUA_SCRIPT_PRIORITY;
        do {
                if (top >= 2) {
                        if (lua_toboolean(L, 2)) {
                                attr.detached = false;
                        } else {
                                attr.detached = true;
                        }
                } else {
                        attr.detached = true;
                        attr.name = lua_pushstring(thdl, arg);
                        attr.privileged = true;
                        break;
                }
                if (top >= 3) {
                        if (lua_isstring(L, 3)) {
                                name = lua_tostring(L, 3);
                                if (NULL != name) {
                                        name = lua_pushstring(thdl, name);
                                } else {
                                        name = lua_pushstring(thdl, arg);
                                }
                        } else {
                                name = lua_pushstring(thdl, arg);
                        }
                        attr.name = name;
                } else {
                        attr.name = lua_pushstring(thdl, arg);
                        attr.privileged = true;
                        break;
                }
                if (top >= 4) {
                        if (lua_toboolean(L, 4)) {
                                attr.privileged = false;
                        } else {
                                attr.privileged = true;
                        }
                } else {
                        attr.privileged = true;
                        break;
                }
        } while (false);
        rc = xwos_thd_create(&thdd, &attr, xwlua_thd_script_main, thdl);
        if (rc < 0) {
                lua_writestringerror("Create thread ... %d.", (int)rc);
                goto err_thd_create;
        }
        *thdsp = thdd;
        rc = xwos_thd_acquire(thdd); /* inc refcount */
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_thd_sp");
        } else {
                lua_pop(L, 1); // pop thdsp
                lua_pushnil(L); // push result
        }
        return 1; // return xwlua_thd_sp;

err_thd_create:
        lua_pop(thdl, 1); // pop name
        lua_pop(thdl, 1); // pop file
err_loadfile:
        lua_close(thdl);
err_new_vm:
        lua_pop(L, 1); // pop thdsp
err_nofile:
        lua_pushnil(L); // push result
        return 1;
}

int xwlua_thd_dostring(lua_State * L)
{
        struct xwos_thd_attr attr;
        xwer_t rc;
        const char * arg;
        const char * name;
        xwos_thd_d thdd;
        xwlua_thd_sp * thdsp;
        lua_State * thdl;
        size_t sl;
        int top;
        int luarc;

        top = lua_gettop(L);
        if (top < 1) {
                lua_writestringerror("%s", "No script string !");
                goto err_nostring;
        }

        arg = luaL_checklstring(L, 1, &sl); // May throw an exception
        thdsp = lua_newuserdatauv(L, sizeof(xwlua_thd_sp), 0); // May throw an exception
        thdl = luaL_newstate();
        if (NULL == thdl) {
                lua_writestringerror("%s", "Create VM ... no memory");
                goto err_new_vm;
        }

        xwlua_openlibs(thdl);
        luarc = luaL_loadbufferx(thdl, arg, sl, "main", NULL);
        if (LUA_OK != luarc) {
                const char * msg = lua_tostring(thdl, -1);
                lua_writestringerror("%s\n", msg);
                lua_pop(thdl, 1);  /* remove error message */
                goto err_loadstring;
        }

        xwos_thd_attr_init(&attr);
        attr.stack = NULL;
        attr.stack_size = XWLUA_THD_STACK_SIZE;
        attr.priority = XWLUA_SCRIPT_PRIORITY;
        do {
                if (top >= 2) {
                        if (lua_toboolean(L, 2)) {
                                attr.detached = false;
                        } else {
                                attr.detached = true;
                        }
                } else {
                        attr.detached = true;
                        attr.name = lua_pushstring(thdl, "anon");
                        attr.privileged = true;
                        break;
                }
                if (top >= 3) {
                        if (lua_isstring(L, 3)) {
                                name = lua_tolstring(L, 3, NULL);
                                if (NULL != name) {
                                        name = lua_pushstring(thdl, name);
                                } else {
                                        name = lua_pushstring(thdl, "anon");
                                }
                        } else {
                                name = lua_pushstring(thdl, "anon");
                        }
                        attr.name = name;
                } else {
                        attr.name = lua_pushstring(thdl, "anon");
                        attr.privileged = true;
                        break;
                }
                if (top >= 4) {
                        if (lua_toboolean(L, 4)) {
                                attr.privileged = false;
                        } else {
                                attr.privileged = true;
                        }
                } else {
                        attr.privileged = true;
                        break;
                }
        } while (false);
        rc = xwos_thd_create(&thdd, &attr, xwlua_thd_script_main, thdl);
        if (rc < XWOK) {
                lua_writestringerror("Create thread ... %d.", (int)rc);
                goto err_thd_create;
        }
        *thdsp = thdd;
        rc = xwos_thd_acquire(thdd); /* inc refcount */
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_thd_sp");
        } else {
                lua_pop(L, 1); // pop thdsp
                lua_pushnil(L); // push result
        }
        return 1; // return xwlua_thd_sp;

err_thd_create:
        lua_pop(thdl, 1); // pop name
        lua_pop(thdl, 1); // pop string
err_loadstring:
        lua_close(thdl);
err_new_vm:
        lua_pop(L, 1); // pop thdsp
err_nostring:
        lua_pushnil(L);
        return 1;
}

extern int xwlua_vm_function_writer(lua_State * src,
                                    const void * b,
                                    size_t size,
                                    void * ud);

int xwlua_thd_call(lua_State * L)
{
        struct xwos_thd_attr attr;
        xwer_t rc;
        luaL_Buffer lb;
        xwos_thd_d thdd;
        xwlua_thd_sp * thdsp;
        lua_State * thdl;
        const char * name;
        const char * func;
        size_t fl;
        int top;
        int luarc;

        top = lua_gettop(L);
        if (top < 1) {
                lua_writestringerror("%s", "No function !");
                goto err_nofunc;
        }

        luaL_checktype(L, 1, LUA_TFUNCTION); // May throw an exception
        thdsp = lua_newuserdatauv(L, sizeof(xwlua_thd_sp), 0); // May throw an exception
        luaL_buffinit(L, &lb); /* push buffer */
        luaL_prepbuffer(&lb);
        lua_pushvalue(L, 1); /* Ensure that top is the function */
        rc = lua_dump(L, xwlua_vm_function_writer, &lb, true);
        lua_pop(L, 1); /* Pop top function */
        if (0 != rc) {
                lua_writestringerror("%s", "unable to dump given function");
                goto err_dump;
        }
        luaL_pushresult(&lb); /* Pop buffer and Push function string */
        func = lua_tolstring(L, -1, &fl);
        thdl = luaL_newstate();
        if (NULL == thdl) {
                lua_writestringerror("%s", "Create VM ... no memory");
                goto err_new_vm;
        }

        xwlua_openlibs(thdl);
        luarc = luaL_loadbufferx(thdl, func, fl, "main", "bt");
        if (LUA_OK != luarc) {
                const char * msg = lua_tostring(thdl, -1);
                lua_writestringerror("%s\n", msg);
                lua_pop(thdl, 1);  /* remove error message */
                goto err_loadfunc;
        }
        lua_pop(L, 1); // pop func

        xwos_thd_attr_init(&attr);
        attr.stack = NULL;
        attr.stack_size = XWLUA_THD_STACK_SIZE;
        attr.priority = XWLUA_SCRIPT_PRIORITY;
        do {
                if (top >= 2) {
                        if (lua_toboolean(L, 2)) {
                                attr.detached = false;
                        } else {
                                attr.detached = true;
                        }
                } else {
                        attr.detached = true;
                        attr.name = lua_pushstring(thdl, "anon");
                        attr.privileged = true;
                        break;
                }
                if (top >= 3) {
                        if (lua_isstring(L, 3)) {
                                name = lua_tolstring(L, 3, NULL);
                                if (NULL != name) {
                                        name = lua_pushstring(thdl, name);
                                } else {
                                        name = lua_pushstring(thdl, "anon");
                                }
                        } else {
                                name = lua_pushstring(thdl, "anon");
                        }
                        attr.name = name;
                } else {
                        attr.name = lua_pushstring(thdl, "anon");
                        attr.privileged = true;
                        break;
                }
                if (top >= 4) {
                        if (lua_toboolean(L, 4)) {
                                attr.privileged = false;
                        } else {
                                attr.privileged = true;
                        }
                } else {
                        attr.privileged = true;
                        break;
                }
        } while (false);
        rc = xwos_thd_create(&thdd, &attr, xwlua_thd_script_main, thdl);
        if (rc < XWOK) {
                lua_writestringerror("Create thread ... %d.", (int)rc);
                goto err_thd_create;
        }
        *thdsp = thdd;
        rc = xwos_thd_acquire(thdd); /* inc refcount */
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_thd_sp");
        } else {
                lua_pop(L, 1); // pop thdsp
                lua_pushnil(L); // push result
        }
        return 1; // return xwlua_thd_sp;

err_thd_create:
        lua_pop(thdl, 1); // pop name
        lua_pop(thdl, 1); // pop func
err_loadfunc:
        lua_close(thdl);
err_new_vm:
        lua_pop(L, 1); // pop func
err_dump:
        lua_pop(L, 1); // pop thdsp
err_nofunc:
        lua_pushnil(L);
        return 1;
}

const luaL_Reg xwlua_thd_libconstructor[] = {
        {"dofile", xwlua_thd_dofile},
        {"dostring", xwlua_thd_dostring},
        {"call", xwlua_thd_call},
        {NULL, NULL},
};

void xwlua_os_open_thd(lua_State * L)
{
        xwlua_os_init_thdsp(L);
        luaL_newlib(L, xwlua_thd_libconstructor);
}

/******** xwos.cthd ********/
int xwlua_cthd_sp(lua_State * L)
{
        xwos_thd_d thdd;
        xwlua_thd_sp * out;

        thdd = xwos_cthd_self();
        out = lua_newuserdatauv(L, sizeof(xwlua_thd_sp), 0);
        out->thd = thdd.thd;
        out->tik = thdd.tik;
        xwos_thd_grab(thdd); /* inc refcount */
        luaL_setmetatable(L, "xwlua_thd_sp");
        return 1;
}

int xwlua_cthd_sleep(lua_State * L)
{
        xwtm_t time;
        xwer_t rc;

        time = (xwtm_t)luaL_checknumber(L, 1);
        rc = xwos_cthd_sleep(time);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_cthd_sleep_to(lua_State * L)
{
        xwtm_t time;
        xwer_t rc;

        time = (xwtm_t)luaL_checknumber(L, 1);
        rc = xwos_cthd_sleep_to(time);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_cthd_sleep_from(lua_State * L)
{
        xwtm_t origin;
        xwtm_t inc;
        xwer_t rc;

        origin = (xwtm_t)luaL_checknumber(L, 1);
        inc = (xwtm_t)luaL_checknumber(L, 2);
        rc = xwos_cthd_sleep_from(&origin, inc);
        lua_pushinteger(L, (lua_Integer)rc);
        lua_pushnumber(L, (lua_Number)origin);
        return 2;
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
        xwer_t rc;

        rc = xwos_cthd_freeze();
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwcthdlib[] = {
        {"sp", xwlua_cthd_sp},
        {"sleep", xwlua_cthd_sleep},
        {"sleep_to", xwlua_cthd_sleep_to},
        {"sleep_from", xwlua_cthd_sleep_from},
        {"yield", xwlua_cthd_yield},
        {"exit", xwlua_cthd_exit},
        {"shld_stop", xwlua_cthd_shld_stop},
        {"frz_shld_stop", xwlua_cthd_frz_shld_stop},
        {"shld_frz", xwlua_cthd_shld_frz},
        {"frz", xwlua_cthd_freeze},
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
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
        if (NULL != D) {
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

int xwlua_thdsp_detach(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwer_t rc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_detach(*thdsp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_thdsp_intr(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwer_t rc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_intr(*thdsp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_thdsp_quit(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwer_t rc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_quit(*thdsp);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_thdsp_join(lua_State * L)
{
        lua_State * childL;
        xwlua_thd_sp * thdsp;
        xwer_t rc;
        xwer_t trc;
        int nres;
        int i;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_join(*thdsp, &trc);
        if (XWOK == rc) {
                lua_pushinteger(L, (lua_Integer)rc);
                childL = err_ptr(trc);
                nres = lua_gettop(childL);
                for (i = 1; i <= nres; i++) {
                        xwlua_vm_copy_element(childL, i, L);
                }
                lua_close(childL);
                nres += 1;
        } else {
                lua_pushinteger(L, (lua_Integer)rc);
                nres = 1;
        }
        return nres;
}

int xwlua_thdsp_stop(lua_State * L)
{
        lua_State * childL;
        xwlua_thd_sp * thdsp;
        xwer_t rc;
        xwer_t trc;
        int nres;
        int i;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        rc = xwos_thd_stop(*thdsp, &trc);
        if (XWOK == rc) {
                lua_pushinteger(L, (lua_Integer)rc);
                childL = err_ptr(trc);
                xwlua_vm_dump_stack(childL);
                xwlua_vm_dump_stack(L);
                nres = lua_gettop(childL);
                for (i = 1; i <= nres; i++) {
                        xwlua_vm_move_element(childL, i, L);
                }
                xwlua_vm_dump_stack(L);
                lua_close(childL);
                nres += 1;
        } else {
                lua_pushinteger(L, (lua_Integer)rc);
                nres = 1;
        }
        return nres;
}

int xwlua_thdsp_migrate(lua_State * L)
{
        xwlua_thd_sp * thdsp;
        xwid_t cpu;
        xwer_t rc;

        thdsp = (xwlua_thd_sp *)luaL_checkudata(L, 1, "xwlua_thd_sp");
        cpu = luaL_checkinteger(L, 2);
        rc = xwos_thd_migrate(*thdsp, cpu);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_thdsp_indexmethod[] = {
        {"detach", xwlua_thdsp_detach},
        {"intr", xwlua_thdsp_intr},
        {"quit", xwlua_thdsp_quit},
        {"join", xwlua_thdsp_join},
        {"stop", xwlua_thdsp_stop},
        {"migrate", xwlua_thdsp_migrate},
        {NULL, NULL},
};

void xwlua_os_init_thdsp(lua_State * L)
{
        /* metatable for struct xwlua_thd_sp */
        luaL_newmetatable(L, "xwlua_thd_sp");
        luaL_setfuncs(L, xwlua_thdsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_thdsp_indexmethod); /* create thdsp method table */
        luaL_setfuncs(L, xwlua_thdsp_indexmethod, 0); /* add thdsp indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
