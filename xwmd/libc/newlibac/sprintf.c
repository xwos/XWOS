/**
 * @file
 * @brief newlib适配层：格式化字符串
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <ctype.h>
#include <xwos/lib/div64.h>

void newlibac_sprintf_linkage_stub(void)
{
}

#define VSNPRINTF_F_ZEROPAD          0x01    /**< pad with zero */
#define VSNPRINTF_F_SIGN             0x02    /**< unsigned/signed long */
#define VSNPRINTF_F_PLUS             0x04    /**< show plus */
#define VSNPRINTF_F_SPACE            0x08    /**< space if plus */
#define VSNPRINTF_F_LEFT             0x10    /**< left justified */
#define VSNPRINTF_F_SMALL            0x20    /**< Must be 32 == 0x20 */
#define VSNPRINTF_F_SPECIAL          0x40    /**< leading flag:0x */

enum vsnprintf_format_type_em {
        VSNPRINTF_FT_NONE, /* a string part */
        VSNPRINTF_FT_WIDTH,
        VSNPRINTF_FT_PRECISION,
        VSNPRINTF_FT_CHAR,
        VSNPRINTF_FT_STR,
        VSNPRINTF_FT_PTR,
        VSNPRINTF_FT_PERCENT_CHAR,
        VSNPRINTF_FT_INVALID,
        VSNPRINTF_FT_LONG_LONG,
        VSNPRINTF_FT_ULONG,
        VSNPRINTF_FT_LONG,
        VSNPRINTF_FT_UBYTE,
        VSNPRINTF_FT_BYTE,
        VSNPRINTF_FT_USHORT,
        VSNPRINTF_FT_SHORT,
        VSNPRINTF_FT_UINT,
        VSNPRINTF_FT_INT,
        VSNPRINTF_FT_XWSZ_T,
        VSNPRINTF_FT_PTRDIFF,
};

struct vsnprintf_format_spec {
        xwu8_t type; /* format_type enum */
        xwu8_t flags; /* flags to number() */
        xwu8_t base; /* number base, 8, 10 or 16 only */
        xwu8_t qualifier; /* number qualifier, one of 'hHlLtzZ' */
        xws16_t field_width; /* width of output field */
        xws16_t precision; /* # of digits/chars */
};

static const char vsnprintf_digits[] = "0123456789ABCDEF";
static const char vsnprintf_nullstr[] = "(null)";

static inline
int vsnprintf_skip_atoi(const unsigned char ** s)
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

static inline
char * vsnprintf_put_dec_trunc(char * buf, unsigned int q)
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

static inline
char * vsnprintf_put_dec_full(char * buf, unsigned int q)
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

static inline
char * vsnprintf_put_dec(char * buf, unsigned long long num)
{
        while (true) {
                unsigned int rem;
                if (num < 100000) {
                        return vsnprintf_put_dec_trunc(buf, (unsigned int)num);
                }
                rem = xwdiv64(&num, 100000);
                buf = vsnprintf_put_dec_full(buf, rem);
        }
}

static inline
char * vsnprintf_format_number(char * buf, char * end,
                               xwu64_t num,
                               struct vsnprintf_format_spec spec)
{
        char tmp[66];
        char sign;
        char locase;
        int need_pfx = ((spec.flags & VSNPRINTF_F_SPECIAL) && spec.base != 10);
        int i;
        bool is_zero = num == 0LL;

        locase = (char)(spec.flags & VSNPRINTF_F_SMALL);
        if (spec.flags & VSNPRINTF_F_LEFT) {
                spec.flags &= ~VSNPRINTF_F_ZEROPAD;
        }/* else {} */
        sign = 0;
        if (spec.flags & VSNPRINTF_F_SIGN) {
                if ((signed long long)num < 0) {
                        sign = '-';
                        num = (unsigned long long)(-(signed long long)num);
                        spec.field_width--;
                } else if (spec.flags & VSNPRINTF_F_PLUS) {
                        sign = '+';
                        spec.field_width--;
                } else if (spec.flags & VSNPRINTF_F_SPACE) {
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
                tmp[i++] = vsnprintf_digits[num] | locase;
        /* Generic code, for any base:
        } else {
                do {
                        tmp[i++] = (vsnprintf_digits[xwdiv64(&num, base)] | locase);
                } while (num != 0);
        */
        } else if (spec.base != 10) { /* 8 or 16 */
                int mask = spec.base - 1;
                int shift = 3;

                if (spec.base == 16) {
                        shift = 4;
                }
                do {
                        tmp[i++] = (vsnprintf_digits[((unsigned char)num) & mask] |
                                    locase);
                        num >>= shift;
                } while (num);
        } else { /* base 10 */
                i = vsnprintf_put_dec(tmp, num) - tmp;
        }

        /* printing 100 using %2d gives "100", not "00" */
        if (i > spec.precision)
                spec.precision = (xws16_t)i;
        /* leading space padding */
        spec.field_width -= spec.precision;
        if (!(spec.flags & (VSNPRINTF_F_ZEROPAD + VSNPRINTF_F_LEFT))) {
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
        if (!(spec.flags & VSNPRINTF_F_LEFT)) {
                char c = (spec.flags & VSNPRINTF_F_ZEROPAD) ? '0' : ' ';
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
        /* actual vsnprintf_digits of result */
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

static inline
char * vsnprintf_format_string(char * buf, char * end, const char * s,
                               struct vsnprintf_format_spec spec)
{
        xwssz_t len, i;

        if ((unsigned long)s <= MAX_ERRNO) {
                s = vsnprintf_nullstr;
        }

        len = (xwssz_t)strlen(s);
        if (len > spec.precision) {
                len = spec.precision;
        }

        if (!(spec.flags & VSNPRINTF_F_LEFT)) {
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

static inline
char * vsnprintf_format_pointer(const char * fmt, char * buf, char * end, void * ptr,
                                struct vsnprintf_format_spec spec)
{
        int default_width;

        (void)fmt;
        default_width = (int)(2 * sizeof(void *) +
                              (spec.flags & VSNPRINTF_F_SPECIAL ? 2 : 0));
        spec.flags |= VSNPRINTF_F_SMALL;
        if (spec.field_width == -1) {
                spec.field_width = (xws16_t)default_width;
                spec.flags |= VSNPRINTF_F_ZEROPAD;
        }
        spec.base = 16;

        return vsnprintf_format_number(buf, end, (xwptr_t)ptr, spec);
}

static inline
int vsnprintf_format_decode(const unsigned char * fmt,
                            struct vsnprintf_format_spec * spec)
{
        const unsigned char * start = fmt;

        /* we finished early by reading the field width */
        if (spec->type == VSNPRINTF_FT_WIDTH) {
                if (spec->field_width < 0) {
                        spec->field_width = -spec->field_width;
                        spec->flags |= VSNPRINTF_F_LEFT;
                }
                spec->type = VSNPRINTF_FT_NONE;
                goto precision;
        }

        /* we finished early by reading the precision */
        if (spec->type == VSNPRINTF_FT_PRECISION) {
                if (spec->precision < 0)
                        spec->precision = 0;

                spec->type = VSNPRINTF_FT_NONE;
                goto qualifier;
        }

        /* By default */
        spec->type = VSNPRINTF_FT_NONE;

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
                        spec->flags |= VSNPRINTF_F_LEFT;
                        break;
                case '+':
                        spec->flags |= VSNPRINTF_F_PLUS;
                        break;
                case ' ':
                        spec->flags |= VSNPRINTF_F_SPACE;
                        break;
                case '#':
                        spec->flags |= VSNPRINTF_F_SPECIAL;
                        break;
                case '0':
                        spec->flags |= VSNPRINTF_F_ZEROPAD;
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
                spec->field_width = (xws16_t)vsnprintf_skip_atoi(&fmt);
        } else if ('*' == *fmt) {
                /* The next argument */
                spec->type = VSNPRINTF_FT_WIDTH;
                fmt++;
                return fmt - start;
        }/* else {} */

precision:
        /* get the precision */
        spec->precision = -1;
        if ('.' == (*fmt)) {
                fmt++ ;
                if (isdigit(*fmt)) {
                        spec->precision = (xws16_t)vsnprintf_skip_atoi(&fmt);
                        if (spec->precision < 0) {
                                spec->precision = 0;
                        }/* else {} */
                } else if (*fmt == '*') {
                        /* it's the next argument */
                        spec->type = VSNPRINTF_FT_PRECISION;
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
                if (spec->qualifier == *fmt) {
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
                spec->type = VSNPRINTF_FT_CHAR;
                fmt++;
                return fmt - start;

        case 's':
                spec->type = VSNPRINTF_FT_STR;
                fmt++;
                return fmt - start;

        case 'p':
                spec->type = VSNPRINTF_FT_PTR;
                return fmt - start;
                /* skip alnum */

        case '%':
                spec->type = VSNPRINTF_FT_PERCENT_CHAR;
                fmt++;
                return fmt - start;

        case 'o':
                spec->base = 8;
                break;

        case 'x':
                spec->flags |= VSNPRINTF_F_SMALL;
                spec->base = 16;
                break;

        case 'X':
                spec->base = 16;
                break;

        case 'd':
        case 'i':
                spec->flags |= VSNPRINTF_F_SIGN;
        case 'u':
                break;

        default:
                spec->type = VSNPRINTF_FT_INVALID;
                return fmt - start;
        }

        if ('L' == spec->qualifier) {
                spec->type = VSNPRINTF_FT_LONG_LONG;
        }
        else if ('l' == spec->qualifier) {
                if (spec->flags & VSNPRINTF_F_SIGN) {
                        spec->type = VSNPRINTF_FT_LONG;
                } else {
                        spec->type = VSNPRINTF_FT_ULONG;
                }
        } else if ('z' == tolower(spec->qualifier)) {
                spec->type = VSNPRINTF_FT_XWSZ_T;
        } else if ('t' == spec->qualifier) {
                spec->type = VSNPRINTF_FT_PTRDIFF;
        } else if ('H' == spec->qualifier) {
                if (spec->flags & VSNPRINTF_F_SIGN) {
                        spec->type = VSNPRINTF_FT_BYTE;
                } else {
                        spec->type = VSNPRINTF_FT_UBYTE;
                }
        } else if ('h' == spec->qualifier) {
                if (spec->flags & VSNPRINTF_F_SIGN) {
                        spec->type = VSNPRINTF_FT_SHORT;
                } else {
                        spec->type = VSNPRINTF_FT_USHORT;
                }
        } else {
                if (spec->flags & VSNPRINTF_F_SIGN) {
                        spec->type = VSNPRINTF_FT_INT;
                } else {
                        spec->type = VSNPRINTF_FT_UINT;
                }
        }

        fmt++;
        return fmt - start;
}

int vsnprintf(char * buf, xwsz_t size, const char * fmt, va_list args)
{
        xwu64_t num;
        char * str, * end;
        struct vsnprintf_format_spec spec = {0};

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
                int read = vsnprintf_format_decode((const unsigned char *)fmt, &spec);

                fmt += read;

                switch (spec.type) {
                case VSNPRINTF_FT_NONE: {
                        int copy = read;
                        if (str < end) {
                                if (copy > end - str)
                                        copy = end - str;
                                memcpy(str, old_fmt, (xwsz_t)copy);
                        }
                        str += read;
                        break;
                }

                case VSNPRINTF_FT_WIDTH:
                        spec.field_width = (xws16_t)va_arg(args, int);
                        break;

                case VSNPRINTF_FT_PRECISION:
                        spec.precision = (xws16_t)va_arg(args, int);
                        break;

                case VSNPRINTF_FT_CHAR: {
                        char c;

                        if (!(spec.flags & VSNPRINTF_F_LEFT)) {
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

                case VSNPRINTF_FT_STR:
                        str = vsnprintf_format_string(str, end, va_arg(args, char *),
                                                      spec);
                        break;

                case VSNPRINTF_FT_PTR:
                        str = vsnprintf_format_pointer(fmt+1, str, end,
                                                       va_arg(args, void *), spec);
                        while (isalnum(*((const unsigned char *)fmt))) {
                                fmt++;
                        }
                        break;

                case VSNPRINTF_FT_PERCENT_CHAR:
                        if (str < end) {
                                *str = '%';
                        }
                        ++str;
                        break;

                case VSNPRINTF_FT_INVALID:
                        if (str < end) {
                                *str = '%';
                        }
                        ++str;
                        break;

                default:
                        switch (spec.type) {
                        case VSNPRINTF_FT_LONG_LONG:
                                num = (xwu64_t)va_arg(args, long long);
                                break;
                        case VSNPRINTF_FT_ULONG:
                                num = (xwu64_t)va_arg(args, unsigned long);
                                break;
                        case VSNPRINTF_FT_LONG:
                                num = (xwu64_t)va_arg(args, long);
                                break;
                        case VSNPRINTF_FT_XWSZ_T:
                                if (spec.flags & VSNPRINTF_F_SIGN) {
                                        num = (xwu64_t)va_arg(args, xwssz_t);
                                } else {
                                        num = (xwu64_t)va_arg(args, xwsz_t);
                                }
                                break;
                        case VSNPRINTF_FT_PTRDIFF:
                                num = (xwu64_t)va_arg(args, ptrdiff_t);
                                break;
                        case VSNPRINTF_FT_UBYTE:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case VSNPRINTF_FT_BYTE:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case VSNPRINTF_FT_USHORT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case VSNPRINTF_FT_SHORT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case VSNPRINTF_FT_INT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        default:
                                num = (xwu64_t)va_arg(args, unsigned int);
                        }
                        str = vsnprintf_format_number(str, end, num, spec);
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

int snprintf(char * buf, xwsz_t size, const char * fmt, ...)
{
        va_list args;
        int i;

        va_start(args, fmt);
        i = vsnprintf(buf, size, fmt, args);
        va_end(args);

        return i;
}

int vsprintf(char * buf, const char * fmt, va_list args)
{
        return vsnprintf(buf, INT_MAX, fmt, args);
}

int sprintf(char * buf, const char * fmt, ...)
{
        va_list args;
        int i;

        va_start(args, fmt);
        i = vsprintf(buf, fmt, args);
        va_end(args);
        return i;
}
