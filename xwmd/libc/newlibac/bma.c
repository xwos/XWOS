/**
 * @file
 * @brief newlib适配层：基于bma的动态内存申请与释放
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwos/mm/bma.h>
#include <xwmd/libc/newlibac/linkage.h>
#include <xwmd/libc/newlibac/check.h>
#include <string.h>
#include <reent.h>
#include <stdio.h>
#include <malloc.h>

#define LIBC_BMA_PAGE_SIZE (4096U)

void newlibac_mem_linkage_stub(void)
{
}

extern struct xwmm_bma * newlibac_bma;

void * _malloc_r(struct _reent * r, size_t size);
void * _realloc_r(struct _reent * r, void * mem, size_t size);
void * _calloc_r(struct _reent * r, size_t elem_nr, size_t elem_sz);
void * _memalign_r(struct _reent * r, size_t alignment, size_t size);
int posix_memalign(void ** memptr, size_t alignment, size_t size);
void * _valloc_r(struct _reent * r, size_t size);
void * _pvalloc_r(struct _reent * r, size_t size);
void _free_r(struct _reent * r, void * mem);
void cfree(void * mem);
int getpagesize(void);
int _mallopt_r(struct _reent * r, int parameter, int value);
struct mallinfo _mallinfo_r(struct _reent * r);
void _malloc_stats_r(struct _reent * r);
size_t _malloc_usable_size_r(struct _reent * r, void * mem);
int _malloc_trim_r(struct _reent * r, size_t pad);

void * _malloc_r(struct _reent * r, size_t size)
{
        void * mem;
        xwer_t rc;

        XWOS_UNUSED(r);

        rc = xwmm_bma_malloc(newlibac_bma, size, &mem);
        errno = -rc;
        return mem;
}

void * _realloc_r(struct _reent * r, void * mem, size_t size)
{
        xwer_t rc;

        XWOS_UNUSED(r);

        rc = xwmm_bma_realloc(newlibac_bma, size, &mem);
        errno = -rc;
        return mem;
}

void * _calloc_r(struct _reent * r, size_t elem_nr, size_t elem_sz)
{
        xwsz_t total;
        void * mem;
        xwer_t rc;

        XWOS_UNUSED(r);

        total = elem_nr * elem_sz;
        rc = xwmm_bma_malloc(newlibac_bma, total, &mem);
        errno = -rc;
        if (XWOK == rc) {
                memset(mem, 0, total); // cppcheck-suppress [misra-c2012-17.7]
        }
        return mem;
}

void * _memalign_r(struct _reent * r, size_t alignment, size_t size)
{
        xwer_t rc;
        void * mem;

        XWOS_UNUSED(r);

        mem = NULL;
        rc = xwmm_bma_memalign(newlibac_bma, alignment, size, &mem);
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
                rc = xwmm_bma_memalign(newlibac_bma, alignment, size, &mem);
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
        return _memalign_r(r, LIBC_BMA_PAGE_SIZE, size);
}

void * _pvalloc_r(struct _reent * r, size_t size)
{
        return _memalign_r(r, LIBC_BMA_PAGE_SIZE, size);
}

void _free_r(struct _reent * r, void * mem)
{
        xwer_t rc;

        XWOS_UNUSED(r);

        rc = xwmm_bma_free(newlibac_bma, mem);
        errno = -rc;
}

void cfree(void * mem)
{
        free(mem);
}

int getpagesize(void)
{
        return LIBC_BMA_PAGE_SIZE;
}

int _mallopt_r(struct _reent * r, int parameter, int value)
{
        XWOS_UNUSED(r);
        XWOS_UNUSED(parameter);
        XWOS_UNUSED(value);
        return 0;
}

struct mallinfo _mallinfo_r(struct _reent * r)
{
        struct mallinfo mi = {0};

        XWOS_UNUSED(r);

        mi.arena = newlibac_bma->zone.size;
        return mi;
}

void _malloc_stats_r(struct _reent * r)
{
        struct mallinfo mi;

        XWOS_UNUSED(r);

        mi = mallinfo();
        // cppcheck-suppress [misra-c2012-17.7]
        fprintf(stderr, "max system bytes = %10lu\n", (long) mi.arena);
}


size_t _malloc_usable_size_r(struct _reent * r, void * mem)
{
        xwer_t rc;
        size_t sz;

        XWOS_UNUSED(r);

        rc = xwmm_bma_malloc_usable_size(newlibac_bma, mem, &sz);
        if (rc < 0) {
                sz = 0U;
        }
        return sz;
}

int _malloc_trim_r(struct _reent * r, size_t pad)
{
        XWOS_UNUSED(r);
        XWOS_UNUSED(pad);
        return 0;
}
