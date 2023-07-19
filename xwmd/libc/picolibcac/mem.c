/**
 * @file
 * @brief picolibc适配层：动态内存申请与释放
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
#include <xwmd/libc/picolibcac/linkage.h>
#include <xwmd/libc/picolibcac/check.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

void picolibcac_mem_linkage_stub(void)
{
}

void * malloc(size_t size);
void * realloc(void * mem, size_t size);
void * calloc(size_t elem_nr, size_t elem_sz);
void * memalign(xwsz_t alignment, xwsz_t size);
int posix_memalign(void ** memptr, size_t alignment, size_t size);
void * aligned_alloc(size_t alignment, size_t size);
void * valloc(size_t size);
void * pvalloc(size_t size);
void free(void * mem);
void cfree(void * mem);
int getpagesize(void);
int mallopt(int parameter, int value);
struct mallinfo mallinfo(void);
void malloc_stats(void);
size_t malloc_usable_size(void * mem);
int malloc_trim(size_t pad);

extern struct xwmm_mempool * picolibcac_mempool;

void * malloc(size_t size)
{
        void * mem;
        xwer_t rc;

        rc = xwmm_mempool_malloc(picolibcac_mempool, size, &mem);
        errno = -rc;
        return mem;
}

void * realloc(void * mem, size_t size)
{
        xwer_t rc;

        rc = xwmm_mempool_realloc(picolibcac_mempool, size, &mem);
        errno = -rc;
        return mem;
}

void * calloc(size_t elem_nr, size_t elem_sz)
{
        xwsz_t total;
        void * mem;
        xwer_t rc;

        total = elem_nr * elem_sz;
        rc = xwmm_mempool_malloc(picolibcac_mempool, total, &mem);
        errno = -rc;
        if (XWOK == rc) {
                memset(mem, 0, total); // cppcheck-suppress [misra-c2012-17.7]
        }
        return mem;
}

void * memalign(xwsz_t alignment, xwsz_t size)
{
        xwer_t rc;
        void * mem;

        mem = NULL;
        rc = xwmm_mempool_memalign(picolibcac_mempool, alignment, size, &mem);
        errno = -rc;
        return mem;
}

int posix_memalign(void ** memptr, size_t alignment, size_t size)
{
        int rc;

        if (((alignment & (alignment - (size_t)1)) != (size_t)0) ||
            (alignment % sizeof(void *) != (size_t)0) ||
            (alignment == (size_t)0)) {
                rc = EINVAL;
        } else {
                void * mem;
                rc = xwmm_mempool_memalign(picolibcac_mempool, alignment, size, &mem);
                if (XWOK == rc) {
                        *memptr = mem;
                } else {
                        rc = -rc;
                }
        }
        return rc;
}

void * aligned_alloc(size_t alignment, size_t size)
{
        return memalign(alignment, size);
}

void * valloc(size_t size)
{
        return memalign(XWMM_MEMPOOL_PAGE_SIZE, size);
}

void * pvalloc(size_t size)
{
        return memalign(XWMM_MEMPOOL_PAGE_SIZE, size);
}

void free(void * mem)
{
        xwer_t rc;

        rc = xwmm_mempool_free(picolibcac_mempool, mem);
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

int mallopt(int parameter, int value)
{
        XWOS_UNUSED(parameter);
        XWOS_UNUSED(value);
        return 0;
}

struct mallinfo mallinfo(void)
{
        struct mallinfo mi = {0};

        mi.arena = picolibcac_mempool->pa.zone.size;
        /* FIXME */
        return mi;
}

void malloc_stats(void)
{
        struct mallinfo mi;
        mi = mallinfo();
        // cppcheck-suppress [misra-c2012-17.7]
        fprintf(stderr, "max system bytes = %10lu\n", (long) mi.arena);
        /* FIXME */
}

size_t malloc_usable_size(void * mem)
{
        xwer_t rc;
        struct xwmm_mempool_page * pg;
        size_t sz;

        rc = xwmm_mempool_page_find(&picolibcac_mempool->pa, mem, &pg);
        if (XWOK == rc) {
                sz = pg->data.value;
        } else {
                sz = 0;
        }
        return sz;
}

int malloc_trim(size_t pad)
{
        XWOS_UNUSED(pad);
        /* TODO */
        return 0;
}
