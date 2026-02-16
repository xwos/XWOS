/**
 * @file
 * @brief XWLUA Prefix Include File
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_prefix_h__
#define __xwmd_vm_lua_xwlua_prefix_h__

#include <xwos/standard.h>
#include <xwos/lib/setjmp.h>

#define XWLUA_BRDLIBS           1
#define XWLUA_THD_STACK_SIZE    (16384U)

#define LUA_MAXINPUT            512U

void * xwlua_alloc(void * ud, void * ptr, xwsz_t osize, xwsz_t nsize);
#define LUA_ALLOC               xwlua_alloc

#define lua_writeline()         (lua_writestring("\r\n", 2), fflush(stdout))

#define LUAI_THROW(L,c)		xwlib_longjmp((c)->b, 1)
#define LUAI_TRY(L,c,a)		if (xwlib_setjmp((c)->b) == 0) { a }
#define luai_jmpbuf		xwjmpbuf_b

#endif /* xwmd/vm/lua/xwlua/prefix.h */
