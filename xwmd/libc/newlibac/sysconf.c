/**
 * @file
 * @brief newlib适配层：sysconf
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
#include <xwos/osal/time.h>
#include <xwos/mm/mempool/allocator.h>
#include <xwmd/libc/newlibac/linkage.h>
#include <xwmd/libc/newlibac/check.h>
#include <unistd.h>

void newlibac_sysconf_linkage_stub(void)
{
}

long sysconf(int name);

long sysconf(int name)
{
        long conf;
        switch (name) {
        case _SC_CLK_TCK:
                errno = 0;
                conf = (long)XWTM_TC_PER_S; // cppcheck-suppress [misra-c2012-10.7]
                break;
        case _SC_PAGESIZE:
                errno = 0;
                conf = XWMM_MEMPOOL_PAGE_SIZE;
                break;
        default:
                errno = EINVAL;
                conf = -1;
                break;
        }
        return conf;
}
