/**
 * @file
 * @brief XWLUA库：设备栈：SPI主机模式控制器
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

#ifndef __xwem_vm_lua_xwlua_xwds_spim_h__
#define __xwem_vm_lua_xwlua_xwds_spim_h__

#include <xwos/standard.h>
#include <xwcd/ds/spi/master.h>
#include "xwem/vm/lua/src/lauxlib.h"

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

#endif /* xwem/vm/lua/xwlua/xwds/spim.h */
