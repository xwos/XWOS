/**
 * @file
 * @brief newlib适配层：setjmp
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
#include <xwmd/libc/newlibac/linkage.h>
#include <xwmd/libc/newlibac/check.h>
#include <setjmp.h> // cppcheck-suppress [misra-c2012-21.4]

void newlibac_setjmp_linkage_stub(void)
{
}

#if (__NEWLIB__ > 4) || (__NEWLIB__ == 4 && __NEWLIB_MINOR__ >= 3)
#  if defined(XWLIBCFG_SETJMP) && (1 == XWLIBCFG_SETJMP)
int setjmp(jmp_buf jmpb); // cppcheck-suppress [misra-c2012-21.2]
void longjmp(jmp_buf jmpb, int rc); // cppcheck-suppress [misra-c2012-21.2]

int setjmp(jmp_buf jmpb)
{
        return xwlib_setjmp((xwstk_t *)jmpb);
}

void longjmp(jmp_buf jmpb, int rc)
{
        xwlib_longjmp((xwstk_t *)jmpb, rc);
}
#  endif /* XWLIBCFG_SETJMP */
#else /* newlib version */
#  warning "setjmp.h in newlib(<4.3.0) has bug! Use <xwos/lib/setjmp.h> instead!"
#endif /* newlib version */
