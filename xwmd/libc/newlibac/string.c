/**
 * @file
 * @brief newlib适配层：string
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
 * - newlib的memset没有对齐访问内存，这会造成产生BUS Fault。
 *   因此重新实现memset覆盖newlib中的函数。
 */
void * memset(void * src, int c, size_t count)
{
        union {
                xwu8_t * u8;
                xwu64_t * u64;
                xwptr_t val;
        } m;
        xwu64_t fill64;

        c = c & 0xFF; // cppcheck-suppress [misra-c2012-17.8]
        fill64 = (xwu64_t)c;
        fill64 = (fill64 << 8) | fill64;
        fill64 = (fill64 << 16) | fill64;
        fill64 = (fill64 << 32) | fill64;
        m.u8 = src;
        while ((m.val & (sizeof(xwu64_t) - 1U)) && (count > 0U)) {
                *m.u8 = (xwu8_t)c;
                m.u8++;
                count--; // cppcheck-suppress [misra-c2012-17.8]
        }
        while (count > 0U) {
                if (count < sizeof(xwu64_t)) {
                        while (count > 0U) {
                                *m.u8 = (xwu8_t)c;
                                m.u8++;
                                count--; // cppcheck-suppress [misra-c2012-17.8]
                        }
                } else {
                        *m.u64 = fill64;
                        m.u64++;
                        count -= sizeof(xwu64_t); // cppcheck-suppress [misra-c2012-17.8]
                }
        }
        return src;
}

// cppcheck-suppress [misra-c2012-8.14]
void * memcpy(void * restrict dst, const void * restrict src, size_t count)
{
        union {
                const xwu8_t * u8;
                const xwu32_t * u32;
                const xwu64_t * u64;
                xwptr_t val;
        } s;
        union {
                xwu8_t * u8;
                xwu32_t * u32;
                xwu64_t * u64;
                xwptr_t val;
        } d;

        s.u8 = src;
        d.u8 = dst;

        if (count < sizeof(xwu32_t)) {
                goto byte_copy;
        } else if (count < sizeof(xwu64_t)) {
                if ((0 == (s.val & (sizeof(xwu32_t) - 1U))) &&
                    (0 == (d.val & (sizeof(xwu32_t) - 1U)))) {
                        goto word_copy;
                } else {
                        goto byte_copy;
                }
        } else {
                if ((0 == (s.val & (sizeof(xwu64_t) - 1U))) &&
                    (0 == (d.val & (sizeof(xwu64_t) - 1U)))) {
                        goto dword_copy;
                } else
                if ((0 == (s.val & (sizeof(xwu32_t) - 1U))) &&
                           (0 == (d.val & (sizeof(xwu32_t) - 1U)))) {
                        goto word_copy;
                } else {
                        goto byte_copy;
                }
        }

dword_copy:
        while (count >= sizeof(xwu64_t)) {
                *d.u64 = *s.u64;
                d.u64++;
                s.u64++;
                count -= sizeof(xwu64_t); // cppcheck-suppress [misra-c2012-17.8]
        }
word_copy:
        while (count >= sizeof(xwu32_t)) {
                *d.u32 = *s.u32;
                d.u32++;
                s.u32++;
                count -= sizeof(xwu32_t); // cppcheck-suppress [misra-c2012-17.8]
        }
byte_copy:
        while (0U != count) {
                *d.u8 = *s.u8;
                d.u8++;
                s.u8++;
                count--; // cppcheck-suppress [misra-c2012-17.8]
        }
        return dst;
}
