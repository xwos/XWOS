/**
 * @file
 * @brief newlib适配代码：errno
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
#include <errno.h>

void newlibac_errno_init(void)
{
}

int * __errno()
{
        struct xwos_thd * thd = xwos_cthd_self();
        return &thd->osthd.newlib.__errno;
}
