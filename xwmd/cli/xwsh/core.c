/**
 * @file
 * @brief XWSH: Core
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/version.h>
#include <stdio.h>
#include <string.h>
#include <xwos/osal/thd.h>
#include "xwmd/cli/xwsh/mi.h"
#include "xwmd/cli/xwsh/core.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "XWSH.CORE"
/* #define XWSH_DBG */
#if defined(XWSH_DBG)
#  define XwshLogD(fmt, ...) xwlogf(D, LOGTAG, fmt, ##__VA_ARGS__)
#else
#  define XwshLogD(fmt, ...)
#endif
#define XwshLogI(fmt, ...) xwlogf(I, LOGTAG, fmt, ##__VA_ARGS__)
#define XwshLogE(fmt, ...) xwlogf(E, LOGTAG, fmt, ##__VA_ARGS__)

extern const struct xwsh_cmd xwsh_cmd_table[];

extern const xwsz_t xwsh_cmd_table_size;

static const struct xwsh_cmd * xwsh_ext_cmd_table = NULL;
static xwsz_t xwsh_ext_cmd_table_size = 0;

void xwsh_show_all_cmds(void)
{
        xwsz_t i;
        printf("--------------------------------------------\r\n");
        /* 显示内置命令 */
        for (i = 0; i < xwsh_cmd_table_size; i++) {
                printf("%s\t%s\r\n", xwsh_cmd_table[i].name, xwsh_cmd_table[i].desc);
        }
        /* 显示外部命令（如果存在） */
        if ((NULL != xwsh_ext_cmd_table) && (0 != xwsh_ext_cmd_table_size)) {
                for (i = 0; i < xwsh_ext_cmd_table_size; i++) {
                        printf("%s\t%s\r\n", xwsh_ext_cmd_table[i].name,
                               xwsh_ext_cmd_table[i].desc);
                }
        }
}

const struct xwsh_cmd * xwsh_find_cmd(char * name)
{
        xwsz_t i;
        const struct xwsh_cmd * cmd = NULL;

        if (!name) {
                goto err_name;
        }
        /* 先在内置命令表查找 */
        for (i = 0; i < xwsh_cmd_table_size; i++) {
                if (!strcmp(name, xwsh_cmd_table[i].name)) {
                        cmd = &xwsh_cmd_table[i];
                }
        }
        if (NULL == cmd) {
                /* 在外部命令表查找 */
                if ((NULL != xwsh_ext_cmd_table) && (0 != xwsh_ext_cmd_table_size)) {
                        for (i = 0; i < xwsh_ext_cmd_table_size; i++) {
                                if (!strcmp(name, xwsh_ext_cmd_table[i].name)) {
                                        cmd = &xwsh_ext_cmd_table[i];
                                        break;
                                }
                        }
                }
        }
err_name:
        return cmd;
}

xwer_t xwsh_set_ext_cmd_table(const struct xwsh_cmd cmd[], xwsz_t num)
{
        xwer_t rc = XWOK;
        xwsz_t i;
        xwsz_t j;

        /* 参数检查 */
        if ((NULL == cmd) && (0 != num)) {
                rc = -EINVAL;
                goto err_param;
        }

        /* 重名检查 */
        for (i = 0; i < num; i++) {
                /* 检查与内置命令重名 */
                for (j = 0; j < xwsh_cmd_table_size; j++) {
                        if (!strcmp(cmd[i].name, xwsh_cmd_table[j].name)) {
                                rc = -EEXIST;
                                goto err_duplicate;
                        }
                }

                /* 检查外部命令之间重名 */
                for (j = 0; j < i; j++) {
                        if (!strcmp(cmd[i].name, cmd[j].name)) {
                                rc = -EEXIST;
                                goto err_duplicate;
                        }
                }
        }

        /* 设置外部命令表 */
        xwsh_ext_cmd_table = cmd;
        xwsh_ext_cmd_table_size = num;
        return XWOK;

err_duplicate:
err_param:
        return rc;
}

xwer_t xwsh_split_cmdline(char * line, xwsz_t * argc, char * argv[], xwsz_t argvsize)
{
        char * p = line;
        xwsz_t count = 0;
        char * start = NULL;
        xwer_t rc = XWOK;

        while (*p) {
                if ((NULL == start) && (' ' == *p)) {
                        p++;
                } else {
                        if (NULL == start) {
                                start = p;
                        }
                        if (*p == ' ') {
                                if (count < argvsize) {
                                        *p = '\0';
                                        argv[count] = start;
                                        count++;
                                } else {
                                        rc = -E2BIG;
                                        break;
                                }
                                start = NULL;
                        }
                        p++;
                }
        }
        if (XWOK == rc) {
                if (start && (count < argvsize)) {
                        argv[count] = start;
                        count++;
                }
                *argc = count;
        }
        return rc;
}

xwer_t xwsh_run_cmdline(char * line)
{
        xwsz_t argc = 0;
        char * argv[XWSH_MAX_PARAM_NUM];
        const struct xwsh_cmd * cmd;
        xwer_t rc;

        rc = xwsh_split_cmdline(line, &argc, argv, XWSH_MAX_PARAM_NUM);
        if (XWOK == rc) {
#ifdef XWSH_DBG
                xwsz_t i;
                XwshLogD("argc: %d\r\n", argc);
                for (i = 0; i < argc; i++) {
                        XwshLogD("argv[%d]: %s\r\n", i, argv[i]);
                }
#endif
                cmd = xwsh_find_cmd(argv[0]);
                if (cmd) {
                        XwshLogD("cmd: %s, %s\r\n", cmd->name, cmd->desc);
                        if (cmd->func) {
                                rc = cmd->func(argc, argv);
                        } else {
                                printf("cmd %s not implemented.\r\n", cmd->name);
                        }
                } else if (argv[0]) {
                        printf("\r\ncan't find cmd %s.\r\n", argv[0]);
                } else {
                        printf("\r\n");
                }
        } else {
                XwshLogE("Input parameter count > %d!\r\n", XWSH_MAX_PARAM_NUM);
        }
        return rc;
}

const char xwsh_logo[] =
    "                      -=-:.      .:-=-                      \r\n"
    "                     :++++++=--=++++++:                     \r\n"
    "                    .+++-:-=++++=-:-+++.                    \r\n"
    "                   :+++-     ..     -+++:.                  \r\n"
    "             ..:=+++++=              =+++++=-..             \r\n"
    "             +++++=:.                  .:=+++++             \r\n"
    "             .+++-                        -+++.             \r\n"
    "              :+++:                      :+++:              \r\n"
    "               -+++.                    .+++-               \r\n"
    "              .+++-                      -+++.              \r\n"
    "              =++=                        =++=              \r\n"
    "             =+++=:.                    .:=+++=             \r\n"
    "             :-=+++++=:              :=+++++=-:             \r\n"
    "                 .:-+++:            :+++-:.                 \r\n"
    "                    :+++:.:-=++=-:.:+++:                    \r\n"
    "                     -+++++++==+++++++-                     \r\n"
    "                      =+=-:.    .:-=+=                      \r\n"
    ".---.   .--- .---    --=.   :--.    :--==-:.       .--==-:. \r\n"
    " -+++. .+++: .+++.  :+++-   +++.  :++++==+++=.   .=+++==++= \r\n"
    "  :+++.=++.   =++-  =++++  :++=  :+++:    -+++.  =+++.   .  \r\n"
    "   :+++++.    :++= .++-++- -++-  =++=      +++-  .++++=-:.  \r\n"
    "   :++++=     .+++ -+= =+= =++.  +++=      =++=    .-=++++=:\r\n"
    "  :++=:+++.    =++-=+: :++-++=   -+++.    :+++:        .=+++\r\n"
    " -++=  :+++:   :+++++   +++++:    -+++=--=+++:   -++=-:-+++-\r\n"
    ":+==.   -===.   ====-   -====.     .-=++++=-     .-=++++=-. \r\n";

void xwsh_show_logo(void)
{
        printf(xwsh_logo);
        printf("Welcome to the XWOS-V%s! Build time %s %s\r\n", XWOS_VERSION, __DATE__,
               __TIME__);
}
