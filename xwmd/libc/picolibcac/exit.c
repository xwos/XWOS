/**
 * @file
 * @brief picolibc适配层：exit
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
#include <xwmd/libc/picolibcac/linkage.h>
#include <xwmd/libc/picolibcac/check.h>

void picolibcac_exit_linkage_stub(void)
{
}

void _exit(int rc);

void _exit(int rc)
{
        /* `exit()` 是结束整个进程。
           RTOS可以看做是单一进程运行多个的线程，`exit()` 相当于结束整个系统。
           但此处将其实现为结束当前线程。 */
        xwos_cthd_exit((xwer_t)rc);
        while (true) {
        }
}
