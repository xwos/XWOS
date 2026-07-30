/**
 * @file
 * @brief XWOS内存管理：伙伴算法内存块分配器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mm_bma_h__
#define __xwos_mm_bma_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwmm_bma 伙伴算法内存块分配器
 * @ingroup xwmm
 * 伙伴算法分配器在分配时，会将内存不断地二等分，直到切割到能满足内存的最小尺寸为止。
 * 释放时会检查与之相邻并等长的内存块（称为伙伴）是否空闲，
 * 如果是，就和“伙伴”合并成更大的内存块，
 * 然后继续检测合并后的内存块是否也存在空闲的伙伴，一直向上合并到不能合并为止。
 *
 * + 优点：
 *   + 支持释放操作；
 *   + 支持大小不固定的内存申请操作；
 *   + 内存碎片的大小只会是单位内存块大小的2的n次方倍，内存碎片更容易被合并。
 * + 缺点：
 *   + 代码稍微复杂；
 *   + 因为存在合并与切割的循环，申请与释放操作的所需要的时间不是特别稳定；
 *   + 内存大小固定为2的指数，如果申请的内存过小，会造成内存浪费。
 * + 适用性：对内存复用性要求高的应用场合。
 * + 上下文的安全性：在任何上下文（中断、中断底半部、线程）都是安全的。
 * @{
 */


#define XWMM_BMA_MAX_ORDER              ((xwu8_t)126) /**< 最大的阶 */
#define XWMM_BMA_COMBINED               ((xwu8_t)127) /**< 块已被合并 */
#define XWMM_BMA_ORDER_MASK             ((xwu8_t)0x7F) /**< 阶的掩码 */
#define XWMM_BMA_INUSED                 ((xwu8_t)0x80) /**< 块正在被使用 */

#define XWMM_BMA_ORDERLIST_SIZE(bo) sizeof(struct xwmm_bma_orderlist[(bo) + 1U])
#define XWMM_BMA_BCB_SIZE(bo) sizeof(struct xwmm_bma_bcb[1U << (bo)])

/**
 * @brief 定义伙伴算法内存块分配器结构体的RAW内存空间，
 *        用于初始化伙伴算法内存块分配器结构体
 * @param[in] name: 内存数组名
 * @param[in] blkodr: 伙伴算法内存块分配器中单位内存块的数量，以2的blkodr次方形式表示
 */
#define XWMM_BMA_RAWOBJ_DEF(name, blkodr) \
        xwu8_t name[sizeof(struct xwmm_bma) + \
                    XWMM_BMA_ORDERLIST_SIZE(blkodr) + XWMM_BMA_BCB_SIZE(blkodr)]

/**
 * @brief 阶链表
 */
struct xwmm_bma_orderlist {
        struct xwlib_bclst_head head; /**< 链表头 */
};

/**
 * @brief 块控制块
 */
struct xwmm_bma_bcb {
        xwu8_t order; /**< 块的阶:
                           0~126: 阶数
                           127: 块已经被前面的块合并
                           bit(7): 块正在使用中 */
};

/**
 * @brief 伙伴算法内存块分配器
 */
struct xwmm_bma {
        struct xwmm_zone zone; /**< 内存区域 */
        const char * name; /**< 名字 */
        xwsz_t blksize; /**< 单位块的大小（单位：字节） */
        xwsq_t blkodr; /**< 单位块的数量，以2的blkodr次方的形式表示 */
        struct xwos_splk lock; /**< 保护 `orderlists` 以及 `bcbs` 的锁 */
        struct xwmm_bma_orderlist * orderlists; /**< 阶链表数组指针 */
        struct xwmm_bma_bcb * bcbs; /**< 内存块控制块数组指针 */
};

/**
 * @brief XWMM API：初始化伙伴算法内存块分配器
 * @param[in] bma: 伙伴算法内存块分配器的指针
 * @param[in] name: 名字
 * @param[in] origin: 内存区域的起始地址
 * @param[in] size: 内存区域的大小
 * @param[in] blksize: 伙伴算法内存块分配器中单位内存块的大小
 * @param[in] blkodr: 伙伴算法内存块分配器中单位内存块的数量，以2的blkodr次方形式表示
 * @return 错误码
 * @retval -ESIZE: 内存区域大小不匹配
 * @note
 * + 单位内存块的数量只能是2的n次方，即 `2, 4, 8, 16, 32, 64, 128, ...` ，对应的 `blkodr`
 *   分别为 `1, 2, 3, 4, 5, 6, 7, ...` ；
 * + 内存区域大小必须满足关系： `size == (blksize * (1 << blkodr))` 。
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：不可重入
 */
xwer_t xwmm_bma_init(struct xwmm_bma * bma, const char * name,
                     xwptr_t origin, xwsz_t size,
                     xwsz_t blksize, xwsz_t blkodr);

/**
 * @brief XWMM API：申请一块连续的内存
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] order: 块数量的阶，内存块大小： `((1 << order) * bma->blksize)`
 * @param[out] membuf: 指向地址缓存的指针，通过此指针缓存返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -ERANGE: order无效
 * @retval -ENOMEM: 内存不足
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 */
xwer_t xwmm_bma_alloc(struct xwmm_bma * bma, xwsq_t order, void ** membuf);

/**
 * @brief XWMM API：释放内存块
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] mem: 内存块的首地址指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 参数错误
 * @retval -ERANGE: 内存块不属于指定的伙伴算法内存块分配器对象
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 */
xwer_t xwmm_bma_free(struct xwmm_bma * bma, void * mem);

/**
 * @brief XWMM API：申请指定大小的连续内存
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
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
xwer_t xwmm_bma_malloc(struct xwmm_bma * bma, xwsz_t size, void ** membuf);


/**
 * @brief XWMM API：调整内存大小
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
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
 *   + 当 `*membuf` 为 `NULL` ，此函数等价于 `xwmm_bma_mealloc(mp, size, membuf)` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 为 `0` ，
 *     此函数等价于 `xwmm_bma_free(mp, *membuf)` ，并且通过 `*membuf` 返回 `NULL` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 比之前的小 ，此函数不重新申请内存，直接返回原来的 `*membuf` 以及 `XWOK` ；
 *   + 当 `*membuf` 不为 `NULL` 且 `size` 比之前的大 ，此函数会尝试重新申请内存：
 *     + 如果申请失败，原来的内存不会受影响，且通过 `*membuf` 返回 `NULL` 以及 错误码；
 *     + 如果申请成功，会将旧内存空间的内容移动到新内存空间内，然后返回新的 `*membuf` 以及 `XWOK` 。
 */
xwer_t xwmm_bma_realloc(struct xwmm_bma * bma, xwsz_t size, void ** membuf);

/**
 * @brief XWMM API：申请对齐的内存
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
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
 * + 此API类似于C标准中的 `aligned_alloc()` 函数：
 *   + `alignment` 如果比 `XWMM_ALIGNMENT` 小，会被扩大为 `XWMM_ALIGNMENT` ：
 *   + `alignment` 只能是2的n次方：
 *   + 若size小于 `alignment` ， `size` 会被扩大为 `alignment` ，
 *   + 若size大于 `alignment` ， `size` 会向上对齐到2的n次方，此时也一定为 `alignment` 的整数倍。
 * + 申请内存失败时，此函数不会修改 `*membuf` 的值。
 */
xwer_t xwmm_bma_memalign(struct xwmm_bma * bma, xwsz_t alignment, xwsz_t size,
                         void ** membuf);

/**
 * @brief XWMM API：返回该块内存实际可用字节数
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] mem: 内存块的起始地址
 * @param[out] size: 指向缓冲区的指针，通过此缓冲区返回内存快实际大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ERANGE: 内存块不在BMA空间
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 * @details
 * + 此API类似于GNUC中的 `malloc_usable_size()` 函数。
 */
xwer_t xwmm_bma_malloc_usable_size(struct xwmm_bma * bma, void * mem, xwsz_t * size);

/**
 * @} xwmm_bma
 */

#endif /* xwos/mm/bma.h */
