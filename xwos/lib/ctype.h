/**
 * @file
 * @brief XWOS通用库：字符类型
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_ctype_h__
#define __xwos_lib_ctype_h__

#if defined(ARCHCFG_COMPILER_CLIB) && (1 == ARCHCFG_COMPILER_CLIB)
#  include <ctype.h>
#else
#  include <xwos/standard.h>

/**
 * @defgroup ctype 字符类型
 * @{
 */

/**
 * @brief 字符类型标签
 */
enum xwlib_ctype_flag_em {
        XWLIB_CTYPE_FLAG_UPPER = 0x01, /**< 大写字母 */
        XWLIB_CTYPE_FLAG_LOWER = 0x02, /**< 小写字母 */
        XWLIB_CTYPE_FLAG_DIGIT = 0x04, /**< 数字字符 */
        XWLIB_CTYPE_FLAG_CTRL = 0x08, /**< 控制字符 */
        XWLIB_CTYPE_FLAG_PUNCT = 0x10, /**< 标点字符 */
        XWLIB_CTYPE_FLAG_WSPC = 0x20, /**< 空白字符(space/lf/tab) */
        XWLIB_CTYPE_FLAG_HEX = 0x40, /**< 十六进制的数字字符 */
        XWLIB_CTYPE_FLAG_SPC = 0x80, /**< 空格0x20 */
};

extern __xwlib_rodata const unsigned char xwlib_ctype[];

/**
 * @brief 查询字符的标签
 */
#  define xwlib_ctype_flag(x) (xwlib_ctype[(int)(unsigned char)(x)])

/**
 * @brief 查询字符是否为字母或数字
 */
#  define isalnum(c)                    \
        ((xwlib_ctype_flag(c) &         \
          (XWLIB_CTYPE_FLAG_UPPER |     \
           XWLIB_CTYPE_FLAG_LOWER |     \
           XWLIB_CTYPE_FLAG_DIGIT)) != 0)

/**
 * @brief 查询字符是否为字母
 */
#  define isalpha(c)                    \
        ((xwlib_ctype_flag(c) &         \
          (XWLIB_CTYPE_FLAG_UPPER |     \
           XWLIB_CTYPE_FLAG_LOWER)) != 0)

/**
 * @brief 查询字符是否为控制字符
 */
#  define iscntrl(c)                    \
        ((xwlib_ctype_flag(c) & (XWLIB_CTYPE_FLAG_CTRL)) != 0)

/**
 * @brief 查询字符是否为数字
 */
#  define isdigit(c)                    \
        ((xwlib_ctype_flag(c) & (XWLIB_CTYPE_FLAG_DIGIT)) != 0)

/**
 * @brief 查询字符是否为可显示字符（不包括空白字符）
 */
#  define isgraph(c)                    \
        ((xwlib_ctype_flag(c) &         \
          (XWLIB_CTYPE_FLAG_PUNCT |     \
           XWLIB_CTYPE_FLAG_UPPER |     \
           XWLIB_CTYPE_FLAG_LOWER |     \
           XWLIB_CTYPE_FLAG_DIGIT)) != 0)

/**
 * @brief 查询字符是否为小写字母
 */
#  define islower(c)                    \
        ((xwlib_ctype_flag(c) & (XWLIB_CTYPE_FLAG_LOWER)) != 0)

/**
 * @brief 查询字符是否为大写字母
 */
#  define isupper(c)                    \
        ((xwlib_ctype_flag(c) & (XWLIB_CTYPE_FLAG_UPPER)) != 0)

/**
 * @brief 查询字符是否为可显示字符（包括空白字符）
 */
#  define isprint(c)                    \
        ((xwlib_ctype_flag(c) &         \
          (XWLIB_CTYPE_FLAG_PUNCT |     \
           XWLIB_CTYPE_FLAG_UPPER |     \
           XWLIB_CTYPE_FLAG_LOWER |     \
           XWLIB_CTYPE_FLAG_DIGIT |     \
           XWLIB_CTYPE_FLAG_SPC)) != 0)

/**
 * @brief 查询字符是否为标点
 */
#  define ispunct(c)                    \
        ((xwlib_ctype_flag(c) & (XWLIB_CTYPE_FLAG_PUNCT)) != 0)

/**
 * @brief 查询字符是否为空白字符
 */
#  define isspace(c)                    \
        ((xwlib_ctype_flag(c) & (XWLIB_CTYPE_FLAG_WSPC)) != 0)

/**
 * @brief 查询字符是否为十六进制数字字符
 */
#  define isxdigit(c)                   \
        ((xwlib_ctype_flag(c) &         \
          (XWLIB_CTYPE_FLAG_DIGIT |     \
           XWLIB_CTYPE_FLAG_HEX)) != 0)

/**
 * @brief 查询字符是否为八进制数字字符
 */
#  define isodigit(c) (((c) >= '0') && ((c) <= '7'))

/**
 * @brief 查询字符是否为ASCII表内字符
 */
#  define isascii(c) (((unsigned char)(c)) <= (unsigned char)0x7f)

/**
 * @brief 将字符转换为ASCII表内字符
 */
#  define toascii(c) (((unsigned char)(c)) & (unsigned char)0x7f)

/**
 * @brief 将大写字母转换为小写字母
 * @param[in] c: 大写字母
 * @return 小写字母
 */
static __xwlib_inline
char tolower(unsigned char c)
{
        if (isupper(c)) {
                c -= ('A'-'a');
        }
        return c;
}

/**
 * @brief 将小写字母转换为大写字母
 * @param[in] c: 小写字母
 * @return 大写字母
 */
static __xwlib_inline
char toupper(unsigned char c)
{
        if (islower(c)) {
                c -= ('a'-'A');
        }
        return c;
}

/**
 * @} ctype
 */

#endif

#endif /* xwos/lib/ctype.h */
