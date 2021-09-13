/**
 * @file
 * @brief newlib适配代码：动态内存申请与释放
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwos/mm/mempool/allocator.h>
#include <string.h>
#include <reent.h>

extern struct xwmm_mempool * newlibac_mempool;

static
void * newlibac_malloc(struct _reent * r, xwsz_t size);

static
void newlibac_free(struct _reent * r, void * mem);

static
void * newlibac_realloc(struct _reent * r, void * mem, xwsz_t size);

static
void * newlibac_memalign(struct _reent * r, xwsz_t alignment, xwsz_t size);

void newlibac_mem_init(void)
{
}

static
void * newlibac_malloc(struct _reent * r, xwsz_t size)
{
        void * mem;
        xwer_t rc;

        rc = xwmm_mempool_malloc(newlibac_mempool, size, &mem);
        errno = -rc;
        return mem;
}

static
void newlibac_free(struct _reent * r, void * mem)
{
        xwer_t rc;

        rc = xwmm_mempool_free(newlibac_mempool, mem);
        errno = -rc;
}

static
void * newlibac_realloc(struct _reent * r, void * mem, xwsz_t size)
{
        xwer_t rc;

        rc = xwmm_mempool_realloc(newlibac_mempool, size, &mem);
        errno = -rc;
        return mem;
}

static
void * newlibac_memalign(struct _reent * r, xwsz_t alignment, xwsz_t size)
{
        xwer_t rc;
        void * mem;

        rc = xwmm_mempool_memalign(newlibac_mempool, alignment, size, &mem);
        errno = -rc;
        return mem;
}

void * _malloc_r(struct _reent * r, size_t n)
{
        return newlibac_malloc(r, n);
}

void * _realloc_r(struct _reent * r, void * p, size_t n)
{
        return newlibac_realloc(r, p, n);
}

void * _calloc_r(struct _reent * r, size_t elem_nr, size_t elem_sz)
{
        xwsz_t total;
        void * mem;

        total = elem_nr * elem_sz;
        mem = newlibac_malloc(r, total);
        if (NULL != mem) {
                memset(mem, 0, total);
        }
        return mem;
}

void * _memalign_r(struct _reent * r, size_t alignment, size_t n)
{
        return newlibac_memalign(r, alignment, n);
}

void * _valloc_r(struct _reent * r, size_t n)
{
        return _memalign_r(r, XWMM_MEMPOOL_PAGE_SIZE, n);
}

void * _pvalloc_r(struct _reent * r, size_t n)
{
        return _memalign_r(r, XWMM_MEMPOOL_PAGE_SIZE, n);
}

void _free_r(struct _reent * r, void * p)
{
        newlibac_free(r, p);
}
