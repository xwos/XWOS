/**
 * @file
 * @brief newlib适配层：检查newlib的配置是否满足要求
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_libc_newlib_check_h__
#define __xwmd_libc_newlib_check_h__

#include <newlib.h>

#ifdef __SINGLE_THREAD__
#  error "newlib must be configured to support multi-thread! Build newlib with the option '--enable-newlib-multithread' !"
#endif

#ifdef _WANT_REENT_THREAD_LOCAL
#  error "_WANT_REENT_THREAD_LOCAL must NOT be defined! Build newlib without the option '--enablenewlib-reent-thread-local' !"
#endif

#ifdef _REENT_ONLY
#  error "_REENT_ONLY must NOT be defined!"
#endif

#ifndef _FVWRITE_IN_STREAMIO
#  error "_FVWRITE_IN_STREAMIO must be defined! Build newlib without the option '--enable-newlib-fvwrite-in-streamio' !"
#endif

#endif /* xwmd/libc/newlib/check.h */
