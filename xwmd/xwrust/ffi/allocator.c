/**
 * @file
 * @brief XWRUST FFI：全局内存申请器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mm/common.h>
#include <xwos/lib/xwbop.h>
#include <stdlib.h>

void * xwrustffi_allocator_alloc(xwsz_t alignment, xwsz_t size)
{
        void * mem;
        xwssz_t p2;

        if ((alignment < XWMM_ALIGNMENT) && (alignment < size)) {
                mem = malloc(size);
        } else {
                if (size <= alignment) {
                        size = alignment;
                } else {
                        p2 = xwbop_fls(xwsz_t, size);
                        while (((xwsz_t)1 << (xwsz_t)p2) < size) {
                                p2++;
                        }
                        size = ((xwsz_t)1 << (xwsz_t)p2);
                }
                mem = aligned_alloc(alignment, size);
        }
        return mem;
}

void xwrustffi_allocator_free(void * mem)
{
        free(mem);
}
