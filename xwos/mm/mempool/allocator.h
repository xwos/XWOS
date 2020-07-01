/**
 * @file
 * @brief XuanWuOS的内存管理机制：内存池分配器
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
#define XWMM_MEMPOOL_OC_192_PAGE_ODR    (0U) /**< 192字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_256_PAGE_ODR    (0U) /**< 256字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_384_PAGE_ODR    (1U) /**< 384字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_512_PAGE_ODR    (1U) /**< 512字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_768_PAGE_ODR    (0U) /**< 768字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_1024_PAGE_ODR   (0U) /**< 1024字节分配器所使用的页的阶数 */
#define XWMM_MEMPOOL_OC_2048_PAGE_ODR   (1U) /**< 2048字节分配器所使用的页的阶数 */

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
/**
 * @brief XWMM API：静态方式初始化内存池。
 * @param mp: (I) 内存池的指针
 * @param name: (I) 名字
 * @param origin: (I) 内存区域的起始地址
 * @param size: (I) 内存区域的总大小
 * @param odrbtree: (I) 阶红黑树数组的指针
 * @param pgarray: (I) 页控制块数组的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -E2SMALL: 内存区域太小
 * @retval -EALIGN: 内存区域没有对齐
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_mempool_init(struct xwmm_mempool * mp, const char * name,
                         xwptr_t origin, xwsz_t size);

/**
 * @brief XWMM API：销毁静态方式初始化的内存池
 * @param mp: (I) 内存池的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_mempool_destroy(struct xwmm_mempool * mp);

/**
 * @brief XWMM API：动态创建内存池
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回新的内存池的指针
 * @param name: (I) 名字
 * @param origin: (I) 内存区域的起始地址
 * @param size: (I) 内存区域的总大小
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -E2SMALL: 内存区域太小
 * @retval -EALIGN: 内存区域没有对齐
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_mempool_create(struct xwmm_mempool ** ptrbuf, const char * name,
                           xwptr_t origin, xwsz_t size);

/**
 * @brief XWMM API：删除动态创建的内存池
 * @param mp: (I) 内存池的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_mempool_delete(struct xwmm_mempool * mp);

/**
 * @brief XWMM API：从内存池中申请内存
 * @param mp: (I) 内存池的指针
 * @param size: (I) 申请的大小
 * @param membuf: (O) 指向缓冲区的指针，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_mempool_malloc(struct xwmm_mempool * mp, xwsz_t size,
                           void ** membuf);

/**
 * @brief XWMM API：释放内存
 * @param mp: (I) 内存池的指针
 * @param mem: (I) 内存的首地址
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 页内存已释放
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_mempool_free(struct xwmm_mempool * mp, void * mem);

/**
 * @brief XWMM API：调整内存大小
 * @param mp: (I) 内存池的指针
 * @param size: (I) 申请的大小，当size == 0，realloc等价于free
 * @param membuf: 指向缓冲区的指针，此缓冲区
 *                (I) 作为输入时，当*membuf == NULL，realloc等价于malloc
 *                (O) 作为输出时，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_mempool_realloc(struct xwmm_mempool * mp, xwsz_t size,
                            void ** membuf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/mm/mempool/allocator.h */
