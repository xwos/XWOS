/**
 * @file
 * @brief 示例：伙伴算法内存块分配器
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
#include <xwos/lib/xwbop.h>
#include <xwos/mm/bma.h>
#include <xwam/example/mm/bma/mif.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMMDEMO_BMA_MEMSIZE            8192U
#define XWMMDEMO_BMA_BLKSIZE            128U

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 伙伴算法内存块分配器的内存区间
 */
xwu8_t __xwcc_aligned_l1cacheline xwmmdemo_bma_mempool[XWMMDEMO_BMA_MEMSIZE];

/**
 * @brief 伙伴算法内存块分配器
 */
struct xwmm_bma * xwmmdemo_bma;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 模块的加载函数
 */
xwer_t example_bma_start(void)
{
        xwer_t rc;
        void * mem;

        /* 建立内存切片分配器：
           + 内存区域：xwmmdemo_bma_mempool
           + 大小：XWMMDEMO_BMA_MEMSIZE
           + 单位块内存大小：XWMMDEMO_BMA_BLKSIZE
           + 单位块的数量：XWMMDEMO_BMA_MEMSIZE / XWMMDEMO_BMA_BLKSIZE
                           （必须满足数量为2的n次方） */
        rc = xwmm_bma_create(&xwmmdemo_bma, "xwmmdemo_bma",
                             (xwptr_t)xwmmdemo_bma_mempool,
                             sizeof(xwmmdemo_bma_mempool),
                             XWMMDEMO_BMA_BLKSIZE);
        if (rc < 0) {
                /* 初始化失败 */
                goto err_bma_init;
        }

        /* 申请内存：
           实际会申请4(pow(2,2))个单位块（共512字节连续的内存） */
        /* 此函数在任意上下文都是安全的 */
        rc = xwmmdemo_bma_alloc(257, &mem);
        if (rc < 0) {
                /* 申请失败 */
                goto err_bma_alloc;
        }

        /* 释放内存 */
        xwmmdemo_bma_free(mem);

        return XWOK;

err_bma_alloc:
err_bma_init:
        return rc;
}

xwer_t xwmmdemo_bma_alloc(xwsz_t memsize, void ** membuf)
{
        xwsz_t stknum;
        xwsq_t numodr;
        xwer_t rc;

        /* 计算至少需要多少个连续的单位块 */
        stknum = DIV_ROUND_UP(memsize, XWMMDEMO_BMA_BLKSIZE);
        /* 单位块的数量必须为2的n次方 */
        numodr = (xwsq_t)xwbop_fls(xwsz_t, stknum);
        if (stknum & (stknum - 1U)) {
                numodr++;
        }
        rc = xwmm_bma_alloc(xwmmdemo_bma, numodr, membuf);
        return rc;
}

xwer_t xwmmdemo_bma_free(void * mem)
{
        return xwmm_bma_free(xwmmdemo_bma, mem);
}
