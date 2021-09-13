/**
 * @file
 * @brief 玄武OS内存管理：伙伴算法内存块分配器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
 * 此算法是在所有上下文（线程、中断、中断底半部）都是安全的。
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
 * @brief 定义伙伴算法内存块分配器结构体的RAW内存空间，
 *        用于初始化伙伴算法内存块分配器结构体
 * @param[in] mem: 内存数组名
 * @param[in] blkodr: 伙伴算法内存块分配器中单位内存块的数量，以2的blkodr次方形式表示
 */
#define XWMM_BMA_DEF(mem, blkodr) \
        xwu8_t mem[sizeof(struct xwmm_bma) + \
                   (sizeof(struct xwmm_bma_bcb) * (1 << (blkodr))) + \
                   (sizeof(struct xwmm_bma_orderlist) * ((blkodr) + 1))]

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
        xwu8_t rem[0]; /**< 结构体剩余的内存空间 */
};

xwer_t xwmm_bma_init(struct xwmm_bma * bma, const char * name,
                     xwptr_t origin, xwsz_t size,
                     xwsz_t blksize, xwsz_t blkodr);
xwer_t xwmm_bma_alloc(struct xwmm_bma * bma, xwsq_t order, void ** membuf);
xwer_t xwmm_bma_free(struct xwmm_bma * bma, void * mem);

/**
 * @} xwmm_bma
 */

#endif /* xwos/mm/bma.h */
