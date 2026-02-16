/**
 * @file
 * @brief XWLUA库：设备栈：UART控制器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwds_uart_h__
#define __xwmd_vm_lua_xwlua_xwds_uart_h__

#include <xwos/standard.h>
#include <xwcd/ds/uart/controller.h>
#include "xwmd/vm/lua/src/lauxlib.h"

/**
 * @defgroup xwlua_xwds_uart UART控制器
 * @ingroup xwlua_xwds
 * @{
 */

/**
 * @brief XWLUA UART控制器对象
 */
struct xwlua_uart {
        struct xwds_uartc * uartc; /**< XWDS的UART控制器对象指针 */
        xwsq_t tik; /**< 对象的标签 */
};

/**
 * @brief 注册UART控制器对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: UART控制器在Lua中的符号名
 * @param[in] uartc: UART控制器对象的指针
 */
void xwlua_uart_register(lua_State * L, const char * name, struct xwds_uartc * uartc);

/**
 * @brief 删除UART控制器对象
 * @param[in] L: 虚拟机指针
 * @param[in] name UART控制器在Lua中的符号名
 */
void xwlua_uart_unregister(lua_State * L, const char * name);

/**
 * @} xwlua_xwds_uart
 */

#endif /* xwmd/vm/lua/xwlua/xwds/uart.h */
