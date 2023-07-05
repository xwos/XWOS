/**
 * @file
 * @brief picolibc适配层：errno
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/thd.h>
#include <xwmd/libc/picolibcac/linkage.h>
#include <xwmd/libc/picolibcac/check.h>
#include <errno.h>

void picolibcac_errno_linkage_stub(void)
{
}

int * __errno(void);

int * __errno(void)
{
        xwos_thd_d thdd = xwos_cthd_self();
        return &thdd.thd->osthd.libc.error_number;
}
