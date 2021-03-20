/**
 * @file
 * @brief 玄武设备栈 Lua Lib：I2C master
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > http://www.lua.org/license.html
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwcd/ds/i2c/master.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwds/i2cm.h"

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
        msg.flag = (xwu16_t)luaL_checkinteger(L, 3);
        if (msg.addr & (xwu16_t)0xFF00) {
                msg.flag |= XWDS_I2C_F_10BITADDR;
        } else {
                msg.flag |= XWDS_I2C_F_7BITADDR;
        }
        msg.size = (xwsz_t)luaL_checkinteger(L, 4);
        if (msg.flag & XWDS_I2C_F_WR) {
                msg.data = (xwu8_t *)luaL_checklstring(L, 5, &txdsize);
                if (msg.size > txdsize) {
                        msg.size = txdsize;
                }
                if (top >= 6) {
                        time = (xwtm_t)luaL_checknumber(L, 6);
                } else {
                        time = XWTM_MAX;
                }
                rc = xwds_i2cm_xfer(luai2cm->i2cm, &msg, &time);
                lua_pushinteger(L, (lua_Integer)rc);
                ret = 1;
        } else {
                luaL_buffinit(L, &b);
                msg.data = (xwu8_t *)luaL_prepbuffer(&b);
                if (top >= 6) {
                        time = (xwtm_t)luaL_checknumber(L, 6);
                } else {
                        time = XWTM_MAX;
                }
                rc = xwds_i2cm_xfer(luai2cm->i2cm, &msg, &time);
                lua_pushinteger(L, (lua_Integer)rc);
                luaL_pushresult(&b);
                ret = 2;
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
