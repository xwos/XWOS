/**
 * @file
 * @brief picolibc适配层：stdio
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwmd/libc/picolibcac/linkage.h>
#include <xwmd/libc/picolibcac/check.h>
#include <stdio.h>

void picolibcac_stdio_linkage_stub(void)
{
}

extern int picolibcac_fops_put(char c);

extern int picolibcac_fops_get(void);

static int stdio_put(char c, FILE * file)
{
        XWOS_UNUSED(file);
        return picolibcac_fops_put(c);
}

static int stdio_get(FILE * file)
{
        XWOS_UNUSED(file);
        return picolibcac_fops_get();
}

FILE stdio_file = FDEV_SETUP_STREAM(stdio_put, stdio_get, NULL, _FDEV_SETUP_RW);
FILE * const stdout = &stdio_file;
FILE * const stderr = &stdio_file;
FILE * const stdin = &stdio_file;
