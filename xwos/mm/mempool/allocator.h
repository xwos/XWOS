/**
 * @file
 * @brief XWOS内存管理：内存池：页分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mm_mempool_allocator_h__
#define __xwos_mm_mempool_allocator_h__

#include <xwos/standard.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwmm_mempool 内存池分配器
 * @ingroup xwmm
 * 内存池是结合了内存切片分配器与伙伴算法分配器的算法。内存管理的基本单位为页，
 * 一页内存为4096字节，页内存使用伙伴算法分配器管理。
 * 内存池还使用对象缓存算法建立各种小尺寸的块：8字节、16字节、32字节、
 * 64字节、96字节、128字节、160字节、192字节、256字节、320字节、384字节、
 * 512字节、768字节、1024字节、2048字节。
 * 当申请内存大于2048字节，直接分配页内存；当申请的内存小于等于2048字节，
 * 就从尺寸最合适的对象缓存分配器中分配一块内存。
 *
 * + 优点：
 *   + 支持释放操作；
 *   + 支持大小不固定的内存申请操作；
 *   + 反复申请与释放不会造成内存碎片；
 *   + 小内存块不会浪费过多的内存。
 * + 缺点：
 *   + 代码复杂；
 *   + 申请与释放操作的所需要的时间不稳定。
 * + 适用性：外接尺寸较大的SRAM或SDRAM的内存管理，C++和Lua虚拟机的对象池。
 * + 上下文的安全性：在任何上下文（中断、中断底半部、线程）都是安全的。
 * @{
 */

#include <xwos/mm/mempool/page.h>
#include <xwos/mm/mempool/objcache.h>

/**
 * @note
 * + 页的最小单位是 @ref XWMM_MEMPOOL_PAGE_SIZE
 * + 页的阶数n是指，以2的n次方的形式表示的单位页的数量。n阶页的大小是(4K * pow(2, n))。
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

#if (XWMM_MEMPOOL_PAGE_SIZE & XWMM_UNALIGNED_MASK)
#  error "Page size must be aligned to XWMM_ALIGNMENT!"
#endif

/**
 * @brief 定义内存池结构体的RAW内存空间，用于初始化内存池结构体
 * @param[in] name: 内存数组名
 * @param[in] pgodr: 页的数量，以2的pgodr次方形式表示
 */
// cppcheck-suppress [misra-c2012-20.7]
#define XWMM_MEMPOOL_RAWOBJ_DEF(name, pgodr) \
        xwu8_t name[sizeof(struct xwmm_mempool) + \
                    sizeof(struct xwmm_mempool_page_odrbtree[(pgodr) + 1U]) + \
                    sizeof(struct xwmm_mempool_page[1U << (pgodr)])]

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

/**
 * @brief XWMM API：初始化内存池
 * @param[in] mp: 内存池的指针
 * @param[in] name: 名字
 * @param[in] origin: 内存区域的起始地址
 * @param[in] size: 内存区域的总大小
 * @param[in] pgodr: 页的数量，以2的pgodr次方形式表示
 * @param[in] pre: 预申请的内存的大小
 * @param[out] membuf: 指向缓冲区的指针，通过此缓冲区返回预申请内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2SMALL: 内存区域太小
 * @retval -ESIZE: 内存区域大小size与pgodr指明的页数量不匹配
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：不可重入
 * @details
 * + 将首地址为 `origin` ，大小为 `size` 的内存初始化为内存池：
 *   + 页的数量用 `pgodr` 表示，只能是2的n次方，即 `2, 4, 8, 16, 32, 64, 128, ...` ，
 *     对应的pgodr分别为 `1, 2, 3, 4, 5, 6, 7, ...` ；
 *   + 内存区域大小必须满足关系： `size == (XWMM_MEMPOOL_PAGE_SIZE * (1 << pgodr))` 。
 * + 如果在内存区域开始的地方，已经由编译器预先分配了一片内存，
 *   用户只想将剩下的内存用作内存池，则需要将编译器预先分配的内存在内存池中申请出来，
 *   防止内存发生重叠。参数 `pre` 以及 `membuf` 就是作为此用途而设计的。
 *   `membuf` 可以为 `NULL` ，表示不需要返回编译器预先申请内存的首地址。
 */
xwer_t xwmm_mempool_init(struct xwmm_mempool * mp, const char * name,
                         xwptr_t origin, xwsz_t size, xwsz_t pgodr,
                         xwsz_t pre, void ** membuf);

/**
 * @brief XWMM API：从内存池中申请内存
 * @param[in] mp: 内存池的指针
 * @param[in] size: 申请的大小
 * @param[out] membuf: 指向缓冲区的指针，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * + 此函数向内存池申请大小为 `size` 的内存：
 *   + 若申请成功，通过 `*membuf` 返回申请到的内存地址 ，返回错误码为 `XWOK` ；
 *   + 若申请失败，通过 `*membuf` 返回 `NULL` ，返回值为负的错误码。
 * + 内存不会被初始化；
 * + 当 `size` 为 `0` ， 通过 `*membuf` 返回 `NULL` ，返回错误码为 `XWOK` ；
 */
xwer_t xwmm_mempool_malloc(struct xwmm_mempool * mp, xwsz_t size,
                           void ** membuf);

/**
 * @brief XWMM API：释放内存
 * @param[in] mp: 内存池的指针
 * @param[in] mem: 内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 页内存已释放
 * @retval -ERANGE: 内存地址不在内存池的范围内
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * 此函数向内存池释放内存。内存必须是此前申请的，如果 `mem` 是错误的地址或之前被释放过，
 * 将产生未定义的错误。 `mem` 可以为 `NULL` ，此函数什么也不做，并且返回错误码 `XWOK` 。
 */
xwer_t xwmm_mempool_free(struct xwmm_mempool * mp, void * mem);

/**
 * @brief XWMM API：调整内存大小
 * @param[in] mp: 内存池的指针
 * @param[in] size: 申请的大小，当size == 0，realloc等价于free
 * @param[in,out] membuf: 指向缓冲区的指针，此缓冲区
 * + (I) 作为输入时，当*membuf == NULL，realloc等价于malloc
 * + (O) 作为输出时，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @retval -ERANGE: 内存地址不在内存池的范围内
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * + 此API类似于C11标准中的 `realloc()` 函数：
 *   + 当 `*membuf` 为 `NULL` ，此函数等价于 `xwmm_mempool_mealloc(mp, size, membuf)` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 为 `0` ，
 *     此函数等价于 `xwmm_mempool_free(mp, *membuf)` ，并且通过 `*membuf` 返回 `NULL` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 比之前的小 ，此函数不重新申请内存，直接返回原来的 `*membuf` 以及 `XWOK` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 比之前的大 ，此函数会尝试重新申请内存：
 *     + 如果申请失败，原来的内存不会受影响，且通过 `*membuf` 返回 `NULL` 以及 错误码；
 *     + 如果申请成功，会将旧内存空间的内容移动到新内存空间内，然后返回新的 `*membuf` 以及 `XWOK` 。
 */
xwer_t xwmm_mempool_realloc(struct xwmm_mempool * mp, xwsz_t size,
                            void ** membuf);

/**
 * @brief XWMM API：从内存池中申请对齐的内存
 * @param[in] mp: 内存池的指针
 * @param[in] alignment: 内存的起始地址对齐的字节数，只能是2的n次方
 * @param[in] size: 申请的大小
 * @param[out] membuf: 指向缓冲区的指针，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * + 此API类似于C标准中的 `memalign()` 函数：
 *   + `alignment` 如果比 `XWMM_ALIGNMENT` 小，会被扩大为 `XWMM_ALIGNMENT` ：
 *   + `alignment` 只能是2的n次方：
 *   + 若size小于 `alignment` ， `size` 会被扩大为 `alignment` ，
 *   + 若size大于 `alignment` ， `size` 会向上对齐到2的n次方，此时也一定为 `alignment` 的整数倍。
 * + 申请内存失败时，此函数不会修改 `*membuf` 的值。
 */
xwer_t xwmm_mempool_memalign(struct xwmm_mempool * mp,
                             xwsz_t alignment, xwsz_t size,
                             void ** membuf);

/**
 * @} xwmm_mempool
 */

#endif /* xwos/mm/mempool/allocator.h */
