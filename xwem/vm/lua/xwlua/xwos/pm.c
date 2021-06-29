/**
 * @file
 * @brief 玄武Lua库：电源管理
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

#include <xwos/standard.h>
#include <xwos/osal/pm.h>
#include "src/lauxlib.h"
#include "xwlua/port.h"
#include "xwlua/xwos/pm.h"

int xwlua_pm_suspend(lua_State * L)
{
        XWOS_UNUSED(L);
        xwos_pm_suspend();
        return 0;
}

const luaL_Reg xwlua_pm_libconstructor[] = {
        {"suspend", xwlua_pm_suspend},
        {NULL, NULL},
};

void xwlua_os_open_pm(lua_State * L)
{
        luaL_newlib(L, xwlua_pm_libconstructor);
}
