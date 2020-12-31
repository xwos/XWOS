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

#ifndef __xwos_lib_jmp_h__
#define __xwos_lib_jmp_h__

#include <xwos/standard.h>

#if defined(ARCHCFG_LIB_JMP) && (1 == ARCHCFG_LIB_JMP)
#include <asmlib/jmp.h>

/**
 * @brief 记录上下文的buffer类型
 */
typedef arch_xwjmpbuf_b xwjmpbuf_b;

/**
 * @brief 设置跳转点并保存上下文到buffer
 * @param ctx: (O) 保存上下文的buffer
 * @retval 0: xwlib_setjmp()正常调用时返回0
 * @retval !0: 从xwlib_longjmp()跳转过来时，返回!0
 */
int xwlib_setjmp(xwjmpbuf_b ctx);

/**
 * @brief 跳转到跳转点，并恢复上下文
 * @param ctx: (I) 待恢复的上下文的buffer
 * @param val: (I) 跳转到跳转点抛出的返回值，val不可为0，当填入0时会被修正为1
 */
__xwcc_noreturn void xwlib_longjmp(xwjmpbuf_b ctx, int val);

#else /* ARCHCFG_LIB_JMP */

#include <setjmp.h>

typedef jmp_buf xwjmpbuf_b;
#define xwlib_setjmp(ctx) setjmp(ctx)
#define xwlib_longjmp(ctx, val) longjmp(ctx, val);

#endif /* !ARCHCFG_LIB_JMP */

#endif /* xwos/lib/jmp.h */