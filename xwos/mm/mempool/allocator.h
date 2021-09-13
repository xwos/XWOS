/**
 * @file
 * @brief 玄武OS内存管理：内存池分配器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mm_mempool_allocator_h__
#define __xwos_mm_mempool_allocator_h__

#include <xwos/standard.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwmm_mempool 内存池分配器
 * 此算法是在所有上下文（线程、中断、中断底半部）都是安全的。
 * @{
 */

#include <xwos/mm/mempool/page.h>
#include <xwos/mm/mempool/objcache.h>

/**
 * @note
 * + 页的最小单位是@ref XWMM_MEMPOOL_PAGE_SIZE (4K字节)
 * + 页的阶数n是指，所包含页的最小单位的数量以2的n次方的形式表示。n阶页
 *   的大小是(4K * pow(2, n))
 */
#define XWMM_MEMPOOL_PAGE_SIZE          (4096U) /**< 每页内存的字节数 */
#define XWMM_MEMPOOL_OC_8_PAGE_ODR      (0U) /**< 8字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_16_PAGE_ODR     (0U) /**< 16字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_32_PAGE_ODR     (0U) /**< 32字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_64_PAGE_ODR     (0U) /**< 64字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_96_PAGE_ODR     (1U) /**< 96字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_128_PAGE_ODR    (0U) /**< 128字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_160_PAGE_ODR    (0U) /**< 160字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_192_PAGE_ODR    (0U) /**< 192字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_256_PAGE_ODR    (0U) /**< 256字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_320_PAGE_ODR    (1U) /**< 320字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_384_PAGE_ODR    (1U) /**< 384字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_512_PAGE_ODR    (1U) /**< 512字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_768_PAGE_ODR    (0U) /**< 768字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_1024_PAGE_ODR   (0U) /**< 1024字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_2048_PAGE_ODR   (1U) /**< 2048字节分配器所使用的页的阶数 */

#if (XWMM_MEMPOOL_PAGE_SIZE & XWMM_ALIGNMENT_MASK)
  #error "Page size must be aligned to XWMM_ALIGNMENT!"
#endif

/**
 * @brief 定义内存池结构体的RAW内存空间，用于初始化内存池结构体
 * @param[in] mem: 内存数组名
 * @param[in] pgodr: 页的数量，以2的pgodr次方形式表示
 */
#define XWMM_MEMPOOL_DEF(mem, pgodr) \
        xwu8_t mem[sizeof(struct xwmm_mempool) + \
                   (sizeof(struct xwmm_mempool_page) * (1 << (pgodr))) + \
                   (sizeof(struct xwmm_mempool_page_odrbtree) * ((pgodr) + 1))]

/**
 * @brief 内存池
 */
struct xwmm_mempool {
        const char * name; /**< 名字 */
        struct xwmm_mempool_page_allocator pa; /**< 页分配器 */
        struct xwmm_mempool_objcache oc_8; /**< 8-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_16; /**< 16-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_32; /**< 32-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_64; /**< 64-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_96; /**< 96-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_128; /**< 128-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_160; /**< 160-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_192; /**< 192-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_256; /**< 256-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_320; /**< 320-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_384; /**< 384-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_512; /**< 512-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_768; /**< 768-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_1024; /**< 1024-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_2048; /**< 2048-Byte对象缓存 */
        xwu8_t rem[0]; /**< 结构体剩余的内存空间 */
};

xwer_t xwmm_mempool_init(struct xwmm_mempool * mp, const char * name,
                         xwptr_t origin, xwsz_t size, xwsz_t pgodr);
xwer_t xwmm_mempool_malloc(struct xwmm_mempool * mp, xwsz_t size,
                           void ** membuf);
xwer_t xwmm_mempool_free(struct xwmm_mempool * mp, void * mem);
xwer_t xwmm_mempool_realloc(struct xwmm_mempool * mp, xwsz_t size,
                            void ** membuf);
xwer_t xwmm_mempool_memalign(struct xwmm_mempool * mp,
                             xwsz_t alignment, xwsz_t size,
                             void ** membuf);

/**
 * @} xwmm_mempool
 */

#endif /* xwos/mm/mempool/allocator.h */
