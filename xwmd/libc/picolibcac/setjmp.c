/**
 * @file
 * @brief picolibc适配层：setjmp
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/setjmp.h>
#include <xwmd/libc/picolibcac/linkage.h>
#include <xwmd/libc/picolibcac/check.h>
#include <setjmp.h> // cppcheck-suppress [misra-c2012-21.4]

void picolibcac_setjmp_linkage_stub(void)
{
}

#if defined(XWLIBCFG_SETJMP) && (1 == XWLIBCFG_SETJMP)
int setjmp(jmp_buf jmpb); // cppcheck-suppress [misra-c2012-21.2]
void longjmp(jmp_buf jmpb, int rc); // cppcheck-suppress [misra-c2012-21.2]

int setjmp(jmp_buf jmpb)
{
        return xwlib_setjmp(jmpb);
}

void longjmp(jmp_buf jmpb, int rc)
{
        xwlib_longjmp(jmpb, rc);
}
#endif
