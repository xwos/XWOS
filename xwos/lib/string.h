/**
 * @file
 * @brief XWOS通用库：string
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_string_h__
#define __xwos_lib_string_h__

#if defined(ARCHCFG_COMPILER_CLIB) && (1 == ARCHCFG_COMPILER_CLIB)
  #include <string.h>
#else
/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwlib_code
void * memset(void * s, int c, xwsz_t count);

__xwlib_code
int memcmp(const void * cs, const void * ct, xwsz_t count);

__xwlib_code
void * memcpy(void * restrict dest, const void * restrict src, xwsz_t count);

__xwlib_code
void * memmove(void * dest, const void * src, xwsz_t count);

__xwlib_code
void * memchr(const void * s, int c, xwsz_t n);

__xwlib_code
xwsz_t strlen(const char * s);

__xwlib_code
xwsz_t strnlen(const char * s, xwsz_t count);

__xwlib_code
char * strpbrk(const char * cs, const char * ct);

__xwlib_code
char * strsep(char ** s, const char * ct);

__xwlib_code
char * strstr(const char * s1, const char * s2);

__xwlib_code
char * strnstr(const char * s1, const char * s2, xwsz_t len);

__xwlib_code
int strcmp(const char * cs, const char * ct);

__xwlib_code
int strncmp(const char * cs, const char * ct, xwsz_t count);

__xwlib_code
char * strchr(const char * s, int c);

__xwlib_code
char * strchrnul(const char * s, int c);

__xwlib_code
char * strrchr(const char * s, int c);

__xwlib_code
char * strnchr(const char * s, xwsz_t count, int c);

#endif /* ARCHCFG_COMPILER_CLIB */
#endif /* xwos/lib/string.h */
