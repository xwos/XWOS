/**
 * @file
 * @brief XWRUST FFI：全局内存申请器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mm/mempool/allocator.h>

extern struct xwmm_mempool * xwrust_mempool;

void * xwrustffi_allocator_alloc(xwsz_t alignment, xwsz_t size)
{
        void * mem = NULL;
        xwmm_mempool_memalign(xwrust_mempool, alignment, size, &mem);
        return mem;
}

void xwrustffi_allocator_free(void * mem)
{
        xwmm_mempool_free(xwrust_mempool, mem);
}
