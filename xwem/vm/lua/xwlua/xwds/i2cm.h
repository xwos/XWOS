/**
 * @file
 * @brief 玄武Lua库：I2C master
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

#ifndef __xwem_vm_lua_xwlua_xwds_i2cm_h__
#define __xwem_vm_lua_xwlua_xwds_i2cm_h__

#include <xwos/standard.h>
#include <xwcd/ds/i2c/master.h>
#include "src/lauxlib.h"

struct xwlua_i2cm {
        struct xwds_i2cm * i2cm;
        xwsq_t tik;
};

void xwlua_i2cm_register(lua_State * L, const char * name, struct xwds_i2cm * i2cm);
void xwlua_i2cm_unregister(lua_State * L, const char * name);
void xwlua_ds_open_i2cm(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwds/i2cm.h */
