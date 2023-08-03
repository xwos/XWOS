/**
 * @file
 * @brief 玄武Lua库：XWLUA VM之间的数据交换
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
#include "xwem/vm/lua/xwlua/xwvm/dataxchg.h"
#include "xwem/vm/lua/xwlua/xwvm/debug.h"
#include "xwem/vm/lua/src/lauxlib.h"

int xwlua_vm_function_writer(lua_State * src, const void * b, size_t size, void * ud)
{
        luaL_Buffer * sb;

        XWOS_UNUSED(src);
        sb = (struct luaL_Buffer *)ud;
        luaL_addlstring(sb, (const char *)b, size);
        return 0;
}

/**
 * @brief 将源虚拟机栈中的函数拷贝到目的虚拟机的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 函数在源虚拟机栈中的位置
 * @param[in] dst: 目的虚拟机
 * @return 错误吗
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 拷贝失败
 * @details
 * 此函数将源虚拟机栈 `src` 中索引为 `sidx` 的 **函数**
 * 拷贝到目的虚拟机 `dst` 的 **栈顶** 。
 *
 * + 若拷贝成功，返回值为 `XWOK` ；
 * + 若拷贝失败，返回值小于 **0** ，目的虚拟机 `dst` 的栈顶会压入一个 `nil` 。
 */
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

/**
 * @brief 将虚拟机中的用户数据拷贝到目的虚拟机的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] idx: 用户数据在源虚拟机栈中的位置
 * @param[in] dst: 目的虚拟机
 * @return 错误吗
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 拷贝失败
 * @details
 * 此函数将源虚拟机栈 `src` 中索引为 `sidx` 的 **用户数据**
 * 拷贝到目的虚拟机 `dst` 的 **栈顶** 。
 *
 * + 若拷贝成功，返回值为 `XWOK` ；
 * + 若拷贝失败，返回值小于 **0** ，目的虚拟机 `dst` 的栈顶会压入一个 `nil` 。
 */
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
 * @param[in] sidx: 源表在源虚拟机栈中的位置
 * @param[in] dst: 目的虚拟机
 * @param[in] didx: 目的表在目的虚拟机栈中的位置
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

/**
 * @brief 将虚拟机中的表拷贝到目的虚拟机的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] idx: 表在源虚拟机栈中的位置
 * @param[in] dst: 目的虚拟机
 * @return 错误吗
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 拷贝失败
 * @details
 * 此函数会在目的虚拟机 `dst` 的 **栈顶** 新建一个空表，
 * 然后将源虚拟机栈 `src` 中索引为 `sidx` 的 **表** 拷贝到这个空表内。
 *
 * + 若拷贝成功，返回值为 `XWOK` ；
 * + 若拷贝失败，返回值小于 **0** ，目的虚拟机 `dst` 的栈顶会压入一个 `nil` 。
 */
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

/**
 * @brief 将源虚拟机栈中的元素拷贝到目的虚拟机栈的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 源表在源虚拟机栈中的位置
 * @param[in] dst: 目的虚拟机
 * @return 错误吗
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 拷贝失败
 * @retval -ENOTSUP: 暂时不支持拷贝的类型
 * @details
 * 此函数将源虚拟机栈 `src` 中索引为 `sidx` 的 **元素**
 * 拷贝到目的虚拟机 `dst` 的 **栈顶** 。
 *
 * + 若拷贝成功，返回值为 `XWOK` ；
 * + 若拷贝失败，返回值小于 **0** ，目的虚拟机 `dst` 的栈顶会压入一个 `nil` 。
 */
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

/**
 * @brief 将源虚拟机栈中的元素移动到目的虚拟机栈的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 源表在源虚拟机栈中的位置
 * @param[in] dst: 目的虚拟机
 * @return 错误吗
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 拷贝失败
 * @retval -ENOTSUP: 暂时不支持拷贝的类型
 * @details
 * 此函数将源虚拟机栈 `src` 中索引为 `sidx` 的 **元素**
 * 移动到目的虚拟机 `dst` 的 **栈顶** 。
 *
 * + 若移动成功，返回值为 `XWOK` ，索引为 `sidx` 的 **元素**
 *   在源虚拟机栈 `src` 中被删除；
 * + 若移动失败，返回值小于 **0** ，目的虚拟机 `dst` 的栈顶会压入一个 `nil` 。
 */
xwer_t xwlua_vm_move_element(lua_State * src, int sidx, lua_State * dst)
{
        xwer_t rc;

        rc = xwlua_vm_copy_element(src, sidx, dst);
        if (XWOK == rc) {
                lua_remove(src, sidx);
        }
        return rc;
}


/**
 * @brief 将源虚拟机中的表元素拷贝到目的虚拟机的环境 `_ENV` 中
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 表在源虚拟机栈中的索引
 * @param[in] dst: 目的虚拟机
 * @note
 * + 此函数只会将源虚拟机表中索引为 **字符串** 的键值对拷贝到目的虚拟机的 `_ENV` 中，
 *   其他类型的索引会被忽略；
 * + 拷贝失败不会中止拷贝过程，目的虚拟机 `_ENV` 中拷贝失败的索引会被设置为 `nil` 。
 */
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
