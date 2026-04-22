/**
 * @file
 * @brief XWOS通用库：格式化字符串
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <math.h>
#include <float.h>
#include <xwos/lib/div64.h>

#define XWVSNPF_F_ZEROPAD          0x01    /**< pad with zero */
#define XWVSNPF_F_SIGN             0x02    /**< unsigned/signed long */
#define XWVSNPF_F_PLUS             0x04    /**< show plus */
#define XWVSNPF_F_SPACE            0x08    /**< space if plus */
#define XWVSNPF_F_LEFT             0x10    /**< left justified */
#define XWVSNPF_F_SMALL            0x20    /**< Must be 32 == 0x20 */
#define XWVSNPF_F_SPECIAL          0x40    /**< leading flag:0x */

enum xwvsnpf_format_type_em {
        XWVSNPF_FT_NONE, /* a string part */
        XWVSNPF_FT_WIDTH,
        XWVSNPF_FT_PRECISION,
        XWVSNPF_FT_CHAR,
        XWVSNPF_FT_STR,
        XWVSNPF_FT_PTR,
        XWVSNPF_FT_PERCENT_CHAR,
        XWVSNPF_FT_INVALID,
        XWVSNPF_FT_LONG_LONG,
        XWVSNPF_FT_ULONG,
        XWVSNPF_FT_LONG,
        XWVSNPF_FT_UBYTE,
        XWVSNPF_FT_BYTE,
        XWVSNPF_FT_USHORT,
        XWVSNPF_FT_SHORT,
        XWVSNPF_FT_UINT,
        XWVSNPF_FT_INT,
        XWVSNPF_FT_XWSZ_T,
        XWVSNPF_FT_PTRDIFF,
        XWVSNPF_FT_FLOAT,
        XWVSNPF_FT_FLOAT_SCI,
};

struct xwvsnpf_format_spec {
        xwu8_t type; /* format_type enum */
        xwu8_t flags; /* flags to number() */
        xwu8_t base; /* number base, 8, 10 or 16 only */
        xwu8_t qualifier; /* number qualifier, one of 'hHlLtzZ' */
        xws16_t field_width; /* width of output field */
        xws16_t precision; /* # of digits/chars */
};

static const char xwvsnpf_digits[] = "0123456789ABCDEF";
static const char xwvsnpf_nullstr[] = "(null)";

static inline
int xwvsnpf_skip_atoi(const char ** s)
{
        int i = 0;

        while (isdigit((int)(**s))) {
                i = i * 10 + **s - '0';
                (*s)++;
        }
        return i;
}

static inline
char * xwvsnpf_put_dec_trunc(char * buf, unsigned int q)
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
char * xwvsnpf_put_dec_full(char * buf, unsigned int q)
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
char * xwvsnpf_put_dec(char * buf, unsigned long long num)
{
        while (true) {
                unsigned int rem;
                if (num < 100000) {
                        return xwvsnpf_put_dec_trunc(buf, (unsigned int)num);
                }
                rem = xwdiv64((xwu64_t *)&num, 100000);
                buf = xwvsnpf_put_dec_full(buf, rem);
        }
}

static inline
char * xwvsnpf_format_number(char * buf, char * end,
                             xwu64_t num,
                             struct xwvsnpf_format_spec spec)
{
        char tmp[66];
        char sign;
        char locase;
        int need_pfx = ((spec.flags & XWVSNPF_F_SPECIAL) && spec.base != 10);
        int i;
        bool is_zero = num == 0LL;

        locase = (char)(spec.flags & XWVSNPF_F_SMALL);
        if (spec.flags & XWVSNPF_F_LEFT) {
                spec.flags &= (xwu8_t)(~XWVSNPF_F_ZEROPAD);
        }
        sign = 0;
        if (spec.flags & XWVSNPF_F_SIGN) {
                if ((signed long long)num < 0) {
                        sign = '-';
                        num = (unsigned long long)(-(signed long long)num);
                        spec.field_width--;
                } else if (spec.flags & XWVSNPF_F_PLUS) {
                        sign = '+';
                        spec.field_width--;
                } else if (spec.flags & XWVSNPF_F_SPACE) {
                        sign = ' ';
                        spec.field_width--;
                } else {}
        }
        if (need_pfx) {
                if (16 == spec.base || 2 == spec.base) {
                        spec.field_width -= 2;
                } else if (!is_zero) {
                        spec.field_width--;
                } else {}
        }

        /* generate full string in tmp[], in reverse order */
        i = 0;
        if (num < spec.base) {
                tmp[i++] = xwvsnpf_digits[num] | locase;
                /* Generic code, for any base:
                   } else {
                   do {
                   tmp[i++] = (xwvsnpf_digits[xwdiv64(&num, base)] | locase);
                   } while (num != 0);
                */
        } else if (spec.base != 10) { /* 2, 8 or 16 */
                int mask = spec.base - 1;
                int shift = 3;

                if (spec.base == 16) {
                        shift = 4;
                } else if (spec.base == 2) {
                        shift = 1;
                }
                do {
                        tmp[i++] = (xwvsnpf_digits[((char)num) & mask] |
                                    locase);
                        num >>= shift;
                } while (num);
        } else { /* base 10 */
                i = xwvsnpf_put_dec(tmp, num) - tmp;
        }

        /* printing 100 using %2d gives "100", not "00" */
        if (i > spec.precision) {
                spec.precision = (xws16_t)i;
        }
        /* leading space padding */
        spec.field_width -= spec.precision;
        if (!(spec.flags & (XWVSNPF_F_ZEROPAD + XWVSNPF_F_LEFT))) {
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
        /* "0x" / "0b" / "0" prefix */
        if (need_pfx) {
                if (spec.base == 16 || spec.base == 2 || !is_zero) {
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
                } else if (spec.base == 2) {
                        if (buf < end) {
                                *buf = ('B' | locase);
                        }
                        buf++;
                }
        }
        /* zero or space padding */
        if (!(spec.flags & XWVSNPF_F_LEFT)) {
                char c = (spec.flags & XWVSNPF_F_ZEROPAD) ? '0' : ' ';
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
        /* actual xwvsnpf_digits of result */
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
void xwvsnpf_format_move_right(char * buf, char * end, xwssz_t len, xwssz_t spaces)
{
        xwssz_t size;

        if (buf >= end) {
                goto end;
        }
        size = (xwssz_t)((xwptr_t)end - (xwptr_t)buf);
        if (size <= spaces) {
                memset(buf, ' ', (xwsz_t)size);
                goto end;
        }
        if (len > 0) {
                if (len > size - spaces) {
                        len = size - spaces;
                }
                memmove(buf + spaces, buf, (xwsz_t)len);
        }
        memset(buf, ' ', (xwsz_t)spaces);

end:
}

static inline
char * xwvsnpf_format_widen_string(char * buf, xwssz_t n, char * end,
                                   struct xwvsnpf_format_spec spec)
{
        xwssz_t spaces;

        if (n >= (xwssz_t)spec.field_width) {
                return buf;
        }
        spaces = (xwssz_t)spec.field_width - n;
        if (0 == (spec.flags & XWVSNPF_F_LEFT)) {
                xwvsnpf_format_move_right(buf - n, end, n, spaces);
                return buf + spaces;
        }
        while (spaces > 0) {
                spaces--;
                if (buf < end) {
                        *buf = ' ';
                }
                buf++;
        }
        return buf;
}

static inline
char * xwvsnpf_format_string_nocheck(char * buf, char * end,
                                     const char * s,
                                     struct xwvsnpf_format_spec spec)
{
        xwssz_t len = 0;
        xwssz_t limit = spec.precision;

        while (limit) {
                char c = *s;
                s++;
                if (0 == c) {
                        break;
                }
                if (buf < end) {
                        *buf = c;
                }
                buf++;
                len++;
                limit--;
        }
        return xwvsnpf_format_widen_string(buf, len, end, spec);
}

static inline
char * xwvsnpf_format_string(char * buf, char * end, const char * s,
                             struct xwvsnpf_format_spec spec)
{

        if (NULL == s) {
                s = xwvsnpf_nullstr;
                if (spec.precision == -1) {
                        spec.precision = 2 * sizeof(void *);
                }
        }
        return xwvsnpf_format_string_nocheck(buf, end, s, spec);
}

static inline
char * xwvsnpf_format_pointer(const char * fmt, char * buf, char * end, void * ptr,
                              struct xwvsnpf_format_spec spec)
{
        int default_width;

        (void)fmt;
        default_width = (int)(2 * sizeof(void *) +
                              (spec.flags & XWVSNPF_F_SPECIAL ? 2 : 0));
        spec.flags |= XWVSNPF_F_SMALL;
        if (spec.field_width == -1) {
                spec.field_width = (xws16_t)default_width;
                spec.flags |= XWVSNPF_F_ZEROPAD;
        }
        spec.base = 16;

        return xwvsnpf_format_number(buf, end, (xwptr_t)ptr, spec);
}

static inline
char * xwvsnpf_put_float_decimal(char * buf, char * end, unsigned long long num, int digits)
{
        char tmp[30];
        int i = 0;
        int j;

        if (num == 0) {
                tmp[i++] = '0';
        } else {
                while (num > 0 && i < 29) {
                        tmp[i++] = (char)('0' + (num % 10));
                        num = num / 10;
                }
        }

        while (i < digits && i < 29) {
                tmp[i++] = '0';
        }

        for (j = i - 1; j >= 0; j--) {
                if (buf < end) {
                        *buf = tmp[j];
                }
                buf++;
        }
        return buf;
}

static inline
char * xwvsnpf_format_float(char * buf, char * end, double num,
                            struct xwvsnpf_format_spec spec)
{
        char tmp[100];
        char *p = tmp;
        char sign = 0;
        int precision = (spec.precision == -1) ? 6 : spec.precision;
        int is_sci = (spec.type == XWVSNPF_FT_FLOAT_SCI);
        int exp = 0;
        unsigned long long int_part = 0;
        unsigned long long frac_part = 0;
        double abs_num;
        int i, len;
        int need_sign = 0;
        char exp_char = (spec.flags & XWVSNPF_F_SMALL) ? 'e' : 'E';

        if (isnan(num)) {
                if (buf < end) *buf++ = 'n';
                if (buf < end) *buf++ = 'a';
                if (buf < end) *buf++ = 'n';
                return buf;
        }

        if (isinf(num)) {
                if (num < 0) {
                        if (buf < end) *buf++ = '-';
                }
                if (buf < end) *buf++ = 'i';
                if (buf < end) *buf++ = 'n';
                if (buf < end) *buf++ = 'f';
                return buf;
        }

        if (num < 0) {
                sign = '-';
                abs_num = -num;
        } else {
                if (spec.flags & XWVSNPF_F_PLUS) {
                        sign = '+';
                } else if (spec.flags & XWVSNPF_F_SPACE) {
                        sign = ' ';
                }
                abs_num = num;
        }

        if (sign) {
                need_sign = 1;
        }

        if (is_sci) {
                if (abs_num == 0.0) {
                        exp = 0;
                } else if (abs_num >= 1.0) {
                        while (abs_num >= 10.0) {
                                abs_num /= 10.0;
                                exp++;
                        }
                } else {
                        while (abs_num < 1.0) {
                                abs_num *= 10.0;
                                exp--;
                        }
                }
        }

        int_part = (unsigned long long)abs_num;
        double frac = abs_num - (double)int_part;
        double mult = 1.0;
        for (i = 0; i < precision; i++) {
                mult *= 10.0;
        }
        frac_part = (unsigned long long)(frac * mult + 0.5);

        if (frac_part >= (unsigned long long)mult) {
                frac_part -= (unsigned long long)mult;
                int_part++;
        }

        p = xwvsnpf_put_float_decimal(p, tmp + 99, int_part, 1);

        if (precision > 0 || (spec.flags & XWVSNPF_F_SPECIAL)) {
                *p++ = '.';
                p = xwvsnpf_put_float_decimal(p, tmp + 99, frac_part, precision);
        }

        if (is_sci) {
                *p++ = exp_char;
                if (exp >= 0) {
                        *p++ = '+';
                } else {
                        *p++ = '-';
                        exp = -exp;
                }
                if (exp < 10) {
                        *p++ = '0';
                }
                p = xwvsnpf_put_float_decimal(p, tmp + 99, (unsigned long long)exp, 1);
        }

        len = (int)(p - tmp);
        int total_width = (spec.field_width != -1) ? spec.field_width : 0;
        int pad_len = (total_width > len + need_sign) ? (total_width - len - need_sign) : 0;

        if (!(spec.flags & XWVSNPF_F_LEFT) && pad_len > 0) {
                char pad_char = (spec.flags & XWVSNPF_F_ZEROPAD) ? '0' : ' ';
                if (pad_char == '0' && sign) {
                        if (buf < end) *buf++ = sign;
                        sign = 0;
                        need_sign = 0;
                }
                for (i = 0; i < pad_len; i++) {
                        if (buf < end) *buf++ = pad_char;
                }
        }

        if (sign) {
                if (buf < end) *buf++ = sign;
        }

        for (i = 0; i < len; i++) {
                if (buf < end) *buf++ = tmp[i];
        }

        if ((spec.flags & XWVSNPF_F_LEFT) && pad_len > 0) {
                for (i = 0; i < pad_len; i++) {
                        if (buf < end) *buf++ = ' ';
                }
        }

        return buf;
}

static inline
int xwvsnpf_format_decode(const char * fmt,
                          struct xwvsnpf_format_spec * spec)
{
        const char * start = fmt;

        /* we finished early by reading the field width */
        if (spec->type == XWVSNPF_FT_WIDTH) {
                if (spec->field_width < 0) {
                        spec->field_width = -spec->field_width;
                        spec->flags |= XWVSNPF_F_LEFT;
                }
                spec->type = XWVSNPF_FT_NONE;
                goto precision;
        }

        /* we finished early by reading the precision */
        if (spec->type == XWVSNPF_FT_PRECISION) {
                if (spec->precision < 0) {
                        spec->precision = 0;
                }
                spec->type = XWVSNPF_FT_NONE;
                goto qualifier;
        }

        /* By default */
        spec->type = XWVSNPF_FT_NONE;
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
                bool found = true;
                fmt++;
                switch (*fmt) {
                case '-':
                        spec->flags |= XWVSNPF_F_LEFT;
                        break;
                case '+':
                        spec->flags |= XWVSNPF_F_PLUS;
                        break;
                case ' ':
                        spec->flags |= XWVSNPF_F_SPACE;
                        break;
                case '#':
                        spec->flags |= XWVSNPF_F_SPECIAL;
                        break;
                case '0':
                        spec->flags |= XWVSNPF_F_ZEROPAD;
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
        if (isdigit((int)(*fmt))) {
                spec->field_width = (xws16_t)xwvsnpf_skip_atoi(&fmt);
        } else if ('*' == *fmt) {
                /* The next argument */
                spec->type = XWVSNPF_FT_WIDTH;
                fmt++;
                return fmt - start;
        } else {}

precision:
        /* get the precision */
        spec->precision = -1;
        if ('.' == (*fmt)) {
                fmt++;
                if (isdigit((int)(*fmt))) {
                        spec->precision = (xws16_t)xwvsnpf_skip_atoi(&fmt);
                        if (spec->precision < 0) {
                                spec->precision = 0;
                        }
                } else if (*fmt == '*') {
                        /* it's the next argument */
                        spec->type = XWVSNPF_FT_PRECISION;
                        fmt++;
                        return fmt - start;
                } else {}
        }

qualifier:
        /* get the conversion qualifier */
        spec->qualifier = 0;
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
                spec->type = XWVSNPF_FT_CHAR;
                fmt++;
                return fmt - start;

        case 's':
                spec->type = XWVSNPF_FT_STR;
                fmt++;
                return fmt - start;

        case 'p':
                spec->type = XWVSNPF_FT_PTR;
                return fmt - start;
                /* skip alnum */

        case '%':
                spec->type = XWVSNPF_FT_PERCENT_CHAR;
                fmt++;
                return fmt - start;

        case 'o':
                spec->base = 8;
                break;

        case 'b':
                spec->flags |= XWVSNPF_F_SMALL;
                spec->base = 2;
                break;

        case 'B':
                spec->base = 2;
                break;

        case 'x':
                spec->flags |= XWVSNPF_F_SMALL;
                spec->base = 16;
                break;

        case 'X':
                spec->base = 16;
                break;

        case 'f':
                spec->type = XWVSNPF_FT_FLOAT;
                fmt++;
                return fmt - start;

        case 'e':
                spec->flags |= XWVSNPF_F_SMALL;
                spec->type = XWVSNPF_FT_FLOAT_SCI;
                fmt++;
                return fmt - start;

        case 'E':
                spec->type = XWVSNPF_FT_FLOAT_SCI;
                fmt++;
                return fmt - start;

        case 'd':
        case 'i':
                spec->flags |= XWVSNPF_F_SIGN;
        case 'u':
                break;

        default:
                spec->type = XWVSNPF_FT_INVALID;
                return fmt - start;
        }

        if ('L' == spec->qualifier) {
                spec->type = XWVSNPF_FT_LONG_LONG;
        } else if ('l' == spec->qualifier) {
                if (spec->flags & XWVSNPF_F_SIGN) {
                        spec->type = XWVSNPF_FT_LONG;
                } else {
                        spec->type = XWVSNPF_FT_ULONG;
                }
        } else if ('z' == tolower(spec->qualifier)) {
                spec->type = XWVSNPF_FT_XWSZ_T;
        } else if ('t' == spec->qualifier) {
                spec->type = XWVSNPF_FT_PTRDIFF;
        } else if ('H' == spec->qualifier) {
                if (spec->flags & XWVSNPF_F_SIGN) {
                        spec->type = XWVSNPF_FT_BYTE;
                } else {
                        spec->type = XWVSNPF_FT_UBYTE;
                }
        } else if ('h' == spec->qualifier) {
                if (spec->flags & XWVSNPF_F_SIGN) {
                        spec->type = XWVSNPF_FT_SHORT;
                } else {
                        spec->type = XWVSNPF_FT_USHORT;
                }
        } else {
                if (spec->flags & XWVSNPF_F_SIGN) {
                        spec->type = XWVSNPF_FT_INT;
                } else {
                        spec->type = XWVSNPF_FT_UINT;
                }
        }
        fmt++;
        return fmt - start;
}

int xwvsnpf(char * buf, xwsz_t size, const char * fmt, va_list args)
{
        int rc;
        xwu64_t num;
        char * str, * end;
        struct xwvsnpf_format_spec spec = {0};

        if (size > XWSSZ_MAX) {
                /* Reject out-of-range values early. */
                rc = 0;
                goto err_oor;
        }

        str = buf;
        end = buf + size;

        /* Make sure end is always >= buf */
        if (end < buf) {
                end = ((void *)-1);
                size = (xwsz_t)(end - buf);
        }

        while (*fmt) {
                const char * oldfmt = fmt;
                int read = xwvsnpf_format_decode(fmt, &spec);
                fmt += read;
                switch (spec.type) {
                case XWVSNPF_FT_NONE: {
                        int copy = read;
                        if (str < end) {
                                if (copy > end - str) {
                                        copy = end - str;
                                }
                                memcpy(str, oldfmt, (xwsz_t)copy);
                        }
                        str += read;
                        break;
                }

                case XWVSNPF_FT_WIDTH:
                        spec.field_width = (xws16_t)va_arg(args, int);
                        break;

                case XWVSNPF_FT_PRECISION:
                        spec.precision = (xws16_t)va_arg(args, int);
                        break;

                case XWVSNPF_FT_CHAR: {
                        char c;

                        if (!(spec.flags & XWVSNPF_F_LEFT)) {
                                while (--spec.field_width > 0) {
                                        if (str < end) {
                                                *str = ' ';
                                        }
                                        ++str;
                                }
                        }
                        c = (char)va_arg(args, int);
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

                case XWVSNPF_FT_STR:
                        str = xwvsnpf_format_string(str, end, va_arg(args, char *),
                                                    spec);
                        break;

                case XWVSNPF_FT_PTR:
                        str = xwvsnpf_format_pointer(fmt+1, str, end,
                                                     va_arg(args, void *), spec);
                        while (isalnum((int)(*fmt))) {
                                fmt++;
                        }
                        break;

                case XWVSNPF_FT_PERCENT_CHAR:
                        if (str < end) {
                                *str = '%';
                        }
                        ++str;
                        break;

                case XWVSNPF_FT_INVALID:
                        if (str < end) {
                                *str = '%';
                        }
                        ++str;
                        break;

                case XWVSNPF_FT_FLOAT:
                case XWVSNPF_FT_FLOAT_SCI:
                        str = xwvsnpf_format_float(str, end, va_arg(args, double), spec);
                        break;

                default:
                        switch (spec.type) {
                        case XWVSNPF_FT_LONG_LONG:
                                num = (xwu64_t)va_arg(args, long long);
                                break;
                        case XWVSNPF_FT_ULONG:
                                num = (xwu64_t)va_arg(args, unsigned long);
                                break;
                        case XWVSNPF_FT_LONG:
                                num = (xwu64_t)va_arg(args, long);
                                break;
                        case XWVSNPF_FT_XWSZ_T:
                                if (spec.flags & XWVSNPF_F_SIGN) {
                                        num = (xwu64_t)va_arg(args, xwssz_t);
                                } else {
                                        num = (xwu64_t)va_arg(args, xwsz_t);
                                }
                                break;
                        case XWVSNPF_FT_PTRDIFF:
                                num = (xwu64_t)va_arg(args, ptrdiff_t);
                                break;
                        case XWVSNPF_FT_UBYTE:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case XWVSNPF_FT_BYTE:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case XWVSNPF_FT_USHORT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case XWVSNPF_FT_SHORT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        case XWVSNPF_FT_INT:
                                num = (xwu64_t)va_arg(args, int);
                                break;
                        default:
                                num = (xwu64_t)va_arg(args, unsigned int);
                        }
                        str = xwvsnpf_format_number(str, end, num, spec);
                }
        }

        if (size > 0) {
                if (str < end) {
                        *str = '\0';
                } else {
                        end[-1] = '\0';
                }
        }
        rc = str - buf;
err_oor:
        return rc;
}

int xwsnpf(char * buf, xwsz_t size, const char * fmt, ...)
{
        va_list args;
        int i;

        va_start(args, fmt);
        i = xwvsnpf(buf, size, fmt, args);
        va_end(args);

        return i;
}

int xwvspf(char * buf, const char * fmt, va_list args)
{
        return xwvsnpf(buf, INT_MAX, fmt, args);
}

int xwspf(char * buf, const char * fmt, ...)
{
        va_list args;
        int i;

        va_start(args, fmt);
        i = xwvspf(buf, fmt, args);
        va_end(args);
        return i;
}
