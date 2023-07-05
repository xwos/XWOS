/**
 * @file
 * @brief newlib适配层：getpid
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwos/osal/thd.h>
#include <xwmd/libc/newlibac/linkage.h>
#include <xwmd/libc/newlibac/check.h>
#include <unistd.h>

void newlibac_getpid_linkage_stub(void)
{
}

pid_t _getpid_r(struct _reent * r);

pid_t _getpid_r(struct _reent * r)
{
        XWOS_UNUSED(r);

        return 1;
}
