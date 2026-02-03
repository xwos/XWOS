/**
 * @file
 * @brief XWLUA库：设备栈：I2C主机控制器
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
        lua_setglobal(L, name);
}

void xwlua_i2cm_unregister(lua_State * L, const char * name)
{
        lua_pushnil(L);
        lua_setglobal(L, name);
}

int xwlua_i2cm_new_msg(lua_State * L)
{
        struct xwlua_i2cm_msg * msg;
        xwsz_t num;
        xwsz_t totalsize;
        xwsz_t i;

        num = (xwsz_t)luaL_checkinteger(L, 1);
        totalsize = sizeof(struct xwds_i2c_msg) * num + sizeof(struct xwlua_i2cm_msg);
        msg = (struct xwlua_i2cm_msg *)lua_newuserdatauv(L, totalsize, 0U);
        msg->num = num;
        for (i = 0; i < num; i++) {
                msg->msg[i].addr = 0U;
                msg->msg[i].flag = 0U;
                msg->msg[i].size = 0U;
                msg->msg[i].data = NULL;
        }
        luaL_setmetatable(L, "xwlua_i2cm_msg");
        return 1;
}

const luaL_Reg xwlua_i2cm_libconstructor[] = {
        {"msg", xwlua_i2cm_new_msg},
        {NULL, NULL},
};

void xwlua_ds_init_i2cm(lua_State * L);
void xwlua_ds_init_i2cm_msg(lua_State * L);

void xwlua_ds_open_i2cm(lua_State * L)
{
        xwlua_ds_init_i2cm(L);
        xwlua_ds_init_i2cm_msg(L);
        luaL_newlib(L, xwlua_i2cm_libconstructor);
}

/******** class xwlua_i2cm ********/
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
        struct xwlua_i2cm_msg * msg;
        xwtm_t to;
        xwer_t rc;

        top = lua_gettop(L);
        luai2cm = (struct xwlua_i2cm *)luaL_checkudata(L, 1, "xwlua_i2cm");
        msg = (struct xwlua_i2cm_msg *)luaL_checkudata(L, 2, "xwlua_i2cm_msg");
        if (top >= 3) {
                to = (xwtm_t)luaL_checknumber(L, 3);
        } else {
                to = XWTM_MAX;
        }
        rc = xwds_i2cm_xfer(luai2cm->i2cm, msg->msg, msg->num, to);
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
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

/******** class xwlua_i2cm_msg ********/
int xwlua_i2cm_msg_set_addr(lua_State * L)
{
        struct xwlua_i2cm_msg * msg;
        xwu32_t idx;
        xwu16_t addr;
        int addr10bit;

        msg = (struct xwlua_i2cm_msg *)luaL_checkudata(L, 1, "xwlua_i2cm_msg");
        idx = (xwsz_t)luaL_checkinteger(L, 2);
        addr = (xwu16_t)luaL_checkinteger(L, 3);
        addr10bit = lua_toboolean(L, 4);

        if (idx < msg->num) {
                msg->msg[idx].addr = addr;
                if (addr10bit) {
                        msg->msg[idx].flag |= XWDS_I2C_F_10BITADDR;
                } else {
                        msg->msg[idx].flag &= ~XWDS_I2C_F_10BITADDR;
                }
        } else {
                luaL_error(L, "Index (%d) is out of range (%d)", idx, msg->num);
        }
        return 0;
}

int xwlua_i2cm_msg_set_read_flag(lua_State * L)
{
        struct xwlua_i2cm_msg * msg;
        xwu32_t idx;
        int readflag;

        msg = (struct xwlua_i2cm_msg *)luaL_checkudata(L, 1, "xwlua_i2cm_msg");
        idx = (xwsz_t)luaL_checkinteger(L, 2);
        readflag = lua_toboolean(L, 3);

        if (idx < msg->num) {
                if (readflag) {
                        msg->msg[idx].flag |= XWDS_I2C_F_RD;
                } else {
                        msg->msg[idx].flag &= ~XWDS_I2C_F_RD;
                }
        } else {
                luaL_error(L, "Index (%d) is out of range (%d)", idx, msg->num);
        }
        return 0;
}

int xwlua_i2cm_msg_set_start_flag(lua_State * L)
{
        struct xwlua_i2cm_msg * msg;
        xwu32_t idx;
        int startflag;

        msg = (struct xwlua_i2cm_msg *)luaL_checkudata(L, 1, "xwlua_i2cm_msg");
        idx = (xwsz_t)luaL_checkinteger(L, 2);
        startflag = lua_toboolean(L, 3);

        if (idx < msg->num) {
                if (startflag) {
                        msg->msg[idx].flag |= XWDS_I2C_F_START;
                } else {
                        msg->msg[idx].flag &= ~XWDS_I2C_F_START;
                }
        } else {
                luaL_error(L, "Index (%d) is out of range (%d)", idx, msg->num);
        }
        return 0;
}

int xwlua_i2cm_msg_set_stop_flag(lua_State * L)
{
        struct xwlua_i2cm_msg * msg;
        xwu32_t idx;
        int stopflag;

        msg = (struct xwlua_i2cm_msg *)luaL_checkudata(L, 1, "xwlua_i2cm_msg");
        idx = (xwsz_t)luaL_checkinteger(L, 2);
        stopflag = lua_toboolean(L, 3);

        if (idx < msg->num) {
                if (stopflag) {
                        msg->msg[idx].flag |= XWDS_I2C_F_STOP;
                } else {
                        msg->msg[idx].flag &= ~XWDS_I2C_F_STOP;
                }
        } else {
                luaL_error(L, "Index (%d) is out of range (%d)", idx, msg->num);
        }
        return 0;
}

int xwlua_i2cm_msg_set_data(lua_State * L)
{
        struct xwlua_i2cm_msg * msg;
        xwu32_t idx;
        xwu8_t * data;
        xwsz_t size;

        msg = (struct xwlua_i2cm_msg *)luaL_checkudata(L, 1, "xwlua_i2cm_msg");
        idx = (xwsz_t)luaL_checkinteger(L, 2);
        data = (xwu8_t *)luaL_checklstring(L, 3, &size);

        if (idx < msg->num) {
                msg->msg[idx].data = data;
                msg->msg[idx].size = size;
        } else {
                luaL_error(L, "Index (%d) is out of range (%d)", idx, msg->num);
        }
        return 0;
}

int xwlua_i2cm_msg_new_buffer(lua_State * L)
{
        struct xwlua_i2cm_msg * msg;
        xwu32_t idx;
        luaL_Buffer b;
        xwsz_t size;
        int ret;

        msg = (struct xwlua_i2cm_msg *)luaL_checkudata(L, 1, "xwlua_i2cm_msg");
        idx = (xwsz_t)luaL_checkinteger(L, 2);
        size = (xwsz_t)luaL_checkinteger(L, 3);
        if (idx < msg->num) {
                msg->msg[idx].data = (xwu8_t *)luaL_buffinitsize(L, &b, size);;
                msg->msg[idx].size = size;
                luaL_pushresult(&b);
                ret = 1;
        } else {
                ret = 0;
                luaL_error(L, "Index (%d) is out of range (%d)", idx, msg->num);
        }
        return ret;
}

const luaL_Reg xwlua_i2cm_msg_indexmethod[] = {
        {"addr", xwlua_i2cm_msg_set_addr},
        {"read", xwlua_i2cm_msg_set_read_flag},
        {"start", xwlua_i2cm_msg_set_start_flag},
        {"stop", xwlua_i2cm_msg_set_stop_flag},
        {"data", xwlua_i2cm_msg_set_data},
        {"buffer", xwlua_i2cm_msg_new_buffer},
        {NULL, NULL},
};

const luaL_Reg xwlua_i2cm_msg_metamethod[] = {
        {"__index", NULL},  /* place holder */
        {"__tostring", NULL},
        {NULL, NULL}
};

void xwlua_ds_init_i2cm_msg(lua_State * L)
{
        /* metatable for xwlua_i2cm */
        luaL_newmetatable(L, "xwlua_i2cm_msg");
        luaL_setfuncs(L, xwlua_i2cm_msg_metamethod, 0); /* add metamethods */
        luaL_newlibtable(L, xwlua_i2cm_msg_indexmethod); /* create i2cm method table */
        luaL_setfuncs(L, xwlua_i2cm_msg_indexmethod, 0); /* add i2cm indexmethod table */
        lua_setfield(L, -2, "__index");  /* metatable.__index = indexmethod table */
        lua_pop(L, 1); /* pop metatable */
}
