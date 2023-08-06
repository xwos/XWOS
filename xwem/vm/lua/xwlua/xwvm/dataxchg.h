/**
 * @file
 * @brief XWLUA库：虚拟机：数据交换
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

#ifndef __xwem_vm_lua_xwlua_xwvm_dataxchg_h__
#define __xwem_vm_lua_xwlua_xwvm_dataxchg_h__

#include <xwos/standard.h>
#include "xwem/vm/lua/src/lua.h"

/**
 * @ingroup xwlua_xwvm
 * @{
 */

/**
 * @brief 将源虚拟机栈中的函数拷贝到目的虚拟机的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 函数在源虚拟机栈中的索引
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
xwer_t xwlua_vm_copy_function(lua_State * src, int sidx, lua_State * dst);

/**
 * @brief 将虚拟机中的用户数据拷贝到目的虚拟机的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] idx: 用户数据在源虚拟机栈中的索引
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
xwer_t xwlua_vm_copy_userdata(lua_State * src, int idx, lua_State * dst);

/**
 * @brief 将虚拟机中的表拷贝到目的虚拟机的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] idx: 表在源虚拟机栈中的索引
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
xwer_t xwlua_vm_copy_table(lua_State * src, int sidx, lua_State * dst);

/**
 * @brief 将源虚拟机栈中的元素拷贝到目的虚拟机栈的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 源表在源虚拟机栈中的索引
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
xwer_t xwlua_vm_copy_element(lua_State * src, int sidx, lua_State * dst);

/**
 * @brief 将源虚拟机栈中的元素移动到目的虚拟机栈的栈顶
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 源表在源虚拟机栈中的索引
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
xwer_t xwlua_vm_move_element(lua_State * src, int sidx, lua_State * dst);

/**
 * @brief 将源虚拟机中的表元素拷贝到目的虚拟机的环境 `_ENV` 中
 * @param[in] src: 源虚拟机
 * @param[in] sidx: 表在源虚拟机栈中的索引
 * @param[in] dst: 目的虚拟机
 * @details
 * + 此函数只会将源虚拟机表中索引为 **字符串** 的键值对拷贝到目的虚拟机的 `_ENV` 中，
 *   其他类型的索引会被忽略；
 * + 拷贝失败不会中止拷贝过程，目的虚拟机 `_ENV` 中拷贝失败的索引会被设置为 `nil` 。
 */
void xwlua_vm_copy_env(lua_State * src, int sidx, lua_State * dst);

/**
 * @} xwlua_xwvm
 */

#endif /* xwem/vm/lua/xwlua/xwvm/dataxchg.h */
