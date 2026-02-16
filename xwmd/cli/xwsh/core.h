/**
 * @file
 * @brief XWSH：Core
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_cli_xwsh_core_h__
#define __xwmd_cli_xwsh_core_h__

#include <xwos/standard.h>

#define XWSH_MAX_PARAM_NUM 16U

xwer_t xwsh_run_cmdline(char * line);
void xwsh_show_all_cmds(void);
void xwsh_show_logo(void);

#endif /* xwmd/cli/xwsh/core.h */
