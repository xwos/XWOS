/**
 * @file
 * @brief picolibc适配层：isatty
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
#include <unistd.h>

void picolibcac_isatty_linkage_stub(void)
{
}

int isatty(int fd);

int isatty(int fd)
{
        int rc;

        switch (fd) {
        case 0:
        case 1:
        case 2:
                rc = 1;
                break;
        default:
                rc = 0;
                break;
        }
        return rc;
}
