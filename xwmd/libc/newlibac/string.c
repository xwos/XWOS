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
#include <xwmd/libc/newlibac/linkage.h>
#include <xwmd/libc/newlibac/check.h>

void newlibac_string_linkage_stub(void)
{
}

// cppcheck-suppress [misra-c2012-21.2]
void * memset(void * src, int c, size_t count);
// cppcheck-suppress [misra-c2012-8.14, misra-c2012-21.2]
void * memcpy(void * restrict dst, const void * restrict src, size_t count);

/**
 * @brief fill memory with a constant byte
 * @param[in] src: the memory area
 * @param[in] c: constant byte
 * @param[in] count: bytes of the memory area
 * @note
 * - newlib中ARMv7m的memset没有对齐访问内存，这会造成产生BUS Fault。
 *   因此重新实现memset覆盖newlib中的函数。
 */
void * memset(void * src, int c, size_t count)
{
        union {
                xwu8_t * u8;
                xwu32_t * u32;
                xwptr_t val;
        } m;
        xwu32_t fill32;

        c = c & 0xFF; // cppcheck-suppress [misra-c2012-17.8]
        fill32 = (xwu32_t)c;
        fill32 = (fill32 << 8) | fill32;
        fill32 = (fill32 << 16) | fill32;
        m.u8 = src;
        while ((m.val & (sizeof(void *) - 1U)) && (0U != count)) {
                *m.u8 = (xwu8_t)c;
                m.u8++;
                count--; // cppcheck-suppress [misra-c2012-17.8]
        }
        do {
                if (count < sizeof(void *)) {
                        while (0U != count) {
                                *m.u8 = (xwu8_t)c;
                                m.u8++;
                                count--; // cppcheck-suppress [misra-c2012-17.8]
                        }
                } else {
                        *m.u32 = fill32;
                        m.u32++;
                        count -= sizeof(void *); // cppcheck-suppress [misra-c2012-17.8]
                }
        } while (0U != count);
        return src;
}

// cppcheck-suppress [misra-c2012-8.14]
void * memcpy(void * restrict dst, const void * restrict src, size_t count)
{
        union {
                const xwu8_t * u8;
                const xwu32_t * u32;
                xwptr_t val;
        } s;
        union {
                xwu8_t * u8;
                xwu32_t * u32;
                xwptr_t val;
        } d;

        s.u8 = src;
        d.u8 = dst;

        if ((s.val & (sizeof(void *) - 1U)) ||
            (d.val & (sizeof(void *) - 1U)) ||
            (count < sizeof(void *))) {
                while (0U != count) {
                        *d.u8 = *s.u8;
                        d.u8++;
                        s.u8++;
                        count--; // cppcheck-suppress [misra-c2012-17.8]
                }
        } else {
                do {
                        *d.u32 = *s.u32;
                        d.u32++;
                        s.u32++;
                        count -= sizeof(void *); // cppcheck-suppress [misra-c2012-17.8]
                } while (count >= sizeof(void *));
                while (0U != count) {
                        *d.u8 = *s.u8;
                        d.u8++;
                        s.u8++;
                        count--; // cppcheck-suppress [misra-c2012-17.8]
                }
        }
        return dst;
}
