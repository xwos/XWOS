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

#ifndef __xwem_vm_lua_xwlua_xwds_i2cm_h__
#define __xwem_vm_lua_xwlua_xwds_i2cm_h__

#include <xwos/standard.h>
#include <xwcd/ds/i2c/master.h>
#include "xwem/vm/lua/src/lauxlib.h"

/**
 * @defgroup xwlua_xwds_i2cm I2C主机控制器
 * @ingroup xwlua_xwds
 * @{
 */

/**
 * @brief XWLUA I2C 主机控制器对象
 */
struct xwlua_i2cm {
        struct xwds_i2cm * i2cm; /**< XWDS的I2C主机控制器对象指针 */
        xwsq_t tik; /**< 对象的标签 */
};

/**
 * @brief XWLUA I2C 主机消息
 */
struct xwlua_i2cm_msg {
        xwsz_t num; /**< 通讯消息的数量 */
        struct xwds_i2c_msg msg[0]; /**< 通讯消息数组 */
};

/**
 * @brief 注册I2C主机模式控制器对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: I2C主机模式控制器在Lua中的符号名
 * @param[in] i2cm: I2C主机模式控制器对象
 */
void xwlua_i2cm_register(lua_State * L, const char * name, struct xwds_i2cm * i2cm);

/**
 * @brief 删除I2C主机模式控制器对象
 * @param[in] L: 虚拟机指针
 * @param[in] name I2C主机模式控制器在Lua中的符号名
 */
void xwlua_i2cm_unregister(lua_State * L, const char * name);

/**
 * @} xwlua_xwds_i2cm
 */

#endif /* xwem/vm/lua/xwlua/xwds/i2cm.h */
