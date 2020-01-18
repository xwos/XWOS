/**
 * @file
 * @brief XuanWuOS的内存管理机制：内存池分配器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mm_mempool_allocator_h__
#define __xwos_mm_mempool_allocator_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/mm/common.h>
#include <xwos/mm/mempool/page.h>
#include <xwos/mm/mempool/objcache.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMM_MEMPOOL_PAGE_SIZE          (4096U)
#define XWMM_MEMPOOL_OC_8_PAGE_ODR      (0U)
#define XWMM_MEMPOOL_OC_16_PAGE_ODR     (0U)
#define XWMM_MEMPOOL_OC_32_PAGE_ODR     (0U)
#define XWMM_MEMPOOL_OC_64_PAGE_ODR     (0U)
#define XWMM_MEMPOOL_OC_96_PAGE_ODR     (1U)
#define XWMM_MEMPOOL_OC_128_PAGE_ODR    (0U)
#define XWMM_MEMPOOL_OC_192_PAGE_ODR    (0U)
#define XWMM_MEMPOOL_OC_256_PAGE_ODR    (0U)
#define XWMM_MEMPOOL_OC_384_PAGE_ODR    (1U)
#define XWMM_MEMPOOL_OC_512_PAGE_ODR    (1U)
#define XWMM_MEMPOOL_OC_768_PAGE_ODR    (0U)
#define XWMM_MEMPOOL_OC_1024_PAGE_ODR   (0U)
#define XWMM_MEMPOOL_OC_2048_PAGE_ODR   (1U)

#if (XWMM_MEMPOOL_PAGE_SIZE & XWMM_ALIGNMENT_MASK)
  #error "Page size must be aligned to XWMM_ALIGNMENT!"
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
        struct xwmm_mempool_objcache oc_192; /**< 192-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_256; /**< 256-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_384; /**< 384-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_512; /**< 512-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_768; /**< 768-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_1024; /**< 1024-Byte对象缓存 */
        struct xwmm_mempool_objcache oc_2048; /**< 2048-Byte对象缓存 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwmm_mempool_init(struct xwmm_mempool * mp, const char * name,
                         xwptr_t origin, xwsz_t size);

__xwos_api
xwer_t xwmm_mempool_destroy(struct xwmm_mempool * mp);

__xwos_api
xwer_t xwmm_mempool_create(struct xwmm_mempool ** ptrbuf, const char * name,
                           xwptr_t origin, xwsz_t size);

__xwos_api
xwer_t xwmm_mempool_delete(struct xwmm_mempool * mp);

__xwos_api
xwer_t xwmm_mempool_malloc(struct xwmm_mempool * mp, xwsz_t size,
                           void ** membuf);

__xwos_api
xwer_t xwmm_mempool_free(struct xwmm_mempool * mp, void * mem);

__xwos_api
xwer_t xwmm_mempool_realloc(struct xwmm_mempool * mp, xwsz_t size,
                            void ** membuf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/mm/mempool/i_allocator.h */
