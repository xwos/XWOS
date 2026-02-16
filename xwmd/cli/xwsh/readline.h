/**
 * @file
 * @brief XWSH：Readline
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_cli_xwsh_readline_h__
#define __xwmd_cli_xwsh_readline_h__

#include <xwos/standard.h>

void xwsh_cherryrl_init(void);
char * xwsh_cherryrl_readline(char buffer[]);

#endif /* xwmd/cli/xwsh/readline.h */
