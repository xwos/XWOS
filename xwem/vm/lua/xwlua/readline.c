/**
 * @file
 * @brief Readline for Lua
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * + http://www.lua.org/license.html
 */

#include <xwos/standard.h>
#include <stdio.h>
#include <xwos/lib/ctype.h>
#include <xwos/osal/skd.h>
#include "lua.h"

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
