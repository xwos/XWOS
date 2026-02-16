/**
 * @file
 * @brief XWSH：模块集成接口
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/thd.h>
#include "xwmd/cli/xwsh/readline.h"
#include "xwmd/cli/xwsh/core.h"
#include "xwmd/cli/xwsh/mi.h"

xwer_t xwsh_thd_mainfunc(void * arg);
struct xwos_thd xwsh_thd;
xwos_thd_d xwsh_thdd;
#define XWSH_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

xwer_t xwsh_start(xwstk_t * stack, xwsz_t stack_size)
{
        xwer_t rc;
        struct xwos_thd_attr attr;

        xwos_thd_attr_init(&attr);
        attr.name = "xwsh.thd";
        attr.stack = stack;
        attr.stack_size = stack_size;
        attr.priority = XWSH_THD_PRIORITY;
        attr.detached = true;
        attr.privileged = true;
        rc = xwos_thd_init(&xwsh_thd, &xwsh_thdd,
                           &attr,
                           xwsh_thd_mainfunc,
                           NULL);
        return rc;
}

void xwsh_init(void)
{
        xwsh_show_logo();
        xwsh_cherryrl_init();
}

void xwsh_loop(char * buf)
{
        char * line;

        line = xwsh_cherryrl_readline(buf);
        if (NULL != line) {
                xwsh_run_cmdline(line);
        }
}

xwer_t xwsh_thd_mainfunc(void * arg)
{
        char buf[XWSH_MAXINPUT];

        XWOS_UNUSED(arg);
        xwsh_init();
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                }
                xwsh_loop(buf);
        }
        return XWOK;
}
