/**
 * @file
 * @brief Prefix Include File for Lua in XWOS
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#ifndef __xwem_vm_lua_xwlua_prefix_h__
#define __xwem_vm_lua_xwlua_prefix_h__

#include <xwos/standard.h>
#include <xwos/lib/setjmp.h>

void * xwlua_alloc(void * ud, void * ptr, size_t osize, size_t nsize);
#define LUA_ALLOC       xwlua_alloc

#define lua_writeline() (lua_writestring("\r\n", 2), fflush(stdout))

#define LUAI_THROW(L,c)		xwlib_longjmp((c)->b, 1)
#define LUAI_TRY(L,c,a)		if (xwlib_setjmp((c)->b) == 0) { a }
#define luai_jmpbuf		xwjmpbuf_b

#endif /* xwem/vm/lua/xwlua/prefix.h */
