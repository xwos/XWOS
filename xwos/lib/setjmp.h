/**
 * @file
 * @brief XWOS通用库：setjmp/longjmp
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_setjmp_h__
#define __xwos_lib_setjmp_h__

#include <xwos/standard.h>

/**
 * @defgroup setjmp setjmp/longjmp
 * @{
 */

#if defined(XWLIBCFG_SETJMP) && (1 == XWLIBCFG_SETJMP)
#  include <xwos/ospl/soc/setjmp.h>

/**
 * @brief 设置跳转点并保存上下文到buffer
 * @param[out] ctx: 保存上下文的buffer
 * @retval 0: xwlib_setjmp()正常调用时返回0
 * @retval !0: 从xwlib_longjmp()跳转过来时，返回!0
 */
int xwlib_setjmp(xwjmpbuf_b ctx);

/**
 * @brief 跳转到跳转点，并恢复上下文
 * @param[in] ctx: 待恢复的上下文的buffer
 * @param[in] val: 跳转到跳转点抛出的返回值，val不可为0，当填入0时会被修正为1
 */
__xwcc_noreturn void xwlib_longjmp(xwjmpbuf_b ctx, int val);

#else

#  include <setjmp.h>

typedef jmp_buf xwjmpbuf_b;
#  define xwlib_setjmp(ctx) setjmp(ctx)
#  define xwlib_longjmp(ctx, val) longjmp(ctx, val);

#endif

/**
 * @} setjmp
 */

#endif /* xwos/lib/setjmp.h */
