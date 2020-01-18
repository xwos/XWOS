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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/lib/ctype.h>
#include <xwos/lib/div64.h>
#include <xwos/lib/xwvspf.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwlib_rodata const char xwlib_xwvspf_digits[] = "0123456789ABCDEF";
static __xwlib_rodata const char xwlib_xwvspf_nullstr[] = "(null)";

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwlib_inline
int xwlib_xwvspf_skip_atoi(const unsigned char ** s)
{
        int i = 0;

        while (isdigit(**s)) {
                i = i * 10 + *((*s)++) - '0';
                /* equal to this
                i = i * 10 + **s - '0';
                (*s)++;
                */
        }
        return i;
}

static __xwlib_inline
char * xwlib_xwvspf_put_dec_trunc(char * buf, unsigned int q)
{
        unsigned int d3, d2, d1, d0;

        d1 = (q >> 4) & 0xf;
        d2 = (q >> 8) & 0xf;
        d3 = (q >> 12);

        d0 = 6 * (d3 + d2 + d1) + (q & 0xf);
        q = (d0 * 0xcd) >> 11;
        d0 = d0 - 10 * q;
        *buf++ = (char)d0 + '0';

        d1 = q + 9 * d3 + 5 * d2 + d1;
        if (d1 != 0) {
                q = (d1 * 0xcd) >> 11;
                d1 = d1 - 10 * q;
                *buf++ = (char)d1 + '0'; /* next digit */

                d2 = q + 2 * d2;
                if ((d2 != 0) || (d3 != 0)) {
                        q = (d2 * 0xd) >> 7;
                        d2 = d2 - 10 * q;
                        *buf++ = (char)d2 + '0'; /* next digit */

                        d3 = q + 4 * d3;
                        if (d3 != 0) {
                                q = (d3 * 0xcd) >> 11;
                                d3 = d3 - 10 * q;
                                *buf++ = (char)d3 + '0';  /* next digit */
                                if (q != 0) {
                                        *buf++ = (char)q + '0'; /* most sign. digit */
                                }
                        }
                }
        }
        return buf;
}

static __xwlib_inline
char * xwlib_xwvspf_put_dec_full(char * buf, unsigned int q)
{
        unsigned int d3, d2, d1, d0;

        d1 = (q >> 4) & 0xf;
        d2 = (q >> 8) & 0xf;
        d3 = (q >> 12);

        d0 = 6 * (d3 + d2 + d1) + (q & 0xf);
        q = (d0 * 0xcd) >> 11;
        d0 = d0 - 10 * q;
        *buf++ = (char)d0 + '0';

        d1 = q + 9 * d3 + 5 * d2 + d1;
        q = (d1 * 0xcd) >> 11;
        d1 = d1 - 10 * q;
        *buf++ = (char)d1 + '0';

        d2 = q + 2 * d2;
        q = (d2 * 0xd) >> 7;
        d2 = d2 - 10 * q;
        *buf++ = (char)d2 + '0';

        d3 = q + 4 * d3;
        q = (d3 * 0xcd) >> 11; /* - shorter code */
        /* q = (d3 * 0x67) >> 10; - would also work */
        d3 = d3 - 10 * q;
        *buf++ = (char)d3 + '0';
        *buf++ = (char)q + '0';

        return buf;
}

static __xwlib_inline
char * xwlib_xwvspf_put_dec(char * buf, unsigned long long num)
{
        while (true) {
                unsigned int rem;
                if (num < 100000) {
                        return xwlib_xwvspf_put_dec_trunc(buf, (unsigned int)num);
                }
                rem = xwdivll(&num, 100000);
                buf = xwlib_xwvspf_put_dec_full(buf, rem);
        }
}

static __xwlib_inline
char * xwlib_xwvspf_fmt_number(char * buf, char * end,
                               unsigned long long num,
                               struct xwlib_xwvspf_fmt_spec spec)
{
        char tmp[66];
        char sign;
        char locase;
        int need_pfx = ((spec.flags & XWLIB_SOVSPF_F_SPECIAL) && spec.base != 10);
        int i;
        bool is_zero = num == 0LL;

        locase = (char)(spec.flags & XWLIB_SOVSPF_F_SMALL);
        if (spec.flags & XWLIB_SOVSPF_F_LEFT) {
                spec.flags &= ~XWLIB_SOVSPF_F_ZEROPAD;
        }/* else {} */
        sign = 0;
        if (spec.flags & XWLIB_SOVSPF_F_SIGN) {
                if ((signed long long)num < 0) {
                        sign = '-';
                        num = (unsigned long long)(-(signed long long)num);
                        spec.field_width--;
                } else if (spec.flags & XWLIB_SOVSPF_F_PLUS) {
                        sign = '+';
                        spec.field_width--;
                } else if (spec.flags & XWLIB_SOVSPF_F_SPACE) {
                        sign = ' ';
                        spec.field_width--;
                }/* else {} */
        }
        if (need_pfx) {
                if (16 == spec.base) {
                        spec.field_width -= 2;
                } else if (!is_zero) {
                        spec.field_width--;
                }/* else {} */
        }/* else {} */

        /* generate full string in tmp[], in reverse order */
        i = 0;
        if (num < spec.base) {
                tmp[i++] = xwlib_xwvspf_digits[num] | locase;
        /* Generic code, for any base:
        } else {
                do {
                        tmp[i++] = (xwlib_xwvspf_digits[xwdivll(&num, base)] | locase);
                } while (num != 0);
        */
        } else if (spec.base != 10) { /* 8 or 16 */
                int mask = spec.base - 1;
                int shift = 3;

                if (spec.base == 16) {
                        shift = 4;
                }
                do {
                        tmp[i++] = (xwlib_xwvspf_digits[((unsigned char)num) & mask] |
                                    locase);
                        num >>= shift;
                } while (num);
        } else { /* base 10 */
                i = xwlib_xwvspf_put_dec(tmp, num) - tmp;
        }

        /* printing 100 using %2d gives "100", not "00" */
        if (i > spec.precision)
                spec.precision = (xws16_t)i;
        /* leading space padding */
        spec.field_width -= spec.precision;
        if (!(spec.flags & (XWLIB_SOVSPF_F_ZEROPAD + XWLIB_SOVSPF_F_LEFT))) {
                while (--spec.field_width >= 0) {
                        if (buf < end) {
                                *buf = ' ';
                        }
                        buf++;
                }
        }
        /* sign */
        if (sign) {
                if (buf < end) {
                        *buf = sign;
                }
                buf++;
        }
        /* "0x" / "0" prefix */
        if (need_pfx) {
                if (spec.base == 16 || !is_zero) {
                        if (buf < end) {
                                *buf = '0';
                        }
                        buf++;
                }
                if (spec.base == 16) {
                        if (buf < end) {
                                *buf = ('X' | locase);
                        }
                        buf++;
                }
        }
        /* zero or space padding */
        if (!(spec.flags & XWLIB_SOVSPF_F_LEFT)) {
                char c = (spec.flags & XWLIB_SOVSPF_F_ZEROPAD) ? '0' : ' ';
                while (--spec.field_width >= 0) {
                        if (buf < end) {
                                *buf = c;
                        }
                        buf++;
                }
        }
        /* hmm even more zero padding? */
        while (i <= --spec.precision) {
                if (buf < end) {
                        *buf = '0';
                }
                buf++;
        }
        /* actual xwlib_xwvspf_digits of result */
        while (--i >= 0) {
                if (buf < end) {
                        *buf = tmp[i];
                }
                buf++;
        }
        /* trailing space padding */
        while (--spec.field_width >= 0) {
                if (buf < end) {
                        *buf = ' ';
                }
                buf++ ;
        }

        return buf;
}

static __xwlib_inline
char * xwlib_xwvspf_fmt_string(char * buf, char * end, const char * s,
                               struct xwlib_xwvspf_fmt_spec spec)
{
        xwssz_t len, i;

        if ((unsigned long)s <= MAX_ERRNO) {
                s = xwlib_xwvspf_nullstr;
        }

        len = (xwssz_t)strlen(s);
        if (len > spec.precision) {
                len = spec.precision;
        }

        if (!(spec.flags & XWLIB_SOVSPF_F_LEFT)) {
                while (len < spec.field_width--) {
                        if (buf < end)
                                *buf = ' ';
                        ++buf;
                }
        }
        for (i = 0; i < len; ++i) {
                if (buf < end) {
                        *buf = *s;
                }
                ++buf;
                ++s;
        }
        while (len < spec.field_width--) {
                if (buf < end) {
                        *buf = ' ';
                }
                ++buf;
        }

        return buf;
}

static __xwlib_inline
char * xwlib_xwvspf_fmt_pointer(const char * fmt, char * buf, char * end, void * ptr,
                                struct xwlib_xwvspf_fmt_spec spec)
{
        int default_width;

        (void)fmt;
        default_width = (int)(2 * sizeof(void *) +
                              (spec.flags & XWLIB_SOVSPF_F_SPECIAL ? 2 : 0));
        spec.flags |= XWLIB_SOVSPF_F_SMALL;
        if (spec.field_width == -1) {
                spec.field_width = (xws16_t)default_width;
                spec.flags |= XWLIB_SOVSPF_F_ZEROPAD;
        }
        spec.base = 16;

        return xwlib_xwvspf_fmt_number(buf, end, (unsigned long)ptr, spec);
}

static __xwlib_inline
int xwlib_xwvspf_fmt_decode(const unsigned char * fmt,
                            struct xwlib_xwvspf_fmt_spec * spec)
{
        const unsigned char * start = fmt;

        /* we finished early by reading the field width */
        if (spec->type == XWLIB_SOVSPF_FT_WIDTH) {
                if (spec->field_width < 0) {
                        spec->field_width = -spec->field_width;
                        spec->flags |= XWLIB_SOVSPF_F_LEFT;
                }
                spec->type = XWLIB_SOVSPF_FT_NONE;
                goto precision;
        }

        /* we finished early by reading the precision */
        if (spec->type == XWLIB_SOVSPF_FT_PRECISION) {
                if (spec->precision < 0)
                        spec->precision = 0;

                spec->type = XWLIB_SOVSPF_FT_NONE;
                goto qualifier;
        }

        /* By default */
        spec->type = XWLIB_SOVSPF_FT_NONE;

        for (; *fmt ; fmt++) {
                if ('%' == *fmt) {
                        break;
                }
        }

        /* Return the current non-format string */
        if ((fmt != start) || !(*fmt)) {
                return fmt - start;
        }

        /* Process flags */
        spec->flags = 0;

        while (true) {
                bool found;

                found = true;
                fmt++;
                switch (*fmt) {
                case '-':
                        spec->flags |= XWLIB_SOVSPF_F_LEFT;
                        break;
                case '+':
                        spec->flags |= XWLIB_SOVSPF_F_PLUS;
                        break;
                case ' ':
                        spec->flags |= XWLIB_SOVSPF_F_SPACE;
                        break;
                case '#':
                        spec->flags |= XWLIB_SOVSPF_F_SPECIAL;
                        break;
                case '0':
                        spec->flags |= XWLIB_SOVSPF_F_ZEROPAD;
                        break;
                default:
                        found = false;
                        break;
                }

                if (!found) {
                        break;
                }
        }

        /* get field width */
        spec->field_width = -1;

        if (isdigit(*fmt)) {
                spec->field_width = (xws16_t)xwlib_xwvspf_skip_atoi(&fmt);
        } else if ('*' == *fmt) {
                /* The next argument */
                spec->type = XWLIB_SOVSPF_FT_WIDTH;
                fmt++;
                return fmt - start;
        }/* else {} */

precision:
        /* get the precision */
        spec->precision = -1;
        if ('.' == (*fmt)) {
                fmt++ ;
                if (isdigit(*fmt)) {
                        spec->precision = (xws16_t)xwlib_xwvspf_skip_atoi(&fmt);
                        if (spec->precision < 0) {
                                spec->precision = 0;
                        }/* else {} */
                } else if (*fmt == '*') {
                        /* it's the next argument */
                        spec->type = XWLIB_SOVSPF_FT_PRECISION;
                        fmt++;
                        return fmt - start;
                }
        }

qualifier:
        /* get the conversion qualifier */
        spec->qualifier = (xwu8_t)(-1);
        if (('h' == *fmt) || ('l' == tolower(*fmt)) ||
            ('z' == tolower(*fmt)) || ('t' == *fmt)) {
                spec->qualifier = *fmt++;
                if (__unlikely(spec->qualifier == *fmt)) {
                        if ('l' == spec->qualifier) {
                                spec->qualifier = 'L';
                                fmt++;
                        } else if ('h' == spec->qualifier) {
                                spec->qualifier = 'H';
                                fmt++;
                        }
                }
        }

        /* default base */
        spec->base = 10;
        switch (*fmt) {
        case 'c':
                spec->type = XWLIB_SOVSPF_FT_CHAR;
                fmt++;
                return fmt - start;

        case 's':
                spec->type = XWLIB_SOVSPF_FT_STR;
                fmt++;
                return fmt - start;

        case 'p':
                spec->type = XWLIB_SOVSPF_FT_PTR;
                return fmt - start;
                /* skip alnum */

        case '%':
                spec->type = XWLIB_SOVSPF_FT_PERCENT_CHAR;
                fmt++;
                return fmt - start;

        case 'o':
                spec->base = 8;
                break;

        case 'x':
                spec->flags |= XWLIB_SOVSPF_F_SMALL;
                spec->base = 16;
                break;

        case 'X':
                spec->base = 16;
                break;

        case 'd':
        case 'i':
                spec->flags |= XWLIB_SOVSPF_F_SIGN;
        case 'u':
                break;

        default:
                spec->type = XWLIB_SOVSPF_FT_INVALID;
                return fmt - start;
        }

        if ('L' == spec->qualifier) {
                spec->type = XWLIB_SOVSPF_FT_LONG_LONG;
        }
        else if ('l' == spec->qualifier) {
                if (spec->flags & XWLIB_SOVSPF_F_SIGN) {
                        spec->type = XWLIB_SOVSPF_FT_LONG;
                } else {
                        spec->type = XWLIB_SOVSPF_FT_ULONG;
                }
        } else if ('z' == tolower(spec->qualifier)) {
                spec->type = XWLIB_SOVSPF_FT_XWSZ_T;
        } else if ('t' == spec->qualifier) {
                spec->type = XWLIB_SOVSPF_FT_PTRDIFF;
        } else if ('H' == spec->qualifier) {
                if (spec->flags & XWLIB_SOVSPF_F_SIGN) {
                        spec->type = XWLIB_SOVSPF_FT_BYTE;
                } else {
                        spec->type = XWLIB_SOVSPF_FT_UBYTE;
                }
        } else if ('h' == spec->qualifier) {
                if (spec->flags & XWLIB_SOVSPF_F_SIGN) {
                        spec->type = XWLIB_SOVSPF_FT_SHORT;
                } else {
                        spec->type = XWLIB_SOVSPF_FT_USHORT;
                }
        } else {
                if (spec->flags & XWLIB_SOVSPF_F_SIGN) {
                        spec->type = XWLIB_SOVSPF_FT_INT;
                } else {
                        spec->type = XWLIB_SOVSPF_FT_UINT;
                }
        }

        fmt++;
        return fmt - start;
}

/**
 * @brief 同vsprintf()函数
 */
__xwlib_code
int xwvsnpf(char * buf, xwsz_t size, const char * fmt, va_list args)
{
        xwu64_t num;
        char * str, * end;
        struct xwlib_xwvspf_fmt_spec spec = {0};

        /* Reject out-of-range values early. */
        if ((xwssz_t)size < 0) {
                return 0;
        }

        str = buf;
        end = buf + size;

        /* Make sure end is always >= buf */
        if (end < buf) {
                end = ((void *)-1);
                size = (xwsz_t)(end - buf);
        }

        while (*fmt) {
                const char * old_fmt = fmt;
                int read = xwlib_xwvspf_fmt_decode((const unsigned char *)fmt, &spec);

                fmt += read;

                switch (spec.type) {
                case XWLIB_SOVSPF_FT_NONE: {
                        int copy = read;
                        if (str < end) {
                                if (copy > end - str)
                                        copy = end - str;
                                memcpy(str, old_fmt, (xwsz_t)copy);
                        }
                        str += read;
                        break;
                }

                case XWLIB_SOVSPF_FT_WIDTH:
                        spec.field_width = (xws16_t)va_arg(args, int);
                        break;

                case XWLIB_SOVSPF_FT_PRECISION:
                        spec.precision = (xws16_t)va_arg(args, int);
                        break;

                case XWLIB_SOVSPF_FT_CHAR: {
                        char c;

                        if (!(spec.flags & XWLIB_SOVSPF_F_LEFT)) {
                                while (--spec.field_width > 0) {
                                        if (str < end) {
                                                *str = ' ';
                                        }
                                        ++str;
                                }
                        }
                        c = (unsigned char)va_arg(args, int);
                        if (str < end) {
                                *str = c;
                        }
                        ++str;
                        while (--spec.field_width > 0) {
                                if (str < end) {
                                        *str = ' ';
                                }
                                ++str;
                        }
                        break;
                }

                case XWLIB_SOVSPF_FT_STR:
                        str = xwlib_xwvspf_fmt_string(str, end, va_arg(args, char *),
                                                      spec);
                        break;

                case XWLIB_SOVSPF_FT_PTR:
                        str = xwlib_xwvspf_fmt_pointer(fmt+1, str, end,
                                                       va_arg(args, void *), spec);
                        while (isalnum(*((const unsigned char *)fmt))) {
                                fmt++;
                        }
                        break;

                case XWLIB_SOVSPF_FT_PERCENT_CHAR:
                        if (str < end) {
                                *str = '%';
                        }
                        ++str;
                        break;

                case XWLIB_SOVSPF_FT_INVALID:
                        if (str < end) {
                                *str = '%';
                        }
                        ++str;
                        break;

                default:
                        switch (spec.type) {
                        case XWLIB_SOVSPF_FT_LONG_LONG:
                                num = (xwu64_t)va_arg(args, long long);
                                break;
                        case XWLIB_SOVSPF_FT_ULONG:
                                num = (xwu64_t)va_arg(args, unsigned long);
                                break;
                        case XWLIB_SOVSPF_FT_LONG:
                                num = (xwu64_t)va_arg(args, long);
                                break;
                        case XWLIB_SOVSPF_FT_XWSZ_T:
                                if (spec.flags & XWLIB_SOVSPF_F_SIGN) {
                                        num = (xwu64_t)va_arg(args, xwssz_t);
                                } else {
                                        num = (xwu64_t)va_arg(args, xwsz_t);
                                }
                                break;
                        case XWLIB_SOVSPF_FT_PTRDIFF:
                                num = (xwu64_t)va_arg(args, ptrdiff_t);
                                break;
                        case XWLIB_SOVSPF_FT_UBYTE:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case XWLIB_SOVSPF_FT_BYTE:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case XWLIB_SOVSPF_FT_USHORT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case XWLIB_SOVSPF_FT_SHORT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case XWLIB_SOVSPF_FT_INT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        default:
                                num = (xwu64_t)va_arg(args, unsigned int);
                        }
                        str = xwlib_xwvspf_fmt_number(str, end, num, spec);
                }
        }

        if (size > 0) {
                if (str < end) {
                        *str = '\0';
                } else {
                        end[-1] = '\0';
                }
        }

        return str - buf;
}

/**
 * @brief 同snprintf()函数
 */
__xwlib_code
int xwsdnpf(char * buf, xwsz_t size, const char * fmt, ...)
{
        va_list args;
        int i;

        va_start(args, fmt);
        i = xwvsnpf(buf, size, fmt, args);
        va_end(args);

        return i;
}

/**
 * @brief 同vsprintf()函数
 */
__xwlib_code
int xwvspf(char * buf, const char * fmt, va_list args)
{
        return xwvsnpf(buf, INT_MAX, fmt, args);
}

/**
 * @brief 同sprintf()函数
 */
__xwlib_code
int xwspf(char * buf, const char * fmt, ...)
{
        va_list args;
        int i;

        va_start(args, fmt);
        i = xwvspf(buf, fmt, args);
        va_end(args);
        return i;
}
