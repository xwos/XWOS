/**
 * @file
 * @brief newlib适配层：格式化字符串
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
#include <xwos/lib/xwspf.h>

void newlibac_sprintf_linkage_stub(void)
{
}

int vsnprintf(char * buf, xwsz_t size, const char * fmt, va_list args)
{
        return xwvsnpf(buf, size, fmt, args);
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
        return xwvspf(buf, fmt, args);
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
