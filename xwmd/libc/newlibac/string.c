/**
 * @file
 * @brief newlib适配层：string
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwmd/libc/newlibac/check.h>

void newlibac_string_linkage_stub(void)
{
}

/**
 * @brief fill memory with a constant byte
 * @param[in] src: the memory area
 * @param[in] c: constant byte
 * @param[in] count: bytes of the memory area
 * @note
 * - newlib中的memset没有对齐访问内存，这会造成ARMv7m产生BUS Fault。因此
 *   重新实现memset覆盖newlib中的函数。
 */
void * memset(void * src, int c, size_t count)
{
        char * m = src;

        while (count--) {
                *m++ = (char)c;
        }
        return src;
}

void * memcpy(void * restrict dst, const void * restrict src, size_t count)
{
        char * tmp = dst;
        const char * s = src;

        while (count--) {
                *tmp = *s;
                tmp++;
                s++;
        }
        return dst;
}

void * memmove(void * dst, const void * src, size_t count)
{
        char * tmp;
        const char * s;

        if (dst <= src) {
                tmp = dst;
                s = src;
                while (count--) {
                        *tmp++ = *s++;
                }
        } else {
                tmp = dst;
                tmp += count;
                s = src;
                s += count;
                while (count--) {
                        *--tmp = *--s;
                }
        }
        return dst;
}
