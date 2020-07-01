/**
 * @file
 * @brief XuanWuOS的内存管理机制：对象缓存
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mm_mempool_objcache_h__
#define __xwos_mm_mempool_objcache_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/mm/common.h>
#include <xwos/mm/mempool/i_allocator.h>
#include <xwos/mm/mempool/page.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 对象缓存
 */
struct xwmm_mempool_objcache {
        struct xwmm_mempool_i_allocator i_a; /**< implements xwmm_mempool_i_allocator */
        struct xwmm_mempool_page_allocator * pa; /**< 页分配器 */
        const char * name; /**< 名字 */
        xwsz_t objsize; /**< 对象大小 */
        xwsz_t alignment; /**< 对齐的大小 */
        xwsq_t pg_order; /**< 每次申请的页的数量的阶(2的n次方) */
        xwsz_t pg_objnr; /**< 每一页中可容纳多少对象 */
        xwptr_t backup; /**< 备份值：
                             - 每个对象的第一段内存字(word)用作无锁队列的
                               链表指针，当分配卡片时，用备份值恢复；
                             - 所有对象初始化后的第一个字(word)必须相同，
                               以保证备份值唯一。
                             */
        ctor_f ctor; /**< 构造函数 */
        dtor_f dtor; /**< 析构函数 */
        __atomic xwsz_t capacity; /**< 总的对象数量 */
        __atomic xwsz_t idleness; /**< 空闲的对象数量 */
        __atomic xwsz_t reserved; /**< 保留的对象数量 */

        struct {
                struct xwlib_bclst_head full; /**< 对象已完全被分配的页链表 */
                struct xwlib_bclst_head available; /**< 还有剩余对象未分配的页链表 */
                struct xwlib_bclst_head idle; /**< 未使用的页链表 */
                struct xwlk_sqlk lock; /**< 页链表锁 */
        } page_list; /**< 页链表 */
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
xwer_t xwmm_mempool_objcache_init(struct xwmm_mempool_objcache * oc,
                                  struct xwmm_mempool_page_allocator * pa,
                                  const char * name,
                                  xwsz_t objsize, xwsz_t alignment, xwsq_t pg_order,
                                  ctor_f ctor, dtor_f dtor);

__xwos_api
xwer_t xwmm_mempool_objcache_destroy(struct xwmm_mempool_objcache * oc);

__xwos_api
xwer_t xwmm_mempool_objcache_create(struct xwmm_mempool_objcache ** ocbuf,
                                    struct xwmm_mempool_page_allocator * pa,
                                    const char * name,
                                    xwsz_t objsize, xwsz_t alignment, xwsq_t pg_order,
                                    ctor_f ctor, dtor_f dtor);

__xwos_api
xwer_t xwmm_mempool_objcache_delete(struct xwmm_mempool_objcache * oc);

__xwos_api
xwer_t xwmm_mempool_objcache_alloc(struct xwmm_mempool_objcache * oc, void ** objbuf);

__xwos_api
xwer_t xwmm_mempool_objcache_free(struct xwmm_mempool_objcache * oc, void * obj);

__xwos_api
xwer_t xwmm_mempool_objcache_reserve(struct xwmm_mempool_objcache * oc,
                                     xwsz_t reserved);

__xwos_api
xwer_t xwmm_mempool_objcache_get_capacity(struct xwmm_mempool_objcache * oc,
                                          xwsz_t * capacity);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/mm/mempool/objcache.h */
