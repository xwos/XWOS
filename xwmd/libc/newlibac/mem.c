/**
 * @file
 * @brief newlib适配层：动态内存申请与释放
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwos/mm/mempool/allocator.h>
#include <xwmd/libc/newlibac/check.h>
#include <string.h>
#include <reent.h>
#include <stdio.h>
#include <malloc.h>

void newlibac_mem_linkage_stub(void)
{
}

extern struct xwmm_mempool * newlibac_mempool;

void * _malloc_r(struct _reent * r, size_t size)
{
        void * mem;
        xwer_t rc;

        rc = xwmm_mempool_malloc(newlibac_mempool, size, &mem);
        errno = -rc;
        return mem;
}

void * _realloc_r(struct _reent * r, void * mem, size_t size)
{
        xwer_t rc;

        rc = xwmm_mempool_realloc(newlibac_mempool, size, &mem);
        errno = -rc;
        return mem;
}

void * _calloc_r(struct _reent * r, size_t elem_nr, size_t elem_sz)
{
        xwsz_t total;
        void * mem;
        xwer_t rc;

        total = elem_nr * elem_sz;
        rc = xwmm_mempool_malloc(newlibac_mempool, total, &mem);
        errno = -rc;
        if (XWOK == rc) {
                memset(mem, 0, total);
        }
        return mem;
}

void * _memalign_r(struct _reent * r, size_t alignment, size_t size)
{
        xwer_t rc;
        void * mem;

        mem = NULL;
        rc = xwmm_mempool_memalign(newlibac_mempool, alignment, size, &mem);
        errno = -rc;
        return mem;
}

int posix_memalign(void ** memptr, size_t alignment, size_t size)
{
        int rc;

        if (((alignment & (alignment - 1)) != 0) ||
            (alignment % sizeof(void *) != 0) ||
            (alignment == 0)) {
                rc = EINVAL;
        } else {
                void * mem;
                rc = xwmm_mempool_memalign(newlibac_mempool, alignment, size, &mem);
                if (XWOK == rc) {
                        *memptr = mem;
                } else {
                        rc = -rc;
                }
        }
        return rc;
}

void * _valloc_r(struct _reent * r, size_t size)
{
        return _memalign_r(r, XWMM_MEMPOOL_PAGE_SIZE, size);
}

void * _pvalloc_r(struct _reent * r, size_t size)
{
        return _memalign_r(r, XWMM_MEMPOOL_PAGE_SIZE, size);
}

void _free_r(struct _reent * r, void * mem)
{
        xwer_t rc;

        rc = xwmm_mempool_free(newlibac_mempool, mem);
        errno = -rc;
}

void cfree(void * mem)
{
        free(mem);
}

int getpagesize(void)
{
        return XWMM_MEMPOOL_PAGE_SIZE;
}

int _mallopt_r(struct _reent * r, int parameter, int value)
{
        return 0;
}

struct mallinfo _mallinfo_r(struct _reent * r)
{
        struct mallinfo mi = {0};

        mi.arena = newlibac_mempool->pa.zone.size;
        /* FIXME */
        return mi;
}

void _malloc_stats_r(struct _reent * r)
{
        struct mallinfo mi;
        mi = mallinfo();
        fprintf(stderr, "max system bytes = %10lu\n", (long) mi.arena);
        /* FIXME */
}


size_t _malloc_usable_size_r(struct _reent * r, void * mem)
{
        xwer_t rc;
        struct xwmm_mempool_page * pg;
        size_t sz;

        rc = xwmm_mempool_page_find(&newlibac_mempool->pa, mem, &pg);
        if (XWOK == rc) {
                sz = pg->data.value;
        } else {
                sz = 0;
        }
        return sz;
}

int _malloc_trim_r(struct _reent * r, size_t pad)
{
        /* TODO */
        return 0;
}
