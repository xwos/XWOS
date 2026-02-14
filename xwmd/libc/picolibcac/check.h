/**
 * @file
 * @brief picolibc适配层：检查picolibc的配置是否满足要求
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_libc_picolibcac_check_h__
#define __xwmd_libc_picolibcac_check_h__

#include <picolibc.h>

#ifdef __SINGLE_THREAD__
#  error "picolibc must be configured to support multi-thread! Please check the build option 'newlib-multithread' in meson_options.txt!"
#endif

#ifdef TINY_STDIO
#  error "XWMD is not compatibled with tinystdio! Please check the build option 'tinystdio' in meson_options.txt!"
#endif

#ifndef __GLOBAL_ERRNO
#  error "__GLOBAL_ERRNO must be defined! Please check the build option 'newlib-global-errno' in meson_options.txt!"
#endif

#ifndef __PICOLIBC_ERRNO_FUNCTION
#  error "__PICOLIBC_ERRNO_FUNCTION must be defined as __errno! Please check the build option 'errno-function' in meson_options.txt!"
#endif

#ifndef __FVWRITE_IN_STREAMIO
#  error "__FVWRITE_IN_STREAMIO must be defined! Please check the build option 'newlib-fvwrite-in-streamio' in meson_options.txt!"
#endif


#endif /* xwmd/libc/picolibcac/check.h */
