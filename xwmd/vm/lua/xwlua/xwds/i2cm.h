/**
 * @file
 * @brief XWLUA库：设备栈：I2C主机控制器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwds_i2cm_h__
#define __xwmd_vm_lua_xwlua_xwds_i2cm_h__

#include <xwos/standard.h>
#include <xwcd/ds/i2c/master.h>
#include "xwmd/vm/lua/src/lauxlib.h"

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

#endif /* xwmd/vm/lua/xwlua/xwds/i2cm.h */
