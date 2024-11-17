/**
 * @file
 * @brief XWLUA库：XWOS内核：条件量
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
#include <xwos/osal/sync/cond.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwos/mtx.h"
#include "xwem/vm/lua/xwlua/xwos/spinlock.h"
#include "xwem/vm/lua/xwlua/xwos/seqlock.h"
#include "xwem/vm/lua/xwlua/xwos/sel.h"
#include "xwem/vm/lua/xwlua/xwos/cond.h"

void xwlua_cond_register(lua_State * L, const char * name,
                         struct xwos_cond * cond)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = lua_newuserdatauv(L, sizeof(xwlua_cond_sp), 0);
        condsp->cond = cond;
        condsp->tik = xwos_cond_get_tik(cond);
        rc = xwos_cond_acquire(*condsp);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_cond_sp");
                lua_setglobal(L, name);
        } else {
                lua_pop(L, 1);
        }
}

int xwlua_cond_new(lua_State * L)
{
        xwer_t rc;
        xwlua_cond_sp * condsp;

        condsp = lua_newuserdatauv(L, sizeof(xwlua_cond_sp), 0);
        rc = xwos_cond_create(condsp);
        if (XWOK == rc) {
                luaL_setmetatable(L, "xwlua_cond_sp");
        } else {
                lua_pop(L, 1);
                lua_pushnil(L);
        }
        return 1;
}

const luaL_Reg xwlua_cond_libconstructor[] = {
        {"new", xwlua_cond_new},
        {NULL, NULL},
};

void xwlua_os_init_condsp(lua_State * L);

void xwlua_os_open_cond(lua_State * L)
{
        xwlua_os_init_condsp(L);
        luaL_newlib(L, xwlua_cond_libconstructor);
}

/******** class xwlua_cond_sp ********/
int xwlua_condsp_gc(lua_State * L)
{
        xwlua_cond_sp * condsp;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        xwos_cond_release(*condsp);
        *condsp = XWLUA_COND_NULLSP;
        return 0;
}

int xwlua_condsp_tostring(lua_State * L)
{
        xwlua_cond_sp * condsp;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        lua_pushfstring(L, "{%p, %d}", condsp->cond, condsp->tik);
        return 1;
}

int xwlua_condsp_copy(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwlua_cond_sp * newcondsp;
        lua_State * D;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        D = (lua_State *)luaL_checkudata(L, 2, "xwlua_vm");
        if (D) {
                xwer_t rc;
                rc = xwos_cond_acquire(*condsp);
                if (XWOK == rc) {
                        newcondsp = lua_newuserdatauv(D, sizeof(xwlua_cond_sp), 0);
                        newcondsp->cond = condsp->cond;
                        newcondsp->tik = condsp->tik;
                        luaL_setmetatable(D, "xwlua_cond_sp");
                } else {
                        lua_pushnil(D);
                }
        } else {
                luaL_error(L, "Destination lua_State is NULL!");
        }
        return 0;
}

const luaL_Reg xwlua_condsp_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__gc", xwlua_condsp_gc},
        {"__tostring", xwlua_condsp_tostring},
        {"__copy", xwlua_condsp_copy},
        {NULL, NULL}
};

int xwlua_condsp_bind(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwlua_sel_sp * selsp;
        xwsq_t pos;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        pos = (xwsq_t)luaL_checkinteger(L, 3);
        rc = xwos_cond_bind(condsp->cond, selsp->sel, pos);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_unbind(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwlua_sel_sp * selsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        selsp = (xwlua_sel_sp *)luaL_checkudata(L, 2, "xwlua_sel_sp");
        rc = xwos_cond_unbind(condsp->cond, selsp->sel);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_freeze(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        rc = xwos_cond_freeze(condsp->cond);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_thaw(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        rc = xwos_cond_thaw(condsp->cond);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_unicast(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        rc = xwos_cond_unicast(condsp->cond);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_broadcast(lua_State * L)
{
        xwlua_cond_sp * condsp;
        xwer_t rc;

        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        rc = xwos_cond_broadcast(condsp->cond);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_wait(lua_State * L)
{
        xwlua_cond_sp * condsp;
        union {
                void * ud;
                xwlua_mtx_sp * mtxsp;
                xwlua_splk_sp * splksp;
                xwlua_sqlk_sp * sqlksp;
        } ulualksp;
        union xwos_ulock ulock;
        int top;
        xwsq_t lktype;
        xwsq_t lkst;
        xwer_t rc;

        top = lua_gettop(L);
        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        if (1 == top) {
                lktype = XWOS_LK_NONE;
                ulock.anon = NULL;
        } else if (LUA_TNIL == lua_type(L, 2)) {
                lktype = XWOS_LK_NONE;
                ulock.anon = NULL;
        } else {
                do {
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_mtx_sp");
                        if (ulualksp.ud) {
                                ulock.osal.mtx = ulualksp.mtxsp->mtx;
                                xwos_mtx_get_status(ulualksp.mtxsp->mtx, &lkst);
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        lktype = XWOS_LK_NONE;
                                } else {
                                        lktype = XWOS_LK_MTX;
                                }
                                break;
                        }
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_splk_sp");
                        if (ulualksp.ud) {
                                ulock.osal.splk = &ulualksp.splksp->luasplk->ossplk;
                                lkst = ulualksp.splksp->luasplk->lkst;
                                if ((xwsq_t)XWLUA_SPLK_LKST_LOCK == lkst) {
                                        lktype = XWOS_LK_SPLK;
                                } else {
                                        lktype = XWOS_LK_NONE;
                                }
                                break;
                        }
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_sqlk_sp");
                        if (ulualksp.ud) {
                                ulock.osal.sqlk = &ulualksp.sqlksp->luasqlk->ossqlk;
                                lkst = ulualksp.sqlksp->luasqlk->lkst;
                                switch (lkst) {
                                case XWLUA_SQLK_LKST_RDEX_LOCK:
                                        lktype = XWOS_LK_SQLK_RDEX;
                                        break;
                                case XWLUA_SQLK_LKST_WR_LOCK:
                                        lktype = XWOS_LK_SQLK_WR;
                                        break;
                                default:
                                        lktype = XWOS_LK_NONE;
                                        break;
                                }
                                break;
                        }
                        lktype = XWOS_LK_NONE;
                        ulock.anon = NULL;
                } while (false);
        }
        rc = xwos_cond_wait(condsp->cond, ulock, lktype, NULL, &lkst);
        if (rc < 0) {
                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                        switch (lktype) {
                        case XWOS_LK_MTX:
                                xwos_mtx_unlock(ulock.osal.mtx);
                                break;
                        case XWOS_LK_SPLK:
                                xwos_splk_unlock(ulock.osal.splk);
                                break;
                        case XWOS_LK_SQLK_RDEX:
                                xwos_sqlk_rdex_unlock(ulock.osal.sqlk);
                                break;
                        case XWOS_LK_SQLK_WR:
                                xwos_sqlk_wr_unlock(ulock.osal.sqlk);
                                break;
                        }
                }
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_wait_to(lua_State * L)
{
        xwlua_cond_sp * condsp;
        union {
                void * ud;
                xwlua_mtx_sp * mtxsp;
                xwlua_splk_sp * splksp;
                xwlua_sqlk_sp * sqlksp;
        } ulualksp;
        union xwos_ulock ulock;
        int top;
        xwsq_t lktype;
        xwsq_t lkst;
        xwtm_t to;
        int isnum;
        xwer_t rc;

        top = lua_gettop(L);
        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        if (1 == top) {
                lktype = XWOS_LK_NONE;
                ulock.anon = NULL;
                to = XWTM_MAX;
        } else {
                do {
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_mtx_sp");
                        if (ulualksp.ud) {
                                ulock.osal.mtx = ulualksp.mtxsp->mtx;
                                xwos_mtx_get_status(ulualksp.mtxsp->mtx, &lkst);
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        lktype = XWOS_LK_NONE;
                                } else {
                                        lktype = XWOS_LK_MTX;
                                }
                                break;
                        }
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_splk_sp");
                        if (ulualksp.ud) {
                                ulock.osal.splk = &ulualksp.splksp->luasplk->ossplk;
                                lkst = ulualksp.splksp->luasplk->lkst;
                                if ((xwsq_t)XWLUA_SPLK_LKST_LOCK == lkst) {
                                        lktype = XWOS_LK_SPLK;
                                } else {
                                        lktype = XWOS_LK_NONE;
                                }
                                break;
                        }
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_sqlk_sp");
                        if (ulualksp.ud) {
                                ulock.osal.sqlk = &ulualksp.sqlksp->luasqlk->ossqlk;
                                lkst = ulualksp.sqlksp->luasqlk->lkst;
                                switch (lkst) {
                                case XWLUA_SQLK_LKST_RDEX_LOCK:
                                        lktype = XWOS_LK_SQLK_RDEX;
                                        break;
                                case XWLUA_SQLK_LKST_WR_LOCK:
                                        lktype = XWOS_LK_SQLK_WR;
                                        break;
                                default:
                                        lktype = XWOS_LK_NONE;
                                        break;
                                }
                                break;
                        }
                        lktype = XWOS_LK_NONE;
                        ulock.anon = NULL;
                } while (false);
                to = (xwtm_t)lua_tonumberx(L, 3, &isnum);
                if (!isnum) {
                        to = XWTM_MAX;
                }
        }
        rc = xwos_cond_wait_to(condsp->cond, ulock, lktype, NULL, to, &lkst);
        if (rc < 0) {
                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                        switch (lktype) {
                        case XWOS_LK_MTX:
                                xwos_mtx_unlock(ulock.osal.mtx);
                                break;
                        case XWOS_LK_SPLK:
                                xwos_splk_unlock(ulock.osal.splk);
                                break;
                        case XWOS_LK_SQLK_RDEX:
                                xwos_sqlk_rdex_unlock(ulock.osal.sqlk);
                                break;
                        case XWOS_LK_SQLK_WR:
                                xwos_sqlk_wr_unlock(ulock.osal.sqlk);
                                break;
                        }
                }
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

int xwlua_condsp_wait_unintr(lua_State * L)
{
        xwlua_cond_sp * condsp;
        union {
                void * ud;
                xwlua_mtx_sp * mtxsp;
                xwlua_splk_sp * splksp;
                xwlua_sqlk_sp * sqlksp;
        } ulualksp;
        union xwos_ulock ulock;
        int top;
        xwsq_t lktype;
        xwsq_t lkst;
        xwer_t rc;

        top = lua_gettop(L);
        condsp = (xwlua_cond_sp *)luaL_checkudata(L, 1, "xwlua_cond_sp");
        if (1 == top) {
                lktype = XWOS_LK_NONE;
                ulock.anon = NULL;
        } else if (LUA_TNIL == lua_type(L, 2)) {
                lktype = XWOS_LK_NONE;
                ulock.anon = NULL;
        } else {
                do {
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_mtx_sp");
                        if (ulualksp.ud) {
                                ulock.osal.mtx = ulualksp.mtxsp->mtx;
                                xwos_mtx_get_status(ulualksp.mtxsp->mtx, &lkst);
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        lktype = XWOS_LK_NONE;
                                } else {
                                        lktype = XWOS_LK_MTX;
                                }
                                break;
                        }
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_splk_sp");
                        if (ulualksp.ud) {
                                ulock.osal.splk = &ulualksp.splksp->luasplk->ossplk;
                                lkst = ulualksp.splksp->luasplk->lkst;
                                if ((xwsq_t)XWLUA_SPLK_LKST_LOCK == lkst) {
                                        lktype = XWOS_LK_SPLK;
                                } else {
                                        lktype = XWOS_LK_NONE;
                                }
                                break;
                        }
                        ulualksp.ud = luaL_testudata(L, 2, "xwlua_sqlk_sp");
                        if (ulualksp.ud) {
                                ulock.osal.sqlk = &ulualksp.sqlksp->luasqlk->ossqlk;
                                lkst = ulualksp.sqlksp->luasqlk->lkst;
                                switch (lkst) {
                                case XWLUA_SQLK_LKST_RDEX_LOCK:
                                        lktype = XWOS_LK_SQLK_RDEX;
                                        break;
                                case XWLUA_SQLK_LKST_WR_LOCK:
                                        lktype = XWOS_LK_SQLK_WR;
                                        break;
                                default:
                                        lktype = XWOS_LK_NONE;
                                        break;
                                }
                                break;
                        }
                        lktype = XWOS_LK_NONE;
                        ulock.anon = NULL;
                } while (false);
        }
        rc = xwos_cond_wait_unintr(condsp->cond, ulock, lktype, NULL, &lkst);
        if (rc < 0) {
                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                        switch (lktype) {
                        case XWOS_LK_MTX:
                                xwos_mtx_unlock(ulock.osal.mtx);
                                break;
                        case XWOS_LK_SPLK:
                                xwos_splk_unlock(ulock.osal.splk);
                                break;
                        case XWOS_LK_SQLK_RDEX:
                                xwos_sqlk_rdex_unlock(ulock.osal.sqlk);
                                break;
                        case XWOS_LK_SQLK_WR:
                                xwos_sqlk_wr_unlock(ulock.osal.sqlk);
                                break;
                        }
                }
        }
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
}

const luaL_Reg xwlua_condsp_indexmethod[] = {
        {"bind", xwlua_condsp_bind},
        {"unbind", xwlua_condsp_unbind},
        {"freeze", xwlua_condsp_freeze},
        {"thaw", xwlua_condsp_thaw},
        {"unicast", xwlua_condsp_unicast},
        {"broadcast", xwlua_condsp_broadcast},
        {"wait", xwlua_condsp_wait},
        {"wait_to", xwlua_condsp_wait_to},
        {"wait_unintr", xwlua_condsp_wait_unintr},
        {NULL, NULL},
};

void xwlua_os_init_condsp(lua_State * L)
{
        /* metatable for xwlua_cond_sp */
        luaL_newmetatable(L, "xwlua_cond_sp");
        luaL_setfuncs(L, xwlua_condsp_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_condsp_indexmethod); /* create cond method table */
        luaL_setfuncs(L, xwlua_condsp_indexmethod, 0); /* add cond indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
