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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/mm/mempool/allocator.h>
#include <string.h>
#include <reent.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
void * newlibac_malloc(xwsz_t size);

static
void newlibac_mfree(void * mem);

static
void * newlibac_mrealloc(void * mem, xwsz_t size);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern struct xwmm_mempool * newlibac_mempool;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void newlibac_mem_linkage_placeholder(void)
{
}

static
void * newlibac_malloc(xwsz_t size)
{
        void * mem;

        xwmm_mempool_malloc(newlibac_mempool, size, &mem);
        return mem;
}

static
void newlibac_mfree(void * mem)
{
        xwmm_mempool_free(newlibac_mempool, mem);
}

static
void * newlibac_mrealloc(void * mem, xwsz_t size)
{
        xwmm_mempool_realloc(newlibac_mempool, size, &mem);
        return mem;
}

void * _malloc_r(struct _reent * r, size_t n)
{
        XWOS_UNUSED(r);
        return newlibac_malloc(n);
}

void * _realloc_r(struct _reent * r, void * p, size_t n)
{
        XWOS_UNUSED(r);
        return newlibac_mrealloc(p, n);
}

void * _calloc_r(struct _reent * r, size_t elem_nr, size_t elem_sz)
{
        xwsz_t total;
        void * mem;

        XWOS_UNUSED(r);
        total = elem_nr * elem_sz;
        mem = newlibac_malloc(total);
        if (NULL != mem) {
                memset(mem, 0, total);
        }
        return mem;
}

void _free_r(struct _reent * r, void * p)
{
        XWOS_UNUSED(r);
        newlibac_mfree(p);
}
