/**
 * @file
 * @brief 玄武C库 Lua Lib：位图
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwem_vm_lua_xwlua_xwlib_bmp_h__
#define __xwem_vm_lua_xwlua_xwlib_bmp_h__

#include <xwos/standard.h>
#include "src/lauxlib.h"

typedef struct {
        xwsz_t bits;
        xwbmp_t bmp[0];
} xwlua_bmp_t;

void xwlua_lib_open_bmp(lua_State * L);

#endif /* xwem/vm/lua/xwlua/xwlib/bmp.h */
