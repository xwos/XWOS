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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化对象缓存
 * @param oc: (I) 对象缓存的指针
 * @param pa: (I) 页分配器的指针
 * @param name: (I) 名字
 * @param objsize: (I) 对象的大小
 * @param alignment: (I) 对齐的大小
 * @param pg_order: (I) 每次申请的页的数量的阶
 * @param ctor: (I) 构造函数
 * @param dtor: (I) 析构函数
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

        XWOS_VALIDATE((oc), "nullptr", -EFAULT);
        XWOS_VALIDATE((pa), "nullptr", -EFAULT);
        XWOS_VALIDATE(((alignment & XWMM_ALIGNMENT_MASK) == 0),
                      "not-aligned", -EALIGN);

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

        objsize = ALIGN(objsize, alignment);
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
        xwlk_sqlk_init(&oc->page_list.lock);
        oc->i_a.malloc = xwmm_mempool_objcache_i_a_malloc;
        oc->i_a.free = xwmm_mempool_objcache_i_a_free;

        return XWOK;
}

/**
 * @brief 销毁对象缓存
 * @param oc: (I) 对象缓存的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
__xwos_code
xwer_t xwmm_mempool_objcache_destroy(struct xwmm_mempool_objcache * oc)
{
        XWOS_UNUSED(oc);
        return XWOK;
}

/**
 * @brief 动态创建对象缓存
 * @param ocbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象缓存的指针
 * @param pa: (I) 页分配器的指针
 * @param name: (I) 名字
 * @param objsize: (I) 对象的大小
 * @param alignment: (I) 对齐的大小
 * @param pg_order: (I) 每次申请的页的数量的阶
 * @param ctor: (I) 构造函数
 * @param dtor: (I) 析构函数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EALIGN: 内存区域没有对齐
 */
__xwos_code
xwer_t xwmm_mempool_objcache_create(struct xwmm_mempool_objcache ** ocbuf,
                                    struct xwmm_mempool_page_allocator * pa,
                                    const char * name,
                                    xwsz_t objsize, xwsz_t alignment, xwsq_t pg_order,
                                    ctor_f ctor, dtor_f dtor)
{
        void * mem;
        struct xwmm_mempool_objcache * oc;
        xwer_t rc;

        XWOS_VALIDATE((ocbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((pa), "nullptr", -EFAULT);
        XWOS_VALIDATE(((alignment & XWMM_ALIGNMENT_MASK) == 0),
                      "not-aligned", -EALIGN);

        rc = xwmm_kma_alloc(sizeof(struct xwmm_mempool_objcache), XWMM_ALIGNMENT,
                            &mem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_kma_alloc;
        }
        oc = mem;

        rc = xwmm_mempool_objcache_init(oc, pa, name, objsize, alignment, pg_order,
                                        ctor, dtor);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_oc_init;
        }
        *ocbuf = oc;

        return XWOK;

err_oc_init:
        xwmm_kma_free(oc);
err_kma_alloc:
        return rc;
}

/**
 * @brief 删除动态创建的对象缓存
 * @param oc: (I) 对象缓存的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
__xwos_code
xwer_t xwmm_mempool_objcache_delete(struct xwmm_mempool_objcache * oc)
{
        XWOS_VALIDATE((oc), "nullptr", -EFAULT);

        xwmm_kma_free(oc);
        return XWOK;
}

/**
 * @brief 初始化新页
 * @param oc: (I) 对象缓存的指针
 * @param pg: (I) 页控制块的指针
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
                xwlib_lfq_init((__xwcc_atomic xwlfq_t *)curr);
                xwlib_lfq_push(&pg->attr.objcache.objhead,
                               (__xwcc_atomic xwlfq_t *)curr);
                curr = next;
        }
}

/**
 * @brief 释放空闲页
 * @param oc: (I) 对象缓存的指针
 * @param nr: (I) 期望释放的数量
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
                xwlk_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
                if (xwlib_bclst_tst_empty(&oc->page_list.idle)) {
                        xwlk_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
                        break;
                }
                pg = xwlib_bclst_last_entry(&oc->page_list.idle,
                                            struct xwmm_mempool_page,
                                            attr.objcache.node);
                xwlib_bclst_del_init(&pg->attr.objcache.node);
                xwlk_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
                xwmm_mempool_page_free(oc->pa, pg);
                xwaop_sub(xwsz_t, &oc->capacity, oc->pg_objnr, NULL, NULL);
                xwaop_sub(xwsz_t, &oc->idleness, oc->pg_objnr, NULL, NULL);
                real++;
        }
        return real;
}

/**
 * @brief 减少页的引用计数
 * @param oc: (I) 对象缓存的指针
 * @param pg: (I) 页控制块的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: 失败
 */
static __xwos_code
void xwmm_mempool_objcache_page_put(struct xwmm_mempool_objcache * oc,
                                    struct xwmm_mempool_page * pg)
{
        xwreg_t flag;

        xwlk_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
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
        xwlk_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
}

/**
 * @brief 获取一页，准备从页内存中分配对象
 * @param oc: (I) 对象缓存的指针
 * @param pgbuf: (O) 指向缓冲区的指针，通过此缓冲区返回页控制块的指针
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

        xwlk_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
        if (!xwlib_bclst_tst_empty(&oc->page_list.available)) {
                pg = xwlib_bclst_first_entry(&oc->page_list.available,
                                             struct xwmm_mempool_page,
                                             attr.objcache.node);
                xwlib_bclst_del_init(&pg->attr.objcache.node);
                xwlk_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
        } else if (!xwlib_bclst_tst_empty(&oc->page_list.idle)) {
                pg = xwlib_bclst_first_entry(&oc->page_list.idle,
                                             struct xwmm_mempool_page,
                                             attr.objcache.node);
                xwlib_bclst_del_init(&pg->attr.objcache.node);
                xwlk_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
        } else {
                xwlk_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
                rc = xwmm_mempool_page_allocate(oc->pa, oc->pg_order, &pg);
                if (rc < 0) {
                        goto err_pg_alloc;
                }
                xwaop_add(xwsz_t, &oc->capacity, oc->pg_objnr, NULL, NULL);
                xwaop_add(xwsz_t, &oc->idleness, oc->pg_objnr, NULL, NULL);
                xwmm_mempool_objcache_page_init(oc, pg);
        }
        *pgbuf = pg;
        return XWOK;

err_pg_alloc:
        return rc;
}

/**
 * @brief 从对象缓存中申请一个对象
 * @param oc: (I) 对象缓存的指针
 * @param objbuf: (O) 指向缓冲区的指针，此缓冲区被用于返回对象的指针
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

        XWOS_VALIDATE((oc), "nullptr", -EFAULT);
        XWOS_VALIDATE((objbuf), "nullptr", -EFAULT);

        rc = xwmm_mempool_objcache_page_get(oc, &pg);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_page_get;
        }

        xwaop_sub(xwsz_t, &oc->idleness, 1, NULL, NULL);
        obj = xwlib_lfq_pop(&pg->attr.objcache.objhead);
        XWOS_BUG_ON(NULL == obj);
        *obj = oc->backup;
        *objbuf = (void *)obj;

        xwlk_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
        XWOS_BUG_ON(pg->attr.objcache.refcnt >= oc->pg_objnr);
        pg->attr.objcache.refcnt++;
        xwlib_bclst_del_init(&pg->attr.objcache.node);
        if (pg->attr.objcache.refcnt == oc->pg_objnr) {
                xwlib_bclst_add_tail(&oc->page_list.full, &pg->attr.objcache.node);
        } else {
                xwlib_bclst_add_head(&oc->page_list.available, &pg->attr.objcache.node);
        }
        xwlk_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);

        return XWOK;

err_page_get:
        return rc;
}

/**
 * @brief 释放对象到对象缓存中
 * @param oc: (I) 对象缓存的指针
 * @param obj: (I) 对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
__xwos_code
xwer_t xwmm_mempool_objcache_free(struct xwmm_mempool_objcache * oc, void * obj)
{
        __xwcc_atomic xwlfq_t * lfq;
        struct xwmm_mempool_page * pg;
        xwsz_t reserved;
        xwsz_t idleness;
        xwer_t rc;

        XWOS_VALIDATE((oc), "nullptr", -EFAULT);
        XWOS_VALIDATE((obj), "nullptr", -EFAULT);

        rc = xwmm_mempool_page_find(oc->pa, obj, &pg);
        if (rc < 0) {
                goto err_pg_find;
        }

        if (((xwptr_t)obj - (xwptr_t)pg->attr.objcache.objhead) % oc->objsize != 0) {
                rc = -EALIGN;
                goto err_not_aligned;
        }

        if (oc->dtor) {
                oc->dtor(obj);
        }

        lfq = (__xwcc_atomic xwlfq_t *)obj;
        xwlib_lfq_push(&pg->attr.objcache.objhead, lfq);

        xwmm_mempool_objcache_page_put(oc, pg);
        xwaop_add(xwsz_t, &oc->idleness, 1, &idleness, NULL);
        reserved = xwaop_load(xwsz_t, &oc->reserved, xwmb_modr_relaxed);

        if (reserved + oc->pg_objnr <= idleness) {
                xwsz_t nr;

                nr = (idleness - reserved) / oc->pg_objnr;
                xwmm_mempool_objcache_free_idle_page(oc, nr);
        }

        return XWOK;

err_not_aligned:
err_pg_find:
        return rc;
}

/**
 * @brief 预留对象到对象缓存中
 * @param oc: (I) 对象缓存的指针
 * @param reserved: (I) 预留的数量
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

        XWOS_VALIDATE((oc), "nullptr", -EFAULT);

        reserved = DIV_ROUND_UP(reserved, oc->pg_objnr);
        xwaop_write(xwsz_t, &oc->reserved, reserved, NULL);
        capacity = xwaop_load(xwsz_t, &oc->capacity, xwmb_modr_relaxed);
        idleness = xwaop_load(xwsz_t, &oc->idleness, xwmb_modr_relaxed);
        if (capacity < reserved) {
                rc = xwmm_mempool_page_allocate(oc->pa, oc->pg_order, &pg);
                if (rc < 0) {
                        goto err_pg_alloc;
                }
                xwaop_add(xwsz_t, &oc->capacity, oc->pg_objnr, NULL, NULL);
                xwaop_add(xwsz_t, &oc->idleness, oc->pg_objnr, NULL, NULL);
                xwmm_mempool_objcache_page_init(oc, pg);
                xwlk_sqlk_wr_lock_cpuirqsv(&oc->page_list.lock, &flag);
                xwlib_bclst_add_tail(&oc->page_list.idle, &pg->attr.objcache.node);
                xwlk_sqlk_wr_unlock_cpuirqrs(&oc->page_list.lock, flag);
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
 * @param oc: (I) 对象缓存的指针
 * @param capacity: (O) 指向缓冲区的指针，通过此缓冲区返回对象缓存的容量
 * @return 错误码
 * @retval XWOK: 没有错误
 */
__xwos_code
xwer_t xwmm_mempool_objcache_get_capacity(struct xwmm_mempool_objcache * oc,
                                          xwsz_t * capacity)
{
        XWOS_VALIDATE((oc), "nullptr", -EFAULT);
        XWOS_VALIDATE((capacity), "nullptr", -EFAULT);

        *capacity = xwaop_load(xwsz_t, &oc->capacity, xwmb_modr_relaxed);
        return XWOK;
}

/**
 * @brief i_allocator接口函数：申请内存
 * @param this: (I) this指针(页分配器)
 * @param size: (I) 申请的大小
 * @param membuf: (O) 指向缓冲区的指针，通过此缓冲区返回申请到的内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
static __xwos_code
xwer_t xwmm_mempool_objcache_i_a_malloc(void * this, xwsz_t size, void ** membuf)
{
        struct xwmm_mempool_objcache * oc;
        xwer_t rc;

        XWOS_VALIDATE((this), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

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
 * @param this: (I) this指针(页分配器)
 * @param mem: (I) 内存的首地址
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
static __xwos_code
xwer_t xwmm_mempool_objcache_i_a_free(void * this, void * mem)
{
        struct xwmm_mempool_objcache * oc;
        xwer_t rc;

        XWOS_VALIDATE((this), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);

        oc = this;
        rc = xwmm_mempool_objcache_free(oc, mem);
        return rc;
}
