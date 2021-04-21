/**
 * @file
 * @brief 玄武OS移植层：SOC setjmp/longjmp
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_setjmp_h__
#define __xwos_ospl_soc_setjmp_h__

#include <xwosimpl_soc_setjmp.h>

typedef soc_jmpbuf_b xwjmpbuf_b;

int xwlib_setjmp(xwjmpbuf_b ctx);
__xwcc_noreturn void xwlib_longjmp(xwjmpbuf_b ctx, int val);

#endif /* xwos/ospl/soc/setjmp.h */
