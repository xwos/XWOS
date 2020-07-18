/**
 * @file
 * @brief newlib适配代码：string
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void newlibac_string_linkage_placeholder(void)
{
}

void * memset(void * src, int c, xwsz_t count)
{
        char * m = src;

        while (count--) {
                *m++ = (char)c;
        }
        return src;
}

void * memcpy(void * restrict dst, const void * restrict src, xwsz_t count)
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

void * memmove(void * dst, const void * src, xwsz_t count)
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
