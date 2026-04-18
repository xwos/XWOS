/**
 * @file
 * @brief XWSH: datalib
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
#include <stdio.h>

/**
 * @brief 去除字符串前后的引号
 * @param[in,out] str: 字符串
 * @return 处理后的字符串指针
 */
static
char * xwsh_data_strip_quotes(char * str)
{
        size_t len;

        len = strlen(str);
        if (len >= 2U) {
                if (((str[0] == '"') && (str[len - 1U] == '"')) ||
                    ((str[0] == '\'') && (str[len - 1U] == '\''))) {
                        str[len - 1U] = '\0';
                        str++;
                }
        }
        return str;
}

/**
 * @brief 检查字符是否是八进制数字
 * @param[in] c: 字符
 * @retval true: 是八进制数字
 * @retval false: 不是八进制数字
 */
static
bool xwsh_data_is_oct_digit(char c)
{
        return (c >= '0') && (c <= '7');
}

/**
 * @brief 检查字符是否是二进制数字
 * @param[in] c: 字符
 * @retval true: 是二进制数字
 * @retval false: 不是二进制数字
 */
static
bool xwsh_data_is_bin_digit(char c)
{
        return (c == '0') || (c == '1');
}

/**
 * @brief 解析转义字符序列
 * @param[in] src: 源字符串
 * @param[out] dst: 目标缓冲区
 * @param[in] dst_size: 目标缓冲区大小
 * @param[out] parsed_len: 返回解析后的长度
 * @return 错误码
 * @retval XWOK: 解析成功
 * @retval -EINVAL: 无效的转义序列
 */
static
xwer_t xwsh_data_parse_escape(const char ** src, xwu8_t * dst, xwsz_t dst_size,
                              xwsz_t * parsed_len)
{
        xwer_t rc = XWOK;
        const char * p = *src;
        xwu8_t val;
        xwsz_t i;
        xwu8_t c;
        bool is_binary;

        XWOS_UNUSED(dst_size);
        p++;
        if ((*p == 'b') || (*p == 'B')) {
                is_binary = true;
                for (i = 0U; i < 8U; i++) {
                        if (!xwsh_data_is_bin_digit(p[1U + i])) {
                                is_binary = false;
                                break;
                        }
                }
                if (is_binary) {
                        p++;
                        val = 0U;
                        for (i = 0U; i < 8U; i++) {
                                val = (val << 1U) | (xwu8_t)(*p - '0');
                                p++;
                        }
                        *dst = val;
                        *parsed_len = 1U;
                        *src = p;
                        return XWOK;
                }
        }

        switch (*p) {
        case '\\':
                *dst = '\\';
                p++;
                break;
        case 'a':
                *dst = '\a';
                p++;
                break;
        case 'b':
                *dst = '\b';
                p++;
                break;
        case 'e':
                *dst = '\x1B';
                p++;
                break;
        case 'f':
                *dst = '\f';
                p++;
                break;
        case 'n':
                *dst = '\n';
                p++;
                break;
        case 'r':
                *dst = '\r';
                p++;
                break;
        case 't':
                *dst = '\t';
                p++;
                break;
        case 'v':
                *dst = '\v';
                p++;
                break;
        case '\'':
                *dst = '\'';
                p++;
                break;
        case '"':
                *dst = '"';
                p++;
                break;
        case '0':
                p++;
                val = 0U;
                for (i = 0U; i < 3U; i++) {
                        if (!xwsh_data_is_oct_digit(*p)) {
                                printf("Error: Octal escape requires 3 digits\r\n");
                                rc = -EINVAL;
                                goto err_invalid;
                        }
                        val = (val << 3U) | (xwu8_t)(*p - '0');
                        p++;
                }
                *dst = val;
                break;
        case 'x':
        case 'X':
                p++;
                val = 0U;
                for (i = 0U; i < 2U; i++) {
                        c = (xwu8_t)*p;
                        if (!isxdigit((unsigned char)c)) {
                                printf("Error: Hex escape requires 2 digits\r\n");
                                rc = -EINVAL;
                                goto err_invalid;
                        }
                        val <<= 4U;
                        if (isdigit((unsigned char)c)) {
                                val |= (xwu8_t)(c - '0');
                        } else {
                                val |= (xwu8_t)(tolower((unsigned char)c) - 'a' + 10);
                        }
                        p++;
                }
                *dst = val;
                break;
        default:
                printf("Error: Invalid escape sequence '\\%c'\r\n", *p);
                rc = -EINVAL;
                goto err_invalid;
        }

        *parsed_len = 1U;
        *src = p;
        return XWOK;

err_invalid:
        return rc;
}

xwer_t xwsh_data_parse_string(char * str, xwu8_t * buf, xwsz_t bufsz, xwsz_t * len)
{
        xwer_t rc = XWOK;
        const char * p;
        xwsz_t idx;
        xwsz_t parsed_len;

        p = xwsh_data_strip_quotes(str);
        idx = 0U;
        while (*p != '\0') {
                if (idx >= bufsz) {
                        break;
                }
                if (*p == '\\') {
                        rc = xwsh_data_parse_escape(&p, &buf[idx], bufsz - idx,
                                                    &parsed_len);
                        if (rc < 0) {
                                goto err_parse;
                        }
                        idx += parsed_len;
                } else {
                        buf[idx] = (xwu8_t)*p;
                        idx++;
                        p++;
                }
        }

        *len = idx;
        return XWOK;

err_parse:
        return rc;
}

void xwsh_data_dump(const xwu8_t * data, xwsz_t size)
{
        xwsz_t i;
        xwsz_t j;

        for (i = 0U; i < size; i += 16U) {
                printf("%08x: ", (unsigned int)i);
                for (j = 0U; j < 16U && (i + j) < size; j++) {
                        printf("%02x ", data[i + j]);
                        if (j == 7U) {
                                printf(" ");
                        }
                }
                while (j < 16U) {
                        printf("   ");
                        if (j == 7U) {
                                printf(" ");
                        }
                        j++;
                }
                printf(" |");
                for (j = 0U; j < 16U && (i + j) < size; j++) {
                        if (data[i + j] >= 32U && data[i + j] <= 126U) {
                                printf("%c", data[i + j]);
                        } else {
                                printf(".");
                        }
                }
                printf("|\r\n");
        }
}
