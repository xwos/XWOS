/**
 * @file
 * @brief 示例：简单内存分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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
#include <xwos/mm/sma.h>

/**
 * @brief 简单内存分配器的内存区间
 */
xwu8_t __xwcc_alignl1cache xwmmdemo_sma_mempool[4096];

/**
 * @brief 简单内存分配器
 */
struct xwmm_sma xwmmdemo_sma;

/**
 * @brief 模块的加载函数
 */
xwer_t example_sma_start(void)
{
        xwer_t rc;
        void * mem;

        /* 建立简单内存分配器：
           + 内存区域：xwmmdemo_sma_mempool
           + 大小：4096
           + 位置：0 （内存分配器从0偏移地址开始分配内存） */
        rc = xwmm_sma_init(&xwmmdemo_sma,
                           (xwptr_t)xwmmdemo_sma_mempool,
                           sizeof(xwmmdemo_sma_mempool),
                           0,
                           "xwmmdemo_sma");
        if (rc < 0) {
                /* 初始化失败 */
                goto err_sma_init;
        }

        /* 申请100字节内存，起始地址对齐到XWMM_ALIGNMENT的边界上 */
        /* 此函数在任意上下文都是安全的 */
        rc = xwmm_sma_alloc(&xwmmdemo_sma, 100, XWMM_ALIGNMENT, &mem);
        if (rc < 0) {
                /* 申请失败 */
                goto err_sma_alloc;
        }

        /* 释放内存，sma的内存不支持释放，xwmm_sma_free()仅仅是一个dummy函数 */
        xwmm_sma_free(&xwmmdemo_sma, mem);

        return XWOK;

err_sma_alloc:
err_sma_init:
        return rc;
}
