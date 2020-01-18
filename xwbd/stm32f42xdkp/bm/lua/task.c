/**
 * @file
 * @brief lua task
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <stdio.h>
#include <string.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwlua/port.h>
#include <bm/lua/mm.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern const char xwlua_test[];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwlua_thrd(void * arg)
{
        __xw_io bool running = true;
        xwtm_t xwtm;
        lua_State * L;
        xwer_t rc = OK;

        XWOS_UNUSED(arg);

        while ((!xwosal_cthrd_frz_shld_stop(NULL)) && (running)) {
                L = xwlua_newstate(xwlua_alloc);
                if (L) {
                        xwlua_openlibs(L);
                        luaL_dostring(L, xwlua_test);
                        lua_close(L);
                        rc = OK;
                } else {
                        rc = -EFAULT;
                }
                xwtm = 20 * XWTM_MS;
                xwosal_cthrd_sleep(&xwtm);
        }
        return rc;
}
