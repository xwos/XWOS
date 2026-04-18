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

#ifndef Component_Xwsh_datalib_h__
#define Component_Xwsh_datalib_h__

#include <xwos/standard.h>

/**
 * @brief 解析带转义字符的数据字符串
 * @param[in] str: 源字符串
 * @param[out] buf: 目标缓冲区
 * @param[in] bufsz: 目标缓冲区大小
 * @param[out] len: 返回解析后的长度
 * @return 错误码
 * @retval XWOK: 解析成功
 * @retval -EINVAL: 解析失败
 * @detials
 * + 转义字符
 *   + `\\` : backslash
 *   + `\a` : alert (BEL)
 *   + `\b` : backspace
 *   + `\e` : escape
 *   + `\f` : form feed
 *   + `\n` : new line
 *   + `\r` : carriage return
 *   + `\t` : horizontal tab
 *   + `\v` : vertical tab
 *   + `\'` : single quote
 *   + `\"` : double quote
 *   + `\0NNN` : 8进制数据，固定3位，不足报错
 *   + `\xHH` 或 `\XHH` : 16进制数据，固定2位，不足报错
 *   + `\bYYYYYYYY` 或 `\BYYYYYYYY` : 2进制数据，固定8位，不足报错
 * + 示例： "\X1F\0000\b11111110012\e\t" 表示 "0x1F 0x0 0xFE 0x30 0x31 0x32 0x1B 0x9"
 *   数据序列，长度8，不包括前后单双引号。
 */
xwer_t xwsh_data_parse_string(char * str, xwu8_t * buf, xwsz_t bufsz, xwsz_t * len);

/**
 * @brief 打印数据
 * @param[in] data: 数据
 * @param[in] size: 数据大小
 */
void xwsh_data_dump(const xwu8_t * data, xwsz_t size);

#endif /* Component/Xwsh/datalib.h */
