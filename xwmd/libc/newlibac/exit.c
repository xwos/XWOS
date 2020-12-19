/**
 * @file
 * @brief newlib适配代码：退出
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/skd.h>

void newlibac_exit_linkage_placeholder(void)
{
}

void _exit(xwer_t rc)
{
        xwos_cthd_exit(rc);
        while (true) {
        }
}
