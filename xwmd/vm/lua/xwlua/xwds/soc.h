/**
 * @file
 * @brief XWLUA库：设备栈：SOC
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwds_soc_h__
#define __xwmd_vm_lua_xwlua_xwds_soc_h__

#include <xwos/standard.h>
#include <xwcd/ds/soc/chip.h>
#include "xwmd/vm/lua/src/lauxlib.h"

/**
 * @defgroup xwlua_xwds_soc SOC
 * @ingroup xwlua_xwds
 * @{
 */

/**
 * @brief XWLUA SOC对象
 */
struct xwlua_soc {
        struct xwds_soc * soc; /**< XWDS的SOC对象指针 */
        xwsq_t tik; /**< 对象的标签 */
};

/**
 * @brief 注册SOC对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: SOC在Lua中的符号名
 * @param[in] soc: SOC对象的指针
 */
void xwlua_soc_register(lua_State * L, const char * name, struct xwds_soc * soc);

/**
 * @brief 删除SOC对象
 * @param[in] L: 虚拟机指针
 * @param[in] name SOC在Lua中的符号名
 */
void xwlua_soc_unregister(lua_State * L, const char * name);

/**
 * @} xwlua_xwds_soc
 */

#endif /* xwmd/vm/lua/xwlua/xwds/soc.h */
