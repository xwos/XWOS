/**
 * @file
 * @brief XuanWuOS的内存管理机制：伙伴算法内存块分配器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/bma.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_mem_to_bcb(struct xwmm_bma * bma, void * mem);

static __xwos_code
void * xwmm_bma_bcb_to_mem(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb);

static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_find_buddy(struct xwmm_bma * bma,
                                          struct xwmm_bma_bcb * bcb);

static __xwos_code
void xwmm_bma_orderlist_add(struct xwmm_bma * bma,
                            struct xwmm_bma_orderlist * ol,
                            struct xwmm_bma_bcb * bcb);

static __xwos_code
xwer_t xwmm_bma_orderlist_remove(struct xwmm_bma * bma,
                                 struct xwmm_bma_orderlist * ol,
                                 xwu8_t odr,
                                 struct xwmm_bma_bcb * bcb);

static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_orderlist_choose(struct xwmm_bma * bma,
                                                struct xwmm_bma_orderlist * ol);

static __xwos_code
void xwmm_bma_divide_block(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb,
                           xwsq_t target_odr, struct xwmm_bma_orderlist * curr_ol);

static __xwos_code
void xwmm_bma_combine(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWMM API：静态方式初始化伙伴算法内存块分配器。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param name: (I) 名字
 * @param origin: (I) 内存区域的起始地址
 * @param total: (I) 内存区域的总大小
 * @param blksize: (I) 单位内存块的大小
 * @param orderlists: (I) 阶链表数组的指针
 * @param bcbs: (I) 块控制块数组的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_bma_init(struct xwmm_bma * bma, const char * name,
                     xwptr_t origin, xwsz_t total, xwsz_t blksize,
                     struct xwmm_bma_orderlist * orderlists,
                     struct xwmm_bma_bcb * bcbs)
{
        xwer_t rc;
        xwsq_t odr;
        xwsz_t num;
        xwsz_t i;
        xwsq_t j;

        XWOS_VALIDATE((bma), "nullptr", -EFAULT);
        XWOS_VALIDATE((orderlists), "nullptr", -EFAULT);
        XWOS_VALIDATE((bcbs), "nullptr", -EFAULT);

        if (__unlikely(total < blksize)) {
                rc = -E2SMALL;
                goto err_mem2small;
        }
        num = total / blksize;
        odr = (xwsq_t)xwbop_fls(xwsz_t, num);
        if (__unlikely(odr > XWMM_BMA_MAX_ORDER)) {
                rc = -ERANGE;
                goto err_odr2large;
        }
        if (__unlikely((1U << odr) != num)) {
                xwmm_bmalogf(WARNING,
                             "The number of blocks is not power(2, order)!\n");
        }

        bma->name = name;
        bma->zone.origin = origin;
        bma->zone.size = total;
        bma->blksize = blksize;
        bma->orderlists = orderlists;
        bma->bcbs = bcbs;
        bma->max_order = odr;

        /* init all blocks */
        for (i = 0; i < num; i++) {
                bcbs[i].order = XWMM_BMA_COMBINED;
        }
        bcbs[0].order = (xwu8_t)odr;

        /* init all order lists */
        for (j = 0; j <= odr; j++) {
                xwlib_bclst_init_head(&orderlists[j].head);
                xwosal_splk_init(&orderlists[j].lock);
        }
        xwmm_bma_orderlist_add(bma, &orderlists[odr], &bcbs[0]);
        return OK;

err_odr2large:
err_mem2small:
        return rc;
}

/**
 * @brief XWMM API：销毁静态方式初始化的伙伴算法内存块分配器。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_bma_destroy(struct xwmm_bma * bma)
{
        XWOS_VALIDATE((bma), "nullptr", -EFAULT);

        XWOS_UNUSED(bma);
        return OK;
}

/**
 * @brief XWMM API：动态方式创建伙伴算法内存块分配器。
 * @param ptrbuf: (O) 用于返回新的伙伴算法内存块分配器对象指针的缓存
 * @param name: (I) 名字
 * @param origin: (I) 内存区域的起始地址
 * @param total: (I) 内存区域的总大小
 * @param blksize: (I) 单位内存块的大小
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_bma_create(struct xwmm_bma ** ptrbuf, const char * name,
                       xwptr_t origin, xwsz_t total, xwsz_t blksize)
{
        void * mem;
        struct xwmm_bma * bma;
        xwsz_t num;
        xwsq_t odr;
        struct xwmm_bma_orderlist * orderlists;
        struct xwmm_bma_bcb * bcbs;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        if (__unlikely(total < blksize)) {
                rc = -E2SMALL;
                goto err_mem2small;
        }
        num = total / blksize;
        odr = (xwsq_t)xwbop_fls(xwsz_t, num);
        if (__unlikely(odr > XWMM_BMA_MAX_ORDER)) {
                rc = -ERANGE;
                goto err_odr2large;
        }

        rc = xwmm_kma_alloc(sizeof(struct xwmm_bma), XWMM_ALIGNMENT, &mem);
        if (__unlikely(rc < 0)) {
                goto err_bma_alloc;
        }
        bma = mem;

        rc = xwmm_kma_alloc((odr + 1) * sizeof(struct xwmm_bma_orderlist),
                            XWMM_ALIGNMENT, &mem);
        if (__unlikely(rc < 0)) {
                goto err_orderlists_alloc;
        }
        orderlists = mem;

        rc = xwmm_kma_alloc(num * sizeof(struct xwmm_bma_bcb),
                            XWMM_ALIGNMENT, &mem);
        if (__unlikely(rc < 0)) {
                goto err_bcbs_alloc;
        }
        bcbs = mem;

        rc = xwmm_bma_init(bma, name, origin, total, blksize, orderlists, bcbs);
        if (__unlikely(rc < 0)) {
                goto err_bma_init;
        }
        *ptrbuf = bma;
        return OK;

err_bma_init:
        xwmm_kma_free(bcbs);
err_bcbs_alloc:
        xwmm_kma_free(orderlists);
err_orderlists_alloc:
        xwmm_kma_free(bma);
err_bma_alloc:
err_odr2large:
err_mem2small:
        return rc;
}

/**
 * @brief XWMM API：删除动态方式创建的伙伴算法内存块分配器。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_bma_delete(struct xwmm_bma * bma)
{
        XWOS_VALIDATE((bma), "nullptr", -EFAULT);

        xwmm_kma_free(((struct xwmm_bma *)bma)->bcbs);
        xwmm_kma_free(((struct xwmm_bma *)bma)->orderlists);
        xwmm_bma_destroy(bma);
        xwmm_kma_free(bma);
        return OK;
}

/**
 * @brief 从内存块首地址获得其控制块的指针。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param mem: (I) 内存块的首地址
 * @return 内存块的控制块指针或类型为指针的错误码
 */
static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_mem_to_bcb(struct xwmm_bma * bma, void * mem)
{
        struct xwmm_bma_bcb * bcb;
        xwptr_t ofs;
        xwptr_t idx;

        ofs = (xwptr_t)mem - bma->zone.origin;
        idx = ofs / (xwptr_t)bma->blksize;
        if (ofs == (idx * bma->blksize)) {
                bcb = &bma->bcbs[idx];
        } else {
                bcb = err_ptr(-EINVAL);
        }
        return bcb;
}

/**
 * @brief 从内存块的控制块指针获得内存块的首地址。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param bcb: (I) 内存块的控制块指针
 * @return 内存块的首地址
 */
static __xwos_code
void * xwmm_bma_bcb_to_mem(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb)
{
        xwptr_t idx;
        xwptr_t mem;

        idx = ((xwptr_t)bcb - (xwptr_t)bma->bcbs) / sizeof(struct xwmm_bma_bcb);
        mem = idx * bma->blksize + bma->zone.origin;
        return (void *)mem;
}

/**
 * @brief 找到内存块的伙伴。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param bcb: (I) 内存块的控制块指针
 * @return 伙伴内存块的控制块指针
 */
static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_find_buddy(struct xwmm_bma * bma,
                                          struct xwmm_bma_bcb * bcb)
{
        xwsq_t seq;
        xwsq_t odr;

        seq = ((xwptr_t)bcb - (xwptr_t)bma->bcbs) / sizeof(struct xwmm_bma_bcb);
        odr = (xwsq_t)bcb->order & XWMM_BMA_ORDER_MASK;
        seq ^= (1U << odr);
        return &bma->bcbs[seq];
}

/**
 * @brief 将一块内存加入到阶链表。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param ol: (I) 阶链表的指针
 * @param bcb: (I) 内存块的控制块指针
 */
static __xwos_code
void xwmm_bma_orderlist_add(struct xwmm_bma * bma,
                            struct xwmm_bma_orderlist * ol,
                            struct xwmm_bma_bcb * bcb)
{
        struct xwlib_bclst_node * n;
        xwreg_t flag;

        n = xwmm_bma_bcb_to_mem(bma, bcb);
        xwlib_bclst_init_node(n);
        xwosal_splk_lock_cpuirqsv(&ol->lock, &flag);
        xwlib_bclst_add_tail(&ol->head, n);
        xwosal_splk_unlock_cpuirqrs(&ol->lock, flag);
}

/**
 * @brief 从阶链表中删除一块内存。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param ol: (I) 阶链表的指针
 * @param odr: (I) 阶链表的阶
 * @param bcb: (I) 内存块的控制块指针
 */
static __xwos_code
xwer_t xwmm_bma_orderlist_remove(struct xwmm_bma * bma,
                                 struct xwmm_bma_orderlist * ol,
                                 xwu8_t odr,
                                 struct xwmm_bma_bcb * bcb)
{
        struct xwlib_bclst_node * n;
        xwreg_t flag;
        xwer_t rc;

        n = xwmm_bma_bcb_to_mem(bma, bcb);

        xwosal_splk_lock_cpuirqsv(&ol->lock, &flag);
        if (odr != bcb->order) {
                rc = -ESRCH;
        } else {
                rc = OK;
                xwlib_bclst_del_init(n);
        }
        xwosal_splk_unlock_cpuirqrs(&ol->lock, flag);
        return rc;
}

/**
 * @brief 从阶链表中选择一块内存，并返回其块控制块。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param ol: (I) 阶链表的指针
 * @return 块控制块指针或类型为指针的错误码
 * @retval -ENOENT: 阶链表为空
 */
static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_orderlist_choose(struct xwmm_bma * bma,
                                                struct xwmm_bma_orderlist * ol)
{
        struct xwlib_bclst_node * n;
        struct xwmm_bma_bcb * bcb;
        xwreg_t flag;

        xwosal_splk_lock_cpuirqsv(&ol->lock, &flag);
        if (xwlib_bclst_tst_empty(&ol->head)) {
                xwosal_splk_unlock_cpuirqrs(&ol->lock, flag);
                bcb = err_ptr(-ENOENT);
        } else {
                n = ol->head.next;
                xwlib_bclst_del_init(n);
                xwosal_splk_unlock_cpuirqrs(&ol->lock, flag);
                bcb = xwmm_bma_mem_to_bcb(bma, n);
                bcb->order |= XWMM_BMA_INUSED;
        }
        return bcb;
}

/**
 * @brief 将一个大块分割成小块。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param bcb: (I) 大块的内存块的控制块的指针
 * @param target_odr: (I) 目标块数量的阶
 * @param curr_ol: (I) 当前被分割的大块所在的阶链表
 */
static __xwos_code
void xwmm_bma_divide_block(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb,
                           xwsq_t target_odr, struct xwmm_bma_orderlist * curr_ol)
{
        xwsq_t blk_odr;
        xwsq_t blk_idx;
        xwsq_t blk_ofs;
        struct xwmm_bma_bcb * buddy;

        blk_odr = (xwsq_t)bcb->order & XWMM_BMA_ORDER_MASK;
        blk_idx = ((xwptr_t)bcb - (xwptr_t)bma->bcbs) / sizeof(struct xwmm_bma_bcb);

        while (blk_odr > target_odr) {
                curr_ol--;
                blk_odr--;
                blk_ofs = 1U << blk_odr;
                buddy = &bma->bcbs[blk_idx + blk_ofs];
                buddy->order = (xwu8_t)blk_odr;
                xwmm_bma_orderlist_add(bma, curr_ol, buddy);
                bcb->order = (xwu8_t)blk_odr | XWMM_BMA_INUSED;
                xwmm_bmalogf(DEBUG,
                             "bcb(idx:0x%X,odr:0x%X), buddy(idx:0x%X,odr:0x%X)\n",
                             (bcb - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                             bcb->order,
                             (buddy - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                             buddy->order);
        }
}

/**
 * @brief XWMM API：申请一块连续的内存。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param order: (I) 块数量的阶，内存块大小：((1 << order) * bma->blksize)
 * @param membuf: (O) 指向指针缓存的指针，此指针缓存用于返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -ERANGE: order无效
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_bma_alloc(struct xwmm_bma * bma, xwsq_t order, void ** membuf)
{
        xwer_t rc;
        xwsq_t o;
        struct xwmm_bma_orderlist * ol;
        struct xwmm_bma_bcb * bcb;

        XWOS_VALIDATE((bma), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((order <= (xwsq_t)XWSSQ_MAX), "out-of-range", -ERANGE);

        ol = NULL;
        bcb = err_ptr(-ENOENT);
        for (o = order; o <= bma->max_order; o++) {
                ol = &bma->orderlists[o];
                bcb = xwmm_bma_orderlist_choose(bma, ol);
                if (!is_err(bcb)) {
                        break;
                }
        }
        if (is_err(bcb)) {
                rc = -ENOMEM;
                *membuf = NULL;
                goto err_nomem;
        }
        xwmm_bmalogf(DEBUG,
                     "bcb(idx:0x%X,odr:0x%X)\n",
                     (bcb - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                     bcb->order);
        xwmm_bma_divide_block(bma, bcb, order, ol);
        *membuf = xwmm_bma_bcb_to_mem(bma, bcb);
        return OK;

err_nomem:
        return rc;
}

/**
 * @brief 找到内存块的伙伴。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param bcb: (I) 内存块的控制块的指针
 * @return 错误码
 */
static __xwos_code
void xwmm_bma_combine(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb)
{
        struct xwmm_bma_bcb * buddy;
        xwsq_t odr;
        xwer_t rc;

        bcb->order &= XWMM_BMA_ORDER_MASK; /* clear the `inused' state */
        odr = (xwsq_t)bcb->order + 1;

        while (odr <= bma->max_order) {
                buddy = xwmm_bma_find_buddy(bma, bcb);
                XWOS_BUG_ON(XWMM_BMA_COMBINED == buddy->order);
                xwmm_bmalogf(DEBUG,
                             "bcb(idx:0x%X,odr:0x%X), buddy(idx:0x%X,odr:0x%X)\n",
                             (bcb - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                             bcb->order,
                             (buddy - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                             buddy->order);
                rc = xwmm_bma_orderlist_remove(bma,
                                               &bma->orderlists[bcb->order],
                                               bcb->order,
                                               buddy);
                if (rc < 0) {
                        break;
                }
                if (buddy > bcb) {
                        buddy->order = XWMM_BMA_COMBINED;
                        bcb->order = (xwu8_t)odr;
                } else {
                        bcb->order = XWMM_BMA_COMBINED;
                        buddy->order = (xwu8_t)odr;
                        bcb = buddy;
                }
                odr++;
        }
        odr = bcb->order;
        xwmm_bma_orderlist_add(bma, &bma->orderlists[odr], bcb);
}

/**
 * @brief XWMM API：释放内存块。
 * @param bma: (I) 伙伴算法内存块分配器对象的指针
 * @param mem: (I) 内存块的首地址指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EINVAL: 参数错误
 * @retval -ERANGE: 内存块不属于指定的伙伴算法内存块分配器对象
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_bma_free(struct xwmm_bma * bma, void * mem)
{
        struct xwmm_bma_bcb * bcb;
        xwer_t rc;

        XWOS_VALIDATE((bma), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);

        if (__unlikely((((xwptr_t)mem < bma->zone.origin) ||
                        ((xwptr_t)mem >= (bma->zone.origin + bma->zone.size))))) {
                rc = -ERANGE;
                goto err_range;
        }
        bcb = xwmm_bma_mem_to_bcb(bma, mem);
        if (__unlikely(is_err(bcb))) {
                rc = ptr_err(bcb);
                goto err_invalmem;
        }
        if (!(XWMM_BMA_INUSED & bcb->order)) {
                rc = -EINVAL;
                goto err_invalmem;
        }
        xwmm_bmalogf(DEBUG,
                     "bcb(idx:0x%X,odr:0x%X)\n",
                     (bcb - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                     bcb->order);
        xwmm_bma_combine(bma, bcb);
        return OK;

err_invalmem:
err_range:
        return rc;
}
