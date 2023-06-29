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

#ifndef __xwos_mm_mempool_page_h__
#define __xwos_mm_mempool_page_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwmm_mempool 内存池分配器
 * @ingroup xwmm
 * @{
 */

#include <xwos/mm/mempool/i_allocator.h>

#define XWMM_MEMPOOL_PAGE_ORDER_CMB             (XWSQ_MAX)
#define XWMM_MEMPOOL_PAGE_MAPPING_FREE          (0U)
#define XWMM_MEMPOOL_PAGE_MAPPING_INVAILD       (XWPTR_MAX)

/**
 * @brief 内存页
 */
struct xwmm_mempool_page {
        xwsq_t order; /**< 页的数量的阶。当取值 `XWMM_MEMPOOL_PAGE_ORDER_CMB` ，
                           表示页被合并在更大的页中 */
        xwptr_t mapping; /**< + 如果页被申请， `mapping` 为页的首地址；
                              + 如果页在阶红黑树中，
                                `mapping` 取值 `XWMM_MEMPOOL_PAGE_MAPPING_FREE` ；
                              + 如果页刚被申请还未映射地址，或合并在更大的页中，
                                `mapping` 取值 `XWMM_MEMPOOL_PAGE_MAPPING_INVAILD` 。 */
        union {
                struct {
                        struct xwlib_rbtree_node rbnode; /**< 红黑树节点 */
                        xwsq_t seq; /**< 序列值 */
                } free; /**< 空闲页 */
                struct {
                        struct xwlib_bclst_node node; /**< 页链表节点 */
                        atomic_xwlfq_t objhead; /**< 空闲对象无锁队列头 */
                        xwsq_t refcnt; /**< 页的引用计数（aka: 已被使用的对象数量）
                                            被 `xwmm_mempool_objcache.page_list.lock`
                                            保护。 */
                } objcache; /**< 页作为对象缓存 */
        } attr;

        union {
                void * pointer;
                xwsq_t value;
        } data; /**< 数据 */
};

struct xwmm_mempool_page_allocator;

/**
 * @brief 阶红黑树
 * @note
 * - 拥有相同阶(2的n次方)的页连接到同一颗阶红黑树上，关键字时页的序号
 */
struct xwmm_mempool_page_odrbtree {
        struct xwlib_rbtree tree; /**< 红黑树 */
        struct xwmm_mempool_page * leftmost; /**< 最小序号的页 */
        xwsq_t order; /**< 红黑树的阶 */
        struct xwmm_mempool_page_allocator * owner; /**< 包含此阶红黑树的页分配器 */
        struct xwos_sqlk lock; /**< 保护此结构体的锁 */
};

/**
 * @brief 页分配器
 */
struct xwmm_mempool_page_allocator {
        struct xwmm_mempool_i_allocator i_a; /**< implements xwmm_mempool_i_allocator */
        struct xwmm_zone zone; /**< 内存区域 */
        const char * name; /**< 名字 */
        xwsz_t pgsize; /**< 单位页的大小（单位：字节） */
        xwsq_t max_order; /**< 最大阶数 */
        struct xwmm_mempool_page_odrbtree * odrbtree; /**< 阶红黑树数组指针 */
        struct xwmm_mempool_page * pgarray; /**< 页数值指针 */
};

xwer_t xwmm_mempool_page_find(struct xwmm_mempool_page_allocator * pa,
                              void * mem,
                              struct xwmm_mempool_page ** pgbuf);
xwer_t xwmm_mempool_page_allocator_init(struct xwmm_mempool_page_allocator * pa,
                                        const char * name,
                                        xwptr_t origin, xwsz_t size, xwsz_t pgsize,
                                        struct xwmm_mempool_page_odrbtree * odrbtree,
                                        struct xwmm_mempool_page * pgarray);
xwer_t xwmm_mempool_page_allocate(struct xwmm_mempool_page_allocator * pa,
                                  xwsq_t order,
                                  struct xwmm_mempool_page ** pgbuf);
xwer_t xwmm_mempool_page_free(struct xwmm_mempool_page_allocator * pa,
                              struct xwmm_mempool_page * pg);

/**
 * @} xwmm_mempool
 */

#endif /* xwos/mm/mempool/page.h */
