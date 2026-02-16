/**
 * @file
 * @brief XWLUA库：设备栈：SPI主机模式控制器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwds_spim_h__
#define __xwmd_vm_lua_xwlua_xwds_spim_h__

#include <xwos/standard.h>
#include <xwcd/ds/spi/master.h>
#include "xwmd/vm/lua/src/lauxlib.h"

/**
 * @defgroup xwlua_xwds_spim SPI主机模式控制器
 * @ingroup xwlua_xwds
 * @{
 */

/**
 * @brief XWLUA SPI主机模式控制器对象
 */
struct xwlua_spim {
        struct xwds_spim * spim; /**< XWDS的SPI主机模式控制器对象指针 */
        xwsq_t tik; /**< 对象的标签 */
};

/**
 * @brief 注册SPI主机模式控制器对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: SPI主机模式控制器在Lua中的符号名
 * @param[in] spim: SPI主机模式控制器对象的指针
 */
void xwlua_spim_register(lua_State * L, const char * name, struct xwds_spim * spim);

/**
 * @brief 删除SPI主机模式控制器对象
 * @param[in] L: 虚拟机指针
 * @param[in] name SPI主机模式控制器在Lua中的符号名
 */
void xwlua_spim_unregister(lua_State * L, const char * name);

/**
 * @} xwlua_xwds_spim
 */

#endif /* xwmd/vm/lua/xwlua/xwds/spim.h */
