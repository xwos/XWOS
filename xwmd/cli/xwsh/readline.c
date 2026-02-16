/**
 * @file
 * @brief XWSH: Readline
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <stdio.h>
#include <string.h>
#include <xwos/osal/thd.h>
#include <xwmd/cli/cherryrl/chry_readline.h>
#include "xwmd/cli/xwsh/mi.h"
#include "xwmd/cli/xwsh/readline.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "XWSH.RL"
/* #define XWSH_DBG */
#if defined(XWSH_DBG)
#  define XwshLogD(fmt, ...) xwlogf(D, LOGTAG, fmt, ##__VA_ARGS__)
#else
#  define XwshLogD(fmt, ...)
#endif
#define XwshLogI(fmt, ...) xwlogf(I, LOGTAG, fmt, ##__VA_ARGS__)
#define XwshLogE(fmt, ...) xwlogf(E, LOGTAG, fmt, ##__VA_ARGS__)

#define XWSH_RL_PROMPT_MAXSIZE          64U
#define XWSH_RL_HISTORY_MAXSIZE         1024U /* size must be power of 2 !!! */

chry_readline_t xwsh_cherryrl;
char xwsh_cherryrl_prompt[XWSH_RL_PROMPT_MAXSIZE];
char xwsh_cherryrl_history[XWSH_RL_HISTORY_MAXSIZE];

int xwsh_cherryrl_ucb(chry_readline_t * rl, uint8_t exec)
{
        /* user event callback will not output newline automatically */
        chry_readline_newline(rl);

        switch (exec) {
        case CHRY_READLINE_EXEC_EOF:
                XwshLogI("EOF\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGINT:
                chry_readline_ignore(rl, false);
                XwshLogI("SIGINT\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGQUIT:
                XwshLogI("SIGQUIT\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGCONT:
                XwshLogI("SIGCONT\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGSTOP:
                XwshLogI("SIGSTOP\r\n");
                break;
        case CHRY_READLINE_EXEC_SIGTSTP:
                XwshLogI("SIGTSTP\r\n");
                break;
        case CHRY_READLINE_EXEC_F1 ... CHRY_READLINE_EXEC_F12:
                XwshLogI("F%d event\r\n", exec - CHRY_READLINE_EXEC_F1 + 1);
                break;
        default:
                XwshLogI("ERROR EXEC %d\r\n", exec);
                return -1;
        }

        /* return 1 will not refresh */
        /* return 0 to refresh whole line (include prompt) */
        /* return -1 to end readline (error) */
        return 0;
}

uint8_t xwsh_cherryrl_acb(chry_readline_t * rl, char * pre,
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

uint16_t xwsh_cherryrl_sput(chry_readline_t * rl, const void * data, uint16_t size)
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

uint16_t xwsh_cherryrl_sget(chry_readline_t * rl, void * data, uint16_t size)
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

void xwsh_cherryrl_init(void)
{
        int rc;

        chry_readline_init_t init = {
                .prompt = xwsh_cherryrl_prompt,
                .pptsize = sizeof(xwsh_cherryrl_prompt),
                .history = xwsh_cherryrl_history,
                .histsize = sizeof(xwsh_cherryrl_history),
                .sget = xwsh_cherryrl_sget,
                .sput = xwsh_cherryrl_sput,
        };
        rc = chry_readline_init(&xwsh_cherryrl, &init);
        if (0 != rc) {
                XwshLogE("chry_readline_init() ... <%ld>\n", rc);
        } else {
                chry_readline_set_completion_cb(&xwsh_cherryrl, xwsh_cherryrl_acb);
                chry_readline_set_user_cb(&xwsh_cherryrl, xwsh_cherryrl_ucb);

                chry_readline_prompt_edit(&xwsh_cherryrl, 0,
                                          (chry_readline_sgr_t){
                                                  .foreground = CHRY_READLINE_SGR_GREEN,
                                                  .bold = 1,
                                          }.raw,
                                          "xwsh");
                chry_readline_prompt_edit(&xwsh_cherryrl, 1, 0, "@");
                chry_readline_prompt_edit(&xwsh_cherryrl, 2,
                                          (chry_readline_sgr_t){
                                                  .foreground = CHRY_READLINE_SGR_BLUE,
                                                  .bold = 1,
                                          }.raw,
                                          "xwos");
                chry_readline_prompt_edit(&xwsh_cherryrl, 3, 0, ":>");
        }
}

char * xwsh_cherryrl_readline(char buffer[])
{
        char linebuf[XWSH_MAXINPUT];
        char * line;
        xwu16_t linesize;
        char * ret;

        line = chry_readline(&xwsh_cherryrl, linebuf, XWSH_MAXINPUT, &linesize);
        if (line == NULL) {
                XwshLogE("chry_readline() ... string error\n");
                ret = NULL;
        } else if (line == (void *)-1) {
                chry_readline_erase_line(&xwsh_cherryrl);
                chry_readline_edit_refresh(&xwsh_cherryrl);
                buffer[0] = '\0';
                ret = buffer;
        } else if (linesize) {
                memcpy(buffer, line, linesize);
                buffer[linesize] = '\0';
                ret = buffer;
        } else {
                buffer[0] = '\0';
                ret = NULL;
        }
        return ret;
}
