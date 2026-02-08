/**
 * @file
 * @brief XWLUA Readline
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <string.h>
#include <xwos/osal/thd.h>
#include "xwem/vm/lua/src/lua.h"
#include "xwem/cli/cherryrl/chry_readline.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "XWLUA.RL"

#define XWLUA_RL_PROMPT_MAXSIZE         64U
#define XWLUA_RL_HISTORY_MAXSIZE        1024U /* size must be power of 2 !!! */

chry_readline_t xwlua_cherryrl;
char xwlua_cherryrl_prompt[XWLUA_RL_PROMPT_MAXSIZE];
char xwlua_cherryrl_history[XWLUA_RL_HISTORY_MAXSIZE];

int xwlua_cherryrl_ucb(chry_readline_t *rl, uint8_t exec)
{
        /*!< user event callback will not output newline automatically */
        chry_readline_newline(rl);

        switch (exec) {
        case CHRY_READLINE_EXEC_EOF:
                xwlogf(I, LOGTAG, "EOF\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGINT:
                chry_readline_ignore(rl, false);
                xwlogf(I, LOGTAG, "SIGINT\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGQUIT:
                xwlogf(I, LOGTAG, "SIGQUIT\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGCONT:
                xwlogf(I, LOGTAG, "SIGCONT\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGSTOP:
                xwlogf(I, LOGTAG, "SIGSTOP\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGTSTP:
                xwlogf(I, LOGTAG, "SIGTSTP\r\n");
                break;
        case CHRY_READLINE_EXEC_F1 ... CHRY_READLINE_EXEC_F12:
                xwlogf(I, LOGTAG, "F%d event\r\n", exec - CHRY_READLINE_EXEC_F1 + 1);
                break;
        default:
                xwlogf(I, LOGTAG, "ERROR EXEC %d\r\n", exec);
                return -1;
        }

        /*!< return 1 will not refresh */
        /*!< return 0 to refresh whole line (include prompt) */
        /*!< return -1 to end readline (error) */
        return 0;
}

uint8_t xwlua_cherryrl_acb(chry_readline_t * rl, char * pre,
                           uint16_t * size, const char ** argv,
                           uint8_t * argl, uint8_t argcmax)
{
        XWOS_UNUSED(rl);
        XWOS_UNUSED(pre);
        XWOS_UNUSED(size);
        XWOS_UNUSED(argv);
        XWOS_UNUSED(argl);
        XWOS_UNUSED(argcmax);
        return 0;
}

uint16_t xwlua_cherryrl_sput(chry_readline_t * rl, const void * data, uint16_t size)
{
        int ret;
        const char * buf;
        (void)rl;

        buf = data;
        if (size > 0) {
                ret = fwrite(buf, size, 1, stdout);
                if (ret > 0) {
                        fflush(stdout);
                } else {
                        ret = 0;
                }
        } else {
                ret = 0;
        }
        return (uint16_t)ret;
}

uint16_t xwlua_cherryrl_sget(chry_readline_t * rl, void * data, uint16_t size)
{
        int ret;
        char * buf;
        (void)rl;

        buf = data;
        ret = fread(buf, size, 1, stdin);
        if (ret < 0) {
                ret = 0;
        }
        return (uint16_t)ret;
}

void xwlua_cherryrl_init(void)
{
        int rc;

        chry_readline_init_t init = {
                .prompt = xwlua_cherryrl_prompt,
                .pptsize = sizeof(xwlua_cherryrl_prompt),
                .history = xwlua_cherryrl_history,
                .histsize = sizeof(xwlua_cherryrl_history),
                .sget = xwlua_cherryrl_sget,
                .sput = xwlua_cherryrl_sput,
        };
        rc = chry_readline_init(&xwlua_cherryrl, &init);
        if (0 != rc) {
                xwlogf(E, LOGTAG, "chry_readline_init() ... <%ld>\n", rc);
        } else {
                chry_readline_set_completion_cb(&xwlua_cherryrl, xwlua_cherryrl_acb);
                chry_readline_set_user_cb(&xwlua_cherryrl, xwlua_cherryrl_ucb);
        }
}

int xwlua_cherryrl_readline(char buffer[], const char * prompt)
{
        char linebuf[LUA_MAXINPUT];
        char * line;
        xwu16_t linesize;
        int ret;

        chry_readline_prompt_edit(&xwlua_cherryrl, 0,
                                  (chry_readline_sgr_t){
                                          .foreground = CHRY_READLINE_SGR_BLUE,
                                          .bold = 1,
                                  }.raw,
                                  "lua");
        chry_readline_prompt_edit(&xwlua_cherryrl, 1, 0, ":");
        chry_readline_prompt_edit(&xwlua_cherryrl, 2,
                                  (chry_readline_sgr_t){
                                          .foreground = CHRY_READLINE_SGR_GREEN,
                                          .bold = 1,
                                  }.raw,
                                  "%s", prompt);
        line = chry_readline(&xwlua_cherryrl, linebuf, LUA_MAXINPUT, &linesize);
        if (line == NULL) {
                xwlogf(E, LOGTAG, "chry_readline() ... string error\n");
                ret = 0;
        } else if (line == (void *)-1) {
                chry_readline_erase_line(&xwlua_cherryrl);
                chry_readline_edit_refresh(&xwlua_cherryrl);
                buffer[0] = '\0';
                ret = 1;
        } else if (linesize) {
                memcpy(buffer, line, linesize);
                buffer[linesize] = '\0';
                ret = 1;
        } else {
                buffer[0] = '\0';
                ret = 1;
        }
        return ret;
}
