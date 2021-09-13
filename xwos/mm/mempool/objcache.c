 /**
 * @file
 * @brief 玄武OS内存管理：对象缓存
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/lfq.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/mempool/page.h>
#include <xwos/mm/mempool/objcache.h>

static __xwos_code
void xwmm_mempool_objcache_page_init(struct xwmm_mempool_objcache * oc,
                                     struct xwmm_mempool_page * pg);

static __xwos_code
xwsz_t xwmm_mempool_objcache_free_idle_page(struct xwmm_mempool_objcache * oc,
                                            xwsz_t nr);

static __xwos_code
void xwmm_mempool_objcache_page_put(struct xwmm_mempool_objcache * oc,
                                    struct xwmm_mempool_page * pg);

static __xwos_code
xwer_t xwmm_mempool_objcache_page_get(struct xwmm_mempool_objcache * oc,
                                      struct xwmm_mempool_page ** pgbuf);

static __xwos_code
xwer_t xwmm_mempool_objcache_i_a_malloc(void * this, xwsz_t size, void ** membuf);

static __xwos_code
xwer_t xwmm_mempool_objcache_i_a_free(void * this, void * mem);

/**
 * @brief 初始化对象缓存
 * @param[in] oc: 对象缓存的指针
 * @param[in] pa: 页分配器的指针
 * @param[in] name: 名字
 * @param[in] objsize: 对象的大小
 * @param[in] alignment: 对齐的大小
 * @param[in] pg_order: 每次申请的页的数量的阶
 * @param[in] ctor: 构造函数
 * @param[in] dtor: 析构函数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EALIGN: 内存区域没有对齐
 */
__xwos_code
xwer_t xwmm_mempool_objcache_init(struct xwmm_mempool_objcache * oc,
                                  struct xwmm_mempool_page_allocator * pa,
                                  const char * name,
                                  xwsz_t objsize, xwsz_t alignment, xwsq_t pg_order,
                                  ctor_f ctor, dtor_f dtor)
{
        xwsz_t nr, pgsize;

/*
 * ----------------------------
 * | struct xwmm_mempool_page |
 * -----------------------------------------------------------------------------
 * |  attr.objcache.objhead   |->| obj | a | obj | a | obj | a | ...
 * |--------------------------|-------------------------------------------------
 * | ...                      |      |       ^ |       ^ |       ^
 * |--------------------------|      |_______| |_______| |_______|
 * obj: object
 * a: alignment area
 */

        objsize = XWBOP_ALIGN(objsize, alignment);
        if (ctor) {
                xwu8_t obj[objsize];
                ctor((void *)obj);
                oc->backup = *((xwptr_t *)obj);
        } else {
                oc->backup = 0;
        }

        pgsize = pa->pgsize << pg_order;
        nr = pgsize / objsize;

        oc->name = name;
        oc->pa = pa;
        oc->objsize = objsize;
        oc->alignment = alignment;
        oc->pg_order = pg_order;
        oc->pg_objnr = nr;
        oc->ctor = ctor;
        oc->dtor = dtor;
        oc->capacity = 0;
        oc->idleness = 0;
        oc->reserved = 0;
        xwlib_bclst_init_head(&oc->page_list.full);
        xwlib_bclst_init_head(&oc->page_list.available);
        xwlib_bclst_init_head(&oc->page_list.idle);
        xwos_sqlk_init(&oc->page_list.lock);
        oc->i_a.malloc = xwmm_mempool_objcache_i_a_malloc;
        oc->i_a.free = xwmm_mempool_objcache_i_a_free;

        return XWOK;
}

/**
 * @brief 初始化新页
 * @param[in] oc: 对象缓存的指针
 * @param[in] pg: 页控制块的指针
 */
static __xwos_code
void xwmm_mempool_objcache_page_init(struct xwmm_mempool_objcache * oc,
                                     struct xwmm_mempool_page * pg)
{
        xwsz_t i, loop;
        xwptr_t curr, next;

        xwlib_bclst_init_node(&pg->attr.objcache.node);
        pg->attr.objcache.refcnt = 0;
        pg->data.value = oc->objsize;

        /* 构造所有对象 */
        if (oc->ctor) {
                curr = pg->mapping;
                next = curr;
                loop = oc->pg_objnr;
                for (i = 0; i < loop; i++) {
                        next += oc->objsize;
                        oc->ctor((void *)curr);
                        curr = next;
                }
        }

        /* 初始化无锁队列 */
        xwlib_lfq_init(&pg->attr.objcache.objhead);
        curr = pg->mapping;
        next = curr;
        loop = oc->pg_objnr;
        for (i = 0; i < loop; i++) {
                next += oc->objsize;
                xwlib_lfq_init((xwlfq_a *)curr);
                xwlib_lfq_push(&pg->attr.objcache.objhead,
                               (xwlfq_a *)curr);
                curr = next;
        }
}

/**
 * @brief 释放空闲页
 * @param[in] oc: 对象缓存的指针
 * @param[in] nr: 期望释放的数量
 * @return 实际释放的数量
 */
static __xwos_code
xwsz_t xwmm_mempool_objcache_free_idle_page(struct xwmm_mempool_objcache * oc,
                                            xwsz_t nr)
{
        struct xwmm_mempool_page * pg;
        xwreg_t flag;
        xwsz_t real;

        real = 0;
        while (real < nr) {
                xwos_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
                if (xwlib_bclst_tst_empty(&oc->page_list.idle)) {
                        xwos_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
                        break;
                }
                pg = xwlib_bclst_last_entry(&oc->page_list.idle,
                                            struct xwmm_mempool_page,
                                            attr.objcache.node);
                xwlib_bclst_del_init(&pg->attr.objcache.node);
                xwos_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
                xwmm_mempool_page_free(oc->pa, pg);
                xwaop_sub(xwsz, &oc->capacity, oc->pg_objnr, NULL, NULL);
                xwaop_sub(xwsz, &oc->idleness, oc->pg_objnr, NULL, NULL);
                real++;
        }
        return real;
}

/**
 * @brief 减少页的引用计数
 * @param[in] oc: 对象缓存的指针
 * @param[in] pg: 页控制块的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: 失败
 */
static __xwos_code
void xwmm_mempool_objcache_page_put(struct xwmm_mempool_objcache * oc,
                                    struct xwmm_mempool_page * pg)
{
        xwreg_t flag;

        xwos_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
        XWOS_BUG_ON(0 == pg->attr.objcache.refcnt);
        pg->attr.objcache.refcnt--;
        xwlib_bclst_del_init(&pg->attr.objcache.node);
        if (0 == pg->attr.objcache.refcnt) {
                xwlib_bclst_add_head(&oc->page_list.idle,
                                     &pg->attr.objcache.node);
        } else {
                xwlib_bclst_add_head(&oc->page_list.available,
                                     &pg->attr.objcache.node);
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
}

/**
 * @brief 获取一页，准备从页内存中分配对象
 * @param[in] oc: 对象缓存的指针
 * @param[out] pgbuf: 指向缓冲区的指针，通过此缓冲区返回页控制块的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
static __xwos_code
xwer_t xwmm_mempool_objcache_page_get(struct xwmm_mempool_objcache * oc,
                                      struct xwmm_mempool_page ** pgbuf)
{
        struct xwmm_mempool_page * pg;
        xwreg_t flag;
        xwer_t rc;

        xwos_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
        if (!xwlib_bclst_tst_empty(&oc->page_list.available)) {
                pg = xwlib_bclst_first_entry(&oc->page_list.available,
                                             struct xwmm_mempool_page,
                                             attr.objcache.node);
                xwlib_bclst_del_init(&pg->attr.objcache.node);
                xwos_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
        } else if (!xwlib_bclst_tst_empty(&oc->page_list.idle)) {
                pg = xwlib_bclst_first_entry(&oc->page_list.idle,
                                             struct xwmm_mempool_page,
                                             attr.objcache.node);
                xwlib_bclst_del_init(&pg->attr.objcache.node);
                xwos_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
        } else {
                xwos_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
                rc = xwmm_mempool_page_allocate(oc->pa, oc->pg_order, &pg);
                if (rc < 0) {
                        goto err_pg_alloc;
                }
                xwaop_add(xwsz, &oc->capacity, oc->pg_objnr, NULL, NULL);
                xwaop_add(xwsz, &oc->idleness, oc->pg_objnr, NULL, NULL);
                xwmm_mempool_objcache_page_init(oc, pg);
        }
        *pgbuf = pg;
        return XWOK;

err_pg_alloc:
        return rc;
}

/**
 * @brief 从对象缓存中申请一个对象
 * @param[in] oc: 对象缓存的指针
 * @param[out] objbuf: 指向缓冲区的指针，此缓冲区被用于返回对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
__xwos_code
xwer_t xwmm_mempool_objcache_alloc(struct xwmm_mempool_objcache * oc, void ** objbuf)
{
        struct xwmm_mempool_page * pg;
        xwlfq_t * obj;
        xwreg_t flag;
        xwer_t rc;

        rc = xwmm_mempool_objcache_page_get(oc, &pg);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_page_get;
        }

        xwaop_sub(xwsz, &oc->idleness, 1, NULL, NULL);
        obj = xwlib_lfq_pop(&pg->attr.objcache.objhead);
        XWOS_BUG_ON(NULL == obj);
        *obj = oc->backup;
        *objbuf = (void *)obj;

        xwos_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
        XWOS_BUG_ON(pg->attr.objcache.refcnt >= oc->pg_objnr);
        pg->attr.objcache.refcnt++;
        xwlib_bclst_del_init(&pg->attr.objcache.node);
        if (pg->attr.objcache.refcnt == oc->pg_objnr) {
                xwlib_bclst_add_tail(&oc->page_list.full, &pg->attr.objcache.node);
        } else {
                xwlib_bclst_add_head(&oc->page_list.available, &pg->attr.objcache.node);
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);

        return XWOK;

err_page_get:
        return rc;
}

/**
 * @brief 释放对象到对象缓存中
 * @param[in] oc: 对象缓存的指针
 * @param[in] obj: 对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
__xwos_code
xwer_t xwmm_mempool_objcache_free(struct xwmm_mempool_objcache * oc, void * obj)
{
        xwlfq_a * lfq;
        struct xwmm_mempool_page * pg;
        xwsz_t reserved;
        xwsz_t idleness;
        xwptr_t offset, origin;
        xwer_t rc;

        rc = xwmm_mempool_page_find(oc->pa, obj, &pg);
        if (rc < 0) {
                goto err_pg_find;
        }

        offset = ((xwptr_t)obj - pg->mapping) % oc->objsize;
        origin = (xwptr_t)obj - offset;
        obj = (void *)origin;

        if (oc->dtor) {
                oc->dtor(obj);
        }

        lfq = (xwlfq_a *)obj;
        xwlib_lfq_push(&pg->attr.objcache.objhead, lfq);

        xwmm_mempool_objcache_page_put(oc, pg);
        xwaop_add(xwsz, &oc->idleness, 1, &idleness, NULL);
        reserved = xwaop_load(xwsz, &oc->reserved, xwmb_modr_relaxed);

        if (reserved + oc->pg_objnr <= idleness) {
                xwsz_t nr;

                nr = (idleness - reserved) / oc->pg_objnr;
                xwmm_mempool_objcache_free_idle_page(oc, nr);
        }

        return XWOK;

err_pg_find:
        return rc;
}

/**
 * @brief 预留对象到对象缓存中
 * @param[in] oc: 对象缓存的指针
 * @param[in] reserved: 预留的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
__xwos_code
xwer_t xwmm_mempool_objcache_reserve(struct xwmm_mempool_objcache * oc,
                                     xwsz_t reserved)
{
        struct xwmm_mempool_page * pg;
        xwsz_t capacity;
        xwsz_t idleness;
        xwreg_t flag;
        xwer_t rc;

        reserved = XWBOP_DIV_ROUND_UP(reserved, oc->pg_objnr);
        xwaop_write(xwsz, &oc->reserved, reserved, NULL);
        capacity = xwaop_load(xwsz, &oc->capacity, xwmb_modr_relaxed);
        idleness = xwaop_load(xwsz, &oc->idleness, xwmb_modr_relaxed);
        if (capacity < reserved) {
                rc = xwmm_mempool_page_allocate(oc->pa, oc->pg_order, &pg);
                if (rc < 0) {
                        goto err_pg_alloc;
                }
                xwaop_add(xwsz, &oc->capacity, oc->pg_objnr, NULL, NULL);
                xwaop_add(xwsz, &oc->idleness, oc->pg_objnr, NULL, NULL);
                xwmm_mempool_objcache_page_init(oc, pg);
                xwos_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
                xwlib_bclst_add_tail(&oc->page_list.idle, &pg->attr.objcache.node);
                xwos_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
        } else if (reserved + oc->pg_objnr < idleness) {
                xwsz_t nr;

                nr = (idleness - reserved) / oc->pg_objnr;
                xwmm_mempool_objcache_free_idle_page(oc, nr);
        } else {
        }
        return XWOK;

err_pg_alloc:
        return rc;
}

/**
 * @brief 获取对象缓存的容量
 * @param[in] oc: 对象缓存的指针
 * @param[out] capacity: 指向缓冲区的指针，通过此缓冲区返回对象缓存的容量
 * @return 错误码
 * @retval XWOK: 没有错误
 */
__xwos_code
xwer_t xwmm_mempool_objcache_get_capacity(struct xwmm_mempool_objcache * oc,
                                          xwsz_t * capacity)
{
        *capacity = xwaop_load(xwsz, &oc->capacity, xwmb_modr_relaxed);
        return XWOK;
}

/**
 * @brief i_allocator接口函数：申请内存
 * @param[in] this: this指针(页分配器)
 * @param[in] size: 申请的大小
 * @param[out] membuf: 指向缓冲区的指针，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
static __xwos_code
xwer_t xwmm_mempool_objcache_i_a_malloc(void * this, xwsz_t size, void ** membuf)
{
        struct xwmm_mempool_objcache * oc;
        xwer_t rc;

        oc = this;
        if (size > oc->objsize) {
                rc = -ESIZE;
                goto err_size;
        }
        rc = xwmm_mempool_objcache_alloc(oc, membuf);
        if (rc < 0) {
                goto err_alloc;
        }
        return XWOK;

err_alloc:
err_size:
        return rc;
}

/**
 * @brief i_allocator接口函数：释放内存
 * @param[in] this: this指针(页分配器)
 * @param[in] mem: 内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
static __xwos_code
xwer_t xwmm_mempool_objcache_i_a_free(void * this, void * mem)
{
        struct xwmm_mempool_objcache * oc;
        xwer_t rc;

        oc = this;
        rc = xwmm_mempool_objcache_free(oc, mem);
        return rc;
}
