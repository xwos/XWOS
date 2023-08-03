/**
 * @file
 * @brief Readline for Lua
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

#include <xwos/standard.h>
#include <stdio.h>
#include <ctype.h>
#include <xwos/osal/thd.h>
#include "xwem/vm/lua/src/lua.h"

#define LUA_MAXINPUT		512
#define CTRL_BS          0x8 /**< BS '\b' */
#define CTRL_LF          0xA /**< LF '\n' */
#define CTRL_CR          0xD /**< CR '\r' */
#define CTRL_DEL         0x7F /**< DEL  */

int xwlua_readline(lua_State * L, char buffer[], const char * prompt)
{
        bool lf;
        char * line;
        xwsq_t pos;
        int c;
        xwer_t rc;
        int ret;

        (void)L;
        line = buffer;
        fputs(prompt, stdout);
        fflush(stdout);
        lf = false;
        pos = 0;
        ret = 1;
        while ((!lf) && (ret)) {
                c = fgetc(stdin);
                switch (c) {
                case CTRL_BS:
                case CTRL_DEL:
                        if (pos > 0) {
                                pos--;
                                fputs("\b \b", stdout);
                                fflush(stdout);
                        }
                        break;
                case CTRL_CR:
                case CTRL_LF:
                        line[pos] = '\n';
                        pos++;
                        line[pos] = '\0';
                        fputs("\r\n", stdout);
                        fflush(stdout);
                        lf = true;
                        break;
                default:
                        if (c < 0) {
                                rc = -errno;
                                if (-EINTR == rc) {
                                        if (xwos_cthd_shld_frz()) {
                                                xwos_cthd_freeze();
                                        } else if (xwos_cthd_shld_stop()) {
                                                ret = 0;
                                        }
                                }
                        } else if (isprint(c)) {
                                line[pos] = c;
                                pos++;
                                fputc(c, stdout);
                                if (pos == (LUA_MAXINPUT - 2)) {
                                        pos--;
                                        fputc('\b', stdout);
                                }
                                fflush(stdout);
                        }
                        break;
                }
        }
        return ret;
}
