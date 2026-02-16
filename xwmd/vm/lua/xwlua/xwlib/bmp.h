/**
 * @file
 * @brief XWLUA库：通用函数库：位图
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_vm_lua_xwlua_xwlib_bmp_h__
#define __xwmd_vm_lua_xwlua_xwlib_bmp_h__

#include <xwos/standard.h>
#include "xwmd/vm/lua/src/lauxlib.h"

typedef struct {
        xwsz_t bits;
        xwbmp_t bmp[0];
} xwlua_bmp_t;

void xwlua_lib_open_bmp(lua_State * L);

#endif /* xwmd/vm/lua/xwlua/xwlib/bmp.h */
