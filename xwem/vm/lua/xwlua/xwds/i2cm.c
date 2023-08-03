/**
 * @file
 * @brief 玄武Lua库：I2C master
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
#include <xwcd/ds/i2c/master.h>
#include "xwem/vm/lua/src/lauxlib.h"
#include "xwem/vm/lua/xwlua/port.h"
#include "xwem/vm/lua/xwlua/xwds/i2cm.h"

void xwlua_i2cm_register(lua_State * L, const char * name, struct xwds_i2cm * i2cm)
{
        struct xwlua_i2cm * luai2cm;

        luai2cm = lua_newuserdatauv(L, sizeof(struct xwlua_i2cm), 0);
        luai2cm->i2cm = i2cm;
        luai2cm->tik = i2cm->dev.obj.xwobj.tik;
        luaL_setmetatable(L, "xwlua_i2cm");
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, name);
        lua_setglobal(L, name);
}

void xwlua_i2cm_unregister(lua_State * L, const char * name)
{
        lua_pushnil(L);
        lua_setglobal(L, name);
}

int xwlua_i2cm_tostring(lua_State * L)
{
        struct xwlua_i2cm * luai2cm;

        luai2cm = (struct xwlua_i2cm *)luaL_checkudata(L, 1, "xwlua_i2cm");
        lua_pushstring(L, luai2cm->i2cm->dev.name);
        return 1;
}

int xwlua_i2cm_xfer(lua_State * L)
{
        int top;
        struct xwlua_i2cm * luai2cm;
        struct xwds_i2c_msg msg;
        xwsz_t txdsize;
        luaL_Buffer b;
        xwtm_t time;
        xwer_t rc;
        int ret;

        top = lua_gettop(L);
        luai2cm = (struct xwlua_i2cm *)luaL_checkudata(L, 1, "xwlua_i2cm");
        msg.addr = (xwu16_t)luaL_checkinteger(L, 2);
        if (msg.addr & (xwu16_t)0xFF00) {
                msg.flag |= XWDS_I2C_F_10BITADDR;
        } else {
                msg.flag |= XWDS_I2C_F_7BITADDR;
        }
        if (lua_toboolean(L, 3)) {
                msg.flag |= XWDS_I2C_F_START;
        }
        if (lua_toboolean(L, 4)) {
                msg.flag |= XWDS_I2C_F_STOP;
        }
        if (lua_toboolean(L, 5)) {
                msg.flag |= XWDS_I2C_F_RD;
        }
        msg.size = (xwsz_t)luaL_checkinteger(L, 6);
        if (msg.flag & XWDS_I2C_F_RD) {
                luaL_buffinit(L, &b);
                msg.data = (xwu8_t *)luaL_prepbuffsize(&b, msg.size);
                if (top >= 7) {
                        time = (xwtm_t)luaL_checknumber(L, 7);
                } else {
                        time = XWTM_MAX;
                }
                rc = xwds_i2cm_xfer(luai2cm->i2cm, &msg, time);
                luaL_addsize(&b, msg.size);
                lua_pushinteger(L, (lua_Integer)rc);
                luaL_pushresult(&b);
                ret = 2;
        } else {
                msg.data = (xwu8_t *)luaL_checklstring(L, 7, &txdsize);
                if (msg.size > txdsize) {
                        msg.size = txdsize;
                }
                if (top >= 8) {
                        time = (xwtm_t)luaL_checknumber(L, 8);
                } else {
                        time = XWTM_MAX;
                }
                rc = xwds_i2cm_xfer(luai2cm->i2cm, &msg, time);
                lua_pushinteger(L, (lua_Integer)rc);
                ret = 1;
        }
        return ret;
}

const luaL_Reg xwlua_i2cm_indexmethod[] = {
        {"xfer", xwlua_i2cm_xfer},
        {NULL, NULL},
};

const luaL_Reg xwlua_i2cm_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__tostring", xwlua_i2cm_tostring},
        {NULL, NULL}
};

void xwlua_ds_init_i2cm(lua_State * L)
{
        /* metatable for xwlua_i2cm */
        luaL_newmetatable(L, "xwlua_i2cm");
        luaL_setfuncs(L, xwlua_i2cm_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_i2cm_indexmethod); /* create i2cm method table */
        luaL_setfuncs(L, xwlua_i2cm_indexmethod, 0); /* add i2cm indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}

const luaL_Reg xwlua_i2cm_libconstructor[] = {
        {"xfer", xwlua_i2cm_xfer},
        {"read", NULL},
        {"write", NULL},
        {"start", NULL},
        {"stop", NULL},
        {NULL, NULL},
};

void xwlua_ds_open_i2cm(lua_State * L)
{
        xwlua_ds_init_i2cm(L);
        luaL_newlib(L, xwlua_i2cm_libconstructor);

        lua_pushinteger(L, XWDS_I2C_F_RD);
        lua_setfield(L, -2, "read");
        lua_pushinteger(L, XWDS_I2C_F_WR);
        lua_setfield(L, -2, "write");
        lua_pushinteger(L, XWDS_I2C_F_START);
        lua_setfield(L, -2, "start");
        lua_pushinteger(L, XWDS_I2C_F_STOP);
        lua_setfield(L, -2, "stop");
}
