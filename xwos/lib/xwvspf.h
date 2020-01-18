/**
 * @file
 * @brief XWOS通用库：vsprintf
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_xwvspf_h__
#define __xwos_lib_xwvspf_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum xwlib_xwvspf_fmt_type_em {
        XWLIB_SOVSPF_FT_NONE, /* a string part */
        XWLIB_SOVSPF_FT_WIDTH,
        XWLIB_SOVSPF_FT_PRECISION,
        XWLIB_SOVSPF_FT_CHAR,
        XWLIB_SOVSPF_FT_STR,
        XWLIB_SOVSPF_FT_PTR,
        XWLIB_SOVSPF_FT_PERCENT_CHAR,
        XWLIB_SOVSPF_FT_INVALID,
        XWLIB_SOVSPF_FT_LONG_LONG,
        XWLIB_SOVSPF_FT_ULONG,
        XWLIB_SOVSPF_FT_LONG,
        XWLIB_SOVSPF_FT_UBYTE,
        XWLIB_SOVSPF_FT_BYTE,
        XWLIB_SOVSPF_FT_USHORT,
        XWLIB_SOVSPF_FT_SHORT,
        XWLIB_SOVSPF_FT_UINT,
        XWLIB_SOVSPF_FT_INT,
        XWLIB_SOVSPF_FT_XWSZ_T,
        XWLIB_SOVSPF_FT_PTRDIFF,
};

struct xwlib_xwvspf_fmt_spec {
        xwu8_t type; /* format_type enum */
        xwu8_t flags; /* flags to number() */
        xwu8_t base; /* number base, 8, 10 or 16 only */
        xwu8_t qualifier; /* number qualifier, one of 'hHlLtzZ' */
        xws16_t field_width; /* width of output field */
        xws16_t precision; /* # of digits/chars */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWLIB_SOVSPF_F_ZEROPAD          0x01    /**< pad with zero */
#define XWLIB_SOVSPF_F_SIGN             0x02    /**< unsigned/signed long */
#define XWLIB_SOVSPF_F_PLUS             0x04    /**< show plus */
#define XWLIB_SOVSPF_F_SPACE            0x08    /**< space if plus */
#define XWLIB_SOVSPF_F_LEFT             0x10    /**< left justified */
#define XWLIB_SOVSPF_F_SMALL            0x20    /**< Must be 32 == 0x20 */
#define XWLIB_SOVSPF_F_SPECIAL          0x40    /**< leading flag:0x */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwlib_code
int xwvsnpf(char * buf, xwsz_t size, const char * fmt, va_list args);

__xwlib_code
int xwsdnpf(char * buf, xwsz_t size, const char * fmt, ...);

__xwlib_code
int xwvspf(char * buf, const char * fmt, va_list args);

__xwlib_code
int xwspf(char * buf, const char * fmt, ...);

#endif /* xwos/lib/xwvspf.h */
