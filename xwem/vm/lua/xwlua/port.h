/**
 * @file
 * @brief Port Lua for XuanWuOS
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

#ifndef __xwem_vm_lua_xwlua_port_h__
#define __xwem_vm_lua_xwlua_port_h__

#include <xwos/standard.h>
#include "src/lua.h"
#include "src/lauxlib.h"

struct xwlua_arg {
        int argc;
        char ** argv;
};

extern struct xwmm_mempool * xwlua_mempool;

int xwlua_readline(lua_State * L, char buffer[], const char * prompt);
void xwlua_openlibs(lua_State * L);
void xwlua_xt_openlibs(lua_State * L);
void xwlua_dump_stack(lua_State* L);
xwer_t xwlua_task(void * arg);

#define XWLUA_LIB_NAME          "xwlib"
#define XWLUA_OS_NAME           "xwos"
#define XWLUA_DS_NAME           "xwds"
#define XWLUA_XT_NAME           "xwxt"
#define XWLUA_THD_STACK         XWEMCFG_vm_lua_THD_STACK

LUAMOD_API int xwlua_open_lib(lua_State * L);
LUAMOD_API int xwlua_open_xt(lua_State * L);
LUAMOD_API int xwlua_open_os(lua_State * L);
LUAMOD_API int xwlua_open_ds(lua_State * L);

#endif /* xwem/vm/lua/xwlua/port.h */
