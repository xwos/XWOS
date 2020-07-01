/**
 * @file
 * @brief newlib适配代码：动态内存申请与释放
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
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
