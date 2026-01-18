/**
 * @file
 * @brief 示例：内存切片分配器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#include <xwos/standard.h>
#include <xwos/mm/memslice.h>
#include <xwam/example/mm/memslice/mif.h>

#define XWMMDEMO_MEMSLICE_MEMSIZE       4096U
#define XWMMDEMO_MEMSLICE_BLKSIZE       128U

/**
 * @brief 内存切片分配器的内存区间
 */
xwu8_t __xwcc_alignl1cache xwmmdemo_memslice_mempool[XWMMDEMO_MEMSLICE_MEMSIZE];

/**
 * @brief 内存切片分配器
 */
struct xwmm_memslice xwmmdemo_memslice;

/**
 * @brief 模块的加载函数
 */
xwer_t xwmm_example_memslice(void)
{
        xwer_t rc;
        void * mem;

        /* 建立内存切片分配器：
           + 内存区域：xwmmdemo_memslice_mempool
           + 大小：XWMMDEMO_MEMSLICE_MEMSIZE
           + 每一块内存大小：XWMMDEMO_MEMSLICE_BLKSIZE
           + 每一块内存的初始化回调函数：NULL
           + 每一块内存的释放回调函数：NULL */
        rc = xwmm_memslice_init(&xwmmdemo_memslice,
                                (xwptr_t)xwmmdemo_memslice_mempool,
                                sizeof(xwmmdemo_memslice_mempool),
                                XWMMDEMO_MEMSLICE_BLKSIZE,
                                "xwmmdemo_memslice",
                                NULL, NULL);
        if (rc < 0) {
                /* 初始化失败 */
                goto err_memslice_init;
        }

        /* 申请一块内存（XWMMDEMO_MEMSLICE_BLKSIZE） */
        /* 此函数在任意上下文都是安全的 */
        rc = xwmm_memslice_alloc(&xwmmdemo_memslice, &mem);
        if (rc < 0) {
                /* 申请失败 */
                goto err_memslice_alloc;
        }

        /* 释放内存 */
        xwmm_memslice_free(&xwmmdemo_memslice, mem);

        return XWOK;

err_memslice_alloc:
err_memslice_init:
        return rc;
}
