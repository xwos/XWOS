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
#include <xwmd/libc/picolibcac/check.h>
#include <string.h>

void picolibcac_mem_linkage_stub(void)
{
}

extern struct xwmm_mempool * picolibcac_mempool;

static
void * picolibcac_malloc(xwsz_t size);

static
void picolibcac_free(void * mem);

static
void * picolibcac_realloc(void * mem, xwsz_t size);

static
void * picolibcac_memalign(xwsz_t alignment, xwsz_t size);

static
void * picolibcac_malloc(xwsz_t size)
{
        void * mem;
        xwer_t rc;

        rc = xwmm_mempool_malloc(picolibcac_mempool, size, &mem);
        errno = -rc;
        return mem;
}

static
void picolibcac_free(void * mem)
{
        xwer_t rc;

        rc = xwmm_mempool_free(picolibcac_mempool, mem);
        errno = -rc;
}

static
void * picolibcac_realloc(void * mem, xwsz_t size)
{
        xwer_t rc;

        rc = xwmm_mempool_realloc(picolibcac_mempool, size, &mem);
        errno = -rc;
        return mem;
}

static
void * picolibcac_memalign(xwsz_t alignment, xwsz_t size)
{
        xwer_t rc;
        void * mem;

        rc = xwmm_mempool_memalign(picolibcac_mempool, alignment, size, &mem);
        errno = -rc;
        return mem;
}

void * malloc(size_t n)
{
        return picolibcac_malloc(n);
}

void * realloc(void * p, size_t n)
{
        return picolibcac_realloc(p, n);
}

void * calloc(size_t elem_nr, size_t elem_sz)
{
        xwsz_t total;
        void * mem;

        total = elem_nr * elem_sz;
        mem = picolibcac_malloc(total);
        if (NULL != mem) {
                memset(mem, 0, total);
        }
        return mem;
}

void * memalign(size_t alignment, size_t n)
{
        return picolibcac_memalign(alignment, n);
}

void * valloc(size_t n)
{
        return memalign(XWMM_MEMPOOL_PAGE_SIZE, n);
}

void * pvalloc(size_t n)
{
        return memalign(XWMM_MEMPOOL_PAGE_SIZE, n);
}

void free(void * p)
{
        picolibcac_free(p);
}

int getpagesize(void)
{
        return XWMM_MEMPOOL_PAGE_SIZE;
}
