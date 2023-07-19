/**
 * @file
 * @brief XWOS内存管理：伙伴算法内存块分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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
 * 伙伴算法分配器在分配时，会将内存不断地二等分，直到切割到能满足要求的最小内存大小为止。
 * 释放时会检查与之相邻并等长的内存块（称为伙伴）是否也空闲，
 * 如果是，就和“伙伴”合并成更大的内存块，
 * 然后继续检测合并后的内存块是否也存在可以合并的伙伴，一直向上合并到不能合并为止。
 *
 * + 优点：
 *   + 支持释放操作；
 *   + 支持大小不固定的内存申请操作；
 *   + 反复申请与释放不会造成内存碎片。
 * + 缺点：
 *   + 代码稍微复杂；
 *  + 因为存在合并与切割的循环，申请与释放操作的所需要的时间不是特别稳定；
 *  + 内存大小固定为2的指数，如果申请的内存过小，会造成内存浪费。
 * + 适用性：对内存复用性要求高的应用场合。
 * + 上下文的安全性：在任何上下文（中断、中断底半部、线程）都是安全的。
 * @{
 */

/* #define XWMM_BMA_LOG */

#define XWMM_BMA_MAX_ORDER              (126) /**< 最大的阶 */
#define XWMM_BMA_COMBINED               (127) /**< 块已被合并 */
#define XWMM_BMA_ORDER_MASK             (0x7FU) /**< 阶的掩码 */
#define XWMM_BMA_INUSED                 (XWBOP_BIT(7)) /**< 块正在使用的标记 */

#if defined(XWMM_BMA_LOG)
#  define xwmm_bmalogf(lv, fmt, ...) xwlogf(lv, fmt, ##__VA_ARGS__)
#else
#  define xwmm_bmalogf(lv, fmt, ...)
#endif

/**
 * @brief 定义伙伴算法内存块分配器结构体类型
 * @param[in] name: 结构体名
 * @param[in] blkodr: 伙伴算法内存块分配器中单位内存块的数量，以2的blkodr次方形式表示
 */
// cppcheck-suppress [misra-c2012-20.7]
#define XWMM_BMA_TYPEDEF(name, blkodr) \
        struct name { \
                struct xwmm_bma bma[1U]; \
                struct xwmm_bma_orderlist orderlist[(blkodr) + 1U]; \
                struct xwmm_bma_bcb bcb[1U << (blkodr)]; \
        }

/**
 * @brief 定义伙伴算法内存块分配器结构体的RAW内存空间，
 *        用于初始化伙伴算法内存块分配器结构体
 * @param[in] name: 内存数组名
 * @param[in] blkodr: 伙伴算法内存块分配器中单位内存块的数量，以2的blkodr次方形式表示
 */
#define XWMM_BMA_DEF(name, blkodr) \
        xwu8_t name[sizeof(XWMM_BMA_TYPEDEF(name, blkodr))]

/**
 * @brief 阶链表
 */
struct xwmm_bma_orderlist {
        struct xwlib_bclst_head head; /**< 链表头 */
        struct xwos_splk lock; /**< 保护list的锁 */
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
 * @} xwmm_bma
 */

#endif /* xwos/mm/bma.h */
