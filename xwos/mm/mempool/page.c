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

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/mm/common.h>
#include <xwos/mm/mempool/i_allocator.h>
#include <xwos/mm/mempool/page.h>

/* #define XWMM_MEMPOOLLOGF */ /**< 调试日志开关 */
#ifdef XWMM_MEMPOOLLOGF
#  define xwmm_mempoollogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwmm_mempoollogf(lv, thd, fmt, ...)
#endif

static __xwos_code
xwer_t xwmm_mempool_page_i_a_malloc(void * this, xwsz_t size, void ** membuf);

static __xwos_code
xwer_t xwmm_mempool_page_i_a_free(void * this, void * mem);

static __xwos_code
void xwmm_mempool_page_divide_page(struct xwmm_mempool_page_allocator * pa,
                                   struct xwmm_mempool_page * pg,
                                   xwsq_t target_odr,
                                   struct xwmm_mempool_page_odrbtree * ot);

static __xwos_code
void xwmm_mempool_page_combine(struct xwmm_mempool_page_allocator * pa,
                               struct xwmm_mempool_page * pg);

static __xwos_code
xwer_t xwmm_mempool_page_odrbtree_add(struct xwmm_mempool_page_odrbtree * ot,
                                      struct xwmm_mempool_page * pg);

static __xwos_code
xwer_t xwmm_mempool_page_odrbtree_remove(struct xwmm_mempool_page_odrbtree * ot,
                                         struct xwmm_mempool_page * pg);

static __xwos_code
struct xwmm_mempool_page *
xwmm_mempool_page_odrbtree_choose(struct xwmm_mempool_page_odrbtree * ot);

static __xwos_code
xwsq_t xwmm_mempool_page_get_seq(struct xwmm_mempool_page_allocator * pa,
                                 struct xwmm_mempool_page * pg);

static __xwos_code
struct xwmm_mempool_page *
xwmm_mempool_mem_to_page(struct xwmm_mempool_page_allocator * pa, void * mem);

static __xwos_code
struct xwmm_mempool_page *
xwmm_mempool_page_get_buddy(struct xwmm_mempool_page_allocator * pa,
                            struct xwmm_mempool_page * pg);

/**
 * @brief XWMM API：初始化页分配器
 * @param[in] pa: 页分配器的指针
 * @param[in] name: 名字
 * @param[in] origin: 内存区域的起始地址
 * @param[in] size: 内存区域的总大小
 * @param[in] pgsize: 单位页的大小
 * @param[in] odrbtree: 阶红黑树数组的指针
 * @param[in] pgarray: 页控制块数组的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -E2SMALL: 内存区域太小
 * @retval -EALIGN: 内存区域没有对齐
 */
__xwos_api
xwer_t xwmm_mempool_page_allocator_init(struct xwmm_mempool_page_allocator * pa,
                                        const char * name,
                                        xwptr_t origin, xwsz_t size, xwsz_t pgsize,
                                        struct xwmm_mempool_page_odrbtree * odrbtree,
                                        struct xwmm_mempool_page * pgarray)
{
        xwsz_t i;
        xwssq_t order;
        xwsz_t nr;
        xwer_t rc;

        if (size < pgsize) {
                rc = -E2SMALL;
                goto err_mem2small;
        }
        // cppcheck-suppress [misra-c2012-10.8]
        if ((xwsz_t)0 != (pgsize & (xwsz_t)XWMM_UNALIGNED_MASK)) {
                rc = -EALIGN;
                goto err_aligned;
        }
        nr = size / pgsize;
        order = xwbop_fls(xwsz_t, nr);
        if ((order < 0) || (nr & ((1U << (xwsz_t)order) - 1U))) {
                rc = -EALIGN;
                goto err_aligned;
        }

        pa->zone.origin = origin;
        pa->zone.size = size;
        pa->name = name;
        pa->pgsize = pgsize;
        pa->max_order = (xwsq_t)order;
        pa->odrbtree = odrbtree;
        pa->pgarray = pgarray;

        for (i = 0; i <= pa->max_order; i++) {
                xwlib_rbtree_init(&pa->odrbtree[i].tree);
                pa->odrbtree[i].leftmost = NULL;
                pa->odrbtree[i].owner = pa;
                pa->odrbtree[i].order = i;
                xwos_sqlk_init(&pa->odrbtree[i].lock);
        }
        for (i = 0; i < nr; i++) {
                pa->pgarray[i].order = XWMM_MEMPOOL_PAGE_ORDER_CMB;
                pa->pgarray[i].mapping = XWMM_MEMPOOL_PAGE_MAPPING_INVAILD;
                pa->pgarray[i].attr.free.seq = i;
                xwlib_rbtree_init_node(&pa->pgarray[i].attr.free.rbnode);
                pa->pgarray[i].data.value = 0;
        }
        // cppcheck-suppress [misra-c2012-17.7]
        xwmm_mempool_page_odrbtree_add(&pa->odrbtree[pa->max_order],
                                       &pa->pgarray[0]);

        pa->i_a.malloc = xwmm_mempool_page_i_a_malloc;
        pa->i_a.free = xwmm_mempool_page_i_a_free;

        return XWOK;

err_aligned:
err_mem2small:
        return rc;
}

/**
 * @brief XWMM API：申请一页内存
 * @param[in] pa: 页分配器的指针
 * @param[in] order: 页数量的阶，页内存大小： `((1 << order) * pa->pgsize)`
 * @param[out] pgbuf: 指向缓冲区的指针，通过此缓冲区返回申请到的页控制块的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 */
__xwos_api
xwer_t xwmm_mempool_page_allocate(struct xwmm_mempool_page_allocator * pa,
                                  xwsq_t order,
                                  struct xwmm_mempool_page ** pgbuf)
{
        xwsq_t odr;
        struct xwmm_mempool_page_odrbtree * ot;
        struct xwmm_mempool_page * pg;
        xwptr_t origin;
        xwer_t rc;

        rc = -ENOMEM;
        for (odr = order; odr <= pa->max_order; odr++) {
                ot = &pa->odrbtree[odr];
                pg = xwmm_mempool_page_odrbtree_choose(ot);
                if (NULL != pg) {
                        xwmm_mempool_page_divide_page(pa, pg, order, ot);
                        origin = pa->zone.origin + (pg->attr.free.seq * pa->pgsize);
                        pg->mapping = origin;
                        *pgbuf = pg;
                        rc = XWOK;
                        break;
                }
        }
        return rc;
}

/**
 * @brief XWMM API：释放一页内存
 * @param[in] pa: 页分配器的指针
 * @param[in] pg: 页控制块的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EALREADY: 页内存已释放
 */
__xwos_api
xwer_t xwmm_mempool_page_free(struct xwmm_mempool_page_allocator * pa,
                              struct xwmm_mempool_page * pg)
{
        xwer_t rc;

        if (XWMM_MEMPOOL_PAGE_MAPPING_FREE == pg->mapping) {
                rc = -EALREADY;
                goto err_already;
        }
        pg->mapping = XWMM_MEMPOOL_PAGE_MAPPING_INVAILD;
        pg->attr.free.seq = xwmm_mempool_page_get_seq(pa, pg);
        xwlib_rbtree_init_node(&pg->attr.free.rbnode);
        pg->data.value = 0;
        xwmm_mempool_page_combine(pa, pg);
        return XWOK;

err_already:
        return rc;
}

/**
 * @brief 将大的页拆分成小的页
 * @param[in] pa: 页分配器的指针
 * @param[in] pg: 被拆分的页控制块的指针
 * @param[in] target_odr: 目标页数量的阶
 * @param[in] ot: 被拆分的页所在的阶红黑树的指针
 */
static __xwos_code
void xwmm_mempool_page_divide_page(struct xwmm_mempool_page_allocator * pa,
                                   struct xwmm_mempool_page * pg,
                                   xwsq_t target_odr,
                                   struct xwmm_mempool_page_odrbtree * ot)
{
        xwsq_t pg_odr;
        xwsq_t pg_seq;
        xwsq_t pg_offset;
        struct xwmm_mempool_page * buddy;

        pg_odr = pg->order;
        pg_seq = pg->attr.free.seq;
        pg_offset = 1U << pg_odr;
        while (pg_odr > target_odr) {
                ot--;
                pg_odr--;
                pg_offset >>= 1;
                buddy = &pa->pgarray[pg_seq + pg_offset];
                // cppcheck-suppress [misra-c2012-17.7]
                xwmm_mempool_page_odrbtree_add(ot, buddy);
                pg->order = pg_odr;
        }
}

/**
 * @brief 合并空闲的伙伴页
 * @param[in] pa: 页分配器的指针
 * @param[in] pg: 页控制块的指针
 */
static __xwos_code
void xwmm_mempool_page_combine(struct xwmm_mempool_page_allocator * pa,
                               struct xwmm_mempool_page * pg)
{
        struct xwmm_mempool_page * buddy;
        xwsq_t odr;
        xwer_t rc;

        odr = pg->order + 1U;
        while (odr < pa->max_order) {
                buddy = xwmm_mempool_page_get_buddy(pa, pg);
                /* `buddy->order` 不能在临界区外使用，
                   因为 `buddy` 在阶红黑树上时，是共享数据。
                   但可以使用 `pg->order` ，因为 `pg` 还未加入到阶红黑树，是本地数据。*/
                rc = xwmm_mempool_page_odrbtree_remove(&pa->odrbtree[pg->order], buddy);
                if (rc < 0) {
                        break;
                }
                if (buddy > pg) {
                        buddy->order = XWMM_MEMPOOL_PAGE_ORDER_CMB;
                        pg->order = odr;
                } else {
                        pg->order = XWMM_MEMPOOL_PAGE_ORDER_CMB;
                        buddy->order = odr;
                        pg = buddy;
                }
                odr++;
        }
        odr = pg->order;
        // cppcheck-suppress [misra-c2012-17.7]
        xwmm_mempool_page_odrbtree_add(&pa->odrbtree[odr], pg);
}

/**
 * @brief 增加页到阶红黑树中
 * @param[in] ot: 阶红黑树的指针
 * @param[in] pg: 页控制块的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 页已在阶红黑树中
 */
static __xwos_code
xwer_t xwmm_mempool_page_odrbtree_add(struct xwmm_mempool_page_odrbtree * ot,
                                      struct xwmm_mempool_page * pg)
{
        struct xwlib_rbtree * tree;
        struct xwlib_rbtree_node ** new;
        xwptr_t lpc;
        struct xwlib_rbtree_node * rbn;
        xwsq_t pgseq;
        struct xwmm_mempool_page * b;
        struct xwmm_mempool_page * leftmost;
        xwreg_t flag;
        xwer_t rc;

        tree = &ot->tree;
        pgseq = pg->attr.free.seq;
        /* 即将加入的页不在阶红黑树中，是本地数据，对 `pg->mapping` 的读可在锁外。*/
        if (XWMM_MEMPOOL_PAGE_MAPPING_FREE == pg->mapping) {
                rc = -EEXIST;
                goto err_exist;
        }

        pg->order = ot->order;
        xwos_sqlk_wr_lock_cpuirqsv(&ot->lock, &flag);
        /* 当页被加入到阶红黑树中，就变成共享数据，对 `pg->mapping` 的写只在锁内。*/
        pg->mapping = XWMM_MEMPOOL_PAGE_MAPPING_FREE;
        new = &tree->root;
        lpc = (xwptr_t)new;
        leftmost = ot->leftmost;
        if (NULL == leftmost) { /* rbtree is empty. */
                ot->leftmost = pg;
        } else if (pgseq < leftmost->attr.free.seq) {
                new = &(leftmost->attr.free.rbnode.left);
                lpc = (xwptr_t)new;
                ot->leftmost = pg;
        } else {
                rbn = *new;
                while (NULL != rbn) {
                        b = xwlib_rbtree_entry(rbn, struct xwmm_mempool_page,
                                               attr.free.rbnode);
                        if (pgseq < b->attr.free.seq) {
                                new = &rbn->left;
                                lpc = (xwptr_t)new;
                                rbn = rbn->left;
                        } else if (pgseq > b->attr.free.seq) {
                                new = &rbn->right;
                                lpc = (xwptr_t)new | (xwptr_t)XWLIB_RBTREE_POS_RIGHT;
                                rbn = rbn->right;
                        } else {
                                XWOS_BUG();
                        }
                }
        }
        xwlib_rbtree_link(&pg->attr.free.rbnode, lpc);
        xwlib_rbtree_insert_color(tree, &pg->attr.free.rbnode);
        xwos_sqlk_wr_unlock_cpuirqrs(&ot->lock, flag);
        return XWOK;

err_exist:
        return rc;
}

/**
 * @brief 从阶红黑树中删除页
 * @param[in] ot: 阶红黑树的指针
 * @param[in] pg: 页控制块的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 页不在阶红黑树中
 * @retval -EBUSY: 页不在阶红黑树中
 */
static __xwos_code
xwer_t xwmm_mempool_page_odrbtree_remove(struct xwmm_mempool_page_odrbtree * ot,
                                         struct xwmm_mempool_page * pg)
{
        struct xwlib_rbtree * tree;
        struct xwlib_rbtree_node * s;
        xwreg_t flag;
        xwer_t rc;

        tree = &ot->tree;
        if (pg->order != ot->order) {
                rc = -ESRCH;
                goto err_odrerr;
        }

        xwos_sqlk_wr_lock_cpuirqsv(&ot->lock, &flag);
        /* 即将被删除的页在阶红黑树中，是共享数据。对 `pg->mapping` 的读只可在锁内。*/
        if (XWMM_MEMPOOL_PAGE_MAPPING_FREE != pg->mapping) {
                rc = -EBUSY;
                goto err_notfree;
        }
        pg->mapping = XWMM_MEMPOOL_PAGE_MAPPING_INVAILD;
        if (pg == ot->leftmost) {
                s = pg->attr.free.rbnode.right;
                if (NULL == s) {
                        s = xwlib_rbtree_get_parent(&pg->attr.free.rbnode);
                }
                if (s != (struct xwlib_rbtree_node *)&tree->root) {
                        ot->leftmost = xwlib_rbtree_entry(s,
                                                          struct xwmm_mempool_page,
                                                          attr.free.rbnode);
                } else {
                        ot->leftmost = NULL;
                }
        }
        xwlib_rbtree_remove(tree, &pg->attr.free.rbnode);
        xwos_sqlk_wr_unlock_cpuirqrs(&ot->lock, flag);
        xwlib_rbtree_init_node(&pg->attr.free.rbnode);
        return XWOK;

err_notfree:
        xwos_sqlk_wr_unlock_cpuirqrs(&ot->lock, flag);
err_odrerr:
        return rc;
}

/**
 * @brief 从阶红黑树中选择一页
 * @param[in] ot: 阶红黑树的指针
 * @return 页控制块的指针或空指针
 */
static __xwos_code
struct xwmm_mempool_page *
xwmm_mempool_page_odrbtree_choose(struct xwmm_mempool_page_odrbtree * ot)
{
        struct xwmm_mempool_page * leftmost;
        xwreg_t flag;
        struct xwlib_rbtree * tree;
        struct xwlib_rbtree_node * s;

        tree = &ot->tree;
        xwos_sqlk_wr_lock_cpuirqsv(&ot->lock, &flag);
        leftmost = ot->leftmost;
        if (NULL == leftmost) {
                xwos_sqlk_wr_unlock_cpuirqrs(&ot->lock, flag);
                goto err_empty;
        }
        /* 即将被选择的页在阶红黑树中，是共享数据。对 `pg->mapping` 的写只可在锁内。*/
        leftmost->mapping = XWMM_MEMPOOL_PAGE_MAPPING_INVAILD;
        s = leftmost->attr.free.rbnode.right;
        if (NULL == s) {
                s = xwlib_rbtree_get_parent(&leftmost->attr.free.rbnode);
        }
        if (s != (struct xwlib_rbtree_node *)&tree->root) {
                ot->leftmost = xwlib_rbtree_entry(s, struct xwmm_mempool_page,
                                                  attr.free.rbnode);
        } else {
                ot->leftmost = NULL;
        }
        xwlib_rbtree_remove(tree, &leftmost->attr.free.rbnode);
        xwos_sqlk_wr_unlock_cpuirqrs(&ot->lock, flag);

        xwlib_rbtree_init_node(&leftmost->attr.free.rbnode);
        return leftmost;

err_empty:
        return NULL;
}

/**
 * @brief 获取页的序列值
 * @param[in] pa: 页分配器的指针
 * @param[in] pg: 页控制块的指针
 * @return 序列值
 */
static __xwos_code
xwsq_t xwmm_mempool_page_get_seq(struct xwmm_mempool_page_allocator * pa,
                                  struct xwmm_mempool_page * pg)
{
        xwsq_t offset;

        offset = (xwsq_t)((xwptr_t)pg - (xwptr_t)(pa->pgarray));
        offset /= (xwsq_t)sizeof(struct xwmm_mempool_page);
        return offset;
}

/**
 * @brief 从页内存地址获得其页控制块的指针
 * @param[in] pa: 页分配器的指针
 * @param[in] mem: 内存块的首地址
 * @return 页的控制块指针
 */
static __xwos_code
struct xwmm_mempool_page *
xwmm_mempool_mem_to_page(struct xwmm_mempool_page_allocator * pa, void * mem)
{
        xwsq_t seq;
        xwptr_t offset;
        struct xwmm_mempool_page * pg;

        offset = (xwptr_t)mem - pa->zone.origin;
        seq = offset / pa->pgsize;
        pg = &pa->pgarray[seq];
        return pg;
}

/**
 * @brief 找到页的伙伴页
 * @param[in] pa: 页分配器的指针
 * @param[in] pg: 页控制块的指针
 * @return 伙伴页控制块的指针
 */
static __xwos_code
struct xwmm_mempool_page *
xwmm_mempool_page_get_buddy(struct xwmm_mempool_page_allocator * pa,
                            struct xwmm_mempool_page * pg)
{
        xwsq_t seq;
        xwsq_t odr;

        seq = pg->attr.free.seq;
        odr = pg->order;
        seq ^= (1U << odr);
        return &pa->pgarray[seq];
}

/**
 * @brief 依据内存地址查找页
 * @param[in] pa: 页分配器的指针
 * @param[in] mem: 内存地址
 * @param[out] pgbuf: 指向缓冲区的指针，通过此缓冲区返回申请到的页控制块的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 内存地址不在内存池的范围内
 */
__xwos_code
xwer_t xwmm_mempool_page_find(struct xwmm_mempool_page_allocator * pa, void * mem,
                              struct xwmm_mempool_page ** pgbuf)
{
        xwer_t rc;
        struct xwmm_mempool_page * pg;

        if (((xwptr_t)mem < pa->zone.origin) ||
            ((xwptr_t)mem >= (pa->zone.origin + pa->zone.size))) {
                rc = -ERANGE;
        } else {
                pg = xwmm_mempool_mem_to_page(pa, mem);
                while (XWMM_MEMPOOL_PAGE_ORDER_CMB == pg->order) {
                        pg--;
                }
                *pgbuf = pg;
                rc = XWOK;
        }
        return rc;
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
xwer_t xwmm_mempool_page_i_a_malloc(void * this, xwsz_t size, void ** membuf)
{
        struct xwmm_mempool_page_allocator * pa;
        struct xwmm_mempool_page * pg;
        xwssq_t order;
        xwsz_t nr;
        xwer_t rc;

        pa = this;
        nr = size / pa->pgsize;
        order = xwbop_fls(xwsz_t, nr);
        if ((order < 0) || ((pa->pgsize << (xwsz_t)order) < size)) {
                order++;
        }
        rc = xwmm_mempool_page_allocate(pa, (xwsq_t)order, &pg);
        if (XWOK == rc) {
                pg->data.value = size;
                *membuf = (void *)pg->mapping;
        }
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
xwer_t xwmm_mempool_page_i_a_free(void * this, void * mem)
{
        struct xwmm_mempool_page_allocator * pa;
        struct xwmm_mempool_page * pg;
        xwer_t rc;

        pa = this;

        if (((xwptr_t)mem < pa->zone.origin) ||
            (((xwptr_t)mem - pa->zone.origin) >= (xwptr_t)pa->zone.size)) {
                rc = -ERANGE;
                goto err_range;
        }

        pg = xwmm_mempool_mem_to_page(pa, mem);
        rc = xwmm_mempool_page_free(pa, pg);
        if (rc < 0) {
                goto err_page_free;
        }
        return XWOK;

err_page_free:
err_range:
        return rc;
}
