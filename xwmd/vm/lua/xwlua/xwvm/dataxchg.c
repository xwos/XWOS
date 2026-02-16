/**
 * @file
 * @brief XWLUA库：虚拟机：数据交换
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include "xwmd/vm/lua/xwlua/xwvm/dataxchg.h"
#include "xwmd/vm/lua/xwlua/xwvm/debug.h"
#include "xwmd/vm/lua/src/lauxlib.h"

int xwlua_vm_function_writer(lua_State * src, const void * b, size_t size, void * ud)
{
        luaL_Buffer * sb;

        XWOS_UNUSED(src);
        sb = (struct luaL_Buffer *)ud;
        luaL_addlstring(sb, (const char *)b, size);
        return 0;
}

xwer_t xwlua_vm_copy_function(lua_State * src, int sidx, lua_State * dst)
{
        const char * msg;
        luaL_Buffer lb;
        const char * func;
        size_t fl;
        int luarc;
        xwer_t rc;

        rc = XWOK;
        sidx = lua_absindex(src, sidx);
        luaL_buffinit(src, &lb); /* push buffer */
        luaL_prepbuffer(&lb);
        lua_pushvalue(src, sidx); /* Ensure function at the top */
        luarc = lua_dump(src, xwlua_vm_function_writer, &lb, true);
        lua_pop(src, 1); /* Pop top function */
        if (0 == luarc) {
                luaL_pushresult(&lb); /* Pop buffer and Push function string */
                func = lua_tolstring(src, -1, &fl);
                luarc = luaL_loadbufferx(dst, func, fl, "function", "bt");
                if (LUA_OK != luarc) {
                        msg = lua_tostring(dst, -1);
                        lua_writestringerror("%s\n", msg);
                        lua_pop(dst, 1); /* Pop error */
                        lua_pushnil(dst);
                        rc = -EINVAL;
                }
                lua_pop(src, 1); /* Pop function string */
        } else {
                lua_pushnil(dst);
                rc = -EINVAL;
        }
        return rc;
}

xwer_t xwlua_vm_copy_userdata(lua_State * src, int idx, lua_State * dst)
{
        const char * msg;
        int luarc;
        int type;
        bool hasmt;
        xwer_t rc;

        rc = XWOK;
        idx = lua_absindex(src, idx);
        hasmt = lua_getmetatable(src, idx); /* Push metatable */
        if (hasmt) {
                type = lua_getfield(src, -1, "__copy"); /* Push metatable.__copy */
                if (LUA_TFUNCTION == type) {
                        lua_pushvalue(src, idx); /* Push arg1 */
                        lua_pushlightuserdata(src, (void *)dst); /* Push arg2 */
                        luaL_setmetatable(src, "xwlua_vm");
                        /* pcall __copy(arg1, arg2) then pop func & args */
                        luarc = lua_pcall(src, 2, LUA_MULTRET, 0);
                        if (LUA_OK != luarc) {
                                msg = lua_tostring(src, -1);
                                lua_writestringerror("%s\n", msg);
                                lua_pop(src, 1); /* Pop error */
                                lua_pushnil(dst);
                                rc = -EINVAL;
                        }
                } else {
                        lua_pop(src, 1); /* pop __copy */
                        lua_pushnil(dst);
                        rc = -EINVAL;
                }
                lua_pop(src, 1); /* pop metatable */
        } else {
                lua_pushnil(dst);
                rc = -EINVAL;
        }
        return rc;
}

/**
 * @brief 将源虚拟机栈中的表元素逐一拷贝到目的虚拟机栈的表中
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 源表在源虚拟机栈中的索引
 * @param[in] dst: 目的虚拟机
 * @param[in] didx: 目的表在目的虚拟机栈中的索引
 * @return 错误吗
 * @retval XWOK: 没有错误
 * @retval <0: 拷贝过程出错
 * @details
 * + 若拷贝出错，过程会被中断，已经拷贝的元素不会被删除。
 */
static
xwer_t xwlua_vm_deepcopy_table(lua_State * src, int sidx, lua_State * dst, int didx)
{
        xwer_t rc;

        rc = XWOK;
        sidx = lua_absindex(src, sidx);
        didx = lua_absindex(dst, didx);
        /* Traverse table at index sidx */
        lua_pushnil(src); /* first key */
        while (0 != lua_next(src, sidx)) { /* Push 'key' & 'value' */
                rc = xwlua_vm_copy_element(src, -2, dst); /* Copy 'key' to dst */
                if (rc < 0) {
                        lua_pop(dst, 1); /* Pop nil */
                        lua_pop(src, 2); /* Pop 'key' & 'value' */
                        break;
                }
                rc = xwlua_vm_copy_element(src, -1, dst); /* Copy 'value' to dst */
                if (rc < 0) {
                        lua_pop(dst, 2); /* Pop nil & 'key' */
                        lua_pop(src, 2); /* Pop 'key' & 'value' */
                        break;
                }
                lua_settable(dst, didx);
                lua_pop(src, 1); /* Keeps 'key' for next iteration */
        }
        return rc;
}

xwer_t xwlua_vm_copy_table(lua_State * src, int sidx, lua_State * dst)
{
        bool hasmt;
        xwer_t rc;

        rc = XWOK;
        sidx = lua_absindex(src, sidx);
        lua_newtable(dst); /* Create an empty table on the top of dst */
        hasmt = lua_getmetatable(src, sidx); /* Push metatable */
        if (hasmt) {
                /* Copy metatable to the empty metatable at the top of dst  */
                rc = xwlua_vm_copy_table(src, -1, dst);
                if (rc < 0) {
                        lua_pop(dst, 2); /* Pop metatable & table */
                        lua_pop(src, 1); /* Pop metatable */
                        lua_pushnil(dst); /* push nil */
                } else {
                        lua_setmetatable(dst, -2); /* Pop and set metatable */
                        lua_pop(src, 1); /* Pop metatable */
                        rc = xwlua_vm_deepcopy_table(src, -1, dst, -1);
                        if (rc < 0) {
                                lua_pop(dst, 1); /* Pop table */
                                lua_pushnil(dst); /* push nil */
                        }
                }
        } else {
                rc = xwlua_vm_deepcopy_table(src, -1, dst, -1);
                if (rc < 0) {
                        lua_pop(dst, 1); /* Pop table */
                        lua_pushnil(dst); /* push nil */
                }
        }
        return rc;
}

xwer_t xwlua_vm_copy_element(lua_State * src, int sidx, lua_State * dst)
{
        xwer_t rc;
        int type;
        union {
                const char * str;
                int boolean;
                lua_Number number;
                lua_Integer integer;
        } value;

        sidx = lua_absindex(src, sidx);
        type = lua_type(src, sidx);
        switch (type) {
        case LUA_TNIL:
                lua_pushnil(dst);
                rc = XWOK;
                break;
        case LUA_TBOOLEAN:
                value.boolean = lua_toboolean(src, sidx);
                lua_pushboolean(dst, value.boolean);
                rc = XWOK;
                break;
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
                rc = xwlua_vm_copy_userdata(src, sidx, dst);
                break;
        case LUA_TNUMBER:
                if (lua_isinteger(src, sidx)) {
                        value.integer = lua_tointegerx(src, sidx, NULL);
                        lua_pushinteger(dst, value.integer);
                } else {
                        value.number = lua_tonumberx(src, sidx, NULL);
                        lua_pushnumber(dst, value.number);
                }
                rc = XWOK;
                break;
        case LUA_TSTRING:
                value.str = lua_tostring(src, sidx);
                lua_pushstring(dst, value.str);
                rc = XWOK;
                break;
        case LUA_TFUNCTION:
                rc = xwlua_vm_copy_function(src, sidx, dst);
                break;
        case LUA_TTABLE:
                rc = xwlua_vm_copy_table(src, sidx, dst);
                break;
        case LUA_TTHREAD:
                lua_pushnil(dst);
                rc = -ENOTSUP;
                break;
        default:
                lua_pushnil(dst);
                rc = -EINVAL;
                break;
        }
        return rc;
}

xwer_t xwlua_vm_move_element(lua_State * src, int sidx, lua_State * dst)
{
        xwer_t rc;

        rc = xwlua_vm_copy_element(src, sidx, dst);
        if (XWOK == rc) {
                lua_remove(src, sidx);
        }
        return rc;
}

void xwlua_vm_copy_env(lua_State * src, int sidx, lua_State * dst)
{
        const char * key;

        sidx = lua_absindex(src, sidx);
        lua_pushnil(src); /* first key */
        while (0 != lua_next(src, sidx)) {
                if (lua_type(src, -2) == LUA_TSTRING) { /* type(key) == "string" */
                        key = lua_tostring(src, -2);
                        xwlua_vm_copy_element(src, -1, dst);
                        lua_setglobal(dst, key);
                }
                lua_pop(src, 1); /* Removes 'value'; keeps 'key' for next iteration */
        }
}
